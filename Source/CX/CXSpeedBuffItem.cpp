// CXSpeedBuffItem.cpp

#include "CXSpeedBuffItem.h"
#include "CXPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACXSpeedBuffItem::ACXSpeedBuffItem()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/StarterContent/Materials/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse"));
	if (MaterialFinder.Succeeded())
	{
		Mesh->SetMaterial(0, MaterialFinder.Object);
	}

	// cone pointing forward like an arrow
	Mesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	Mesh->SetRelativeScale3D(FVector(0.8f));
}

void ACXSpeedBuffItem::ApplyItemEffect(ACXPlayerState* TargetPlayerState)
{
	TargetPlayerState->AddBonusMaxSpeed(BonusMaxSpeedAmount);
}
