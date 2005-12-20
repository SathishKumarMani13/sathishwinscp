//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <StrUtils.hpp>
#include <Common.h>

#include "Preferences.h"

#include <ScpMain.h>
#include <Terminal.h>

#include "VCLCommon.h"
#include "GUITools.h"
#include "Tools.h"
#include "TextsWin.h"
#include "HelpWin.h"
#include "WinInterface.h"
#include "WinConfiguration.h"
#include "Setup.h"
//---------------------------------------------------------------------
#pragma link "GeneralSettings"
#pragma link "LogSettings"
#pragma link "XPThemes"
#pragma link "CopyParams"
#pragma link "UpDownEdit"
#pragma link "IEComboBox"
#pragma link "HistoryComboBox"
#pragma link "PasswordEdit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------
bool __fastcall DoPreferencesDialog(TPreferencesMode APreferencesMode,
  TPreferencesDialogData * DialogData)
{
  bool Result;
  TPreferencesDialog * PreferencesDialog = new TPreferencesDialog(Application);
  try
  {
    PreferencesDialog->PreferencesMode = APreferencesMode;
    Result = PreferencesDialog->Execute(DialogData);
  }
  __finally
  {
    delete PreferencesDialog;
  }
  return Result;
}
//---------------------------------------------------------------------
__fastcall TPreferencesDialog::TPreferencesDialog(TComponent* AOwner)
  : TForm(AOwner)
{
  SetCorrectFormParent(this);

  FPreferencesMode = pmDefault;
  CopyParamsFrame->Direction = pdAll;
  FEditorFont = new TFont();
  FEditorFont->Color = clWindowText;
  // color tends to reset in object inspector
  EditorFontLabel->Color = clWindow;
  // currently useless
  FAfterFilenameEditDialog = false;
  FCustomCommands = new TCustomCommands();
  FCustomCommandChanging = false;
  FListViewDragDest = -1;
  FCopyParamList = new TCopyParamList();
  FEditorList = new TEditorList();
  UseSystemSettings(this);

  InstallPathWordBreakProc(RandomSeedFileEdit);
  InstallPathWordBreakProc(DDTemporaryDirectoryEdit);
  InstallPathWordBreakProc(PuttyPathEdit);
  HintLabel(ShellIconsText);
}
//---------------------------------------------------------------------------
__fastcall TPreferencesDialog::~TPreferencesDialog()
{
  delete FEditorFont;
  delete FCustomCommands;
  delete FCopyParamList;
  delete FEditorList;
}
//---------------------------------------------------------------------
bool __fastcall TPreferencesDialog::Execute(TPreferencesDialogData * DialogData)
{
  FDialogData = DialogData;
  LoadConfiguration();
  CopyParamsFrame->BeforeExecute();
  bool Result = (ShowModal() == mrOk);
  if (Result)
  {
    CopyParamsFrame->AfterExecute();
    SaveConfiguration();
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PrepareNavigationTree(TTreeView * Tree)
{
  Tree->FullExpand();
  int i = 0;
  while (i < Tree->Items->Count)
  {
    if ((!WinConfiguration->ExpertMode &&
         Tree->Items->Item[i]->SelectedIndex & 128))
    {
      Tree->Items->Delete(Tree->Items->Item[i]);
    }
    else
    {
      for (int pi = 0; pi < PageControl->PageCount; pi++)
      {
        if (PageControl->Pages[pi]->Tag == (Tree->Items->Item[i]->SelectedIndex & 127))
        {
          if (PageControl->Pages[pi]->Enabled)
          {
            Tree->Items->Item[i]->Text = PageControl->Pages[pi]->Hint;
            PageControl->Pages[pi]->Hint = "";
          }
          else
          {
            Tree->Items->Delete(Tree->Items->Item[i]);
            i--;
          }
          break;
        }
      }
      i++;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::LoadConfiguration()
{
  if (FPreferencesMode != pmLogin)
  {
    LoggingFrame->LoadConfiguration();
    GeneralSettingsFrame->LoadConfiguration();
  }
  #define BOOLPROP(PROP) PROP ## Check->Checked = WinConfiguration->PROP;
  BOOLPROP(DefaultDirIsHome);
  BOOLPROP(PreservePanelState);
  BOOLPROP(DeleteToRecycleBin);
  BOOLPROP(DDTransferConfirmation);
  BOOLPROP(DDWarnLackOfTempSpace);
  BOOLPROP(ShowHiddenFiles);
  BOOLPROP(ShowInaccesibleDirectories);
  BOOLPROP(CopyOnDoubleClickConfirmation);
  BOOLPROP(ConfirmOverwriting);
  BOOLPROP(ConfirmResume);
  BOOLPROP(ConfirmDeleting);
  BOOLPROP(ConfirmClosingSession);
  BOOLPROP(ConfirmExitOnCompletion);
  BOOLPROP(UseLocationProfiles);
  BOOLPROP(ConfirmCommandSession);
  BOOLPROP(ContinueOnError);
  BOOLPROP(DDAllowMoveInit);
  BOOLPROP(BeepOnFinish);
  BOOLPROP(TemporaryDirectoryCleanup);
  BOOLPROP(ConfirmTemporaryDirectoryCleanup);

  BeepOnFinishAfterEdit->AsInteger =
    static_cast<double>(GUIConfiguration->BeepOnFinishAfter) * (24*60*60);

  CompareByTimeCheck->Checked = WinConfiguration->ScpCommander.CompareByTime;
  CompareBySizeCheck->Checked = WinConfiguration->ScpCommander.CompareBySize;

  DDExtEnabledButton->Checked = WinConfiguration->DDExtEnabled;
  DDExtDisabledButton->Checked = !DDExtEnabledButton->Checked;
  DDWarnOnMoveCheck->Checked = !WinConfiguration->DDAllowMove;

  if (WinConfiguration->DDTemporaryDirectory.IsEmpty())
  {
    DDSystemTemporaryDirectoryButton->Checked = true;
    DDTemporaryDirectoryEdit->Text = SystemTemporaryDirectory();
  }
  else
  {
    DDCustomTemporaryDirectoryButton->Checked = true;
    DDTemporaryDirectoryEdit->Text = WinConfiguration->DDTemporaryDirectory;
  }

  if (WinConfiguration->ScpCommander.NortonLikeMode == nlOff)
  {
    NortonLikeModeCombo->ItemIndex = 2;
  }
  else if (WinConfiguration->ScpCommander.NortonLikeMode == nlKeyboard)
  {
    NortonLikeModeCombo->ItemIndex = 1;
  }
  else
  {
    NortonLikeModeCombo->ItemIndex = 0;
  }

  PreserveLocalDirectoryCheck->Checked =
    WinConfiguration->ScpCommander.PreserveLocalDirectory;
  SwappedPanelsCheck->Checked =
    WinConfiguration->ScpCommander.SwappedPanels;
  FullRowSelectCheck->Checked = WinConfiguration->ScpCommander.FullRowSelect;
  ShowFullAddressCheck->Checked =
    WinConfiguration->ScpExplorer.ShowFullAddress;
  RegistryStorageButton->Checked = (Configuration->Storage == stRegistry);
  IniFileStorageButton->Checked = (Configuration->Storage == stIniFile);

  RandomSeedFileEdit->Text = Configuration->RandomSeedFile;

  // editor
  EditorSingleEditorOnCheck->Checked = WinConfiguration->Editor.SingleEditor;
  EditorSingleEditorOffCheck->Checked = !WinConfiguration->Editor.SingleEditor;

  EditorWordWrapCheck->Checked = WinConfiguration->Editor.WordWrap;
  FEditorFont->Name = WinConfiguration->Editor.FontName;
  FEditorFont->Height = WinConfiguration->Editor.FontHeight;
  FEditorFont->Charset = (TFontCharset)WinConfiguration->Editor.FontCharset;
  FEditorFont->Style = IntToFontStyles(WinConfiguration->Editor.FontStyle);
  (*FEditorList) = *WinConfiguration->EditorList;
  UpdateEditorListView();

  CopyParamsFrame->Params = GUIConfiguration->DefaultCopyParam;
  ResumeOnButton->Checked = GUIConfiguration->DefaultCopyParam.ResumeSupport == rsOn;
  ResumeSmartButton->Checked = GUIConfiguration->DefaultCopyParam.ResumeSupport == rsSmart;
  ResumeOffButton->Checked = GUIConfiguration->DefaultCopyParam.ResumeSupport == rsOff;
  ResumeThresholdEdit->Value = GUIConfiguration->DefaultCopyParam.ResumeThreshold / 1024;
  SessionReopenAutoCheck->Checked = (Configuration->SessionReopenAuto > 0);
  SessionReopenAutoEdit->Value = (Configuration->SessionReopenAuto > 0 ?
    (Configuration->SessionReopenAuto / 1000): 5);

  EnableControl(SessionReopenAutoEdit, SessionReopenAutoCheck->Checked);
  EnableControl(SessionReopenAutoLabel, SessionReopenAutoEdit->Enabled);
  EnableControl(SessionReopenAutoSecLabel, SessionReopenAutoEdit->Enabled);

  TransferSheet->Enabled = WinConfiguration->ExpertMode;
  GeneralSheet->Enabled = (PreferencesMode != pmLogin) && WinConfiguration->ExpertMode;
  ExplorerSheet->Enabled = WinConfiguration->ExpertMode;
  CommanderSheet->Enabled = WinConfiguration->ExpertMode;
  GeneralSheet->Enabled = (PreferencesMode != pmLogin);
  EditorSheet->Enabled = WinConfiguration->ExpertMode && !WinConfiguration->DisableOpenEdit;

  StorageGroup->Visible = WinConfiguration->ExpertMode;
  RandomSeedFileLabel->Visible = WinConfiguration->ExpertMode;
  RandomSeedFileEdit->Visible = WinConfiguration->ExpertMode;

  FCustomCommands->Assign(WinConfiguration->CustomCommands);
  UpdateCustomCommandsView();

  PuttyPathEdit->Text = GUIConfiguration->PuttyPath;
  PuttyPasswordCheck->Checked = GUIConfiguration->PuttyPassword;
  AutoOpenInPuttyCheck->Checked = WinConfiguration->AutoOpenInPutty;

  // Queue
  QueueTransferLimitEdit->AsInteger = GUIConfiguration->QueueTransfersLimit;
  QueueAutoPopupCheck->Checked = GUIConfiguration->QueueAutoPopup;
  QueueCheck->Checked = GUIConfiguration->DefaultCopyParam.Queue;
  QueueNoConfirmationCheck->Checked = GUIConfiguration->DefaultCopyParam.QueueNoConfirmation;
  RememberPasswordCheck->Checked = GUIConfiguration->QueueRememberPassword;
  if (WinConfiguration->QueueView.Show == qvShow)
  {
    QueueViewShowButton->Checked = true;
  }
  else if (WinConfiguration->QueueView.Show == qvHideWhenEmpty)
  {
    QueueViewHideWhenEmptyButton->Checked = true;
  }
  else
  {
    QueueViewHideButton->Checked = true;
  }

  // panels
  if (WinConfiguration->PathInCaption == picFull)
  {
    PathInCaptionFullButton->Checked = true;
  }
  else if (WinConfiguration->PathInCaption == picShort)
  {
    PathInCaptionShortButton->Checked = true;
  }
  else
  {
    PathInCaptionNoneButton->Checked = true;
  }
  DoubleClickActionCombo->ItemIndex = WinConfiguration->DoubleClickAction;

  // updates
  TUpdatesConfiguration Updates = WinConfiguration->Updates;
  if (int(Updates.Period) <= 0)
  {
    UpdatesNeverButton->Checked = true;
  }
  else if (int(Updates.Period) <= 1)
  {
    UpdatesDailyButton->Checked = true;
  }
  else if (int(Updates.Period) <= 7)
  {
    UpdatesWeeklyButton->Checked = true;
  }
  else
  {
    UpdatesMonthlyButton->Checked = true;
  }

  UpdatesProxyCheck->Checked = !Updates.ProxyHost.IsEmpty();
  UpdatesProxyHostEdit->Text =
    Updates.ProxyHost.IsEmpty() ? AnsiString("proxy") : Updates.ProxyHost;
  UpdatesProxyPortEdit->AsInteger = Updates.ProxyPort;

  // presets
  (*FCopyParamList) = *WinConfiguration->CopyParamList;
  UpdateCopyParamListView();
  BOOLPROP(CopyParamAutoSelectNotice);

  // interface
  if (WinConfiguration->Theme == "OfficeXP")
  {
    ThemeCombo->ItemIndex = 1;
  }
  else if (WinConfiguration->Theme == "Office2003")
  {
    ThemeCombo->ItemIndex = 2;
  }
  else
  {
    ThemeCombo->ItemIndex = 0;
  }

  #undef BOOLPROP

  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::SaveConfiguration()
{
  Configuration->BeginUpdate();
  try
  {
    TGUICopyParamType CopyParam = GUIConfiguration->DefaultCopyParam;

    if (FPreferencesMode != pmLogin)
    {
      LoggingFrame->SaveConfiguration();
      GeneralSettingsFrame->SaveConfiguration();
    }
    #define BOOLPROP(PROP) WinConfiguration->PROP = PROP ## Check->Checked
    BOOLPROP(DefaultDirIsHome);
    BOOLPROP(PreservePanelState);
    BOOLPROP(DeleteToRecycleBin);
    BOOLPROP(DDTransferConfirmation);
    BOOLPROP(DDWarnLackOfTempSpace);
    BOOLPROP(ShowHiddenFiles);
    BOOLPROP(ShowInaccesibleDirectories);
    BOOLPROP(CopyOnDoubleClickConfirmation);
    BOOLPROP(ConfirmOverwriting);
    BOOLPROP(ConfirmResume);
    BOOLPROP(ConfirmDeleting);
    BOOLPROP(ConfirmClosingSession);
    BOOLPROP(ConfirmExitOnCompletion);
    BOOLPROP(UseLocationProfiles);
    BOOLPROP(ConfirmCommandSession);
    BOOLPROP(ContinueOnError);
    BOOLPROP(DDAllowMoveInit);
    BOOLPROP(BeepOnFinish);
    BOOLPROP(TemporaryDirectoryCleanup);
    BOOLPROP(ConfirmTemporaryDirectoryCleanup);

    GUIConfiguration->BeepOnFinishAfter =
      static_cast<double>(BeepOnFinishAfterEdit->Value / (24*60*60));

    WinConfiguration->ScpCommander.CompareByTime = CompareByTimeCheck->Checked;
    WinConfiguration->ScpCommander.CompareBySize = CompareBySizeCheck->Checked;
    WinConfiguration->DDAllowMove = !DDWarnOnMoveCheck->Checked;
    WinConfiguration->DDExtEnabled = DDExtEnabledButton->Checked;

    if (DDSystemTemporaryDirectoryButton->Checked)
    {
      WinConfiguration->DDTemporaryDirectory = "";
    }
    else
    {
      WinConfiguration->DDTemporaryDirectory = DDTemporaryDirectoryEdit->Text;
    }

    Configuration->Storage = RegistryStorageButton->Checked ? stRegistry : stIniFile;

    TScpCommanderConfiguration ScpCommander = WinConfiguration->ScpCommander;
    if (NortonLikeModeCombo->ItemIndex == 2)
    {
      ScpCommander.NortonLikeMode = nlOff;
    }
    else if (NortonLikeModeCombo->ItemIndex == 1)
    {
      ScpCommander.NortonLikeMode = nlKeyboard;
    }
    else
    {
      ScpCommander.NortonLikeMode = nlOn;
    }
    ScpCommander.PreserveLocalDirectory = PreserveLocalDirectoryCheck->Checked;
    ScpCommander.SwappedPanels = SwappedPanelsCheck->Checked;
    ScpCommander.FullRowSelect = FullRowSelectCheck->Checked;
    WinConfiguration->ScpCommander = ScpCommander;

    TScpExplorerConfiguration ScpExplorer = WinConfiguration->ScpExplorer;
    ScpExplorer.ShowFullAddress = ShowFullAddressCheck->Checked;
    WinConfiguration->ScpExplorer = ScpExplorer;

    Configuration->RandomSeedFile = RandomSeedFileEdit->Text;

    // editor
    WinConfiguration->Editor.SingleEditor = EditorSingleEditorOnCheck->Checked;

    WinConfiguration->Editor.WordWrap = EditorWordWrapCheck->Checked;
    WinConfiguration->Editor.FontName = FEditorFont->Name;
    WinConfiguration->Editor.FontHeight = FEditorFont->Height;
    WinConfiguration->Editor.FontCharset = FEditorFont->Charset;
    WinConfiguration->Editor.FontStyle = FontStylesToInt(FEditorFont->Style);
    WinConfiguration->EditorList = FEditorList;

    // overwrites only TCopyParamType fields
    CopyParam = CopyParamsFrame->Params;
    if (ResumeOnButton->Checked) CopyParam.ResumeSupport = rsOn;
    if (ResumeSmartButton->Checked) CopyParam.ResumeSupport = rsSmart;
    if (ResumeOffButton->Checked) CopyParam.ResumeSupport = rsOff;
    CopyParam.ResumeThreshold = ResumeThresholdEdit->Value * 1024;

    Configuration->SessionReopenAuto =
      (SessionReopenAutoCheck->Checked ? (SessionReopenAutoEdit->Value * 1000) : 0);

    WinConfiguration->CustomCommands = FCustomCommands;

    GUIConfiguration->PuttyPath = PuttyPathEdit->Text;
    GUIConfiguration->PuttyPassword = PuttyPasswordCheck->Checked;
    WinConfiguration->AutoOpenInPutty = AutoOpenInPuttyCheck->Checked;

    // Queue
    GUIConfiguration->QueueTransfersLimit = QueueTransferLimitEdit->AsInteger;
    GUIConfiguration->QueueAutoPopup = QueueAutoPopupCheck->Checked;
    CopyParam.Queue = QueueCheck->Checked;
    CopyParam.QueueNoConfirmation = QueueNoConfirmationCheck->Checked;
    GUIConfiguration->QueueRememberPassword = RememberPasswordCheck->Checked;

    if (QueueViewShowButton->Checked)
    {
      WinConfiguration->QueueView.Show = qvShow;
    }
    else if (QueueViewHideWhenEmptyButton->Checked)
    {
      WinConfiguration->QueueView.Show = qvHideWhenEmpty;
    }
    else
    {
      WinConfiguration->QueueView.Show = qvHide;
    }

    GUIConfiguration->DefaultCopyParam = CopyParam;

    // panels
    if (PathInCaptionFullButton->Checked)
    {
       WinConfiguration->PathInCaption = picFull;
    }
    else if (PathInCaptionShortButton->Checked)
    {
      WinConfiguration->PathInCaption = picShort;
    }
    else
    {
      WinConfiguration->PathInCaption = picNone;
    }
    WinConfiguration->DoubleClickAction = (TDoubleClickAction)DoubleClickActionCombo->ItemIndex;

    // updates
    TUpdatesConfiguration Updates = WinConfiguration->Updates;
    if (UpdatesNeverButton->Checked)
    {
      Updates.Period = 0;
    }
    else if (UpdatesDailyButton->Checked)
    {
      Updates.Period = 1;
    }
    else if (UpdatesWeeklyButton->Checked)
    {
      Updates.Period = 7;
    }
    else
    {
      Updates.Period = 30;
    }

    Updates.ProxyHost = UpdatesProxyCheck->Checked ? UpdatesProxyHostEdit->Text : AnsiString();
    Updates.ProxyPort = UpdatesProxyPortEdit->AsInteger;

    WinConfiguration->Updates = Updates;

    // presets
    WinConfiguration->CopyParamList = FCopyParamList;
    BOOLPROP(CopyParamAutoSelectNotice);

    // interface
    if (ThemeCombo->ItemIndex == 1)
    {
      WinConfiguration->Theme = "OfficeXP";
    }
    else if (ThemeCombo->ItemIndex == 2)
    {
      WinConfiguration->Theme = "Office2003";
    }
    else
    {
      WinConfiguration->Theme = "Default";
    }
    #undef BOOLPROP
  }
  __finally
  {
    Configuration->EndUpdate();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::SetPreferencesMode(TPreferencesMode value)
{
  if (PreferencesMode != value)
  {
    FPreferencesMode = value;

    GeneralSheet->Enabled = (value != pmLogin);
    LogSheet->Enabled = (value != pmLogin);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::FormShow(TObject * /*Sender*/)
{
  PrepareNavigationTree(NavigationTree);

  for (int Index = 0; Index < PageControl->PageCount; Index++)
  {
    PageControl->Pages[Index]->TabVisible = false;
  }
  // change form height by height of hidden tabs
  ClientHeight -= 50;

  switch (PreferencesMode) {
    case pmEditor: PageControl->ActivePage = EditorSheet; break;
    case pmCustomCommands: PageControl->ActivePage = CustomCommandsSheet; break;
    case pmQueue: PageControl->ActivePage = QueueSheet; break;
    case pmTransfer: PageControl->ActivePage = TransferSheet; break;
    case pmLogging: PageControl->ActivePage = LogSheet; break;
    case pmUpdates: PageControl->ActivePage = UpdatesSheet; break;
    case pmPresets: PageControl->ActivePage = CopyParamListSheet; break;
    case pmEditors: PageControl->ActivePage = EditorSheet; break;
    default: PageControl->ActivePage = PreferencesSheet; break;
  }
  PageControlChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ControlChange(TObject * /*Sender*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpdateControls()
{
  EnableControl(BeepOnFinishAfterEdit, BeepOnFinishCheck->Checked);
  EnableControl(BeepOnFinishAfterText, BeepOnFinishCheck->Checked);
  EnableControl(ResumeThresholdEdit, ResumeSmartButton->Checked);
  EnableControl(SessionReopenAutoEdit, SessionReopenAutoCheck->Checked);
  EnableControl(SessionReopenAutoLabel, SessionReopenAutoEdit->Enabled);
  EnableControl(SessionReopenAutoSecLabel, SessionReopenAutoEdit->Enabled);

  EnableControl(CopyOnDoubleClickConfirmationCheck, (DoubleClickActionCombo->ItemIndex == 1));

  EditorFontLabel->Caption = FMTLOAD(EDITOR_FONT_FMT,
    (FEditorFont->Name, FEditorFont->Size));
  EditorFontLabel->Font = FEditorFont;

  bool CommandSelected = (CustomCommandsView->Selected != NULL);
  EnableControl(EditCommandButton, CommandSelected);
  EnableControl(RemoveCommandButton, CommandSelected);
  EnableControl(UpCommandButton, CommandSelected &&
    CustomCommandsView->ItemIndex > 0);
  EnableControl(DownCommandButton, CommandSelected &&
    (CustomCommandsView->ItemIndex < CustomCommandsView->Items->Count - 1));

  bool CopyParamSelected = (CopyParamListView->Selected != NULL);
  EnableControl(EditCopyParamButton, CopyParamSelected);
  EnableControl(DuplicateCopyParamButton, CopyParamSelected);
  EnableControl(RemoveCopyParamButton, CopyParamSelected);
  EnableControl(UpCopyParamButton, CopyParamSelected &&
    (CopyParamListView->ItemIndex > 0));
  EnableControl(DownCopyParamButton, CopyParamSelected &&
    (CopyParamListView->ItemIndex < CopyParamListView->Items->Count - 1));
  EnableControl(CopyParamAutoSelectNoticeCheck, FCopyParamList->AnyRule);

  EnableControl(DDExtEnabledButton, WinConfiguration->DDExtInstalled);
  EnableControl(DDExtEnabledLabel, WinConfiguration->DDExtInstalled);
  EnableControl(DDExtDisabledPanel, DDExtDisabledButton->Checked);
  EnableControl(DDTemporaryDirectoryEdit, DDCustomTemporaryDirectoryButton->Enabled &&
    DDCustomTemporaryDirectoryButton->Checked);
  EnableControl(DDWarnOnMoveCheck, DDExtDisabledButton->Checked &&
    DDAllowMoveInitCheck->Checked);
  EnableControl(ConfirmTemporaryDirectoryCleanupCheck,
    TemporaryDirectoryCleanupCheck->Checked);
  IniFileStorageButton->Caption =
    AnsiReplaceStr(IniFileStorageButton->Caption, "winscp3.ini",
      ExtractFileName(Configuration->IniFileStorageName));

  EditorFontLabel->WordWrap = EditorWordWrapCheck->Checked;
  bool EditorSelected = (EditorListView->Selected != NULL);
  EnableControl(EditEditorButton, EditorSelected);
  EnableControl(RemoveEditorButton, EditorSelected);
  EnableControl(UpEditorButton, EditorSelected &&
    (EditorListView->ItemIndex > 0));
  EnableControl(DownEditorButton, EditorSelected &&
    (EditorListView->ItemIndex < EditorListView->Items->Count - 1));

  EnableControl(UpdatesProxyHostEdit, UpdatesProxyCheck->Checked);
  EnableControl(UpdatesProxyPortEdit, UpdatesProxyCheck->Checked);

  EnableControl(PuttyPasswordCheck, !PuttyPathEdit->Text.IsEmpty());
  EnableControl(AutoOpenInPuttyCheck, PuttyPasswordCheck->Enabled);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorFontButtonClick(TObject * /*Sender*/)
{
  TFontDialog * Dialog = new TFontDialog(Application);
  try
  {
    Dialog->Device = fdScreen;
    Dialog->Options = TFontDialogOptions() << fdForceFontExist;
    Dialog->Font = FEditorFont;
    if (Dialog->Execute())
    {
      FEditorFont->Assign(Dialog->Font);
      UpdateControls();
    }
  }
  __finally
  {
    delete Dialog;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::FilenameEditExit(TObject * Sender)
{
  // duplicated in TExternalEditorDialog::FilenameEditExit
  THistoryComboBox * FilenameEdit = dynamic_cast<THistoryComboBox *>(Sender);
  try
  {
    AnsiString Filename = FilenameEdit->Text;
    if (!Filename.IsEmpty())
    {
      ReformatFileNameCommand(Filename);
      FilenameEdit->Text = Filename;
    }
    ControlChange(Sender);
  }
  catch(...)
  {
    FilenameEdit->SelectAll();
    FilenameEdit->SetFocus();
    throw;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::FilenameEditChange(
  TObject * Sender)
{
  // duplicated in TExternalEditorDialog::FilenameEditChange
  if (FAfterFilenameEditDialog)
  {
    FAfterFilenameEditDialog = false;
    FilenameEditExit(Sender);
  }
  else
  {
    ControlChange(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::FormCloseQuery(TObject * /*Sender*/,
  bool & /*CanClose*/)
{
  if (ModalResult != mrCancel)
  {
    ExitActiveControl(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::IconButtonClick(TObject *Sender)
{
  AnsiString IconName, Params;
  int SpecialFolder;

  if (Sender == DesktopIconButton)
  {
    IconName = AppNameVersion;
    int Result =
      MessageDialog(LoadStr(CREATE_DESKTOP_ICON), qtConfirmation,
        qaYes | qaNo | qaCancel, HELP_CREATE_ICON);
    switch (Result)
    {
      case qaYes:
        SpecialFolder = CSIDL_COMMON_DESKTOPDIRECTORY;
        break;

      case qaNo:
        SpecialFolder = CSIDL_DESKTOPDIRECTORY;
        break;

      default:
        Abort();
        break;
    }
  }
  else
  {
    if (MessageDialog(LoadStr(CONFIRM_CREATE_ICON),
          qtConfirmation, qaYes | qaNo, HELP_CREATE_ICON) == qaYes)
    {
      if (Sender == SendToHookButton)
      {
        IconName = FMTLOAD(SENDTO_HOOK_NAME, (AppNameVersion));
        SpecialFolder = CSIDL_SENDTO;
        Params = "/upload";
      }
      else if (Sender == QuickLaunchIconButton)
      {
        IconName = "Microsoft\\Internet Explorer\\Quick Launch\\" +
          AppNameVersion;
        SpecialFolder = CSIDL_APPDATA;
      }
    }
    else
    {
      Abort();
    }
  }

  CreateDesktopShortCut(IconName,
    Application->ExeName, Params, "", SpecialFolder);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandsViewData(TObject * /*Sender*/,
      TListItem * Item)
{
  assert(FCustomCommands);
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FCustomCommands->Count);
  Item->Caption = StringReplace(FCustomCommands->Names[Index], "&", "",
    TReplaceFlags() << rfReplaceAll);
  assert(!Item->SubItems->Count);
  AnsiString Name = FCustomCommands->Names[Index];
  Item->SubItems->Add(FCustomCommands->Values[Name]);
  int Params = FCustomCommands->Params[Name];
  Item->SubItems->Add(LoadStr(
    FLAGSET(Params, ccLocal) ? CUSTOM_COMMAND_LOCAL : CUSTOM_COMMAND_REMOTE));
  AnsiString ParamsStr;
  #define ADDPARAM(PARAM, STR) \
    if (FLAGSET(Params, PARAM)) \
      ParamsStr += (ParamsStr.IsEmpty() ? "" : "/") + LoadStr(STR);
  ADDPARAM(ccApplyToDirectories, CUSTOM_COMMAND_DIRECTORIES);
  ADDPARAM(ccRecursive, CUSTOM_COMMAND_RECURSE);
  #undef ADDPARAM
  Item->SubItems->Add(ParamsStr);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ListViewSelectItem(
  TObject * /*Sender*/, TListItem * /*Item*/, bool /*Selected*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpdateCustomCommandsView()
{
  CustomCommandsView->Items->Count = FCustomCommands->Count;
  AdjustListColumnsWidth(CustomCommandsView, FCustomCommands->Count);
  CustomCommandsView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandsViewKeyDown(
      TObject * /*Sender*/, WORD & Key, TShiftState /*Shift*/)
{
  if (RemoveCommandButton->Enabled && (Key == VK_DELETE))
  {
    RemoveCommandButtonClick(NULL);
  }

  if (AddCommandButton->Enabled && (Key == VK_INSERT))
  {
    AddEditCommandButtonClick(AddCommandButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandsViewDblClick(
  TObject * /*Sender*/)
{
  if (EditCommandButton->Enabled)
  {
    AddEditCommandButtonClick(EditCommandButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::AddEditCommandButtonClick(TObject * Sender)
{
  bool Edit = (Sender == EditCommandButton);
  AnsiString Description;
  AnsiString Command;
  int Params = 0;

  if (Edit)
  {
    int Index = CustomCommandsView->ItemIndex;
    assert(Index >= 0 && Index <= FCustomCommands->Count);

    Description = FCustomCommands->Names[Index];
    Command = FCustomCommands->Values[Description];
    Params = FCustomCommands->Params[Description];
  }

  if (DoCustomCommandDialog(Description, Command, Params, FCustomCommands,
        (Edit ? ccmEdit : ccmAdd), NULL))
  {
    int Index = CustomCommandsView->ItemIndex;
    AnsiString Record = FORMAT("%s=%s", (Description, Command));
    if (Edit)
    {
      FCustomCommands->Strings[Index] = Record;
    }
    else
    {
      if (Index >= 0)
      {
        FCustomCommands->Insert(Index, Record);
      }
      else
      {
        Index = FCustomCommands->Add(Record);
      }
    }

    FCustomCommands->Params[Description] = Params;
    UpdateCustomCommandsView();
    CustomCommandsView->ItemIndex = Index;
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RemoveCommandButtonClick(
      TObject * /*Sender*/)
{
  assert(CustomCommandsView->ItemIndex >= 0 &&
    CustomCommandsView->ItemIndex < FCustomCommands->Count);
  FCustomCommands->Delete(CustomCommandsView->ItemIndex);
  UpdateCustomCommandsView();
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandMove(int Source, int Dest)
{
  if (Source >= 0 && Source < FCustomCommands->Count &&
      Dest >= 0 && Dest < FCustomCommands->Count)
  {
    FCustomCommands->Move(Source, Dest);
    // workaround for bug in VCL
    CustomCommandsView->ItemIndex = -1;
    CustomCommandsView->ItemFocused = CustomCommandsView->Selected;
    CustomCommandsView->ItemIndex = Dest;
    UpdateCustomCommandsView();
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpDownCommandButtonClick(TObject * Sender)
{
  CustomCommandMove(CustomCommandsView->ItemIndex,
    CustomCommandsView->ItemIndex + (Sender == UpCommandButton ? -1 : 1));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ListViewStartDrag(
      TObject * Sender, TDragObject *& /*DragObject*/)
{
  FListViewDragSource = dynamic_cast<TListView*>(Sender)->ItemIndex;
  FListViewDragDest = -1;
}
//---------------------------------------------------------------------------
bool __fastcall TPreferencesDialog::AllowListViewDrag(TObject * Sender, int X, int Y)
{
  TListItem * Item = dynamic_cast<TListView*>(Sender)->GetItemAt(X, Y);
  FListViewDragDest = Item ? Item->Index : -1;
  return (FListViewDragDest >= 0) && (FListViewDragDest != FListViewDragSource);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandsViewDragDrop(
      TObject * Sender, TObject * Source, int X, int Y)
{
  if (Source == CustomCommandsView)
  {
    if (AllowListViewDrag(Sender, X, Y))
    {
      CustomCommandMove(FListViewDragSource, FListViewDragDest);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ListViewDragOver(
      TObject * Sender, TObject * Source, int /*X*/, int /*Y*/,
      TDragState /*State*/, bool & Accept)
{
  if (Source == Sender)
  {
    // cannot use AllowListViewDrag(X, Y) because of bug in VCL
    // (when dropped on item itself, when it was dragged over another item before,
    // that another item remains highlighted forever)
    Accept = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamMove(int Source, int Dest)
{
  if (Source >= 0 && Source < FCopyParamList->Count &&
      Dest >= 0 && Dest < FCopyParamList->Count)
  {
    FCopyParamList->Move(Source, Dest);
    // workaround for bug in VCL
    CopyParamListView->ItemIndex = -1;
    CopyParamListView->ItemFocused = CopyParamListView->Selected;
    CopyParamListView->ItemIndex = Dest;
    UpdateCopyParamListView();
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamListViewDragDrop(
  TObject * Sender, TObject * Source, int X, int Y)
{
  if (Source == CopyParamListView)
  {
    if (AllowListViewDrag(Sender, X, Y))
    {
      CopyParamMove(FListViewDragSource, FListViewDragDest);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpDownCopyParamButtonClick(TObject * Sender)
{
  CopyParamMove(CopyParamListView->ItemIndex,
    CopyParamListView->ItemIndex + (Sender == UpCopyParamButton ? -1 : 1));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RemoveCopyParamButtonClick(
  TObject * /*Sender*/)
{
  assert(CopyParamListView->ItemIndex >= 0 &&
    CopyParamListView->ItemIndex < FCopyParamList->Count);
  FCopyParamList->Delete(CopyParamListView->ItemIndex);
  UpdateCopyParamListView();
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::AddEditCopyParamButtonClick(
  TObject * Sender)
{
  TCopyParamPresetMode Mode;
  if (Sender == EditCopyParamButton)
  {
    Mode = cpmEdit;
  }
  else if (Sender == DuplicateCopyParamButton)
  {
    Mode = cpmDuplicate;
  }
  else
  {
    Mode = cpmAdd;
  }
  int Index = CopyParamListView->ItemIndex;
  TCopyParamRuleData * CopyParamRuleData =
    (FDialogData != NULL ? FDialogData->CopyParamRuleData : NULL);
  if (DoCopyParamPresetDialog(FCopyParamList, Index, Mode, CopyParamRuleData))
  {
    UpdateCopyParamListView();
    CopyParamListView->ItemIndex = Index;
    // when using duplicate button, focu remains on original item
    CopyParamListView->ItemFocused = CopyParamListView->Selected;
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamListViewDblClick(
  TObject * /*Sender*/)
{
  if (EditCopyParamButton->Enabled)
  {
    AddEditCopyParamButtonClick(EditCopyParamButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamListViewKeyDown(
  TObject * /*Sender*/, WORD & Key, TShiftState /*Shift*/)
{
  if (RemoveCopyParamButton->Enabled && (Key == VK_DELETE))
  {
    RemoveCopyParamButtonClick(NULL);
  }

  if (AddCopyParamButton->Enabled && (Key == VK_INSERT))
  {
    AddEditCopyParamButtonClick(AddCopyParamButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorMove(int Source, int Dest)
{
  if (Source >= 0 && Source < FEditorList->Count &&
      Dest >= 0 && Dest < FEditorList->Count)
  {
    FEditorList->Move(Source, Dest);
    // workaround for bug in VCL
    EditorListView->ItemIndex = -1;
    EditorListView->ItemFocused = EditorListView->Selected;
    EditorListView->ItemIndex = Dest;
    UpdateEditorListView();
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListViewDragDrop(TObject * Sender,
  TObject * Source, int X, int Y)
{
  if (Source == EditorListView)
  {
    if (AllowListViewDrag(Sender, X, Y))
    {
      EditorMove(FListViewDragSource, FListViewDragDest);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpDownEditorButtonClick(TObject *Sender)
{
  EditorMove(EditorListView->ItemIndex,
    EditorListView->ItemIndex + (Sender == UpEditorButton ? -1 : 1));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RemoveEditorButtonClick(
  TObject * /*Sender*/)
{
  assert(EditorListView->ItemIndex >= 0 &&
    EditorListView->ItemIndex < FEditorList->Count);
  FEditorList->Delete(EditorListView->ItemIndex);
  UpdateEditorListView();
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::AddEditEditorButtonClick(TObject * Sender)
{
  TEditorPreferencesMode Mode = (Sender == EditEditorButton ? epmEdit : epmAdd);
  int Index = EditorListView->ItemIndex;
  TEditorPreferences * Editor;
  if (Mode == epmEdit)
  {
    Editor = new TEditorPreferences(*FEditorList->Editors[Index]);
  }
  else
  {
    Editor = new TEditorPreferences();
  }

  try
  {
    if (DoEditorPreferencesDialog(Editor, Mode))
    {
      if (Mode == epmEdit)
      {
        FEditorList->Change(Index, Editor);
      }
      else
      {
        if (Index < 0)
        {
          Index = FEditorList->Count;
          FEditorList->Add(Editor);
        }
        else
        {
          FEditorList->Insert(Index, Editor);
        }
      }
      // ownership of the object lost
      Editor = NULL;

      UpdateEditorListView();
      EditorListView->ItemIndex = Index;
      UpdateControls();
    }
  }
  __finally
  {
    delete Editor;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListViewDblClick(TObject * /*Sender*/)
{
  if (EditEditorButton->Enabled)
  {
    AddEditEditorButtonClick(EditEditorButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListViewKeyDown(TObject * /*Sender*/,
  WORD & Key, TShiftState /*Shift*/)
{
  if (RemoveEditorButton->Enabled && (Key == VK_DELETE))
  {
    RemoveEditorButtonClick(NULL);
  }

  if (AddEditorButton->Enabled && (Key == VK_INSERT))
  {
    AddEditEditorButtonClick(AddEditorButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpdateEditorListView()
{
  EditorListView->Items->Count = FEditorList->Count;
  AdjustListColumnsWidth(EditorListView, FEditorList->Count);
  EditorListView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListViewData(TObject * /*Sender*/,
  TListItem * Item)
{
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FEditorList->Count);
  const TEditorPreferences * Editor = FEditorList->Editors[Index];
  Item->Caption = Editor->Data.FileMask.Masks;
  Item->SubItems->Add(Editor->Name);
  if (Editor->Data.Editor == edExternal)
  {
    Item->SubItems->Add(BooleanToStr(Editor->Data.MDIExternalEditor));
    Item->SubItems->Add(BooleanToStr(Editor->Data.ExternalEditorText));
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::NavigationTreeChange(TObject * /*Sender*/,
      TTreeNode *Node)
{
  if (Node->SelectedIndex)
  {
    for (Integer Index = 0; Index < PageControl->PageCount; Index++)
    {
      if (PageControl->Pages[Index]->Tag == (Node->SelectedIndex & 127))
      {
        PageControl->ActivePage = PageControl->Pages[Index];
        // reshow the accelerators, etc
        ResetSystemSettings(this);
        return;
      }
    }
  }
  assert(false);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PageControlChange(TObject * /*Sender*/)
{
  bool Found = false;
  if (PageControl->ActivePage->Tag)
  {
    for (int Index = 0; Index < NavigationTree->Items->Count; Index++)
    {
      if ((NavigationTree->Items->Item[Index]->SelectedIndex & 127) ==
            PageControl->ActivePage->Tag)
      {
        NavigationTree->Items->Item[Index]->Selected = true;
        Found = true;
      }
    }
  }

  assert(Found);
  if (Found)
  {
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CMDialogKey(TWMKeyDown & Message)
{
  if (Message.CharCode == VK_TAB)
  {
    TShiftState Shift = KeyDataToShiftState(Message.KeyData);
    if (Shift.Contains(ssCtrl))
    {
      TTreeNode * Node = NavigationTree->Selected;
      if (!Shift.Contains(ssShift))
      {
        Node = Node->GetNext();
        if (!Node) Node = NavigationTree->Items->GetFirstNode();
      }
      else
      {
        if (Node->GetPrev()) Node = Node->GetPrev();
          else
        while (Node->GetNext()) Node = Node->GetNext();
      }
      Node->Selected = True;
      Message.Result = 1;
      return;
    }
  }
  TForm::Dispatch(&Message);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::Dispatch(void *Message)
{
  TMessage * M = reinterpret_cast<TMessage*>(Message);
  assert(M);
  if (M->Msg == CM_DIALOGKEY)
  {
    CMDialogKey(*((TWMKeyDown *)Message));
  }
  else
  {
    TForm::Dispatch(Message);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RegisterAsUrlHandlerButtonClick(
  TObject * /*Sender*/)
{
  if (MessageDialog(LoadStr(CONFIRM_REGISTER_URL),
        qtConfirmation, qaYes | qaNo, HELP_REGISTER_URL) == qaYes)
  {
    RegisterAsUrlHandler();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::DDExtLabelClick(TObject * Sender)
{
  ((Sender == DDExtEnabledLabel) ? DDExtEnabledButton : DDExtDisabledButton)->
    SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::AddSearchPathButtonClick(
  TObject * /*Sender*/)
{
  AnsiString AppPath = ExtractFilePath(Application->ExeName);
  if (MessageDialog(FMTLOAD(CONFIRM_ADD_SEARCH_PATH, (AppPath)),
        qtConfirmation, qaYes | qaNo, HELP_ADD_SEARCH_PATH) == qaYes)
  {
    AddSearchPath(AppPath);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorFontLabelDblClick(
  TObject * Sender)
{
  EditorFontButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpdateCopyParamListView()
{
  CopyParamListView->Items->Count = FCopyParamList->Count;
  AdjustListColumnsWidth(CopyParamListView, FCopyParamList->Count);
  CopyParamListView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamListViewData(TObject * /*Sender*/,
  TListItem * Item)
{
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FCopyParamList->Count);
  Item->Caption = StringReplace(FCopyParamList->Names[Index], "&", "",
    TReplaceFlags() << rfReplaceAll);
  Item->SubItems->Add(BooleanToStr(FCopyParamList->Rules[Index] != NULL));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CopyParamListViewInfoTip(
  TObject * /*Sender*/, TListItem * Item, AnsiString & InfoTip)
{
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FCopyParamList->Count);
  const TCopyParamType * CopyParam = FCopyParamList->CopyParams[Index];
  const TCopyParamRule * Rule = FCopyParamList->Rules[Index];
  InfoTip = CopyParam->GetInfoStr("; ", 0);
  if (Rule != NULL)
  {
    InfoTip += "\n-\n" + Rule->GetInfoStr("; ");
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::HelpButtonClick(TObject * /*Sender*/)
{
  FormHelp(this, PageControl->ActivePage);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PuttyPathBrowseButtonClick(
  TObject * /*Sender*/)
{
  BrowseForExecutable(PuttyPathEdit, LoadStr(PREFERENCES_SELECT_PUTTY),
    LoadStr(PREFERENCES_PUTTY_FILTER), false);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PuttyPathResetButtonClick(
  TObject * /*Sender*/)
{
  PuttyPathEdit->Text = WinConfiguration->DefaultPuttyPath;
}
//---------------------------------------------------------------------------

