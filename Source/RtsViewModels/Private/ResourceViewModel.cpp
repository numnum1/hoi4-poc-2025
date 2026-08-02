#include "ResourceViewModel.h"

#include "ResourceDetailViewModel.h"
#include "RtsResources.h"

void UResourceViewModel::SetSource(const EResourceType InResource, const FRtsResourceContext& InContext)
{
	ResourceType = InResource;

	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, URtsResources::GetDisplayName(InResource));
	UE_MVVM_SET_PROPERTY_VALUE(Icon, URtsResources::GetIcon(InResource));

	const int32 Total = URtsResources::GetAmount(InResource, EResourceDetail::RD_Total, InContext);
	UE_MVVM_SET_PROPERTY_VALUE(Amount, Total);
	UE_MVVM_SET_PROPERTY_VALUE(AmountText, URtsResources::FormatAmount(InResource, Total));

	const TArray<EResourceDetail> ResourceDetails = URtsResources::GetDetails(InResource);

	TArray<TObjectPtr<UResourceDetailViewModel>> NewDetails;
	NewDetails.Reserve(ResourceDetails.Num());

	for (int32 Index = 0; Index < ResourceDetails.Num(); ++Index)
	{
		if (!DetailCache.IsValidIndex(Index))
		{
			DetailCache.Add(NewObject<UResourceDetailViewModel>(this));
		}

		const EResourceDetail DetailType = ResourceDetails[Index];
		const int32 DetailAmount = URtsResources::GetAmount(InResource, DetailType, InContext);

		UResourceDetailViewModel* Entry = DetailCache[Index];
		Entry->SetSource(InResource, DetailType, DetailAmount, URtsResources::FormatAmount(InResource, DetailAmount));
		NewDetails.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Details, NewDetails);
	UE_MVVM_SET_PROPERTY_VALUE(DetailCount, NewDetails.Num());
}
