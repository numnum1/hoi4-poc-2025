#include "CharacterActionsViewModel.h"

#include "ActionViewModel.h"
#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "Organizations.h"
#include "RtsActions.h"
#include "RtsProxySubsystem.h"

void UCharacterActionsViewModel::Initialize(const FCharacterId InCharacterId)
{
	URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this);
	UCharacterProxy* NewProxy = ProxySubsystem ? ProxySubsystem->GetCharacter(InCharacterId) : nullptr;

	if (Proxy == NewProxy && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();
	Proxy = NewProxy;
	CharacterId = InCharacterId;
	Organizations = ProxySubsystem ? ProxySubsystem->GetOrganizationsContainer() : nullptr;

	Context = FRtsActionContext();
	Context.Actor = Proxy;

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UCharacterActionsViewModel::HandleProxyChanged);
	}
	if (Organizations)
	{
		Organizations->OnOrganizationAdded.AddDynamic(this, &UCharacterActionsViewModel::HandleOrganizationAdded);
	}

	Refresh();
}

void UCharacterActionsViewModel::SetScopeSelf()
{
	Context.Scope = ERtsActionScope::RAS_Self;
	Context.OrganizationId = FOrganizationId();
	Context.RoleId = FRoleId();
	Refresh();
}

void UCharacterActionsViewModel::SetScopePosition(const int32 InPositionIndex)
{
	Context.Scope = ERtsActionScope::RAS_Position;

	// Organization and role are read from the same position index, so a context can never claim a
	// role at an organization the character does not hold it at.
	const UOrganizationProxy* Organization = Proxy ? Proxy->GetOrganization(InPositionIndex) : nullptr;
	Context.OrganizationId = Organization ? Organization->GetId() : FOrganizationId();
	Context.RoleId = Organization ? Proxy->GetPositionRole(InPositionIndex) : FRoleId();

	Refresh();
}

void UCharacterActionsViewModel::Refresh_Implementation()
{
	TArray<TSubclassOf<UActionViewModel>> OfferedClasses;
	RtsActions::CollectClassesForScope(Context.Scope, OfferedClasses);

	TArray<TObjectPtr<UActionViewModel>> NewActions;
	NewActions.Reserve(OfferedClasses.Num());

	for (const TSubclassOf<UActionViewModel>& ActionClass : OfferedClasses)
	{
		TObjectPtr<UActionViewModel>& Entry = EntryCache.FindOrAdd(ActionClass);
		if (Entry == nullptr)
		{
			Entry = NewObject<UActionViewModel>(this, ActionClass);
		}

		Entry->SetContext(Context);
		NewActions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Actions, NewActions);
	UE_MVVM_SET_PROPERTY_VALUE(ActionCount, NewActions.Num());
}

void UCharacterActionsViewModel::HandleProxyChanged()
{
	Refresh();
}

void UCharacterActionsViewModel::HandleOrganizationAdded()
{
	Refresh();
}

void UCharacterActionsViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
	if (Organizations)
	{
		Organizations->OnOrganizationAdded.RemoveAll(this);
	}
}

void UCharacterActionsViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
