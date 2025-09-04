#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChatGameMode.generated.h"

class AChatPlayerController;

UCLASS()
class HW09_API AChatGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& Number);

	FString JudgeResult(const FString& SecretNumber, const FString& GuessNumber);

	virtual void BeginPlay() override;

	void PrintChatMessageString(AChatPlayerController* InChattingPlayerController, const FString& Message);

	void IncreaseGuessCount(AChatPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(AChatPlayerController* InChattingPlayerController,int InStrikeCount);

protected:
	FString SecretNumber;

	TArray<TObjectPtr<AChatPlayerController>> AllPlayerControllers;
#pragma region TurnCount
	FTimerHandle TurnCountHandle;

	int32 TurnDurationSeconds = 20;

	int32 CurrentTurnIndex = 0;

	bool bTurnActive = false;
	
	void StartTurn();
	void HandleTurnTick();
	void EndTurn();
#pragma endregion

	
};


