#include "PropHausGLTFExporterStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"

TSharedPtr<FSlateStyleSet> FPropHausGLTFExporterStyle::StyleInstance = nullptr;

void FPropHausGLTFExporterStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void FPropHausGLTFExporterStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
    ensure(StyleInstance.IsUnique());
    StyleInstance.Reset();
}

void FPropHausGLTFExporterStyle::ReloadTextures()
{
    FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FPropHausGLTFExporterStyle::Get()
{
    return *StyleInstance;
}

FName FPropHausGLTFExporterStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("PropHausGLTFExporterStyle"));
    return StyleSetName;
}

TSharedRef<FSlateStyleSet> FPropHausGLTFExporterStyle::Create()
{
    TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(
        FPropHausGLTFExporterStyle::GetStyleSetName(), 
        "/Game/UI/Styles", 
        "/Game/UI/Styles"
    );
    return StyleRef;
}
