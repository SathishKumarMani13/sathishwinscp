//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SynchronizeProgress.h"
#include <Common.h>
#include <Configuration.h>
#include <ScpMain.h>
#include <TextsWin.h>
#include <WinInterface.h>
#include <VCLCommon.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PathLabel"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TSynchronizeProgressForm::TSynchronizeProgressForm(TComponent* Owner)
  : TForm(Owner)
{
  FStarted = false;
  FCanceled = false;
  FElapsed = EncodeTime(0, 0, 0, 0);
  FShowAsModalStorage = NULL;
}
//---------------------------------------------------------------------------
__fastcall TSynchronizeProgressForm::~TSynchronizeProgressForm()
{
  ReleaseAsModal(this, FShowAsModalStorage);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::Start()
{
  FStarted = true;
  FStartTime = Now();
  UpdateTimer->Enabled = true;
  StartTimeLabel->Caption = FStartTime.TimeString();
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
  TimeElapsedLabel->Caption = FormatDateTime(Configuration->TimeFormat, FElapsed);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeProgressForm::CancelButtonClick(TObject * /*Sender*/)
{
  if (!FCanceled && (MessageDialog(LoadStr(CANCEL_OPERATION), qtConfirmation,
       qaOK | qaCancel, 0) == qaOK))
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

