#include "PropHausGLTFExporter.h"
#include "PropHausGLTFExporterStyle.h"
#include "PropHausGLTFExporterCommands.h"
#include "SPropHausGLTFExporterWidget.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FPropHausGLTFExporterModule"

const FName FPropHausGLTFExporterModule::PropHausGLTFExporterTabName(TEXT("PropHausGLTFExporter"));

void FPropHausGLTFExporterModule::StartupModule()
{
    // Initialize style and commands
    FPropHausGLTFExporterStyle::Initialize();
    FPropHausGLTFExporterStyle::ReloadTextures();
    FPropHausGLTFExporterCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FPropHausGLTFExporterCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FPropHausGLTFExporterModule::PluginButtonClicked),
        FCanExecuteAction());

    // Register the tab spawner
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        PropHausGLTFExporterTabName,
        FOnSpawnTab::CreateRaw(this, &FPropHausGLTFExporterModule::OnSpawnPluginTab)
    ).SetDisplayName(LOCTEXT("FPropHausGLTFExporterTabTitle", "PropHausGLTFExporter"))
    .SetMenuType(ETabSpawnerMenuType::Hidden);

    // Extend the menu and toolbar
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    {
        TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
        MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FPropHausGLTFExporterModule::AddMenuExtension));
        LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
    }

    {
        TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender());
        ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FPropHausGLTFExporterModule::AddToolbarExtension));
        LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
    }
}

void FPropHausGLTFExporterModule::ShutdownModule()
{
    // Unregister style and tab spawner
    FPropHausGLTFExporterStyle::Shutdown();
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PropHausGLTFExporterTabName);
}

void FPropHausGLTFExporterModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(PropHausGLTFExporterTabName);
}

TSharedRef<SDockTab> FPropHausGLTFExporterModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SPropHausGLTFExporterWidget)
        ];
}

void FPropHausGLTFExporterModule::AddMenuExtension(FMenuBuilder& Builder)
{
    Builder.AddMenuEntry(FPropHausGLTFExporterCommands::Get().OpenPluginWindow);
}

void FPropHausGLTFExporterModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FPropHausGLTFExporterCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPropHausGLTFExporterModule, PropHausGLTFExporter)
