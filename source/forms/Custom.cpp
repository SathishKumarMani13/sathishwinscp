//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Dialogs.hpp>
//---------------------------------------------------------------------
#include <Common.h>
#include <CustomWinConfiguration.h>
#include <WinInterface.h>
#include <VCLCommon.h>
#include <TextsWin.h>
#include <HelpWin.h>
#include <CoreMain.h>

#include "Custom.h"
//---------------------------------------------------------------------
#pragma link "PasswordEdit"
#ifndef NO_RESOURCES
#pragma resource "*.dfm"
#endif
//---------------------------------------------------------------------
__fastcall TCustomDialog::TCustomDialog(UnicodeString AHelpKeyword)
  : TForm(GetFormOwner())
{
  UseSystemSettings(this);

  FPos = 8;

  HelpKeyword = AHelpKeyword;

  TBorderIcons BI = BorderIcons;
  if (HelpKeyword.IsEmpty())
  {
    BI >> biHelp;

    OKButton->Left = CancelButton->Left;
    CancelButton->Left = HelpButton->Left;
    HelpButton->Visible = false;
  }
  else
  {
    BI << biHelp;
  }
  BorderIcons = BI;
}
//---------------------------------------------------------------------
bool __fastcall TCustomDialog::Execute()
{
  Changed();
  return (ShowModal() == mrOk);
}
//---------------------------------------------------------------------
void __fastcall TCustomDialog::DoChange(bool & /*CanSubmit*/)
{
  // noop
}
//---------------------------------------------------------------------
void __fastcall TCustomDialog::Changed()
{
  bool CanSubmit = true;
  DoChange(CanSubmit);
  EnableControl(OKButton, CanSubmit);
}
//---------------------------------------------------------------------
void __fastcall TCustomDialog::Change(TObject * /*Sender*/)
{
  Changed();
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::HelpButtonClick(TObject * /*Sender*/)
{
  FormHelp(this);
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::DoShow()
{
  OKButton->TabOrder = FCount;
  CancelButton->TabOrder = static_cast<short>(FCount + 1);
  HelpButton->TabOrder = static_cast<short>(FCount + 2);
  Changed();
  TForm::DoShow();
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::DoValidate()
{
  // noop
}
//---------------------------------------------------------------------------
bool __fastcall TCustomDialog::CloseQuery()
{
  if (ModalResult == mrOk)
  {
    DoValidate();
  }
  return TForm::CloseQuery();
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::AddWinControl(TWinControl * Control)
{
  Control->TabOrder = FCount;
  FCount++;
}
//---------------------------------------------------------------------------
TLabel * __fastcall TCustomDialog::CreateLabel(UnicodeString Label)
{
  TLabel * Result = new TLabel(this);
  Result->Caption = Label;
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::AddEditLikeControl(TWinControl * Edit, TLabel * Label)
{
  int PrePos = FPos;
  Label->Parent = this;
  Label->Left = 8;
  Label->Top = FPos;
  FPos += 16;

  Edit->Parent = this;
  Edit->Left = 8;
  Edit->Top = FPos;
  Edit->Width = ClientWidth - (Edit->Left * 2);
  // this updates Height property to real value
  Edit->HandleNeeded();
  FPos += Edit->Height + 8;

  if (Label->FocusControl == NULL)
  {
    Label->FocusControl = Edit;
  }
  else
  {
    assert(Label->FocusControl == Edit);
  }

  ClientHeight = ClientHeight + (FPos - PrePos);

  AddWinControl(Edit);
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::AddEdit(TCustomEdit * Edit, TLabel * Label)
{
  AddEditLikeControl(Edit, Label);

  TEdit * PublicEdit = reinterpret_cast<TEdit *>(Edit);
  if (PublicEdit->OnChange == NULL)
  {
    PublicEdit->OnChange = Change;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::AddComboBox(TCustomCombo * Combo, TLabel * Label)
{
  AddEditLikeControl(Combo, Label);

  TComboBox * PublicCombo = reinterpret_cast<TComboBox *>(Combo);
  if (PublicCombo->OnChange == NULL)
  {
    PublicCombo->OnChange = Change;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCustomDialog::AddButtonControl(TButtonControl * Control)
{
  int PrePos = FPos;
  Control->Parent = this;
  Control->Left = 14;
  Control->Top = FPos;
  Control->Width = ClientWidth - Control->Left - 8;
  // this updates Height property to real value
  Control->HandleNeeded();
  FPos += Control->Height + 8;

  ClientHeight = ClientHeight + (FPos - PrePos);

  AddWinControl(Control);

  TCheckBox * PublicControl = reinterpret_cast<TCheckBox *>(Control);
  if (PublicControl->OnClick == NULL)
  {
    PublicControl->OnClick = Change;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TSaveSessionDialog : public TCustomDialog
{
public:
  __fastcall TSaveSessionDialog(TSessionData * OriginalSession, bool CanSavePassword, bool NotRecommendedSavingPassword);

  bool __fastcall Execute(UnicodeString & SessionName, bool & SavePassword);

protected:
  DYNAMIC void __fastcall DoShow();
  virtual void __fastcall DoValidate();
  virtual void __fastcall DoChange(bool & CanSubmit);

private:
  TSessionData * FOriginalSession;
  TComboBox * SessionNameCombo;
  TCheckBox * SavePasswordCheck;
};
//---------------------------------------------------------------------------
__fastcall TSaveSessionDialog::TSaveSessionDialog(
    TSessionData * OriginalSession, bool CanSavePassword, bool NotRecommendedSavingPassword) :
  TCustomDialog(HELP_SESSION_SAVE),
  FOriginalSession(OriginalSession)
{
  Caption = LoadStr(SAVE_SESSION_CAPTION);

  SessionNameCombo = new TComboBox(this);
  SessionNameCombo->AutoComplete = false;
  AddComboBox(SessionNameCombo, CreateLabel(LoadStr(SAVE_SESSION_PROMPT)));
  SessionNameCombo->Items->BeginUpdate();
  try
  {
    for (int Index = 0; Index < StoredSessions->Count; Index++)
    {
      TSessionData * Data = StoredSessions->Sessions[Index];
      if (!Data->Special)
      {
        SessionNameCombo->Items->Add(Data->Name);
      }
    }
  }
  __finally
  {
    SessionNameCombo->Items->EndUpdate();
  }

  SavePasswordCheck = new TCheckBox(this);
  SavePasswordCheck->Caption = LoadStr(
    NotRecommendedSavingPassword ? SAVE_SESSION_PASSWORD :
      (CustomWinConfiguration->UseMasterPassword ? SAVE_SESSION_PASSWORD_MASTER : SAVE_SESSION_PASSWORD_RECOMMENDED));
  AddButtonControl(SavePasswordCheck);

  EnableControl(SavePasswordCheck, CanSavePassword);
}
//---------------------------------------------------------------------------
bool __fastcall TSaveSessionDialog::Execute(UnicodeString & SessionName, bool & SavePassword)
{
  SessionNameCombo->Text = SessionName;
  SavePasswordCheck->Checked = SavePassword;
  bool Result = TCustomDialog::Execute();
  if (Result)
  {
    SessionName = SessionNameCombo->Text;
    SavePassword = SavePasswordCheck->Checked;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TSaveSessionDialog::DoShow()
{
  InstallPathWordBreakProc(SessionNameCombo);

  int P = SessionNameCombo->Text.LastDelimiter(L"/");
  if (P > 0)
  {
    SessionNameCombo->SetFocus();
    SessionNameCombo->SelStart = P;
    SessionNameCombo->SelLength = SessionNameCombo->Text.Length() - P;
  }
  TCustomDialog::DoShow();
}
//---------------------------------------------------------------------------
void __fastcall TSaveSessionDialog::DoValidate()
{
  SessionNameValidate(SessionNameCombo->Text, FOriginalSession);

  UnicodeString Folder = UnixExtractFileDir(SessionNameCombo->Text);
  if (!Folder.IsEmpty() && StoredSessions->IsWorkspace(Folder))
  {
    throw Exception(FMTLOAD(WORKSPACE_NOT_FOLDER, (Folder)));
  }

  if (SavePasswordCheck->Enabled && SavePasswordCheck->Checked &&
      CustomWinConfiguration->UseMasterPassword)
  {
    CustomWinConfiguration->AskForMasterPasswordIfNotSet();
  }

  TCustomDialog::DoValidate();
}
//---------------------------------------------------------------------------
void __fastcall TSaveSessionDialog::DoChange(bool & CanSubmit)
{
  CanSubmit = !SessionNameCombo->Text.IsEmpty();
  TCustomDialog::DoChange(CanSubmit);
}
//---------------------------------------------------------------------------
bool __fastcall DoSaveSessionDialog(UnicodeString & SessionName,
  bool * SavePassword, TSessionData * OriginalSession, bool NotRecommendedSavingPassword)
{
  bool Result;
  TSaveSessionDialog * Dialog = new TSaveSessionDialog(
    OriginalSession, (SavePassword != NULL), NotRecommendedSavingPassword);
  try
  {
    bool Dummy = false;
    if (SavePassword == NULL)
    {
      SavePassword = &Dummy;
    }
    Result = Dialog->Execute(SessionName, *SavePassword);
  }
  __finally
  {
    delete Dialog;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall SessionNameValidate(const UnicodeString & Text,
  TSessionData * RenamingSession)
{
  TSessionData::ValidatePath(Text);

  assert(StoredSessions);
  TSessionData * Data = (TSessionData *)StoredSessions->FindByName(Text);
  if (Data && Data->Special)
  {
    MessageDialog(FMTLOAD(CANNOT_OVERWRITE_SPECIAL_SESSION, (Text)),
      qtError, qaOK, HELP_NONE);
    Abort();
  }
  else if (Data && (Data != RenamingSession) &&
    MessageDialog(FMTLOAD(CONFIRM_OVERWRITE_SESSION, (Text)),
      qtConfirmation, qaYes | qaNo, HELP_SESSION_SAVE_OVERWRITE) != qaYes)
  {
    Abort();
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TSaveWorkspaceDialog : public TCustomDialog
{
public:
  __fastcall TSaveWorkspaceDialog(bool CanSavePasswords,
    bool NotRecommendedSavingPasswords);

  bool __fastcall Execute(
    UnicodeString & WorkspaceName, bool & SavePasswords, bool & CreateShortcut,
    bool & EnableAutoSave);

protected:
  virtual void __fastcall DoValidate();
  virtual void __fastcall DoChange(bool & CanSubmit);

private:
  TComboBox * WorkspaceNameCombo;
  TCheckBox * SavePasswordsCheck;
  TCheckBox * CreateShortcutCheck;
  TCheckBox * EnableAutoSaveCheck;
};
//---------------------------------------------------------------------------
__fastcall TSaveWorkspaceDialog::TSaveWorkspaceDialog(
    bool CanSavePasswords, bool NotRecommendedSavingPasswords) :
  TCustomDialog(HELP_WORKSPACE_SAVE)
{
  Caption = LoadStr(SAVE_WORKSPACE_CAPTION);

  WorkspaceNameCombo = new TComboBox(this);
  WorkspaceNameCombo->AutoComplete = false;
  AddComboBox(WorkspaceNameCombo, CreateLabel(LoadStr(SAVE_WORKSPACE_PROMPT)));

  std::auto_ptr<TStrings> Workspaces(StoredSessions->GetWorkspaces());
  WorkspaceNameCombo->Items->AddStrings(Workspaces.get());

  SavePasswordsCheck = new TCheckBox(this);
  SavePasswordsCheck->Caption = LoadStr(
    NotRecommendedSavingPasswords ? SAVE_WORKSPACE_PASSWORDS :
      (CustomWinConfiguration->UseMasterPassword ?
        SAVE_WORKSPACE_PASSWORDS_MASTER : SAVE_WORKSPACE_PASSWORDS_RECOMMENDED));
  AddButtonControl(SavePasswordsCheck);

  EnableControl(SavePasswordsCheck, CanSavePasswords);

  CreateShortcutCheck = new TCheckBox(this);
  CreateShortcutCheck->Caption = LoadStr(SAVE_WORKSPACE_SHORTCUT);
  AddButtonControl(CreateShortcutCheck);

  EnableAutoSaveCheck = new TCheckBox(this);
  EnableAutoSaveCheck->Caption = LoadStr(SAVE_WORKSPACE_AUTO);
  AddButtonControl(EnableAutoSaveCheck);
}
//---------------------------------------------------------------------------
bool __fastcall TSaveWorkspaceDialog::Execute(
  UnicodeString & WorkspaceName, bool & SavePasswords, bool & CreateShortcut,
  bool & EnableAutoSave)
{
  WorkspaceNameCombo->Text = WorkspaceName;
  SavePasswordsCheck->Checked = SavePasswords;
  CreateShortcutCheck->Checked = CreateShortcut;
  EnableAutoSaveCheck->Checked = EnableAutoSave;
  bool Result = TCustomDialog::Execute();
  if (Result)
  {
    WorkspaceName = WorkspaceNameCombo->Text;
    SavePasswords = SavePasswordsCheck->Checked;
    CreateShortcut = CreateShortcutCheck->Checked;
    EnableAutoSave = EnableAutoSaveCheck->Checked;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TSaveWorkspaceDialog::DoValidate()
{
  TSessionData::ValidateName(WorkspaceNameCombo->Text);

  if (StoredSessions->IsFolder(WorkspaceNameCombo->Text))
  {
    throw Exception(FMTLOAD(FOLDER_NOT_WORKSPACE, (WorkspaceNameCombo->Text)));
  }

  if (SavePasswordsCheck->Enabled && SavePasswordsCheck->Checked &&
      CustomWinConfiguration->UseMasterPassword)
  {
    CustomWinConfiguration->AskForMasterPasswordIfNotSet();
  }

  TCustomDialog::DoValidate();
}
//---------------------------------------------------------------------------
void __fastcall TSaveWorkspaceDialog::DoChange(bool & CanSubmit)
{
  CanSubmit = !WorkspaceNameCombo->Text.IsEmpty();

  TCustomDialog::DoChange(CanSubmit);
}
//---------------------------------------------------------------------------
bool __fastcall DoSaveWorkspaceDialog(UnicodeString & WorkspaceName,
  bool * SavePasswords, bool NotRecommendedSavingPasswords,
  bool & CreateShortcut, bool & EnableAutoSave)
{
  std::auto_ptr<TSaveWorkspaceDialog> Dialog(
    new TSaveWorkspaceDialog((SavePasswords != NULL), NotRecommendedSavingPasswords));

  bool Dummy = false;
  if (SavePasswords == NULL)
  {
    SavePasswords = &Dummy;
  }
  return
    Dialog->Execute(
      WorkspaceName, *SavePasswords, CreateShortcut, EnableAutoSave);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TShortCutDialog : public TCustomDialog
{
public:
  __fastcall TShortCutDialog(const TShortCuts & ShortCuts, UnicodeString HelpKeyword);

  bool __fastcall Execute(TShortCut & ShortCut);

private:
  TComboBox * ShortCutCombo;
};
//---------------------------------------------------------------------------
__fastcall TShortCutDialog::TShortCutDialog(const TShortCuts & ShortCuts, UnicodeString HelpKeyword) :
  TCustomDialog(HelpKeyword)
{
  Caption = LoadStr(SHORTCUT_CAPTION);

  ShortCutCombo = new TComboBox(this);
  AddComboBox(ShortCutCombo, CreateLabel(LoadStr(SHORTCUT_LABEL)));
  InitializeShortCutCombo(ShortCutCombo, ShortCuts);
}
//---------------------------------------------------------------------------
bool __fastcall TShortCutDialog::Execute(TShortCut & ShortCut)
{
  SetShortCutCombo(ShortCutCombo, ShortCut);
  bool Result = TCustomDialog::Execute();
  if (Result)
  {
    ShortCut = GetShortCutCombo(ShortCutCombo);
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall DoShortCutDialog(TShortCut & ShortCut,
  const TShortCuts & ShortCuts, UnicodeString HelpKeyword)
{
  bool Result;
  TShortCutDialog * Dialog = new TShortCutDialog(ShortCuts, HelpKeyword);
  try
  {
    Result = Dialog->Execute(ShortCut);
  }
  __finally
  {
    delete Dialog;
  }
  return Result;
}
