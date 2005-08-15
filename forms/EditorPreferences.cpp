//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Common.h>
#include <WinConfiguration.h>
#include <WinInterface.h>
#include <VCLCommon.h>
#include <TextsWin.h>
#include <Tools.h>
#include <ScpMain.h>
#include "EditorPreferences.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XPThemes"
#pragma link "HistoryComboBox"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
bool __fastcall DoEditorPreferencesDialog(TEditorPreferences * Editor,
  TEditorPreferencesMode Mode)
{
  bool Result;
  TEditorPreferencesDialog * Dialog = new TEditorPreferencesDialog(Application, Mode);
  try
  {
    Result = Dialog->Execute(Editor);
  }
  __finally
  {
    delete Dialog;
  }
  return Result;
}
//---------------------------------------------------------------------------
__fastcall TEditorPreferencesDialog::TEditorPreferencesDialog(
  TComponent * Owner, TEditorPreferencesMode Mode) : 
  TForm(Owner)
{
  UseSystemSettings(this);

  FMode = Mode;

  Caption = LoadStr(Mode == epmEdit ? EDITOR_EDIT : EDITOR_ADD);

  InstallPathWordBreakProc(ExternalEditorEdit);
}
//---------------------------------------------------------------------------
bool __fastcall TEditorPreferencesDialog::Execute(TEditorPreferences * Editor)
{
  EditorInternalButton->Checked = (Editor->Data.Editor == edInternal);
  EditorExternalButton->Checked = (Editor->Data.Editor == edExternal);
  AnsiString ExternalEditor = Editor->Data.ExternalEditor;
  if (!ExternalEditor.IsEmpty())
  {
    TWinConfiguration::ReformatFileNameCommand(ExternalEditor);
  }
  ExternalEditorEdit->Text = ExternalEditor;
  ExternalEditorEdit->Items = CustomWinConfiguration->History["ExternalEditor"];
  MaskEdit->Text = Editor->Data.FileMask.Masks;
  MaskEdit->Items = CustomWinConfiguration->History["Mask"];
  ExternalEditorTextCheck->Checked = Editor->Data.ExternalEditorText;
  MDIExternalEditorCheck->Checked = Editor->Data.MDIExternalEditor;

  bool Result = (ShowModal() == mrOk);

  if (Result)
  {
    Editor->Data.Editor = (EditorInternalButton->Checked ? edInternal : edExternal);
    Editor->Data.ExternalEditor = ExternalEditorEdit->Text;
    ExternalEditorEdit->SaveToHistory();
    CustomWinConfiguration->History["ExternalEditor"] = ExternalEditorEdit->Items;
    Editor->Data.FileMask = MaskEdit->Text;
    MaskEdit->SaveToHistory();
    CustomWinConfiguration->History["Mask"] = MaskEdit->Items;
    Editor->Data.ExternalEditorText = ExternalEditorTextCheck->Checked;
    Editor->Data.MDIExternalEditor = MDIExternalEditorCheck->Checked;
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::ExternalEditorEditChange(
  TObject * Sender)
{
  // duplicated in TPreferencesDialog::FilenameEditChange
  if (FAfterFilenameEditDialog)
  {
    FAfterFilenameEditDialog = false;
    ExternalEditorEditExit(Sender);
  }
  else
  {
    ControlChange(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::ExternalEditorEditExit(
  TObject * Sender)
{
  // duplicated in TPreferencesDialog::FilenameEditExit
  THistoryComboBox * FilenameEdit = dynamic_cast<THistoryComboBox *>(Sender);
  try
  {
    AnsiString Filename = FilenameEdit->Text;
    if (!Filename.IsEmpty())
    {
      TWinConfiguration::ReformatFileNameCommand(Filename);
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
void __fastcall TEditorPreferencesDialog::ExternalEditorBrowseButtonClick(
  TObject * /*Sender*/)
{
  AnsiString ExternalEditor, Program, Params, Dir;
  ExternalEditor = ExternalEditorEdit->Text;
  TWinConfiguration::ReformatFileNameCommand(ExternalEditor);
  SplitCommand(ExternalEditor, Program, Params, Dir);

  TOpenDialog * FileDialog = new TOpenDialog(this);
  try
  {
    FileDialog->FileName = Program;
    FileDialog->Filter = LoadStr(PREFERENCES_EXTERNAL_EDITOR_FILTER);
    FileDialog->Title = LoadStr(PREFERENCES_SELECT_EXTERNAL_EDITOR);

    if (FileDialog->Execute())
    {
      FAfterFilenameEditDialog = true;
      ExternalEditorEdit->Text = FormatCommand(FileDialog->FileName, Params);
      FAfterFilenameEditDialog = false;
      ExternalEditorEditExit(ExternalEditorEdit);
    }
  }
  __finally
  {
    delete FileDialog;
  }
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::HelpButtonClick(TObject * /*Sender*/)
{
  FormHelp(this);
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::ControlChange(TObject * /*Sender*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::UpdateControls()
{
  EnableControl(OkButton,
    EditorInternalButton->Checked || !ExternalEditorEdit->Text.IsEmpty());
  EnableControl(ExternalEditorEdit, EditorExternalButton->Checked);  
  EnableControl(ExternalEditorBrowseButton, EditorExternalButton->Checked);  
  EnableControl(ExternalEditorGroup, EditorExternalButton->Checked);  
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::FormCloseQuery(TObject * /*Sender*/,
  bool & /*CanClose*/)
{
  if (ModalResult != mrCancel)
  {
    ExitActiveControl(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TEditorPreferencesDialog::MaskEditExit(TObject * /*Sender*/)
{
  ValidateMaskEdit(MaskEdit);
}
//---------------------------------------------------------------------------

