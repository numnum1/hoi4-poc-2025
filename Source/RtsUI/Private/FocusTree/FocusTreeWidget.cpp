#include "FocusTree/FocusTreeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Focuses/FocusNodeViewModel.h"
#include "Focuses/UseOrganizationFocusTree.h"
#include "FocusTree/FocusNodeWidget.h"
#include "Rendering/DrawElementTypes.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UFocusTreeWidget::SetHUDViewModel(URtsHUDViewModel* InHUDViewModel)
{
	UnbindFocusTree();

	HUDViewModel = InHUDViewModel;
	BoundFocusTree = InHUDViewModel ? InHUDViewModel->GetFocusTree() : nullptr;

	if (BoundFocusTree)
	{
		NodesChangedHandle = BoundFocusTree->AddFieldValueChangedDelegate(
			UUseOrganizationFocusTree::FFieldNotificationClassDescriptor::Nodes,
			INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UFocusTreeWidget::HandleNodesChanged));
	}

	RebuildNodes();
}

void UFocusTreeWidget::SelectTree(const int32 InTreeIndex)
{
	if (BoundFocusTree)
	{
		BoundFocusTree->SelectTree(InTreeIndex);
	}
}

void UFocusTreeWidget::NativeDestruct()
{
	UnbindFocusTree();

	Super::NativeDestruct();
}

void UFocusTreeWidget::UnbindFocusTree()
{
	if (BoundFocusTree && NodesChangedHandle.IsValid())
	{
		BoundFocusTree->RemoveFieldValueChangedDelegate(UUseOrganizationFocusTree::FFieldNotificationClassDescriptor::Nodes, NodesChangedHandle);
	}

	NodesChangedHandle.Reset();
	BoundFocusTree = nullptr;
}

void UFocusTreeWidget::HandleNodesChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	RebuildNodes();
}

FVector2f UFocusTreeWidget::ComputeNodeTopLeft(const UFocusNodeViewModel* InNode) const
{
	const FVector2f Cell(CellSize);
	return FVector2f(InNode->GetGridColumn() * Cell.X, InNode->GetGridRow() * Cell.Y);
}

void UFocusTreeWidget::RebuildNodes()
{
	if (NodeCanvas == nullptr)
	{
		return;
	}

	NodeCanvas->ClearChildren();

	if (BoundFocusTree == nullptr || NodeWidgetClass == nullptr)
	{
		return;
	}

	for (UFocusNodeViewModel* Node : BoundFocusTree->GetNodes())
	{
		UFocusNodeWidget* NodeWidget = CreateWidget<UFocusNodeWidget>(this, NodeWidgetClass);
		if (NodeWidget == nullptr)
		{
			continue;
		}

		NodeWidget->SetNode(Node, HUDViewModel);

		UCanvasPanelSlot* NodeSlot = NodeCanvas->AddChildToCanvas(NodeWidget);
		NodeSlot->SetAutoSize(false);
		NodeSlot->SetPosition(FVector2D(ComputeNodeTopLeft(Node)));
		NodeSlot->SetSize(NodeSize);
	}
}

FVector2f UFocusTreeWidget::CanvasToLocal(const FGeometry& InAllottedGeometry, const FVector2f& InCanvasPosition) const
{
	const FVector2f Absolute = NodeCanvas->GetCachedGeometry().LocalToAbsolute(InCanvasPosition);
	return InAllottedGeometry.AbsoluteToLocal(Absolute);
}

int32 UFocusTreeWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 MaxLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (BoundFocusTree == nullptr || NodeCanvas == nullptr)
	{
		return MaxLayer;
	}

	const TArray<TObjectPtr<UFocusNodeViewModel>>& Nodes = BoundFocusTree->GetNodes();
	const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry();
	const int32 LineLayer = MaxLayer + 1;
	const FVector2f Extent(NodeSize);
	const FVector2f HalfNode = Extent * 0.5f;

	for (const FFocusTreeEdge& Edge : BoundFocusTree->GetEdges())
	{
		if (!Nodes.IsValidIndex(Edge.FromIndex) || !Nodes.IsValidIndex(Edge.ToIndex))
		{
			continue;
		}

		const FVector2f Start = CanvasToLocal(AllottedGeometry, ComputeNodeTopLeft(Nodes[Edge.FromIndex]) + FVector2f(HalfNode.X, Extent.Y));
		const FVector2f End = CanvasToLocal(AllottedGeometry, ComputeNodeTopLeft(Nodes[Edge.ToIndex]) + FVector2f(HalfNode.X, 0.f));
		const float MidY = (Start.Y + End.Y) * 0.5f;

		TArray<FVector2f> Points;
		Points.Add(Start);
		Points.Add(FVector2f(Start.X, MidY));
		Points.Add(FVector2f(End.X, MidY));
		Points.Add(End);

		if (Edge.bIsAlternative)
		{
			FSlateDrawElement::MakeDashedLines(OutDrawElements, LineLayer, PaintGeometry, MoveTemp(Points), ESlateDrawEffect::None, AlternativeLineColor, LineThickness, DashLength);
		}
		else
		{
			FSlateDrawElement::MakeLines(OutDrawElements, LineLayer, PaintGeometry, MoveTemp(Points), ESlateDrawEffect::None, RequiredLineColor, true, LineThickness);
		}
	}

	for (const FFocusTreeExclusiveLink& Link : BoundFocusTree->GetExclusiveLinks())
	{
		if (!Nodes.IsValidIndex(Link.FirstIndex) || !Nodes.IsValidIndex(Link.SecondIndex))
		{
			continue;
		}

		TArray<FVector2f> Points;
		Points.Add(CanvasToLocal(AllottedGeometry, ComputeNodeTopLeft(Nodes[Link.FirstIndex]) + FVector2f(Extent.X, HalfNode.Y)));
		Points.Add(CanvasToLocal(AllottedGeometry, ComputeNodeTopLeft(Nodes[Link.SecondIndex]) + FVector2f(0.f, HalfNode.Y)));

		FSlateDrawElement::MakeLines(OutDrawElements, LineLayer, PaintGeometry, MoveTemp(Points), ESlateDrawEffect::None, ExclusiveLineColor, true, LineThickness);
	}

	return LineLayer;
}
