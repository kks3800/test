#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

class SPropHausGLTFExporterWidgetHelpers
{
public:
	static TSharedRef<STextBlock> MakeHeader(const FString& Text);
	static TSharedRef<STextBlock> MakeText(const FString& Text);
	static TSharedRef<SButton> MakeButton(SCompoundWidget* Owner, const FString& Text, FReply(SCompoundWidget::*Handler)());
	static TSharedRef<SVerticalBox> MakePathSetting(SCompoundWidget* Owner, const FMargin& Padding, const FString& Label, TSharedPtr<SEditableTextBox>& TextBox, const FString& ButtonLabel, FReply(SCompoundWidget::*Handler)());
	static TSharedRef<SCheckBox> MakeCheckBox(SCompoundWidget* Owner, const FString& Label, TFunction<bool()> IsCheckedLambda, TFunction<void(ECheckBoxState)> OnCheckStateChangedLambda);
};
