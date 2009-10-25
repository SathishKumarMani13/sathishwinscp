//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <StrUtils.hpp>
#include <Common.h>

#include "Preferences.h"

#include <CoreMain.h>
#include <Terminal.h>
#include <Bookmarks.h>

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
#pragma link "CopyParams"
#pragma link "UpDownEdit"
#ifndef NO_RESOURCES
#pragma resource "*.dfm"
#endif
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

  FNoUpdate = 0;
  FPreferencesMode = pmDefault;
  FEditorFont = new TFont();
  FEditorFont->Color = clWindowText;
  // color tends to reset in object inspector
  EditorFontLabel->Color = clWindow;
  // currently useless
  FAfterFilenameEditDialog = false;
  FCustomCommandList = new TCustomCommandList();
  FCustomCommandChanging = false;
  FListViewDragDest = -1;
  FCopyParamList = new TCopyParamList();
  FEditorList = new TEditorList();
  UseSystemSettings(this);

  FCustomCommandsScrollOnDragOver = new TListViewScrollOnDragOver(CustomCommandsView, true);
  FCopyParamScrollOnDragOver = new TListViewScrollOnDragOver(CopyParamListView, true);
  FEditorScrollOnDragOver = new TListViewScrollOnDragOver(EditorListView2, true);

  ComboAutoSwitchInitialize(UpdatesBetaVersionsCombo);

  LoggingFrame->Init();
  InstallPathWordBreakProc(RandomSeedFileEdit);
  InstallPathWordBreakProc(DDTemporaryDirectoryEdit);
  InstallPathWordBreakProc(PuttyPathEdit);
  HintLabel(ShellIconsText);
}
//---------------------------------------------------------------------------
__fastcall TPreferencesDialog::~TPreferencesDialog()
{
  SAFE_DESTROY(FEditorScrollOnDragOver);
  SAFE_DESTROY(FCopyParamScrollOnDragOver);
  SAFE_DESTROY(FCustomCommandsScrollOnDragOver);
  delete FEditorFont;
  delete FCustomCommandList;
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
  FNoUpdate++;
  try
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
    BOOLPROP(RenameWholeName);
    BOOLPROP(ShowInaccesibleDirectories);
    BOOLPROP(CopyOnDoubleClickConfirmation);
    BOOLPROP(ConfirmTransferring);
    BOOLPROP(ConfirmOverwriting);
    BOOLPROP(ConfirmResume);
    BOOLPROP(ConfirmDeleting);
    BOOLPROP(ConfirmRecycling);
    BOOLPROP(ConfirmClosingSession);
    BOOLPROP(ConfirmExitOnCompletion);
    BOOLPROP(UseLocationProfiles);
    BOOLPROP(ConfirmCommandSession);
    BOOLPROP(ContinueOnError);
    BOOLPROP(DDAllowMoveInit);
    BOOLPROP(BeepOnFinish);
    BOOLPROP(TemporaryDirectoryAppendSession);
    BOOLPROP(TemporaryDirectoryAppendPath);
    BOOLPROP(TemporaryDirectoryCleanup);
    BOOLPROP(ConfirmTemporaryDirectoryCleanup);

    BeepOnFinishAfterEdit->AsInteger =
      static_cast<double>(GUIConfiguration->BeepOnFinishAfter) * (24*60*60);
    BOOLPROP(BalloonNotifications);

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
    TreeOnLeftCheck->Checked = WinConfiguration->ScpCommander.TreeOnLeft;
    ShowFullAddressCheck->Checked =
      WinConfiguration->ScpExplorer.ShowFullAddress;
    RegistryStorageButton->Checked = (Configuration->Storage == stRegistry);
    IniFileStorageButton2->Checked = (Configuration->Storage == stIniFile);

    RandomSeedFileEdit->Text = Configuration->RandomSeedFile;

    // editor
    EditorWordWrapCheck->Checked = WinConfiguration->Editor.WordWrap;
    EditorTabSizeEdit->AsInteger = WinConfiguration->Editor.TabSize;
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
    SessionReopenAutoIdleCheck->Checked = (GUIConfiguration->SessionReopenAutoIdle > 0);
    SessionReopenAutoEdit->Value = (Configuration->SessionReopenAuto > 0 ?
      (Configuration->SessionReopenAuto / 1000) : 5);
    SessionReopenTimeoutEdit->Value = (Configuration->SessionReopenTimeout / 1000);

    TransferSheet->Enabled = WinConfiguration->ExpertMode;
    GeneralSheet->Enabled = (PreferencesMode != pmLogin) && WinConfiguration->ExpertMode;
    ExplorerSheet->Enabled = WinConfiguration->ExpertMode;
    CommanderSheet->Enabled = WinConfiguration->ExpertMode;
    GeneralSheet->Enabled = (PreferencesMode != pmLogin);
    EditorSheet->Enabled = WinConfiguration->ExpertMode && !WinConfiguration->DisableOpenEdit;

    StorageGroup->Visible = WinConfiguration->ExpertMode;
    RandomSeedFileLabel->Visible = WinConfiguration->ExpertMode;
    RandomSeedFileEdit->Visible = WinConfiguration->ExpertMode;

    FCustomCommandList->Assign(WinConfiguration->CustomCommandList);
    UpdateCustomCommandsView();

    PuttyPathEdit->Text = GUIConfiguration->PuttyPath;
    PuttyPasswordCheck2->Checked = GUIConfiguration->PuttyPassword;
    AutoOpenInPuttyCheck->Checked = WinConfiguration->AutoOpenInPutty;
    TelnetForFtpInPuttyCheck->Checked = WinConfiguration->TelnetForFtpInPutty;

    // Queue
    QueueTransferLimitEdit->AsInteger = GUIConfiguration->QueueTransfersLimit;
    QueueAutoPopupCheck->Checked = GUIConfiguration->QueueAutoPopup;
    QueueCheck->Checked = GUIConfiguration->DefaultCopyParam.Queue;
    QueueIndividuallyCheck->Checked = GUIConfiguration->DefaultCopyParam.QueueIndividually;
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

    // window
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
    BOOLPROP(MinimizeToTray);

    // panels
    DoubleClickActionCombo->ItemIndex = WinConfiguration->DoubleClickAction;
    BOOLPROP(AutoReadDirectoryAfterOp);

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

    ComboAutoSwitchLoad(UpdatesBetaVersionsCombo, Updates.BetaVersions);

    switch (Updates.ConnectionType)
    {
      case ctDirect:
      default:
        UpdatesDirectCheck->Checked = true;
        break;

      case ctAuto:
        UpdatesAutoCheck->Checked = true;
        break;

      case ctProxy:
        UpdatesProxyCheck->Checked = true;
        break;
    }

    UpdatesProxyHostEdit->Text = Updates.ProxyHost;
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

    // security
    UseMasterPasswordCheck->Checked = WinConfiguration->UseMasterPassword;

    #undef BOOLPROP
  }
  __finally
  {
    FNoUpdate--;
  }

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
    BOOLPROP(RenameWholeName);
    BOOLPROP(ShowInaccesibleDirectories);
    BOOLPROP(CopyOnDoubleClickConfirmation);
    BOOLPROP(ConfirmTransferring);
    BOOLPROP(ConfirmOverwriting);
    BOOLPROP(ConfirmResume);
    BOOLPROP(ConfirmDeleting);
    BOOLPROP(ConfirmRecycling);
    BOOLPROP(ConfirmClosingSession);
    BOOLPROP(ConfirmExitOnCompletion);
    BOOLPROP(UseLocationProfiles);
    BOOLPROP(ConfirmCommandSession);
    BOOLPROP(ContinueOnError);
    BOOLPROP(DDAllowMoveInit);
    BOOLPROP(BeepOnFinish);
    BOOLPROP(TemporaryDirectoryAppendSession);
    BOOLPROP(TemporaryDirectoryAppendPath);
    BOOLPROP(TemporaryDirectoryCleanup);
    BOOLPROP(ConfirmTemporaryDirectoryCleanup);

    GUIConfiguration->BeepOnFinishAfter =
      static_cast<double>(BeepOnFinishAfterEdit->Value / (24*60*60));
    BOOLPROP(BalloonNotifications);

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
    ScpCommander.TreeOnLeft = TreeOnLeftCheck->Checked;
    WinConfiguration->ScpCommander = ScpCommander;

    TScpExplorerConfiguration ScpExplorer = WinConfiguration->ScpExplorer;
    ScpExplorer.ShowFullAddress = ShowFullAddressCheck->Checked;
    WinConfiguration->ScpExplorer = ScpExplorer;

    Configuration->RandomSeedFile = RandomSeedFileEdit->Text;

    // editor
    WinConfiguration->Editor.WordWrap = EditorWordWrapCheck->Checked;
    WinConfiguration->Editor.TabSize = EditorTabSizeEdit->AsInteger;
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
    GUIConfiguration->SessionReopenAutoIdle =
      (SessionReopenAutoIdleCheck->Checked ? (SessionReopenAutoEdit->Value * 1000) : 0);
    Configuration->SessionReopenTimeout = (SessionReopenTimeoutEdit->Value * 1000);

    WinConfiguration->CustomCommandList = FCustomCommandList;

    GUIConfiguration->PuttyPath = PuttyPathEdit->Text;
    GUIConfiguration->PuttyPassword = PuttyPasswordCheck2->Checked;
    WinConfiguration->AutoOpenInPutty = AutoOpenInPuttyCheck->Checked;
    WinConfiguration->TelnetForFtpInPutty = TelnetForFtpInPuttyCheck->Checked;

    // Queue
    GUIConfiguration->QueueTransfersLimit = QueueTransferLimitEdit->AsInteger;
    GUIConfiguration->QueueAutoPopup = QueueAutoPopupCheck->Checked;
    CopyParam.Queue = QueueCheck->Checked;
    CopyParam.QueueIndividually = QueueIndividuallyCheck->Checked;
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

    // window
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
    BOOLPROP(MinimizeToTray);

    // panels
    WinConfiguration->DoubleClickAction = (TDoubleClickAction)DoubleClickActionCombo->ItemIndex;
    BOOLPROP(AutoReadDirectoryAfterOp);

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

    Updates.BetaVersions = ComboAutoSwitchSave(UpdatesBetaVersionsCombo);

    if (UpdatesDirectCheck->Checked)
    {
      Updates.ConnectionType = ctDirect;
    }
    else if (UpdatesAutoCheck->Checked)
    {
      Updates.ConnectionType = ctAuto;
    }
    else if (UpdatesProxyCheck->Checked)
    {
      if (!UpdatesProxyHostEdit->Text.IsEmpty())
      {
        Updates.ConnectionType = ctProxy;
      }
      else
      {
        Updates.ConnectionType = ctDirect;
      }
    }
    Updates.ProxyHost = UpdatesProxyHostEdit->Text;
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
AnsiString __fastcall TPreferencesDialog::TabSample(AnsiString Values)
{
  AnsiString Result;
  for (int Index = 1; Index <= Values.Length(); Index++)
  {
    if (Index > 1)
    {
      Result += ' ';
      if (EditorTabSizeEdit->AsInteger > 2)
      {
        Result += AnsiString::StringOfChar(' ', EditorTabSizeEdit->AsInteger - 2);
      }
    }

    Result += Values[Index];
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UpdateControls()
{
  if (FNoUpdate == 0)
  {
    EnableControl(BeepOnFinishAfterEdit, BeepOnFinishCheck->Checked);
    EnableControl(BeepOnFinishAfterText, BeepOnFinishCheck->Checked);
    EnableControl(BalloonNotificationsCheck, TTrayIcon::SupportsBalloons());

    EnableControl(ResumeThresholdEdit, ResumeSmartButton->Checked);
    EnableControl(ResumeThresholdUnitLabel, ResumeThresholdEdit->Enabled);
    EnableControl(SessionReopenAutoEdit,
      SessionReopenAutoCheck->Checked || SessionReopenAutoIdleCheck->Checked);
    EnableControl(SessionReopenAutoLabel, SessionReopenAutoEdit->Enabled);
    EnableControl(SessionReopenAutoSecLabel, SessionReopenAutoEdit->Enabled);
    EnableControl(SessionReopenTimeoutEdit, SessionReopenAutoEdit->Enabled);
    EnableControl(SessionReopenTimeoutLabel, SessionReopenAutoEdit->Enabled);
    EnableControl(SessionReopenTimeoutSecLabel, SessionReopenAutoEdit->Enabled);

    EnableControl(CopyOnDoubleClickConfirmationCheck,
      (DoubleClickActionCombo->ItemIndex == 1) && ConfirmTransferringCheck->Checked);

    AnsiString EditorFontLabelText;
    EditorFontLabelText = FMTLOAD(EDITOR_FONT_FMT,
      (FEditorFont->Name, FEditorFont->Size)) + "\n\n";
    EditorFontLabelText += TabSample("ABCD") + "\n";
    EditorFontLabelText += TabSample("1234");
    EditorFontLabel->Caption = EditorFontLabelText;
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
    IniFileStorageButton2->Caption =
      AnsiReplaceStr(IniFileStorageButton2->Caption, "winscp.ini",
        ExtractFileName(ExpandEnvironmentVariables(Configuration->IniFileStorageName)));

    EditorFontLabel->WordWrap = EditorWordWrapCheck->Checked;
    bool EditorSelected = (EditorListView2->Selected != NULL);
    EnableControl(EditEditorButton, EditorSelected);
    EnableControl(RemoveEditorButton, EditorSelected);
    EnableControl(UpEditorButton, EditorSelected &&
      (EditorListView2->ItemIndex > 0));
    EnableControl(DownEditorButton, EditorSelected &&
      (EditorListView2->ItemIndex < EditorListView2->Items->Count - 1));

    EnableControl(UpdatesProxyHostEdit, UpdatesProxyCheck->Checked);
    EnableControl(UpdatesProxyHostLabel, UpdatesProxyHostEdit->Enabled);
    EnableControl(UpdatesProxyPortEdit, UpdatesProxyCheck->Checked);
    EnableControl(UpdatesProxyPortLabel, UpdatesProxyPortEdit->Enabled);

    EnableControl(PuttyPasswordCheck2, !PuttyPathEdit->Text.IsEmpty());
    EnableControl(AutoOpenInPuttyCheck, PuttyPasswordCheck2->Enabled);
    EnableControl(TelnetForFtpInPuttyCheck, PuttyPasswordCheck2->Enabled);

    EnableControl(SetMasterPasswordButton, WinConfiguration->UseMasterPassword);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorFontButtonClick(TObject * /*Sender*/)
{
  if (FontDialog(FEditorFont))
  {
    UpdateControls();
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
    IconName = AppName;
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
        IconName = FMTLOAD(SENDTO_HOOK_NAME, (AppName));
        SpecialFolder = CSIDL_SENDTO;
        Params = "/upload";
      }
      else if (Sender == QuickLaunchIconButton)
      {
        IconName = "Microsoft\\Internet Explorer\\Quick Launch\\" +
          AppName;
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
  assert(FCustomCommandList != NULL);
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FCustomCommandList->Count);
  const TCustomCommandType * Command = FCustomCommandList->Commands[Index];
  AnsiString Caption = StringReplace(Command->Name, "&", "", TReplaceFlags() << rfReplaceAll);
  if (Command->ShortCut != 0)
  {
    Caption = FORMAT("%s (%s)", (Caption, ShortCutToText(Command->ShortCut)));
  }
  Item->Caption = Caption;
  assert(!Item->SubItems->Count);
  Item->SubItems->Add(Command->Command);
  int Params = Command->Params;
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
  CustomCommandsView->Items->Count = FCustomCommandList->Count;
  AdjustListColumnsWidth(CustomCommandsView, FCustomCommandList->Count);
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
  TCustomCommandType Command;

  if (Edit)
  {
    int Index = CustomCommandsView->ItemIndex;
    assert(Index >= 0 && Index <= FCustomCommandList->Count);

    Command = *FCustomCommandList->Commands[Index];
  }

  TShortCuts ShortCuts;
  if (WinConfiguration->SharedBookmarks != NULL)
  {
    WinConfiguration->SharedBookmarks->ShortCuts(ShortCuts);
  }
  FCustomCommandList->ShortCuts(ShortCuts);

  if (DoCustomCommandDialog(Command, FCustomCommandList,
        (Edit ? ccmEdit : ccmAdd), 0, NULL, &ShortCuts))
  {
    int Index = CustomCommandsView->ItemIndex;
    TCustomCommandType * ACommand = new TCustomCommandType(Command);
    if (Edit)
    {
      FCustomCommandList->Change(Index, ACommand);
    }
    else
    {
      if (Index >= 0)
      {
        FCustomCommandList->Insert(Index, ACommand);
      }
      else
      {
        FCustomCommandList->Add(ACommand);
        Index = FCustomCommandList->Count - 1;
      }
    }

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
    CustomCommandsView->ItemIndex < FCustomCommandList->Count);
  FCustomCommandList->Delete(CustomCommandsView->ItemIndex);
  UpdateCustomCommandsView();
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::CustomCommandMove(int Source, int Dest)
{
  if (Source >= 0 && Source < FCustomCommandList->Count &&
      Dest >= 0 && Dest < FCustomCommandList->Count)
  {
    FCustomCommandList->Move(Source, Dest);
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
TListViewScrollOnDragOver * __fastcall TPreferencesDialog::ScrollOnDragOver(TObject * ListView)
{
  if (ListView == CopyParamListView)
  {
    return FCopyParamScrollOnDragOver;
  }
  else if (ListView == CustomCommandsView)
  {
    return FCustomCommandsScrollOnDragOver;
  }
  else if (ListView == EditorListView2)
  {
    return FEditorScrollOnDragOver;
  }
  else
  {
    assert(false);
    return NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ListViewStartDrag(
      TObject * Sender, TDragObject *& /*DragObject*/)
{
  FListViewDragSource = dynamic_cast<TListView*>(Sender)->ItemIndex;
  FListViewDragDest = -1;
  ScrollOnDragOver(Sender)->StartDrag();
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
  TObject * Sender, TObject * Source, int X, int Y,
  TDragState /*State*/, bool & Accept)
{
  if (Source == Sender)
  {
    // cannot use AllowListViewDrag(X, Y) because of bug in VCL
    // (when dropped on item itself, when it was dragged over another item before,
    // that another item remains highlighted forever)
    Accept = true;

    ScrollOnDragOver(Source)->DragOver(TPoint(X, Y));
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
    EditorListView2->ItemIndex = -1;
    EditorListView2->ItemFocused = EditorListView2->Selected;
    EditorListView2->ItemIndex = Dest;
    UpdateEditorListView();
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListView2DragDrop(TObject * Sender,
  TObject * Source, int X, int Y)
{
  if (Source == EditorListView2)
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
  EditorMove(EditorListView2->ItemIndex,
    EditorListView2->ItemIndex + (Sender == UpEditorButton ? -1 : 1));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RemoveEditorButtonClick(
  TObject * /*Sender*/)
{
  assert(EditorListView2->ItemIndex >= 0 &&
    EditorListView2->ItemIndex < FEditorList->Count);
  FEditorList->Delete(EditorListView2->ItemIndex);
  UpdateEditorListView();
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::AddEditEditorButtonClick(TObject * Sender)
{
  TEditorPreferencesMode Mode = (Sender == EditEditorButton ? epmEdit : epmAdd);
  int Index = EditorListView2->ItemIndex;
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
    bool DummyRemember = false;
    if (DoEditorPreferencesDialog(Editor->GetData(), DummyRemember, Mode, true))
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
      EditorListView2->ItemIndex = Index;
      UpdateControls();
    }
  }
  __finally
  {
    delete Editor;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListView2DblClick(TObject * /*Sender*/)
{
  if (EditEditorButton->Enabled)
  {
    AddEditEditorButtonClick(EditEditorButton);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListView2KeyDown(TObject * /*Sender*/,
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
  EditorListView2->Items->Count = FEditorList->Count;
  AdjustListColumnsWidth(EditorListView2, FEditorList->Count);
  EditorListView2->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::EditorListView2Data(TObject * /*Sender*/,
  TListItem * Item)
{
  int Index = Item->Index;
  assert(Index >= 0 && Index <= FEditorList->Count);
  const TEditorPreferences * Editor = FEditorList->Editors[Index];
  Item->Caption = Editor->Data->FileMask.Masks;
  Item->SubItems->Add(Editor->Name);
  if (Editor->Data->Editor == edExternal)
  {
    Item->SubItems->Add(BooleanToStr(Editor->Data->ExternalEditorText));
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
void __fastcall TPreferencesDialog::WMHelp(TWMHelp & Message)
{
  assert(Message.HelpInfo != NULL);

  if (Message.HelpInfo->iContextType == HELPINFO_WINDOW)
  {
    // invoke help for active page (not for whole form), regardless of focus
    // (e.g. even if focus is on control outside pagecontrol)
    Message.HelpInfo->hItemHandle = PageControl->ActivePage->Handle;
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
  else if (M->Msg == WM_HELP)
  {
    WMHelp(*((TWMHelp *)Message));
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
  FormHelp(this);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PuttyPathBrowseButtonClick(
  TObject * /*Sender*/)
{
  BrowseForExecutable(PuttyPathEdit, LoadStr(PREFERENCES_SELECT_PUTTY),
    LoadStr(PREFERENCES_PUTTY_FILTER), false, false);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PuttyPathResetButtonClick(
  TObject * /*Sender*/)
{
  PuttyPathEdit->Text = WinConfiguration->DefaultPuttyPath;
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ExportButtonClick(TObject * /*Sender*/)
{
  AnsiString PersonalDirectory;
  ::SpecialFolderLocation(CSIDL_PERSONAL, PersonalDirectory);
  AnsiString FileName = IncludeTrailingBackslash(PersonalDirectory) +
    ExtractFileName(ExpandEnvironmentVariables(Configuration->IniFileStorageName));
  if (SaveDialog(LoadStr(EXPORT_CONF_TITLE), LoadStr(EXPORT_CONF_FILTER), "ini", FileName))
  {
    Configuration->Export(FileName);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PathEditBeforeDialog(
  TObject * /*Sender*/, AnsiString & Name, bool & /*Action*/)
{
  FBeforeDialogPath = Name;
  Name = ExpandEnvironmentVariables(Name);
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::PathEditAfterDialog(
  TObject * /*Sender*/, AnsiString & Name, bool & /*Action*/)
{
  if (CompareFileName(Name, ExpandEnvironmentVariables(FBeforeDialogPath)))
  {
    Name = FBeforeDialogPath;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::NavigationTreeCollapsing(
  TObject * /*Sender*/, TTreeNode * /*Node*/, bool & AllowCollapse)
{
  AllowCollapse = false;
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::ListViewEndDrag(
  TObject * Sender, TObject * /*Target*/, int /*X*/, int /*Y*/)
{
  ScrollOnDragOver(Sender)->EndDrag();
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::RandomSeedFileEditCreateEditDialog(
  TObject * Sender, TFileDialogKind DialogKind, TOpenDialog *& Dialog)
{
  USEDPARAM(DialogKind);
  assert(DialogKind == dkOpen);
  Dialog = CreateOpenDialog(dynamic_cast<TComponent *>(Sender));
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::SessionReopenTimeoutEditSetValue(
  TObject * /*Sender*/, Extended Value, AnsiString & Text, bool & Handled)
{
  if (Value == 0)
  {
    Text = LoadStr(PREFERENCES_RECONNECT_TIMEOUT_UNLIMITED);
    Handled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::SessionReopenTimeoutEditGetValue(
  TObject * /*Sender*/, AnsiString Text, Extended & Value, bool & Handled)
{
  if (AnsiSameText(Text, LoadStr(PREFERENCES_RECONNECT_TIMEOUT_UNLIMITED)))
  {
    Value = 0;
    Handled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::UseMasterPasswordCheckClick(
  TObject * /*Sender*/)
{
  if (UseMasterPasswordCheck->Checked != WinConfiguration->UseMasterPassword)
  {
    try
    {
      if (UseMasterPasswordCheck->Checked)
      {
        if (DoChangeMasterPasswordDialog())
        {
          MessageDialog(LoadStr(MASTER_PASSWORD_SET), qtInformation, qaOK, HELP_MASTER_PASSWORD);
        }
      }
      else
      {
        if (DoMasterPasswordDialog())
        {
          WinConfiguration->ClearMasterPassword();
          MessageDialog(LoadStr(MASTER_PASSWORD_CLEARED), qtInformation, qaOK, HELP_MASTER_PASSWORD);
        }
      }
    }
    __finally
    {
      UseMasterPasswordCheck->Checked = WinConfiguration->UseMasterPassword;
      UpdateControls();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPreferencesDialog::SetMasterPasswordButtonClick(
  TObject * /*Sender*/)
{
  DoChangeMasterPasswordDialog();
  MessageDialog(LoadStr(MASTER_PASSWORD_CHANGED), qtInformation, qaOK, HELP_MASTER_PASSWORD);
}
//---------------------------------------------------------------------------
