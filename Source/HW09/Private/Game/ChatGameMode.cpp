#include "Game/ChatGameMode.h"

#include "Game/ChatGameStateBase.h"
#include "Player/ChatPlayerController.h"
#include "EngineUtils.h"
#include "Player/ChatPlayerState.h"

void AChatGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(NewPlayer);
	if (IsValid(ChatPlayerController)==true)
	{
		ChatPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(ChatPlayerController);

		AChatPlayerState* ChatPS = ChatPlayerController->GetPlayerState<AChatPlayerState>();
		if (IsValid(ChatPS)==true)
		{
			ChatPS->PlayerNameString =TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AChatGameStateBase* ChatGameStateBase = GetGameState<AChatGameStateBase>();
		if (IsValid(ChatGameStateBase)==true)
		{
			ChatGameStateBase->MulticastRPCLoginMessage(ChatPS->PlayerNameString);
		}
	}
}

FString AChatGameMode::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num){return Num > 0;});

	FString Result;
	for (int32 i =0;i<3;++i)
	{
		int32 Index = FMath::RandRange(0,Numbers.Num()-1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	
	return Result;
}

bool AChatGameMode::IsGuessNumberString(const FString& Number)
{
	bool bCanPlay = false;

	do
	{
		if (Number.Len() != 3)
		{
			break;
		}
		bool bIsUnique = true;
		TSet<TCHAR> UniqueNumbers;
		for (TCHAR C : Number)
		{
			if (FChar::IsDigit(C)==false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueNumbers.Add(C);
		}
		if (bIsUnique==false)
		{
			break;
		}
		bCanPlay = true;
	}while (false);
	
	return bCanPlay;
}

FString AChatGameMode::JudgeResult(const FString& InSecretNumberString, const FString& GuessNumber)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i]==GuessNumber[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"),GuessNumber[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT!");
	}

	return FString::Printf(TEXT("%dS/%dB"), StrikeCount, BallCount);
}

void AChatGameMode::BeginPlay()
{
	Super::BeginPlay();

	SecretNumber = GenerateSecretNumber();
}

void AChatGameMode::PrintChatMessageString(AChatPlayerController* InChattingPlayerController, const FString& Message)
{
	int Index = Message.Len() -3;
	FString GuessNumber = Message.RightChop(Index);
	if (IsGuessNumberString(GuessNumber)==true)
	{
		if (!bTurnActive) StartTurn();
		FString JudgeResultString = JudgeResult(SecretNumber, GuessNumber);
		IncreaseGuessCount(InChattingPlayerController);
		FString Prefix;
		if (AChatPlayerState* PS = InChattingPlayerController->GetPlayerState<AChatPlayerState>())
		{
			Prefix = PS->GetPlayerInfoString() + TEXT(": "); // +1 반영된 최신값
		}
		const FString Combined = Prefix + Message + TEXT("->") + JudgeResultString;
		for (TActorIterator<AChatPlayerController> It(GetWorld());It;++It)
		{
			AChatPlayerController* PlayerController = *It;
			if (IsValid(PlayerController)==true)
			{
				PlayerController->ClientRPCPrintChatMessageString(Combined);
			}
		}
		int32 StrikeCount = 0;
		if (JudgeResultString.Len() >= 1 && FChar::IsDigit(JudgeResultString[0]))
		{
			StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		}
		JudgeGame(InChattingPlayerController, StrikeCount);

		if (StrikeCount < 3)
		{
			bTurnActive = false;
			GetWorld()->GetTimerManager().ClearTimer(TurnCountHandle);
			if (AllPlayerControllers.Num() > 0)
			{
				CurrentTurnIndex = (CurrentTurnIndex + 1) % AllPlayerControllers.Num();
			}
			StartTurn();
		}
		else
		{
			bTurnActive = false;
			GetWorld()->GetTimerManager().ClearTimer(TurnCountHandle);
		}
	}
	else
	{
		FString Prefix;
		if (AChatPlayerState* PS = InChattingPlayerController->GetPlayerState<AChatPlayerState>())
		{
			Prefix = PS->GetPlayerInfoString() + TEXT(": ");
		}

		const FString Combined = Prefix + Message;
		
		for (TActorIterator<AChatPlayerController> It(GetWorld());It;++It)
		{
			AChatPlayerController* PlayerController = *It;
			if (IsValid(PlayerController)==true)
			{
				PlayerController->ClientRPCPrintChatMessageString(Combined);
			}
		}
	}
}

void AChatGameMode::IncreaseGuessCount(AChatPlayerController* InChattingPlayerController)
{
	AChatPlayerState* ChatPS = InChattingPlayerController->GetPlayerState<AChatPlayerState>();
	if (IsValid(ChatPS)==true)
	{
		ChatPS->CurrentGuessCount++;
	}
}

void AChatGameMode::ResetGame()
{
	SecretNumber = GenerateSecretNumber();
	GetWorld()->GetTimerManager().ClearTimer(TurnCountHandle);
	bTurnActive = false;
	CurrentTurnIndex = 0;
	if (auto* GS = GetGameState<AChatGameStateBase>())
	{
		GS->RemainingTurnTime = 0;
	}

	for (const auto& ChatPlyerController : AllPlayerControllers)
	{
		AChatPlayerState* ChatPS = ChatPlyerController->GetPlayerState<AChatPlayerState>();
		if (IsValid(ChatPS)==true)
		{
			ChatPS->CurrentGuessCount = 0;
		}
	}
}

void AChatGameMode::JudgeGame(AChatPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3==InStrikeCount)
	{
		AChatPlayerState* ChatPS = InChattingPlayerController->GetPlayerState<AChatPlayerState>();
		for (const auto& ChatPlayerController : AllPlayerControllers)
		{
			if (IsValid(ChatPS)==true)
			{
				FString Combined = ChatPS->PlayerNameString + TEXT(" has won the game.");
				ChatPlayerController->NotificationText = FText::FromString(Combined);

				ResetGame();
			}
		}
	}
	else
	{
		bool IsDraw = true;
		for (const auto& ChatPlayerController : AllPlayerControllers)
		{
			AChatPlayerState* ChatPS = ChatPlayerController->GetPlayerState<AChatPlayerState>();
			if (IsValid(ChatPS)==true)
			{
				if (ChatPS->CurrentGuessCount < ChatPS->MaxGuessCount)
				{
					IsDraw = false;
					break;
				}
			}
		}
		if (IsDraw == true)
		{
			for (const auto& ChatPlayerController : AllPlayerControllers)
			{
				ChatPlayerController->NotificationText = FText::FromString(TEXT("Draw...."));

				ResetGame();
			}
		}
	}

	
}

void AChatGameMode::StartTurn()
{
	if (bTurnActive) return;
	bTurnActive = true;
	
	AChatGameStateBase* ChatPS = GetGameState<AChatGameStateBase>();
	if (IsValid(ChatPS)==true)
	{
		ChatPS->RemainingTurnTime = TurnDurationSeconds;
	}

	GetWorld()->GetTimerManager().ClearTimer(TurnCountHandle);
	GetWorld()->GetTimerManager().SetTimer(
		TurnCountHandle,
		this,
		&AChatGameMode::HandleTurnTick,
		1.0f,
		true);
}

void AChatGameMode::HandleTurnTick()
{
	AChatGameStateBase* ChatPS = GetGameState<AChatGameStateBase>();
	if (IsValid(ChatPS)==true)
	{
		ChatPS->RemainingTurnTime = FMath::Max(0,ChatPS->RemainingTurnTime - 1);
		const FString TimeStr = FString::FromInt(ChatPS->RemainingTurnTime);
		for (auto& PC : AllPlayerControllers)
		{
			if (IsValid(PC)==true)
			{
				PC->PlayCountText = FText::FromString(TimeStr);
			}
		}
		
		if (ChatPS->RemainingTurnTime == 0)
		{
			EndTurn();
		}
	}
}

void AChatGameMode::EndTurn()
{
	bTurnActive = false;
	GetWorld()->GetTimerManager().ClearTimer(TurnCountHandle);

	AChatPlayerController* CurrentPC =
		AllPlayerControllers.IsValidIndex(CurrentTurnIndex) ? AllPlayerControllers[CurrentTurnIndex] : nullptr;

	if (IsValid(CurrentPC)==true)
	{
		IncreaseGuessCount(CurrentPC);
		JudgeGame(CurrentPC, 0);
		for (auto& PC : AllPlayerControllers)
		{
			if (IsValid(PC)==true)
			{
				PC->NotificationText.FText::FromString(TEXT("Time out! Count+1"));
			}
		}
	}
	if (AllPlayerControllers.Num() > 0)
	{
		CurrentTurnIndex = (CurrentTurnIndex + 1) % AllPlayerControllers.Num();
	}
	StartTurn();
}



