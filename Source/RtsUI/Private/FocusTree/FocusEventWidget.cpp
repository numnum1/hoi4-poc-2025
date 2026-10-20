#include "FocusTree/FocusEventWidget.h"

#include "Components/PanelWidget.h"
#include "Focuses/FocusEventOptionViewModel.h"
#include "Focuses/UseOrganizationFocusTree.h"
#include "FocusTree/FocusEventOptionWidget.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UFocusEventWidget::SetHUDViewModel(URtsHUDViewModel* InHUDViewModel)
{
	UnbindFocusTree();

	HUDViewModel = InHUDViewModel;
	BoundFocusTree = InHUDViewModel ? InHUDViewModel->GetFocusTree() : nullptr;

	if (BoundFocusTree)
	{
		EventOptionsChangedHandle = BoundFocusTree->AddFieldValueChangedDelegate(
			UUseOrganizationFocusTree::FFieldNotificationClassDescriptor::EventOptions,
			INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UFocusEventWidget::HandleEventOptionsChanged));
	}

	RebuildOptions();
}

void UFocusEventWidget::NativeDestruct()
{
	UnbindFocusTree();

	Super::NativeDestruct();
}

void UFocusEventWidget::UnbindFocusTree()
{
	if (BoundFocusTree && EventOptionsChangedHandle.IsValid())
	{
		BoundFocusTree->RemoveFieldValueChangedDelegate(UUseOrganizationFocusTree::FFieldNotificationClassDescriptor::EventOptions, EventOptionsChangedHandle);
	}

	EventOptionsChangedHandle.Reset();
	BoundFocusTree = nullptr;
}

void UFocusEventWidget::HandleEventOptionsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	RebuildOptions();
}

void UFocusEventWidget::RebuildOptions()
{
	if (OptionsBox == nullptr)
	{
		return;
	}

	OptionsBox->ClearChildren();

	if (BoundFocusTree == nullptr || OptionWidgetClass == nullptr)
	{
		return;
	}

	for (UFocusEventOptionViewModel* Option : BoundFocusTree->GetEventOptions())
	{
		UFocusEventOptionWidget* OptionWidget = CreateWidget<UFocusEventOptionWidget>(this, OptionWidgetClass);
		if (OptionWidget == nullptr)
		{
			continue;
		}

		OptionWidget->SetOption(Option, HUDViewModel);
		OptionsBox->AddChild(OptionWidget);
	}
}
