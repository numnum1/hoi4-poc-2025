#include "OrganizationProxy.h"

#include "BuildingProxy.h"
#include "Buildings.h"
#include "CharacterProxy.h"
#include "Characters.h"
#include "NationProxy.h"
#include "Organizations.h"
#include "RtsProxySubsystem.h"

void UOrganizationProxy::Setup(URtsProxySubsystem* InProxies, const FOrganizationId InOrganizationId, UOrganizations* InOrganizations)
{
	Proxies = InProxies;
	OrganizationId = InOrganizationId;
	IdValue = InOrganizationId.Value;
	Organizations = InOrganizations;
	BindEvents();
}

void UOrganizationProxy::BindEvents()
{
	if (Organizations == nullptr)
	{
		return;
	}

	Organizations->OnNationSettled.AddDynamic(this, &UOrganizationProxy::HandleOrganizationEvent);
	Organizations->OnAlignmentPublished.AddDynamic(this, &UOrganizationProxy::HandleOrganizationEvent);
	Organizations->OnOrganizationDissolved.AddDynamic(this, &UOrganizationProxy::HandleOrganizationEvent);
}

void UOrganizationProxy::UnbindEvents()
{
	if (Organizations == nullptr)
	{
		return;
	}

	Organizations->OnNationSettled.RemoveAll(this);
	Organizations->OnAlignmentPublished.RemoveAll(this);
	Organizations->OnOrganizationDissolved.RemoveAll(this);
}

void UOrganizationProxy::HandleOrganizationEvent(const FOrganizationId InOrganizationId)
{
	if (InOrganizationId == OrganizationId)
	{
		NotifyChanged();
	}
}

bool UOrganizationProxy::IsAlive() const
{
	return Organizations != nullptr && Organizations->IsAlive(OrganizationId);
}

FText UOrganizationProxy::GetDisplayName() const
{
	return Organizations ? Organizations->GetOrganizationInfo(OrganizationId).OrganizationName : FText::GetEmpty();
}

FSlateBrush UOrganizationProxy::GetLogo() const
{
	return Organizations ? Organizations->GetOrganizationInfo(OrganizationId).Logo : FSlateBrush();
}

FOrganizationAlignment UOrganizationProxy::GetAlignment() const
{
	return Organizations ? Organizations->GetAlignment(OrganizationId) : FOrganizationAlignment();
}

bool UOrganizationProxy::IsAlignmentPublished() const
{
	return GetAlignment().bPublished;
}

TArray<float> UOrganizationProxy::GetAlignmentBranches() const
{
	return GetAlignment().Branches;
}

float UOrganizationProxy::GetAlignmentBranch(const int32 InIndex) const
{
	const TArray<float> Branches = GetAlignment().Branches;
	return Branches.IsValidIndex(InIndex) ? Branches[InIndex] : 0.0f;
}

TArray<UNationProxy*> UOrganizationProxy::GetSettledNations() const
{
	TArray<UNationProxy*> Result;
	if (Organizations == nullptr || Proxies == nullptr)
	{
		return Result;
	}

	const TArray<FNationId> Ids = Organizations->GetSettledNations(OrganizationId);
	Result.Reserve(Ids.Num());
	for (const FNationId Id : Ids)
	{
		if (UNationProxy* Nation = Proxies->GetNation(Id))
		{
			Result.Add(Nation);
		}
	}
	return Result;
}

TArray<UCharacterProxy*> UOrganizationProxy::GetMembers() const
{
	TArray<UCharacterProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	// UCharacters has no organization index, so this walks the whole list. Convenience over
	// speed, deliberately — see URtsEntityProxy.
	UCharacters* CharactersContainer = Proxies->GetCharactersContainer();
	if (CharactersContainer == nullptr)
	{
		return Result;
	}

	for (int32 Index = 0; Index < CharactersContainer->Num(); ++Index)
	{
		const FCharacterId CharacterId(Index);
		if (!CharactersContainer->IsAlive(CharacterId))
		{
			continue;
		}

		bool bHoldsPosition = false;
		for (const FPosition& Position : CharactersContainer->GetPositions(CharacterId))
		{
			if (Position.OrganizationId == OrganizationId)
			{
				bHoldsPosition = true;
				break;
			}
		}

		if (bHoldsPosition)
		{
			if (UCharacterProxy* Character = Proxies->GetCharacter(CharacterId))
			{
				Result.Add(Character);
			}
		}
	}
	return Result;
}

TArray<UBuildingProxy*> UOrganizationProxy::GetOwnedBuildings() const
{
	TArray<UBuildingProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	UBuildings* BuildingsContainer = Proxies->GetBuildingsContainer();
	if (BuildingsContainer == nullptr)
	{
		return Result;
	}

	for (const FBuildingId BuildingId : BuildingsContainer->CollectBuildingsOwnedByOrganization(OrganizationId))
	{
		if (UBuildingProxy* Building = Proxies->GetBuilding(BuildingId))
		{
			Result.Add(Building);
		}
	}
	return Result;
}

void UOrganizationProxy::PublishAlignment(const TArray<float>& InAlignment)
{
	if (Organizations)
	{
		Organizations->PublishAlignment(OrganizationId, InAlignment);
	}
}

void UOrganizationProxy::SettleIn(const UNationProxy* InNation)
{
	if (Organizations == nullptr || InNation == nullptr)
	{
		return;
	}
	Organizations->SettleNation(OrganizationId, InNation->GetId());
}

void UOrganizationProxy::Dissolve()
{
	if (Organizations)
	{
		Organizations->Dissolve(OrganizationId);
	}
}
