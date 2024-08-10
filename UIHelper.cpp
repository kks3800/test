#include "UIHelper.h"

TSharedRef<STextBlock> UIHelper::CreateHeader(const FString& Text)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Text))
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16));
}

TSharedRef<STextBlock> UIHelper::CreateText(const FString& Text)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Text));
}
