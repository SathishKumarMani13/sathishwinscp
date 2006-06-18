//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <WinInterface.h>
#include "SynchronizeProgress.h"
#include <Common.h>
#include <Configuration.h>
#include <ScpMain.h>
#include <TextsWin.h>
#include <VCLCommon.h>
#include <GUITools.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PathLabel"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TSynchronizeProgressForm::TSynchronizeProgressForm(TComponent * Owner,
  bool AllowMinimize, bool CompareOnly)
  : TForm(Owner)
{
  FStarted = false;
  FCanceled = false;
  FElapsed = EncodeTime(0, 0, 0, 0);
  FShowAsModalStorage = NULL;
  FMinimizedByMe = false;
  FCompareOnly = CompareOnly;
  if (!AllowMinimize)
  {
    CancelButton->Left = CancelButton->Left +
      (MinimizeButton->Left - CancelButton->Left) / 2;
    MinimizeButton->Visible = false;
  }
}
//---------------------------------------------------------------------------
__fastcall TSynchronizeProgressForm::~TSynchronizeProgressForm()
{
  if (IsIconic(Application->Handle) && FMinimizedByMe)
  {
    Application->Restore();
    Application->BringToFront();
  }

  ReleaseAsModal(this, FShowAsModalStorage);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::Start()
{
  FStarted = true;
  FStartTime = Now();
  UpdateTimer->Enabled = true;
  StartTimeLabel->Caption = FStartTime.TimeString();
  Caption = LoadStr(FCompareOnly ? SYNCHRONIZE_PROGRESS_COMPARE : SYNCHRONIZE_PROGRESS_SYNCHRONIZE);
  ShowAsModal(this, FShowAsModalStorage);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::Stop()
{
  HideAsModal(this, FShowAsModalStorage);
  FStarted = false;
  UpdateTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::SetData(const AnsiString LocalDirectory,
  const AnsiString RemoteDirectory, bool & Continue)
{
  assert(FStarted);
  LocalDirectoryLabel->Caption = LocalDirectory;
  RemoteDirectoryLabel->Caption = RemoteDirectory;
  Continue = !FCanceled;
  
  UpdateControls();
  Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::UpdateControls()
{
  if (FStarted)
  {
    FElapsed = Now() - FStartTime;
  }
  TimeElapsedLabel->Caption = FormatDateTimeSpan(Configuration->TimeFormat, FElapsed);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::CancelButtonClick(TObject * /*Sender*/)
{
  if (!FCanceled && (MessageDialog(LoadStr(CANCEL_OPERATION), qtConfirmation,
       qaOK | qaCancel, HELP_NONE) == qaOK))
  {
    FCanceled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::UpdateTimerTimer(TObject * /*Sender*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::MinimizeButtonClick(
  TObject * /*Sender*/)
{
  Application->Minimize();
  FMinimizedByMe = true;
}
//---------------------------------------------------------------------------

