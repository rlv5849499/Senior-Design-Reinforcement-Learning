// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OxCharacter.generated.h"

UCLASS()
class MONKEYANDTHEOX_API AOxCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOxCharacter();

	//Numbers must be between 0 and 1
	UPROPERTY(EditAnywhere)
		float Epsilon;

	UPROPERTY(EditAnywhere)
		float LearningRate;  //Alpha

	UPROPERTY(EditAnywhere)
		float DiscountFactor; //Gamma

	
	TArray<FString, FDefaultAllocator> saveLoadAndRemoveSlashes();

	//Number of States and Actions
	UPROPERTY(EditAnywhere)
		int States;

	UPROPERTY(EditAnywhere)
		int Actions;

	float** ary;
	int actionSARSA;
	TArray<int> QValsOfState;
	TArray<int> QValsOfStatePrime;
	TArray<int> featuresOfState;
	FString logFilePath;



	//Callable Functions inside of the Blueprints
	UFUNCTION(BlueprintCallable)
		void CreateQMatrix();
	UFUNCTION(BlueprintCallable)
		int SARSAUpdateRL(TArray<bool> newObsState, float reward);
	UFUNCTION(BlueprintCallable)
		int startOfSARSA(TArray<bool> curState, int curA);

	UFUNCTION(BlueprintCallable)
		void setLogFileLocation(FString logLocation);

	UFUNCTION(BlueprintCallable)
		TArray<bool> UpdateRL(TArray<bool> nextState, float reward, int currentA);

	UFUNCTION(BlueprintCallable)
		int SelectAction(TArray<bool> curStateBool);

	/*UFUNCTION(BlueprintCallable)
		void LoadQMatrix();*/
	UFUNCTION(BlueprintCallable)
		float calculateReward(float rewardVal);

	UFUNCTION(BlueprintCallable)
		TArray<int> setState(TArray<bool> features);

	UFUNCTION(BlueprintCallable)
		TArray<bool> setStateToNextState(TArray<int> binVals);

	UPROPERTY(EditAnywhere)
		int CurrentS;

	UPROPERTY(EditAnywhere)
		int CurrentA;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
