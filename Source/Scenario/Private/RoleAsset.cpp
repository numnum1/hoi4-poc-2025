// Fill out your copyright notice in the Description page of Project Settings.


#include "RoleAsset.h"

URoleAsset::URoleAsset()
{
	RoleName = FText::FromString("Unnamed Role");
	Permissions = 0;
	bIsLeadership = false;
}
