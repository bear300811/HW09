#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatPlayerController.generated.h"

class UChatInput;
class UUserWidget;

UCLASS()
class HW09_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AChatPlayerController();
	
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& Message);

	void PrintChatMessageString(const FString& Message);

	UFUNCTION(Client,Reliable)
	void ClientRPCPrintChatMessageString(const FString& Message);
	
	UFUNCTION(Server,Reliable)
	void ServerRPCPrintChatMessageString(const FString& Message);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated,BlueprintReadOnly)
	FText NotificationText;

	UPROPERTY(Replicated,BlueprintReadOnly)
	FText PlayCountText;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatInput> ChatInputWidgetClass;
	UPROPERTY()
	TObjectPtr<UChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayCountTextWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> PlayCountTextWidgetInstance;
	
};
