//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop                     

#include "ScpExplorer.h"

#include <Common.h>
#include <ScpMain.h>

#include "NonVisual.h"
#include "Tools.h"
#include "WinConfiguration.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AssociatedStatusBar"
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
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TScpExplorerForm::TScpExplorerForm(TComponent* Owner)
        : TCustomScpExplorerForm(Owner)
{          
  BackButton->DropdownMenu = RemoteDirView->BackMenu;
  ForwardButton->DropdownMenu = RemoteDirView->ForwardMenu;
  SavedSessionsButton->OnClick = DropDownButtonMenu;

  TopCoolBar->PopupMenu = NonVisualDataModule->ExplorerBarPopup;
  RemoteStatusBar->PopupMenu = NonVisualDataModule->ExplorerBarPopup;

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

  // called later once again after menu font is updated (see FormShow)
  SetCoolBandsMinWidth(TopCoolBar);

  TCustomScpExplorerForm::RestoreParams();

  RemoteDirView->UnixColProperties->ParamsStr = WinConfiguration->ScpExplorer.DirViewParams;
  RemoteDirView->ViewStyle = (TViewStyle)WinConfiguration->ScpExplorer.ViewStyle;
  LoadCoolbarLayoutStr(TopCoolBar, WinConfiguration->ScpExplorer.CoolBarLayout);
  RemoteStatusBar->Visible = WinConfiguration->ScpExplorer.StatusBar;
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::StoreParams()
{
  assert(Configuration);

  Configuration->BeginUpdate();
  try
  {
    WinConfiguration->ScpExplorer.CoolBarLayout = GetCoolbarLayoutStr(TopCoolBar);
    WinConfiguration->ScpExplorer.StatusBar = RemoteStatusBar->Visible;

    WinConfiguration->ScpExplorer.WindowParams = WinConfiguration->StoreForm(this);;
    WinConfiguration->ScpExplorer.DirViewParams = RemoteDirView->UnixColProperties->ParamsStr;
    WinConfiguration->ScpExplorer.ViewStyle = RemoteDirView->ViewStyle;
    TCustomScpExplorerForm::StoreParams();
  }
  __finally
  {
    WinConfiguration->EndUpdate();
  }
}
//---------------------------------------------------------------------------
bool __fastcall TScpExplorerForm::CopyParamDialog(TTransferDirection Direction,
  TTransferType Type, Boolean DragDrop, TStrings * FileList,
  AnsiString & TargetDirectory, TCopyParamType & CopyParam, bool Confirm)
{
  if ((Direction == tdToLocal) && !DragDrop)
  {
    TargetDirectory = WinConfiguration->ScpExplorer.LastLocalTargetDirectory;
  }
  bool Result = TCustomScpExplorerForm::CopyParamDialog(
    Direction, Type, DragDrop, FileList, TargetDirectory, CopyParam, Confirm);
  if (Result && (Direction == tdToLocal) && !DragDrop)
  {
    WinConfiguration->ScpExplorer.LastLocalTargetDirectory = TargetDirectory;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::FormShow(TObject * /*Sender*/)
{
  FLastDirView = RemoteDirView; // Only dir view
  RemoteDirView->SetFocus();

  // called for second time after menu font was updated (see also RestoreParams)
  SetCoolBandsMinWidth(TopCoolBar);
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
    case fcSessionCombo: return SessionCombo;
    case fcMenuToolBar: return MenuToolBar;
    default: return TCustomScpExplorerForm::GetComponent(Component);
  }
}
//---------------------------------------------------------------------------
void __fastcall TScpExplorerForm::FullSynchronizeDirectories()
{
  AnsiString LocalDirectory = WinConfiguration->ScpExplorer.LastLocalTargetDirectory;
  AnsiString RemoteDirectory = RemoteDirView->PathName;
  if (DoFullSynchronizeDirectories(LocalDirectory, RemoteDirectory))
  {
    WinConfiguration->ScpExplorer.LastLocalTargetDirectory = LocalDirectory; 
  }
}



