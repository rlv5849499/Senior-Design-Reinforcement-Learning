// Fill out your copyright notice in the Description page of Project Settings.


#include "OxCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/GameEngine.h"
#include "Misc/FileHelper.h"
#include <Runtime\Core\Public\HAL\PlatformFilemanager.h>
#include "HAL/FileManager.h"
#include "Misc/DefaultValueHelper.h"
#include "Containers/UnrealString.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Containers/UnrealString.h"
#include "Math/RandomStream.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;


#define Debug(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, x);}

// Sets default values
AOxCharacter::AOxCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOxCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//TArray<bool> into a TArray<int>
TArray<int> AOxCharacter::setState(TArray<bool> features) {
	int size = features.Num();
	TArray<int> stateArrOfInt;
	for (int i = 0; i < size; i++) {
		int temp = (int)features[i];
		stateArrOfInt.Add(temp);
	}
	return stateArrOfInt;
}

//TArray<int> into a TArray<bool>
TArray<bool> AOxCharacter::setStateToNextState(TArray<int> binVals) {
	int size = binVals.Num();
	TArray<bool> boolFeatureArr;
	for (int i = 0; i < size; i++) {
		if (binVals[i] == 1)
			boolFeatureArr.Add(true);
		else if (binVals[i] == 0)
			boolFeatureArr.Add(false);
	}
	return boolFeatureArr;
}

/*
int convertTArrayToInt(TArray<int> conArr) { //may delete
	FString conStr;
	for (int i = 0; i < conArr.Num(); i++)
	{
		conStr.AppendInt(conArr.Pop());	//loops through the array to get the values into a string
	}
	std::string MyStdString(TCHAR_TO_UTF8(*conStr));

	int parsedBin = stoi(MyStdString, nullptr, 2);


	return parsedBin;
}

*/

//TArray<int> into a TArray<bool>
void AOxCharacter::setLogFileLocation(FString logLocation){

	logFilePath = logLocation;
}



//Creates a QMatrix and file if that doesn't exist 
void AOxCharacter::CreateQMatrix()
{

	FString TextToSave = FString("\n");
	FString space = FString("/");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	float value;
	int N = States;		//Row
	int M = Actions;	//column

	// dynamic allocation
	ary = new float* [N];
	for (int i = 0; i < N; i++)
		ary[i] = new float[M];

	// fill and initilize weights randomly
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			float generatingFloats = FMath::RandRange(0.1, 5);
			ary[i][j] = generatingFloats;
		}
	}
		

	//printing values
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			value = ary[i][j];
			
			FString newString = FString::SanitizeFloat(value);
			//Debug(newString);

			FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
			FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}
		//FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}
}


//Select Action is where Q(s,a) vals are calculated and an action is chosen based off of epsilon greedy policy
int AOxCharacter::SelectAction(TArray<bool> curStateBool) { //Observing the current state using TArray
	int selectedAction = 0;
	TArray<int> currentState = setState(curStateBool);

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//***********************************************************************************

	

	FString temp = "";
	int N = States;		//Row
	int M = Actions;	//column
	int counter = 0;

	featuresOfState = currentState;
	int selectA = 0;

	// dynamic allocation
	ary = new float* [N];
	for (int i = 0; i < N; i++)
		ary[i] = new float[M];

	//Read values of Array into ary.
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			temp = ArrayVals[counter];
			float newFloat = FCString::Atod(*temp);
			ary[i][j] = newFloat;
			counter++;
		}
	}
	QValsOfState.SetNum(N);
	QValsOfStatePrime.SetNum(N);
	for (int i = 0; i < Actions; i++) //calculate Q(current state, all actions)
	{
		int Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int j = 0; j < N; j++) //looping through all the state features
		{
			//if the feature is active (=1), it's value is accumulated in the Q value calculation for this action
			if (currentState[j] == 1) {
				Qsum += ary[j][i]; //i = action; j = the current state feature
			}
			
		}
		QValsOfState[i] = Qsum; //each action gets its calculated QValue put into an array
	}
	

	//generate random number
	int randomNum = FMath::RandRange(0, 100);
	float randomNumber = randomNum / 100;


	if (randomNumber > Epsilon) 
	{ //select the best action

		  
		for (int j = 0; j < Actions; j++)//loops through all actions
		{			

			//For all the QVals, go through to find the best (highest) value
			if (QValsOfState[j] > QValsOfState[selectA])//
			{										
				selectA = j;
			}
		}
	
	}
	else
	{
		//do exploration. Select a random action.   Reduce epsilon as agent becomes more confident selecting values.  
		int randomAction = FMath::RandRange(0, Actions-1);
		selectA = randomAction;
		Epsilon = Epsilon - 0.01;
	} 
	return selectA; //selectA = the action choice as determined above
}

//Can be reworked better later
float AOxCharacter::calculateReward(float rewardVal)
{//"receive reward r"

	float reward = 0.0;	//init reward float
	
	if (rewardVal == 10.0) //The reward is good
	{
		reward = 10.0;
	}

	else if(rewardVal == -10.0) //The reward is bad
	{
		reward = -10.0;
	}

	return reward;
}


TArray<bool> AOxCharacter::UpdateRL(TArray<bool> nextState, float reward, int currentA) {

	TArray<int> statePrime = setState(nextState);
	int N = States;		//Row 
	int M = Actions;			//column
	int counter = 0;
	int maxQNext = 0;


	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();


	//Dynamically allocating Ary again
	ary = new float*[N];
	for (int i = 0; i < N; i++)
		ary[i] = new float[M];


	//Read values of Array into ary.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {

			FString temp = ArrayVals[counter];
			float MyShinyNewInt = FCString::Atod(*temp);
			ary[i][j] = MyShinyNewInt;
			counter++;

		}
	}

	//formats the next state information into an int array 
	for (int i = 0; i < Actions; i++) //calculate Q(statePrime features, all actions)
	{
		int Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int j = 0; j < N; j++) //looping through all the state features
		{
			//if the feature is active (=1), it's value is accumulated in the Q value calculation for this action
			if (statePrime[j] == 1) {
				Qsum += ary[j][i]; //i = action; j = the feature of state prime
			}

		}
		if (Qsum > maxQNext)	//looks at the Qsum of each action, and the comparison ensures that maxQNext will be set appropriately
			maxQNext = Qsum;

		QValsOfStatePrime[i] = Qsum; //each action gets its calculated QValue put into an array
	}

	int QofSA = 0;
	for (int i = 0; i > QValsOfState.Num(); i++)
	{
		if (QValsOfState[i] > QofSA) //if the Qval @ i > than the current value for Q(s,a), Q(s,a) is set to that value
			QofSA = QValsOfState[i];
	}


	//update the weights in ary
	for (int w = 0; w < N; w++)
	{
		if (featuresOfState[w] == 1) //feature = 1, meaning that feature is active
		{	// weight = weight + learningRate( reward + discountFactor (maxQnext) - Q(s,a) )
			ary[w][currentA] = ary[w][currentA] + LearningRate*(reward + DiscountFactor*(maxQNext) - QofSA);
		}

	}

	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*logFilePath);
	FString space = FString("/");

	//write updated table to text file
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {

			float value = ary[i][j];

			FString newString = FString::SanitizeFloat(value);
			FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
			FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	
		}
	}
	
	//set current state to = next state (AKA state prime) and return to the top of the loop
	TArray<bool> nextS = setStateToNextState(featuresOfState);
	return nextS;
}


//Start of SARSA: gets random action and the current state for the 1st run of SARSA
int AOxCharacter::startOfSARSA(TArray<bool> curState, int curA) {
	
	int randomAction = FMath::RandRange(0, Actions - 1);
	actionSARSA = randomAction;

	TArray<int> curIntState = setState(curState); //turns the array of bool to ints
	featuresOfState = curIntState;	//featuresOfState is a global TArray <int>, function SARSAUpdateRL uses this value

	return randomAction;
}

int AOxCharacter::SARSAUpdateRL(TArray<bool> newObsState, float reward) { //SARSA all takes place in this one function, taking on the performed action's reward and the new state

	//1st run: the chosen action and current state gets taken care of in startOfSARSA function 
	//After that run: a global variable sets the current state in the last run of this 

	//actionSARSA has the performed action value
	//featuresOfState has the current state value
	
	//Observe new state s'   *******************************************************************************************************************
	TArray<int> nextIntState = setState(newObsState); //turns the array of bool to ints


	//Calc Q(s',a...) for all actions	********************************************************************************************************

	FString temp = "";
	int N = States;		//Row
	int M = Actions;	//column
	int counter = 0;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();


	// dynamic allocation
	ary = new float*[N];
	for (int i = 0; i < N; i++)
		ary[i] = new float[M];


	//Read values of Array into ary.
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			temp = ArrayVals[counter];
			float newFloat = FCString::Atod(*temp);
			ary[i][j] = newFloat;
			counter++;
		}
	}

	QValsOfState.SetNum(N);	// This is where the Q(s',a...) for all actions is stored
	QValsOfStatePrime.SetNum(N);

	for (int i = 0; i < Actions; i++) //calculate Q(state', all actions)
	{
		int Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int j = 0; j < N; j++) //looping through all the state features
		{
			//if the feature is active (=1), it's value is accumulated in the Q value calculation for this action
			if (nextIntState[j] == 1) {
				Qsum += ary[j][i]; //i = action being considered; j = the state feature being considered
			}

		}
		QValsOfState[i] = Qsum; //each action gets its calculated QValue put into an array
	}


	//Select next action a', based on s' and Epsilon Greedy    *********************************************************************************************

	int selectNextA = 0;
		
	int randomNum = FMath::RandRange(0, 100);	//Generate Random Number for epsilon-greedy policy
	float randomNumber = randomNum / 100;


	if (randomNumber > Epsilon)
	{ //select the best action

		for (int j = 0; j < Actions; j++)//loops through all actions
		{
			//For all the QVals, go through to find the best (highest) value
			if (QValsOfState[j] > QValsOfState[selectNextA])//
			{
				selectNextA = j;
			}
		}

	}
	else
	{
		//do exploration. Select a random action.   Reduce epsilon as agent becomes more confident selecting values.  
		int randomAction = FMath::RandRange(0, Actions - 1);
		selectNextA = randomAction;
		Epsilon = Epsilon - 0.01;
	}




	//Calculate the Q(s',a') and Q(s,a)    *****************************************************************************************************************
	int valueQandA = 0;
	int valueQandAPrime = 0;

	
	//**For Q(s,a)**  
	for (int i = 0; i < Actions; i++) //calculate Q(statePrime features, all actions)
	{
		int Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int j = 0; j < N; j++) //looping through all the state features
		{
			//if the feature is active (=1), it's value is accumulated in the Q value calculation for this action
			if (featuresOfState[j] == 1) {
				Qsum += ary[j][i]; //i = action; j = the feature of state
			}
		}
		QValsOfState[i] = Qsum; //each action gets its calculated QValue put into an array, still need these QValsOfState array so that weights can be updated 
	}

	valueQandA = QValsOfState[actionSARSA]; //the value in QVals array @ actionSARSA is the Q value used for weight calculations


	//**For Q(s',a')**  
	for (int i = 0; i < Actions; i++) //calculate Q(statePrime features, all actions)
	{
		int Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int j = 0; j < N; j++) //looping through all the state features
		{
			//if the feature is active (=1), it's value is accumulated in the Q value calculation for this action
			if (nextIntState[j] == 1) {
				Qsum += ary[j][i]; //i = action; j = the feature of state prime
			}
		}
		QValsOfStatePrime[i] = Qsum; //each action gets its calculated QValue put into an array, still need these QValsOfStatePrime array so that weights can be updated 
	}

	valueQandAPrime = QValsOfStatePrime[selectNextA]; //the value in QVals array @ selectNextA is the Q value used for weight calculations


	//	Update the weights with : w = w + alpha(r + delta(Q(s'a')) - Q(s, a))   ****************************************************************************

	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*logFilePath);
	FString space = FString("/");

	for (int w = 0; w < N; w++)
	{
		if (featuresOfState[w] == 1) //feature = 1, meaning that feature is active
		{	// weight = weight + learningRate( reward + discountFactor (maxQnext) - Q(s,a) )
			ary[w][actionSARSA] = ary[w][actionSARSA] + LearningRate * (reward + DiscountFactor * (valueQandAPrime) - valueQandA);
		}

	}

	//write updated table to text file
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {

			float value = ary[i][j];

			FString newString = FString::SanitizeFloat(value);
			FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
			FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

		}
	}


	//Set state = s'   *************************************************************************************************************************************
	featuresOfState = nextIntState;	//s = featuresOfState (a global variable); s' = nextIntState
	
	
	
	
	//Set action = a'   ************************************************************************************************************************************
	actionSARSA = selectNextA; //a = actionSARSA; a' = selectNextA

	
	return actionSARSA; //returns the action 

}



TArray<FString, FDefaultAllocator> AOxCharacter::saveLoadAndRemoveSlashes() {
	//for saving / loading files.  
	FString CompleteFilePath = logFilePath;
	FString FileData = "";
	//FString space = FString("/");
	FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

	//Remove Slashes
	TArray<FString, FDefaultAllocator> Array;
	FString MyString(FileData);
	MyString.ParseIntoArray(Array, TEXT("/"), true);

	return Array;
}