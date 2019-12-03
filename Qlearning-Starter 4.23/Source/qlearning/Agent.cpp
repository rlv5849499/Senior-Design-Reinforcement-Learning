// Fill out your copyright notice in the Description page of Project Settings.

#include "Agent.h"
#include "Engine/GameEngine.h"
#define Debug(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, x);}

// Example using Debug for showing a variable
// int a = 1;
// Debug(FString::Printf(TEXT("Some int variable: %i"), a));
// float f = 1.0;
// Debug(FString::Printf(TEXT("Some float variable: %f"), f));


// Sets default values
AAgent::AAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

}

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();

	FNestedArray Array;
	Array.Action.Add(0.5);
	Array.Action.Add(0.5);

	QTable.Add(Array);
	QTable.Add(Array);

	previousY = MeshComp->GetRelativeTransform().GetLocation().Y;

}
int  i = 0;
// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	i++;
	if (i == 50) {	//Making Q learning process happen every 8 ticks just so it is easier to see the movement
		int use;
		float r;

		use = SelectAction();
		PerformSelectedAction(use);
		r = obtainReward();
		UpdateQ(r, use);
		UpdateState();
		i = 0;
	}
	

}

// Select action a based on Greedy policy,
// Choose action with highest Q(s, a) value in the current state s
// To get Q(s,a), use QTable[s].Action[a]
// current state is the variable State
int32 AAgent::SelectAction()
{//"select action a based on policy π "
	
    Debug(FString::Printf(TEXT("Selecting Action")));
	int s = State;	//current state

	if (QTable[s].Action[0] > QTable[s].Action[1])// if Q(s, a) for action 0 is greate than Q(s, a) for action 1
	{
		return 0;	//Action == 0
	}
	else //( if Q(s, a) for action 1 is greater than Q(s, a) for action 0
	{
		return 1;	//Action == 1
	}

	return -1;

}

// Perform Action a0 or a1 
// The result of each action is set from the problem itself
// and outside of the control of the learning algorithm
void AAgent::PerformSelectedAction(int32 Action)
{//"perform action a"

	Debug(FString::Printf(TEXT("Performing Action")));
	if (Action == 0)
	{
		if (State == 0)
		{
			FVector loc = MeshComp->GetRelativeTransform().GetLocation();
			loc.Y += 20;
			MeshComp->SetRelativeLocation(loc);
		}
		else if (State == 1)
		{
			FVector loc = MeshComp->GetRelativeTransform().GetLocation();
			loc.Y -= 20;
			MeshComp->SetRelativeLocation(loc);
		}

		State_p = 1;	//"observe new state s'"
		FVector scale = MeshComp->GetRelativeTransform().GetScale3D();
		scale.Y = 0.5;
		scale.Z = 2;
		MeshComp->SetRelativeScale3D(scale);

	}
	else if (Action == 1)
	{
		if (State == 0)
		{
			FVector loc = MeshComp->GetRelativeTransform().GetLocation();
			loc.Y -= 20;
			MeshComp->SetRelativeLocation(loc);
		}
		else if (State == 1)
		{
			FVector loc = MeshComp->GetRelativeTransform().GetLocation();
			loc.Y += 20;
			MeshComp->SetRelativeLocation(loc);
		}

		State_p = 0;	//"observe new state s'"
		FVector scale = MeshComp->GetRelativeTransform().GetScale3D();
		scale.Y = 1.5;
		scale.Z = 1;
		MeshComp->SetRelativeScale3D(scale);

	}
}

// Calcualte the reward r
// The reward should be how far the AI has moved forward, in Y-direction
// The further it moves, the larger the reward
// a variable that tracks the previous Y-coordinate is available: previousY
float AAgent::obtainReward()
{//"receive reward r"
	Debug(FString::Printf(TEXT("Obtaining Rewards")));
	float reward = 0;

	FVector loc = MeshComp->GetRelativeTransform().GetLocation();//Gets current location
	reward = loc.Y - previousY; //get current val - previous val

	/*This was just being used for debugging purposes*/
	//float f = reward;
	//Debug(FString::Printf(TEXT("Some REWARD: %f"), f));
	//float V = loc.Y;
	//Debug(FString::Printf(TEXT("Some LOC.Y: %f"), V));
	//float sa = previousY;
	//Debug(FString::Printf(TEXT("Some PREVIOUS Y: %f"), sa));


	return reward;
}

// Update Q value of the current State and action
void AAgent::UpdateQ(float reward, int32 action)
{//"update state-action value: Q(s,a) = r"
	Debug(FString::Printf(TEXT("Upadating Q values")));
	int s = State;
	int a = action;
	QTable[s].Action[a] = reward;

}


//  Set State to be the next state, State_p
void AAgent::UpdateState()
{	//"update state-action value: s = s' "
	Debug(FString::Printf(TEXT("Updating states")));
	State = State_p;
}