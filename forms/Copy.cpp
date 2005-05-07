//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Common.h>
#include <WinInterface.h>
#include <ScpMain.h>
#include <TextsWin.h>
#include <VCLCommon.h>
#include <CustomWinConfiguration.h>

#include "Copy.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MoreButton"
#pragma link "Rights"
#pragma link "CopyParams"
#pragma link "HistoryComboBox"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
bool __fastcall DoCopyDialog(bool ToRemote,
  bool Move, TStrings * FileList, AnsiString & TargetDirectory,
  TGUICopyParamType * Params, int Options, int * OutputOptions)
{
  bool Result;
  TCopyDialog *CopyDialog = new TCopyDialog(Application);
  try
  {
    if ((Options & coDisableTransferMode) != 0)
    {
      // If local and remote EOL types are the same, there is no need
      // for ASCII (or Automatic) mode
      Params->TransferMode = tmBinary;
    }
    CopyDialog->ToRemote = ToRemote;
    CopyDialog->Options = Options;
    CopyDialog->Directory = TargetDirectory;
    CopyDialog->FileList = FileList;
    CopyDialog->Params = *Params;
    CopyDialog->Move = Move;
    if (OutputOptions != NULL)
    {
      CopyDialog->OutputOptions = *OutputOptions;
    }
    Result = CopyDialog->Execute();
    if (Result)
    {
      TargetDirectory = CopyDialog->Directory;
      *Params = CopyDialog->Params;
      if (OutputOptions != NULL)
      {
        *OutputOptions = CopyDialog->OutputOptions;
      }
    }
  }
  __finally
  {
    delete CopyDialog;
  }
  return Result;
}
//---------------------------------------------------------------------------
__fastcall TCopyDialog::TCopyDialog(TComponent* Owner)
        : TForm(Owner)
{
  // on start set different value than we want to allow property-setter to proceed
  FToRemote = false;
  FMove = true;

  ToRemote = true;
  Move = false;
  FOptions = 0;
  FOutputOptions = 0;
  FPresetsMenu = new TPopupMenu(this);

  UseSystemSettings(this);
}
//---------------------------------------------------------------------------
__fastcall TCopyDialog::~TCopyDialog()
{
  delete FPresetsMenu;
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::AdjustControls()
{
  if (FLAGSET(Options, coDoNotShowAgain))
  {
    SaveSettingsCheck->Caption = LoadStr(NEVER_SHOW_DIALOG_AGAIN);
  }
  RemoteDirectoryEdit->Visible = false;
  LocalDirectoryEdit->Visible = false;
  DirectoryEdit->Visible = FLAGCLEAR(Options, coTemp);
  EnableControl(DirectoryEdit, FLAGCLEAR(Options, coDisableDirectory));
  EnableControl(DirectoryLabel, DirectoryEdit->Enabled);
  EnableControl(LocalDirectoryBrowseButton, DirectoryEdit->Enabled);
  DirectoryLabel->FocusControl = DirectoryEdit;
  CopyParamsFrame->Direction = !ToRemote ? pdToLocal : pdToRemote;
  CopyParamsFrame->Options =
    FLAGMASK(FLAGCLEAR(Options, coDisableTransferMode), cfAllowTransferMode) |
    FLAGMASK(!Move, cfAllowExcludeMask) |
    FLAGMASK(!Move && ToRemote, cfAllowClearArchive);
  EnableControl(NewerOnlyCheck, FLAGCLEAR(Options, coDisableNewerOnly));

  if (FileList && FileList->Count)
  {
    AnsiString TransferStr = LoadStr(!Move ? COPY_COPY : COPY_MOVE);
    // currently the copy dialog is shown when downloading to temp folder
    // only for drag&drop downloads, for we dare to display d&d specific prompt
    AnsiString DirectionStr =
      LoadStr(((Options & coTemp) != 0) ? COPY_TODROP :
        (ToRemote ? COPY_TOREMOTE : COPY_TOLOCAL));

    if (FileList->Count == 1)
    {
      AnsiString FileName;
      if (!ToRemote) FileName = UnixExtractFileName(FFileList->Strings[0]);
        else FileName = ExtractFileName(FFileList->Strings[0]);
      DirectoryLabel->Caption = FMTLOAD(COPY_FILE,
        (TransferStr, FileName, DirectionStr));
    }
    else
    {
      DirectoryLabel->Caption = FMTLOAD(COPY_FILES,
        (TransferStr, FFileList->Count, DirectionStr));
    }
  }

  if (!Move)
  {
    Caption = LoadStr(COPY_COPY_CAPTION);
    CopyButton->Caption = LoadStr(COPY_COPY_BUTTON);
  }
  else
  {
    Caption = LoadStr(COPY_MOVE_CAPTION);
    CopyButton->Caption = LoadStr(COPY_MOVE_BUTTON);
  }

  LocalDirectoryBrowseButton->Visible = !ToRemote &&
    ((Options & coTemp) == 0);

  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetToRemote(bool value)
{
  if (FToRemote != value)
  {
    AnsiString ADirectory = DirectoryEdit->Text;
    FToRemote = value;
    DirectoryEdit->Text = ADirectory;

    AdjustControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetOptions(int value)
{
  if (Options != value)
  {
    FOptions = value;

    AdjustControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetOutputOptions(int value)
{
  if (OutputOptions != value)
  {
    SaveSettingsCheck->Checked = FLAGSET(FOutputOptions, cooDoNotShowAgain);
    FOutputOptions = (value & ~cooDoNotShowAgain);
  }
}
//---------------------------------------------------------------------------
int __fastcall TCopyDialog::GetOutputOptions()
{
  return FOutputOptions |
    FLAGMASK(SaveSettingsCheck->Checked, cooDoNotShowAgain);
}
//---------------------------------------------------------------------------
THistoryComboBox * __fastcall TCopyDialog::GetDirectoryEdit()
{
  return ToRemote ? RemoteDirectoryEdit : LocalDirectoryEdit;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCopyDialog::GetFileMask()
{
  return ToRemote ? UnixExtractFileName(DirectoryEdit->Text) :
    ExtractFileName(DirectoryEdit->Text);
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetParams(const TGUICopyParamType & value)
{
  FParams = value;
  CopyParamsFrame->Params = value;
  DirectoryEdit->Text = Directory + FParams.FileMask;
  QueueCheck->Checked = FParams.Queue;
  QueueNoConfirmationCheck->Checked = FParams.QueueNoConfirmation;
  NewerOnlyCheck->Checked = FLAGCLEAR(Options, coDisableNewerOnly) && FParams.NewerOnly;
}
//---------------------------------------------------------------------------
TGUICopyParamType __fastcall TCopyDialog::GetParams()
{
  // overwrites TCopyParamType files only
  FParams = CopyParamsFrame->Params;
  FParams.FileMask = GetFileMask();
  FParams.Queue = QueueCheck->Checked;
  FParams.QueueNoConfirmation = QueueNoConfirmationCheck->Checked;
  FParams.NewerOnly = FLAGCLEAR(Options, coDisableNewerOnly) && NewerOnlyCheck->Checked;
  return FParams;
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetDirectory(AnsiString value)
{
  if (!value.IsEmpty())
  {
    value = ToRemote ? UnixIncludeTrailingBackslash(value) :
      IncludeTrailingBackslash(value);
  }
  DirectoryEdit->Text = value + GetFileMask();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCopyDialog::GetDirectory()
{
  assert(DirectoryEdit);

  AnsiString Result = DirectoryEdit->Text;
  if (ToRemote)
  {
    Result = UnixExtractFilePath(Result);
    if (!Result.IsEmpty())
    {
      Result = UnixIncludeTrailingBackslash(Result);
    }
  }
  else
  {
    Result = ExtractFilePath(Result);
    if (!Result.IsEmpty())
    {
      Result = IncludeTrailingBackslash(Result);
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetFileList(TStrings * value)
{
  if (FFileList != value)
  {
    FFileList = value;
    AdjustControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::UpdateControls()
{
  EnableControl(QueueCheck,
    (Options & (coDisableQueue | coTemp)) == 0);
  EnableControl(QueueNoConfirmationCheck,
    ((Options & coTemp) == 0) && QueueCheck->Checked);
  QueueNoConfirmationCheck->Visible = MoreButton->Expanded;
  EnableControl(SaveSettingsCheck, FLAGCLEAR(Options, coDisableSaveSettings));
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::SetMove(bool value)
{
  if (Move != value)
  {
    FMove = value;
    AdjustControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::FormShow(TObject * /*Sender*/)
{
  assert(FileList && (FileList->Count > 0));
  if (DirectoryEdit->Enabled && DirectoryEdit->Visible)
  {
    DirectoryEdit->SetFocus();
  }
  else
  if (MoreButton->Expanded)
  {
    MorePanel->SetFocus();
  }
  else
  {
    CopyButton->SetFocus();
  }
  UpdateControls();

  InstallPathWordBreakProc(RemoteDirectoryEdit);
  InstallPathWordBreakProc(LocalDirectoryEdit);
}
//---------------------------------------------------------------------------
bool __fastcall TCopyDialog::Execute()
{
  // at start assume that copy param is current preset
  FPreset = GUIConfiguration->CopyParamCurrent;
  DirectoryEdit->Items = CustomWinConfiguration->History[
    ToRemote ? "RemoteTarget" : "LocalTarget"];
  MoreButton->Expanded = GUIConfiguration->CopyParamDialogExpanded;
  CopyParamsFrame->BeforeExecute();
  bool Result = (ShowModal() == mrOk);
  if (Result)
  {
    CopyParamsFrame->AfterExecute();
    Configuration->BeginUpdate();
    try
    {
      GUIConfiguration->CopyParamDialogExpanded = MoreButton->Expanded;
      if (FLAGSET(OutputOptions, cooSaveSettings) &&
          FLAGCLEAR(Options, coDisableSaveSettings))
      {
        GUIConfiguration->DefaultCopyParam = Params;
      }
      DirectoryEdit->SaveToHistory();
      CustomWinConfiguration->History[ToRemote ?
        "RemoteTarget" : "LocalTarget"] = DirectoryEdit->Items;
    }
    __finally
    {
      Configuration->EndUpdate();
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::FormCloseQuery(TObject * /*Sender*/,
      bool &CanClose)
{
  if (ModalResult != mrCancel)
  {
    if (!ToRemote && ((Options & coTemp) == 0))
    {
      AnsiString Dir = Directory;
      AnsiString Drive = ExtractFileDrive(Dir);
      if (!DirectoryExists(Dir))
      {
        if (MessageDialog(FMTLOAD(CREATE_LOCAL_DIRECTORY, (Dir)),
              qtConfirmation, qaOK | qaCancel, HELP_NONE) != qaCancel)
        {
          if (!ForceDirectories(Dir))
          {
            SimpleErrorDialog(FMTLOAD(CREATE_LOCAL_DIR_ERROR, (Dir)));
            CanClose = false;
          }
        }
        else
        {
          CanClose = False;
        }
      }

      if (!CanClose)
      {
        DirectoryEdit->SelectAll();
        DirectoryEdit->SetFocus();
      }
    };

    if (CanClose)
    {
      CopyParamsFrame->Validate();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::LocalDirectoryBrowseButtonClick(
  TObject * /*Sender*/)
{
  assert(!ToRemote);
  AnsiString Directory = LocalDirectoryEdit->Text;
  if (SelectDirectory(Directory, LoadStr(SELECT_LOCAL_DIRECTORY), true))
  {
    LocalDirectoryEdit->Text = Directory;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::ControlChange(TObject * /*Sender*/)
{
  UpdateControls();
  ResetSystemSettings(this);
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::PresetsButtonClick(TObject * /*Sender*/)
{
  TCopyParamType Param = Params;
  CopyParamListPopup(
    PresetsButton->ClientToScreen(TPoint(0, PresetsButton->Height)),
    FPresetsMenu, Params, FPreset, CopyParamClick, cplNone);
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::CopyParamClick(TObject * Sender)
{
  TCopyParamType Param = Params;
  if (CopyParamListPopupClick(Sender, Param, FPreset))
  {
    Params = Param;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyDialog::HelpButtonClick(TObject * /*Sender*/)
{
  FormHelp(this);
}
//---------------------------------------------------------------------------
