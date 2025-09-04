#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChatGameStateBase.generated.h"


UCLASS()
class HW09_API AChatGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCLoginMessage(const FString& Message = FString(TEXT("XXXXXX")));

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingTurnTime = 0;
};
