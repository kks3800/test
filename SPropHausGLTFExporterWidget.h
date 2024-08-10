#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "IDetailsView.h"
#include "Options/GLTFExportOptions.h"
#include "GLTFExportSettings.h"

class SEditableTextBox;
class STextBlock;

class SPropHausGLTFExporterWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPropHausGLTFExporterWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual ~SPropHausGLTFExporterWidget();

    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;

private:
    // UI Elements
    TSharedPtr<SEditableTextBox> ExportPathTextBox;
    TSharedPtr<SEditableTextBox> ConfigFolderPathTextBox;
    TSharedPtr<SEditableTextBox> ConfigFilenameTextBox;
    TSharedPtr<STextBlock> StatusText;
    TSharedPtr<SScrollBox> StaticMeshListContainer;
    TSharedPtr<SScrollBox> MaterialListContainer;
    TSharedPtr<SButton> ExportButton;

    // Export Settings
    TArray<FAssetData> StaticMeshList;
    TArray<FAssetData> MaterialList;
    TWeakObjectPtr<UGLTFExportOptions> ExportOptions;

    // New functionality for adding similar named materials
    bool bAutoAddSimilarMaterials;

    // Drag-and-drop state
    bool bIsDraggingStaticMesh = false;
    bool bIsDraggingMaterial = false;

    // UI Functions
    TSharedRef<SWidget> CreateStaticMeshList();
    TSharedRef<SWidget> CreateMaterialList();
    TSharedRef<SWidget> CreateAssetEntry(const FAssetData& AssetData, bool bIsStaticMesh);
    FReply OnAddAssetsButtonClicked();
    FReply OnRemoveStaticMeshClicked(FAssetData AssetData);
    FReply OnRemoveMaterialClicked(FAssetData AssetData);
    FReply OnExportButtonClicked();
    void ExportAssets();
    void UpdateExportButtonState();

    FReply OnSelectExportFolderClicked();
    FReply OnShowExportOptionsClicked();
    FReply OnSaveButtonClicked();
    FReply OnLoadButtonClicked();
    FReply OnClearListButtonClicked();
    FReply OnSelectConfigFolderClicked(); 


    // New functions for the "Auto-Add Similar Named Materials" feature
    void OnAddStaticMeshToExportList(UStaticMesh* StaticMesh); 
    FString ExtractCoreMaterialName(const FString& MaterialName);
    void FindSimilarNamedMaterials(const FString& CoreName, const FString& FolderPath, TArray<FAssetData>& OutMaterials);
    void AddMaterialToExportList(const FAssetData& MaterialAsset);

    // Config Handling
    void SaveConfig();
    void LoadConfigFromFile(const FString& ConfigPath);

    // Functions to get the border color of containers
    FSlateColor GetStaticMeshContainerBorderColor() const;
    FSlateColor GetMaterialContainerBorderColor() const;
};
