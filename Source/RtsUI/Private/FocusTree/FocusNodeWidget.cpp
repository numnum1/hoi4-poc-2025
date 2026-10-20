#include "FocusTree/FocusNodeWidget.h"

#include "Focuses/FocusNodeViewModel.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UFocusNodeWidget::SetNode(UFocusNodeViewModel* InNode, URtsHUDViewModel* InHUDViewModel)
{
	Node = InNode;
	HUDViewModel = InHUDViewModel;
	OnNodeSet(InNode);
}

void UFocusNodeWidget::Click()
{
	if (Node && HUDViewModel)
	{
		HUDViewModel->StartFocus(Node->GetFocus());
	}
}
