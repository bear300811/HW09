#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ChatPlayerState.generated.h"


UCLASS()
class HW09_API AChatPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AChatPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	FString GetPlayerInfoString();
	
};
