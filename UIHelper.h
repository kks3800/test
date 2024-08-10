#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

class UIHelper
{
public:
	static TSharedRef<STextBlock> CreateHeader(const FString& Text);
	static TSharedRef<STextBlock> CreateText(const FString& Text);

	template<typename WidgetType>
	static TSharedRef<SButton> CreateButton(WidgetType* Owner, const FString& Text, FReply(WidgetType::*Handler)())
	{
		return SNew(SButton)
			.Text(FText::FromString(Text))
			.OnClicked(Owner, Handler);
	}

	template<typename WidgetType>
	static TSharedRef<SVerticalBox> CreatePathSetting(WidgetType* Owner, const FMargin& Padding, const FString& Label, TSharedPtr<SEditableTextBox>& TextBox, const FString& ButtonLabel, FReply(WidgetType::*Handler)())
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[CreateText(Label)]
			+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[SAssignNew(TextBox, SEditableTextBox)]
			+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[CreateButton(Owner, ButtonLabel, Handler)];
	}

	template<typename WidgetType>
	static TSharedRef<SCheckBox> CreateCheckBox(WidgetType* Owner, const FString& Label, TFunction<bool()> IsChecked, TFunction<void(ECheckBoxState)> OnCheckStateChanged)
	{
		return SNew(SCheckBox)
			.IsChecked_Lambda([IsChecked]() { return IsChecked() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
			.OnCheckStateChanged_Lambda([OnCheckStateChanged](ECheckBoxState NewState) { OnCheckStateChanged(NewState); })
			.Content()[CreateText(Label)];
	}
};
