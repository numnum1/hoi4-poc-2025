#pragma once

#include "CoreMinimal.h"

/**
 * The globe's geometry, as free functions: a point on the sphere is a UV in the map textures and
 * back. Kept out of ARtsWorldMap so anything that has to reason about where a province sits — the
 * label layout, for one — can do so without an actor, and without a radius it has no business
 * knowing.
 */
namespace MapProjection
{
	/** Where a point on the globe's surface lands in map UV space. Independent of the radius: the
	 * point is normalized first. */
	inline FVector2D VectorToUV(const FVector& InPoint)
	{
		const FVector Normal = InPoint.GetSafeNormal();
		return FVector2D(
			FMath::Atan2(Normal.X, Normal.Y) / (2.0f * PI) + 0.5f,
			FMath::Asin(-Normal.Z) / PI + 0.5f);
	}

	/** The inverse of VectorToUV, on a sphere of InRadius, relative to the sphere's centre. */
	inline FVector UVToLocation(const FVector2D& InUV, const float InRadius)
	{
		const float Theta = InUV.X * 2.0f * PI;
		const float Phi = InUV.Y * PI;
		return FVector(
			-InRadius * FMath::Sin(Phi) * FMath::Sin(Theta),
			-InRadius * FMath::Sin(Phi) * FMath::Cos(Theta),
			InRadius * FMath::Cos(Phi));
	}
}
