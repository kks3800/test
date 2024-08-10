#include "SPropHausGLTFExporterWidgetHelpers.h"


TSharedRef<STextBlock> SPropHausGLTFExporterWidgetHelpers::MakeHeader(const FString& Text)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Text))
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16));
}

TSharedRef<STextBlock> SPropHausGLTFExporterWidgetHelpers::MakeText(const FString& Text)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Text));
}

TSharedRef<SButton> SPropHausGLTFExporterWidgetHelpers::MakeButton(SCompoundWidget* Owner, const FString& Text, FReply(SCompoundWidget::*Handler)())
{
	return SNew(SButton)
		.Text(FText::FromString(Text))
		.OnClicked(Owner, Handler);
}

TSharedRef<SVerticalBox> SPropHausGLTFExporterWidgetHelpers::MakePathSetting(SCompoundWidget* Owner, const FMargin& Padding, const FString& Label, TSharedPtr<SEditableTextBox>& TextBox, const FString& ButtonLabel, FReply(SCompoundWidget::*Handler)())
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[MakeText(Label)]
		+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[SAssignNew(TextBox, SEditableTextBox)]
		+ SVerticalBox::Slot().AutoHeight().Padding(Padding)[MakeButton(Owner, ButtonLabel, Handler)];
}

TSharedRef<SCheckBox> SPropHausGLTFExporterWidgetHelpers::MakeCheckBox(SCompoundWidget* Owner, const FString& Label, TFunction<bool()> IsCheckedLambda, TFunction<void(ECheckBoxState)> OnCheckStateChangedLambda)
{
	return SNew(SCheckBox)
		.IsChecked_Lambda([IsCheckedLambda]() { return IsCheckedLambda() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
		.OnCheckStateChanged_Lambda([OnCheckStateChangedLambda](ECheckBoxState NewState) { OnCheckStateChangedLambda(NewState); })
		.Content()[MakeText(Label)];
}
