#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FPropHausGLTFExporterModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void AddMenuExtension(FMenuBuilder& Builder);
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void PluginButtonClicked();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	static const FName PropHausGLTFExporterTabName;
};
