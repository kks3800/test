#include "SPropHausGLTFExporterWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Options/GLTFExportOptions.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Misc/FileHelper.h"
#include "Exporters/GLTFExporter.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "DesktopPlatformModule.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "IDetailsView.h"
#include "GLTFExportSettings.h"
#include "JsonObjectConverter.h"
#include "UIHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DragAndDrop/AssetDragDropOp.h"

void SPropHausGLTFExporterWidget::Construct(const FArguments& InArgs)
{
    bIsDraggingStaticMesh = false;
    bIsDraggingMaterial = false;
    
    ExportOptions = NewObject<UGLTFExportOptions>();
    bAutoAddSimilarMaterials = false;

    const FMargin Pad(5);

    // Set the background and outline colors to subtle differences
    FSlateColor BackgroundColor = FSlateColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f)); // Dark gray
    FSlateColor OutlineColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f)); // Slightly lighter gray

    ChildSlot
    [
        SNew(SBorder)
        .Padding(Pad)
        .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
        .BorderBackgroundColor(OutlineColor) // Outer border outline
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()[UIHelper::CreateHeader("Path Settings")]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()[UIHelper::CreatePathSetting(this, Pad, "Export Path:", ExportPathTextBox, "Select Export Folder", &SPropHausGLTFExporterWidget::OnSelectExportFolderClicked)]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()[UIHelper::CreatePathSetting(this, Pad, "Config Folder:", ConfigFolderPathTextBox, "Select Config Folder", &SPropHausGLTFExporterWidget::OnSelectConfigFolderClicked)]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().Padding(Pad)[UIHelper::CreateText("Config Filename:")]
                + SVerticalBox::Slot().AutoHeight().Padding(Pad)[SAssignNew(ConfigFilenameTextBox, SEditableTextBox)]
            ]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()[UIHelper::CreateHeader("Actions")]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(2)[UIHelper::CreateButton(this, "Add Assets", &SPropHausGLTFExporterWidget::OnAddAssetsButtonClicked)]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)
                [
                    SAssignNew(ExportButton, SButton)
                    .Text(FText::FromString("Export"))
                    .OnClicked(this, &SPropHausGLTFExporterWidget::OnExportButtonClicked)
                    .IsEnabled(false)
                ]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)[UIHelper::CreateButton(this, "Export Options", &SPropHausGLTFExporterWidget::OnShowExportOptionsClicked)]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)[UIHelper::CreateButton(this, "Save", &SPropHausGLTFExporterWidget::OnSaveButtonClicked)]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)[UIHelper::CreateButton(this, "Load", &SPropHausGLTFExporterWidget::OnLoadButtonClicked)]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)[UIHelper::CreateButton(this, "Clear List", &SPropHausGLTFExporterWidget::OnClearListButtonClicked)]
                + SHorizontalBox::Slot().AutoWidth().Padding(2)
                [
                    UIHelper::CreateCheckBox(this, "Auto-Add Similar Named Materials",
                        [this]() { return bAutoAddSimilarMaterials; },
                        [this](ECheckBoxState NewState) { bAutoAddSimilarMaterials = (NewState == ECheckBoxState::Checked); })
                ]
            ]
            + SVerticalBox::Slot().Padding(Pad).FillHeight(1.0f)
            [
                SNew(SSplitter)
                + SSplitter::Slot()
                .Value(0.5f) // Initial split ratio
                [
                    SNew(SBorder)
                    .Padding(0)
                    .BorderBackgroundColor(this, &SPropHausGLTFExporterWidget::GetStaticMeshContainerBorderColor)
                    [
                        SNew(SBorder)
                        .Padding(1)
                        .BorderBackgroundColor(BackgroundColor) // Dark gray background
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot().AutoHeight()[UIHelper::CreateHeader("Static Meshes")]
                            + SVerticalBox::Slot().FillHeight(1.0f)[SAssignNew(StaticMeshListContainer, SScrollBox)]
                        ]
                    ]
                ]
                + SSplitter::Slot()
                .Value(0.5f) // Initial split ratio
                [
                    SNew(SBorder)
                    .Padding(0)
                    .BorderBackgroundColor(this, &SPropHausGLTFExporterWidget::GetMaterialContainerBorderColor)
                    [
                        SNew(SBorder)
                        .Padding(1)
                        .BorderBackgroundColor(BackgroundColor) // Dark gray background
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot().AutoHeight()[UIHelper::CreateHeader("Materials and Material Instances")]
                            + SVerticalBox::Slot().FillHeight(1.0f)[SAssignNew(MaterialListContainer, SScrollBox)]
                        ]
                    ]
                ]
            ]
            + SVerticalBox::Slot().Padding(Pad).AutoHeight()[SAssignNew(StatusText, STextBlock).Text(FText::FromString(""))]
        ]
    ];

    CreateStaticMeshList();
    CreateMaterialList();
}


SPropHausGLTFExporterWidget::~SPropHausGLTFExporterWidget()
{
    if (ExportOptions.IsValid())
    {
        ExportOptions->MarkAsGarbage();
    }
}

FReply SPropHausGLTFExporterWidget::OnExportButtonClicked()
{
    ExportAssets();
    return FReply::Handled();
}

TSharedRef<SWidget> SPropHausGLTFExporterWidget::CreateStaticMeshList()
{
    StaticMeshListContainer->ClearChildren();
    for (const FAssetData& AssetData : StaticMeshList)
    {
        StaticMeshListContainer->AddSlot()
        .Padding(2)
        [
            CreateAssetEntry(AssetData, true)
        ];
    }
    UpdateExportButtonState();
    return StaticMeshListContainer.ToSharedRef();
}

TSharedRef<SWidget> SPropHausGLTFExporterWidget::CreateMaterialList()
{
    MaterialListContainer->ClearChildren();
    for (const FAssetData& AssetData : MaterialList)
    {
        MaterialListContainer->AddSlot()
        .Padding(2)
        [
            CreateAssetEntry(AssetData, false)
        ];
    }
    UpdateExportButtonState();
    return MaterialListContainer.ToSharedRef();
}

TSharedRef<SWidget> SPropHausGLTFExporterWidget::CreateAssetEntry(const FAssetData& AssetData, bool bIsStaticMesh)
{
    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .FillWidth(1.0f)
    [
        SNew(STextBlock)
        .Text(FText::FromString(AssetData.AssetName.ToString()))
        .ColorAndOpacity(FLinearColor::Yellow)
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(SButton)
        .Text(FText::FromString("Remove"))
        .OnClicked(this, bIsStaticMesh ? &SPropHausGLTFExporterWidget::OnRemoveStaticMeshClicked : &SPropHausGLTFExporterWidget::OnRemoveMaterialClicked, AssetData)
    ];
}

FReply SPropHausGLTFExporterWidget::OnAddAssetsButtonClicked()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    IContentBrowserSingleton& ContentBrowser = ContentBrowserModule.Get();
    TArray<FAssetData> SelectedAssets;
    ContentBrowser.GetSelectedAssets(SelectedAssets);

    for (const FAssetData& AssetData : SelectedAssets)
    {
        if (!StaticMeshList.Contains(AssetData) && AssetData.AssetClassPath == UStaticMesh::StaticClass()->GetClassPathName())
        {
            StaticMeshList.Add(AssetData);
            UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset());
            if (StaticMesh && bAutoAddSimilarMaterials)
            {
                OnAddStaticMeshToExportList(StaticMesh);
            }
        }
        else if (!MaterialList.Contains(AssetData))
        {
            UObject* Asset = AssetData.GetAsset();
            if (Asset->IsA<UMaterial>() || Asset->IsA<UMaterialInstance>())
            {
                MaterialList.Add(AssetData);
            }
        }
    }

    CreateStaticMeshList();
    CreateMaterialList();
    return FReply::Handled();
}

void SPropHausGLTFExporterWidget::OnAddStaticMeshToExportList(UStaticMesh* StaticMesh)
{
    if (bAutoAddSimilarMaterials)
    {
        for (int32 MaterialIndex = 0; MaterialIndex < StaticMesh->GetStaticMaterials().Num(); ++MaterialIndex)
        {
            UMaterialInterface* MaterialInterface = StaticMesh->GetMaterial(MaterialIndex);
            if (MaterialInterface)
            {
                FString MaterialName = MaterialInterface->GetName();
                FString CoreName = ExtractCoreMaterialName(MaterialName);
                FString MaterialFolderPath = FPaths::GetPath(MaterialInterface->GetPathName());

                TArray<FAssetData> FoundMaterials;
                FindSimilarNamedMaterials(CoreName, MaterialFolderPath, FoundMaterials);

                for (const FAssetData& AssetData : FoundMaterials)
                {
                    AddMaterialToExportList(AssetData);
                }
            }
        }
    }
}

FReply SPropHausGLTFExporterWidget::OnRemoveStaticMeshClicked(FAssetData AssetData)
{
    StaticMeshList.Remove(AssetData);
    CreateStaticMeshList();
    return FReply::Handled();
}

FReply SPropHausGLTFExporterWidget::OnRemoveMaterialClicked(FAssetData AssetData)
{
    MaterialList.Remove(AssetData);
    CreateMaterialList();
    return FReply::Handled();
}

FReply SPropHausGLTFExporterWidget::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    // Get the drag operation
    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
    if (Operation.IsValid() && Operation->IsOfType<FAssetDragDropOp>())
    {
        // Handle asset drag drop
        TSharedPtr<FAssetDragDropOp> AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
        const TArray<FAssetData>& DroppedAssets = AssetDragDropOp->GetAssets();

        for (const FAssetData& AssetData : DroppedAssets)
        {
            if (!StaticMeshList.Contains(AssetData) && AssetData.AssetClassPath == UStaticMesh::StaticClass()->GetClassPathName())
            {
                StaticMeshList.Add(AssetData);
                UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset());
                if (StaticMesh && bAutoAddSimilarMaterials)
                {
                    OnAddStaticMeshToExportList(StaticMesh);
                }
            }
            else if (!MaterialList.Contains(AssetData))
            {
                UObject* Asset = AssetData.GetAsset();
                if (Asset->IsA<UMaterial>() || Asset->IsA<UMaterialInstance>())
                {
                    MaterialList.Add(AssetData);
                }
            }
        }

        bIsDraggingStaticMesh = false;
        bIsDraggingMaterial = false;

        CreateStaticMeshList();
        CreateMaterialList();
        return FReply::Handled();
    }

    return SCompoundWidget::OnDrop(MyGeometry, DragDropEvent);
}

void SPropHausGLTFExporterWidget::UpdateExportButtonState()
{
    ExportButton->SetEnabled(StaticMeshList.Num() > 0 || MaterialList.Num() > 0);
}

void SPropHausGLTFExporterWidget::ExportAssets()
{
    if (!ExportPathTextBox.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("Invalid export path")));
        return;
    }

    FString ExportPath = ExportPathTextBox->GetText().ToString();
    if (ExportPath.IsEmpty())
    {
        StatusText->SetText(FText::FromString(TEXT("Please specify an export path")));
        return;
    }

    if (!FPaths::DirectoryExists(ExportPath))
    {
        StatusText->SetText(FText::FromString(TEXT("Export path does not exist")));
        return;
    }

    StatusText->SetText(FText::FromString(TEXT("Exporting assets...")));

    if (!ExportOptions.IsValid())
    {
        ExportOptions = NewObject<UGLTFExportOptions>();
    }

    TSet<AActor*> SelectedActors;

    auto ConstructFullExportPath = [&](const FAssetData& AssetData) -> FString
    {
        FString AssetPath = AssetData.PackagePath.ToString();

        TArray<FString> PathComponents;
        AssetPath.ParseIntoArray(PathComponents, TEXT("/"));

        if (PathComponents.Num() >= 3)
        {
            FString PropPackName = PathComponents[1];
            FString AssetFolderName = PathComponents.Last();
            return FPaths::Combine(ExportPath, PropPackName, AssetFolderName);
        }

        return ExportPath;
    };

    for (const FAssetData& AssetData : StaticMeshList)
    {
        if (!AssetData.IsValid())
        {
            continue;
        }

        UObject* Asset = AssetData.GetAsset();
        if (!Asset)
        {
            continue;
        }

        FString FullExportPath = ConstructFullExportPath(AssetData);
        FString FileName = AssetData.AssetName.ToString() + TEXT(".gltf");
        FString FullExportFilePath = FPaths::Combine(FullExportPath, FileName);

        if (!FPaths::DirectoryExists(FullExportPath))
        {
            FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*FullExportPath);
        }

        if (Asset->IsA<UStaticMesh>())
        {
            UStaticMesh* StaticMesh = Cast<UStaticMesh>(Asset);
            if (StaticMesh)
            {
                FGLTFExportMessages Messages = FGLTFExportMessages();
UGLTFExporter::ExportToGLTF(StaticMesh, FullExportFilePath, ExportOptions.Get(), SelectedActors, Messages);            }
        }
    }

    for (const FAssetData& AssetData : MaterialList)
    {
        if (!AssetData.IsValid())
        {
            continue;
        }

        UObject* Asset = AssetData.GetAsset();
        if (!Asset)
        {
            continue;
        }

        FString FullExportPath = ConstructFullExportPath(AssetData);
        FString FileName = AssetData.AssetName.ToString() + TEXT(".gltf");
        FString FullExportFilePath = FPaths::Combine(FullExportPath, FileName);

        if (!FPaths::DirectoryExists(FullExportPath))
        {
            FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*FullExportPath);
        }

        if (Asset->IsA<UMaterialInstance>())
        {
            UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(Asset);
            if (MaterialInstance)
            {
                FGLTFExportMessages Messages;
                UGLTFExporter::ExportToGLTF(MaterialInstance, FullExportFilePath, ExportOptions.Get(), SelectedActors, Messages);
            }
        }
        else if (Asset->IsA<UMaterial>())
        {
            UMaterial* Material = Cast<UMaterial>(Asset);
            if (Material)
            {
                FGLTFExportMessages Messages;
                UGLTFExporter::ExportToGLTF(Material, FullExportFilePath, ExportOptions.Get(), SelectedActors, Messages);
            }
        }
    }

    StatusText->SetText(FText::FromString(TEXT("Export completed.")));
}

void SPropHausGLTFExporterWidget::FindSimilarNamedMaterials(const FString& CoreName, const FString& FolderPath, TArray<FAssetData>& OutMaterials)
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FARFilter Filter;
    Filter.PackagePaths.Add(*FolderPath);
    Filter.ClassPaths.Add(UMaterial::StaticClass()->GetClassPathName());
    Filter.ClassPaths.Add(UMaterialInstance::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = false;

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    for (const FAssetData& AssetData : Assets)
    {
        FString AssetName = AssetData.AssetName.ToString();
        if (AssetName.Contains(CoreName))
        {
            OutMaterials.Add(AssetData);
        }
    }
}

FString SPropHausGLTFExporterWidget::ExtractCoreMaterialName(const FString& MaterialName)
{
    TArray<FString> NameParts;
    MaterialName.ParseIntoArray(NameParts, TEXT("_"));
    
    if (NameParts.Num() > 1)
    {
        return NameParts[1];
    }
    
    return MaterialName;
}



void SPropHausGLTFExporterWidget::AddMaterialToExportList(const FAssetData& MaterialAsset)
{
    if (!MaterialList.Contains(MaterialAsset))
    {
        MaterialList.Add(MaterialAsset);
        CreateMaterialList();
        UpdateExportButtonState();
    }
}

FReply SPropHausGLTFExporterWidget::OnSelectExportFolderClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        FString FolderName;
        const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
        const FString Title = TEXT("Select Export Folder");
        const FString DefaultPath = FPaths::ProjectDir();

        if (DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, Title, DefaultPath, FolderName))
        {
            ExportPathTextBox->SetText(FText::FromString(FolderName));
        }
    }

    return FReply::Handled();
}

FReply SPropHausGLTFExporterWidget::OnSelectConfigFolderClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        FString FolderName;
        const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
        const FString Title = TEXT("Select Config Folder");
        const FString DefaultPath = FPaths::ProjectDir();

        if (DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, Title, DefaultPath, FolderName))
        {
          ConfigFolderPathTextBox->SetText(FText::FromString(FolderName));
        }
    }

    return FReply::Handled();
}


FReply SPropHausGLTFExporterWidget::OnShowExportOptionsClicked()
{
    if (!ExportOptions.IsValid())
    {
        ExportOptions = NewObject<UGLTFExportOptions>();
    }

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bHideSelectionTip = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bSearchInitialKeyFocus = false;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bShowOptions = true;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    TSharedRef<IDetailsView> DetailView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailView->SetObject(ExportOptions.Get());

    TSharedRef<SWindow> OptionsWindow = SNew(SWindow)
        .Title(FText::FromString("GLTF Export Options"))
        .ClientSize(FVector2D(400, 600))
        .SupportsMinimize(false)
        .SupportsMaximize(false)
        [
            DetailView
        ];

    FSlateApplication::Get().AddWindow(OptionsWindow);

    return FReply::Handled();
}

FReply SPropHausGLTFExporterWidget::OnSaveButtonClicked()
{
    SaveConfig();
    return FReply::Handled();
}

FReply SPropHausGLTFExporterWidget::OnLoadButtonClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFiles;
        const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
        const FString Title = TEXT("Select Config File");
        const FString DefaultPath = FPaths::ProjectDir();
        const FString FileTypes = TEXT("JSON Files (*.json)|*.json|All Files (*.*)|*.*");

        if (DesktopPlatform->OpenFileDialog(ParentWindowHandle, Title, DefaultPath, TEXT(""), FileTypes, EFileDialogFlags::None, OutFiles))
        {
            if (OutFiles.Num() > 0)
            {
                LoadConfigFromFile(OutFiles[0]);
            }
        }
    }

    return FReply::Handled();
}

void SPropHausGLTFExporterWidget::SaveConfig()
{
    FGLTFExportSettings Settings;
    Settings.ExportPath = ExportPathTextBox->GetText().ToString();
    Settings.ConfigFolderPath = ConfigFolderPathTextBox->GetText().ToString();

    for (const FAssetData& AssetData : StaticMeshList)
    {
        Settings.StaticMeshPaths.Add(AssetData.GetObjectPathString());
    }

    for (const FAssetData& AssetData : MaterialList)
    {
        Settings.MaterialPaths.Add(AssetData.GetObjectPathString());
    }

    FString ConfigFilename = ConfigFilenameTextBox->GetText().ToString();
    if (ConfigFilename.IsEmpty())
    {
        ConfigFilename = TEXT("GLTFExportConfig.json");
    }
    else if (!ConfigFilename.EndsWith(TEXT(".json")))
    {
        ConfigFilename.Append(TEXT(".json"));
    }

    const FString ConfigFolder = ConfigFolderPathTextBox->GetText().ToString();
    const FString ConfigPath = FPaths::Combine(ConfigFolder, ConfigFilename);

    FString JsonString;
    if (FJsonObjectConverter::UStructToJsonObjectString(Settings, JsonString))
    {
        if (FFileHelper::SaveStringToFile(JsonString, *ConfigPath))
        {
            StatusText->SetText(FText::FromString(TEXT("Config saved successfully.")));
        }
        else
        {
            StatusText->SetText(FText::FromString(TEXT("Failed to save config.")));
        }
    }
    else
    {
        StatusText->SetText(FText::FromString(TEXT("Failed to serialize config.")));
    }
}

void SPropHausGLTFExporterWidget::LoadConfigFromFile(const FString& ConfigPath)
{
    FString JsonString;
    if (FFileHelper::LoadFileToString(JsonString, *ConfigPath))
    {
        FGLTFExportSettings Settings;
        if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &Settings, 0, 0))
        {
            ExportPathTextBox->SetText(FText::FromString(Settings.ExportPath));
            ConfigFolderPathTextBox->SetText(FText::FromString(Settings.ConfigFolderPath));

            StaticMeshList.Empty();
            for (const FString& Path : Settings.StaticMeshPaths)
            {
                FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
                FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(Path));
                if (AssetData.IsValid())
                {
                    StaticMeshList.Add(AssetData);
                }
            }

            MaterialList.Empty();
            for (const FString& Path : Settings.MaterialPaths)
            {
                FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
                FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(Path));
                if (AssetData.IsValid())
                {
                    MaterialList.Add(AssetData);
                }
            }

            CreateStaticMeshList();
            CreateMaterialList();
            UpdateExportButtonState();

            StatusText->SetText(FText::FromString(TEXT("Config loaded successfully.")));
        }
        else
        {
            StatusText->SetText(FText::FromString(TEXT("Failed to parse config.")));
        }
    }
    else
    {
        StatusText->SetText(FText::FromString(TEXT("Failed to load config.")));
    }
}

void SPropHausGLTFExporterWidget::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
    if (Operation.IsValid() && Operation->IsOfType<FAssetDragDropOp>())
    {
        TSharedPtr<FAssetDragDropOp> AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
        const TArray<FAssetData>& DroppedAssets = AssetDragDropOp->GetAssets();

        for (const FAssetData& AssetData : DroppedAssets)
        {
            if (AssetData.AssetClassPath == UStaticMesh::StaticClass()->GetClassPathName())
            {
                bIsDraggingStaticMesh = true;
            }
            else if (AssetData.AssetClassPath == UMaterial::StaticClass()->GetClassPathName() || 
                     AssetData.AssetClassPath == UMaterialInstance::StaticClass()->GetClassPathName())
            {
                bIsDraggingMaterial = true;
            }
        }
    }

    SCompoundWidget::OnDragEnter(MyGeometry, DragDropEvent);
}

void SPropHausGLTFExporterWidget::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    bIsDraggingStaticMesh = false;
    bIsDraggingMaterial = false;

    SCompoundWidget::OnDragLeave(DragDropEvent);
}
FReply SPropHausGLTFExporterWidget::OnClearListButtonClicked()
{
    StaticMeshList.Empty();
    MaterialList.Empty();
    CreateStaticMeshList();
    CreateMaterialList();
    UpdateExportButtonState();
    StatusText->SetText(FText::FromString(TEXT("Lists cleared.")));
    return FReply::Handled();
}

FSlateColor SPropHausGLTFExporterWidget::GetStaticMeshContainerBorderColor() const
{
    return bIsDraggingStaticMesh ? FLinearColor::Yellow : FLinearColor(0.25f, 0.25f, 0.25f, 1.0f); // Yellow when dragging
}

FSlateColor SPropHausGLTFExporterWidget::GetMaterialContainerBorderColor() const
{
    return bIsDraggingMaterial ? FLinearColor::Yellow : FLinearColor(0.25f, 0.25f, 0.25f, 1.0f); // Yellow when dragging
}
