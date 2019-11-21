// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Agent.generated.h"


USTRUCT()
struct FNestedArray {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> Action;
};


UCLASS()
class QLEARNING_API AAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FNestedArray> QTable;
	//Access like so
	//QTable[s0].Action[a0];

	// the current state
	int32 State;
	// the next state State_prime
	int32 State_p;

	float previousY;

	UFUNCTION(BlueprintCallable)
	int32 SelectAction();

	UFUNCTION(BlueprintCallable)
	void PerformSelectedAction(int32 Action);

	UFUNCTION(BlueprintCallable)
	float obtainReward();

	UFUNCTION(BlueprintCallable)
    void UpdateQ(float reward, int32 action);

	UFUNCTION(BlueprintCallable)
	void UpdateState();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent *MeshComp;
};
