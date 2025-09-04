#include "UI/ChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/ChatPlayerController.h"

void UChatInput::NativeConstruct()
{
	Super::NativeConstruct();
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this,&ThisClass::OnChatInputTextCommitted)== false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this,&ThisClass::OnChatInputTextCommitted);
	}
}

void UChatInput::NativeDestruct()
{
	Super::NativeDestruct();
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this,&ThisClass::OnChatInputTextCommitted)== true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this,&ThisClass::OnChatInputTextCommitted);
	}
}

void UChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController)==true)
		{
			AChatPlayerController* OwningChatPlayerController = Cast<AChatPlayerController>(OwningPlayerController);
			if (IsValid(OwningChatPlayerController)==true)
			{
				OwningChatPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
