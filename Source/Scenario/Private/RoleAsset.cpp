// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.


#include "RoleAsset.h"

URoleAsset::URoleAsset()
{
	RoleName = FText::FromString("Unnamed Role");
	Permissions = 0;
	bIsLeadership = false;
}
