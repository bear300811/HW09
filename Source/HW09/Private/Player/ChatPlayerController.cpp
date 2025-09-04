#include "Player/ChatPlayerController.h"

#include "UI/ChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "HW09/HW09.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ChatGameMode.h"
#include "Player/ChatPlayerState.h"
#include "Net/UnrealNetwork.h"

AChatPlayerController::AChatPlayerController()
{
	bReplicates = true;
}

void AChatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass)==true)
	{
		ChatInputWidgetInstance = CreateWidget<UChatInput>(this,ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance)==true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass)==true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this,NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance)==true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(PlayCountTextWidgetClass)==true)
	{
		PlayCountTextWidgetInstance = CreateWidget<UUserWidget>(this,PlayCountTextWidgetClass);
		if (IsValid(PlayCountTextWidgetInstance)==true)
		{
			PlayCountTextWidgetInstance->AddToViewport();
		}
	}
	
}

void AChatPlayerController::SetChatMessageString(const FString& Message)
{
	ChatMessageString = Message;
	
	if (IsLocalController())
	{
		ServerRPCPrintChatMessageString(ChatMessageString); 
	}
}

void AChatPlayerController::PrintChatMessageString(const FString& Message)
{
	HW09FunctionLibrary::MyPrintString(this, Message, 10.f);
}

void AChatPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
	DOREPLIFETIME(ThisClass, PlayCountText);
}

void AChatPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& Message)
{
	PrintChatMessageString(Message);
}


void AChatPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& Message)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM)==true)
	{
		AChatGameMode* ChatGM = Cast<AChatGameMode>(GM);
		if (IsValid(ChatGM)==true)
		{
			ChatGM->PrintChatMessageString(this, Message);
		}
	}
}
