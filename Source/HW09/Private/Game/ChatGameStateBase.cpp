#include "Game/ChatGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/ChatPlayerController.h"
#include "Net/UnrealNetwork.h"

void AChatGameStateBase::MulticastRPCLoginMessage_Implementation(const FString& Message)
{
	if (HasAuthority()==false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
		if (IsValid(PC)==true)
		{
			AChatPlayerController* ChatPC = Cast<AChatPlayerController>(PC);
			if (IsValid(ChatPC)==true)
			{
				FString NotificationString = Message + TEXT(" has joined the game");
				ChatPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

void AChatGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass,RemainingTurnTime);
}
