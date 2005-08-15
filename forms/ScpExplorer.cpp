//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop                     

#include "ScpExplorer.h"

#include <Common.h>
#include <ScpMain.h>

#include "NonVisual.h"
#include "Glyphs.h"
#include "Tools.h"
#include "WinConfiguration.h"
#include <VCLCommon.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CustomDirView"
#pragma link "CustomScpExplorer"
#pragma link "CustomUnixDirView"
#pragma link "IEListView"
#pragma link "NortonLikeListView"
#pragma link "UnixDirView"
#pragma link "CustomPathComboBox"
#pragma link "IEComboBox"
#pragma link "UnixPathComboBox"
#pragma link "IEPathComboBox"
#pragma link "CustomDriveView"
#pragma link "UnixDriveView"
#pragma link "TB2Dock"
#pragma link "TBX"
#pragma link "TB2Item"
#pragma link "TB2Toolbar"
#pragma link "TBXStatusBars"
#pragma link "TBXExtItems"
#pragma link "TB2ExtItems"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TScpExplorerForm::TScpExplorerForm(TComponent* Owner)
        : TCustomScpExplorerForm(Owner)
{          
  BackButton->LinkSubitems = HistoryMenu(osRemote, true)->Items;
  ForwardButton->LinkSubitems = HistoryMenu(osRemote, false)->Items;

  TopDock->PopupMenu = NonVisualDataModule->ExplorerBarPopup;
  RemoteStatusBar->PopupMenu = TopDock->PopupMenu;
  QueueDock->PopupMenu = TopDock->PopupMenu;
  RemoteDriveView->PopupMenu = TopDock->PopupMenu;
  BottomDock->PopupMenu = TopDock->PopupMenu;
  LeftDock->PopupMenu = TopDock->PopupMenu;
  RightDock->PopupMenu = TopDock->PopupMenu;
  reinterpret_cast<TLabel*>(RemotePanelSplitter)->OnDblClick = RemotePanelSplitterDblClick;

  QueuePanel->Parent = RemotePanel;
  QueueSplitter->Parent = RemotePanel;

  // set common explorer shorcuts to our actions
  NonVisualDataModule->ExplorerShortcuts();
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::RestoreFormParams()
{
  assert(Configuration);

  TCustomScpExplorerForm::RestoreFormParams();
  WinConfiguration->RestoreForm(WinConfiguration->ScpExplorer.WindowParams, this);
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::ConfigurationChanged()
{
  TCustomScpExplorerForm::ConfigurationChanged();
  UnixPathComboBox->ShowFullPath = WinConfiguration->ScpExplorer.ShowFullAddress;
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::RestoreParams()
{
  assert(Configuration);

  TCustomScpExplorerForm::RestoreParams();

  RemoteDirView->UnixColProperties->ParamsStr = WinConfiguration->ScpExplorer.DirViewParams;
  RemoteDirView->UnixColProperties->ExtVisible = false; // just to make sure
  RemoteDirView->ViewStyle = (TViewStyle)WinConfiguration->ScpExplorer.ViewStyle;
  LoadToolbarsLayoutStr(WinConfiguration->ScpExplorer.ToolbarsLayout);
  SessionCombo->EditWidth = WinConfiguration->ScpExplorer.SessionComboWidth;
  RemoteStatusBar->Visible = WinConfiguration->ScpExplorer.StatusBar;
  RemoteDriveView->Visible = WinConfiguration->ScpExplorer.DriveView;
  RemoteDriveView->Width = WinConfiguration->ScpExplorer.DriveViewWidth;
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::StoreParams()
{
  assert(Configuration);

  Configuration->BeginUpdate();
  try
  {
    WinConfiguration->ScpExplorer.ToolbarsLayout = GetToolbarsLayoutStr();
    WinConfiguration->ScpExplorer.SessionComboWidth = SessionCombo->EditWidth;
    WinConfiguration->ScpExplorer.StatusBar = RemoteStatusBar->Visible;

    WinConfiguration->ScpExplorer.WindowParams = WinConfiguration->StoreForm(this);;
    WinConfiguration->ScpExplorer.DirViewParams = RemoteDirView->UnixColProperties->ParamsStr;
    WinConfiguration->ScpExplorer.ViewStyle = RemoteDirView->ViewStyle;
    WinConfiguration->ScpExplorer.DriveView = RemoteDriveView->Visible;
    WinConfiguration->ScpExplorer.DriveViewWidth = RemoteDriveView->Width;
    TCustomScpExplorerForm::StoreParams();
  }
  __finally
  {
    WinConfiguration->EndUpdate();
  }
}
//---------------------------------------------------------------------------
bool __fastcall TScpExplorerForm::CopyParamDialog(TTransferDirection Direction,
  TTransferType Type, Boolean Temp, TStrings * FileList,
  AnsiString & TargetDirectory, TGUICopyParamType & CopyParam, bool Confirm)
{
  // Temp means d&d here so far, may change in future!
  if ((Direction == tdToLocal) && !Temp && TargetDirectory.IsEmpty())
  {
    TargetDirectory = WinConfiguration->ScpExplorer.LastLocalTargetDirectory;
  }
  bool Result = TCustomScpExplorerForm::CopyParamDialog(
    Direction, Type, Temp, FileList, TargetDirectory, CopyParam, Confirm);
  if (Result && (Direction == tdToLocal) && !Temp)
  {
    WinConfiguration->ScpExplorer.LastLocalTargetDirectory = TargetDirectory;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::DoShow()
{
  TCustomScpExplorerForm::DoShow();

  RemoteDirView->SetFocus();
}
//---------------------------------------------------------------------------
bool __fastcall TScpExplorerForm::AllowedAction(TAction * Action, TActionAllowed Allowed)
{
  #define FLAG ((TActionFlag)(Action->Tag))
  return
    // always require Explorer flag
    (FLAG & afExplorer) &&
    // if action is execution or update, we don't require any other flag
    // if we check for shortcut, we require that action is designed for remote panel
    ((Allowed != aaShortCut) || (FLAG & afRemote));
  #undef FLAG
}
//---------------------------------------------------------------------------
TControl * __fastcall TScpExplorerForm::GetComponent(Byte Component)
{
  switch (Component) {
    case fcSessionCombo: return reinterpret_cast<TControl*>(SessionCombo);
    case fcTransferCombo: return reinterpret_cast<TControl*>(TransferCombo);
    case fcSessionToolbar: return SessionToolbar;

    case fcExplorerMenuBand: return MenuToolbar;
    case fcExplorerAddressBand: return AddressToolbar;
    case fcExplorerToolbarBand: return ButtonsToolbar;
    case fcExplorerSelectionBand: return SelectionToolbar;
    case fcExplorerSessionBand: return SessionToolbar;
    case fcExplorerPreferencesBand: return PreferencesToolbar;
    case fcExplorerSortBand: return SortToolbar;
    case fcExplorerUpdatesBand: return UpdatesToolbar;
    case fcExplorerTransferBand: return TransferToolbar;
    default: return TCustomScpExplorerForm::GetComponent(Component);
  }
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::SynchronizeDirectories()
{
  AnsiString LocalDirectory = WinConfiguration->ScpExplorer.LastLocalTargetDirectory;
  AnsiString RemoteDirectory = RemoteDirView->PathName;
  if (DoSynchronizeDirectories(LocalDirectory, RemoteDirectory))
  {
    WinConfiguration->ScpExplorer.LastLocalTargetDirectory = LocalDirectory;
  }
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::FullSynchronizeDirectories()
{
  AnsiString LocalDirectory = WinConfiguration->ScpExplorer.LastLocalTargetDirectory;
  AnsiString RemoteDirectory = RemoteDirView->PathName;
  bool SaveMode = true;
  TSynchronizeMode Mode = (TSynchronizeMode)GUIConfiguration->SynchronizeMode;
  if (DoFullSynchronizeDirectories(LocalDirectory, RemoteDirectory, Mode, SaveMode))
  {
    WinConfiguration->ScpExplorer.LastLocalTargetDirectory = LocalDirectory;
    if (SaveMode)
    {
      GUIConfiguration->SynchronizeMode = Mode;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::FixControlsPlacement()
{
  TCustomScpExplorerForm::FixControlsPlacement();
  
  TControl * ControlsOrder[] =
    { RemoteDirView, QueueSplitter, QueuePanel, BottomDock, RemoteStatusBar };
  SetVerticalControlsOrder(ControlsOrder, LENOF(ControlsOrder));

  TControl * RemoteControlsOrder[] =
    { RemoteDriveView, RemotePanelSplitter, RemoteDirView };
  SetHorizontalControlsOrder(RemoteControlsOrder, LENOF(RemoteControlsOrder));
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::RemoteStatusBarDblClick(TObject * /*Sender*/)
{
  DoFileSystemInfoDialog(Terminal);
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::RemoteDirViewUpdateStatusBar(
  TObject * /*Sender*/, const TStatusFileInfo & FileInfo)
{
  UpdateFileStatusBar(RemoteStatusBar, FileInfo, 0);
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::RemotePanelSplitterDblClick(TObject * /*Sender*/)
{
  // for some reason PostComponentHide is not necessary here (see queue panel)
  ComponentVisible[fcRemoteTree] = false;
}

