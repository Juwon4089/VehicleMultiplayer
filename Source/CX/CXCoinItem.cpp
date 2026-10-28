// CXCoinItem.cpp

#include "CXCoinItem.h"
#include "CXPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACXCoinItem::ACXCoinItem()
{
	// a flat golden cylinder standing on its edge
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold"));
	if (MaterialFinder.Succeeded())
	{
		Mesh->SetMaterial(0, MaterialFinder.Object);
	}

	Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.15f));
}

void ACXCoinItem::ApplyItemEffect(ACXPlayerState* TargetPlayerState)
{
	TargetPlayerState->AddCoins(CoinAmount);
}
