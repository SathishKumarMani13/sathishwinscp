//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TerminalManager.h"
#include "CustomScpExplorer.h"
#include "LogMemo.h"
#include "NonVisual.h"
#include "WinConfiguration.h"
#include <Log.h>
#include <OperationStatus.h>
#include <Common.h>
#include <ScpMain.h>
#include <TextsWin.h>
#include <Progress.h>
#include <Queue.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TTerminalManager * TTerminalManager::FInstance = NULL;
//---------------------------------------------------------------------------
TTerminalManager * __fastcall TTerminalManager::Instance(bool ForceCreation)
{
  if (!FInstance && ForceCreation)
  {
    FInstance = new TTerminalManager();
  }
  return FInstance;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::DestroyInstance()
{
  assert(FInstance);
  SAFE_DESTROY(FInstance);
}
//---------------------------------------------------------------------------
__fastcall TTerminalManager::TTerminalManager() :
  TTerminalList(Configuration)
{
  FLogMemo = NULL;
  FActiveTerminal = NULL;
  FScpExplorer = NULL;
  FDestroying = false;
  FTerminalPendingAction = tpNull;
  FDirectoryReadingStart = 0;

  assert(Application && !Application->OnException);
  Application->OnException = ApplicationException;
  assert(Application->OnShowHint == NULL);
  Application->OnShowHint = ApplicationShowHint;

  assert(Configuration && !Configuration->OnChange);
  Configuration->OnChange = ConfigurationChange;
  FOnLastTerminalClosed = NULL;
  FOnTerminalListChanged = NULL;
  FOnTerminalClosed = NULL;

  FTerminalList = new TStringList();
  FQueues = new TList();
}
//---------------------------------------------------------------------------
__fastcall TTerminalManager::~TTerminalManager()
{
  FreeAll();

  assert(!ScpExplorer);

  assert(Configuration->OnChange == ConfigurationChange);
  Configuration->OnChange = NULL;

  assert(Application && (Application->OnException == ApplicationException));
  Application->OnException = NULL;
  assert(Application->OnShowHint == ApplicationShowHint);
  Application->OnShowHint = ApplicationShowHint;

  delete FQueues;
  delete FTerminalList;
}
//---------------------------------------------------------------------------
TTerminal * __fastcall TTerminalManager::NewTerminal(TSessionData * Data)
{
  FTerminalList->Clear();
  TTerminal * Terminal = TTerminalList::NewTerminal(Data);
  try
  {
    TTerminalQueue * Queue = new TTerminalQueue(Terminal, Configuration);
    Queue->TransfersLimit = GUIConfiguration->QueueTransfersLimit;
    Queue->OnQueryUser = TerminalQueryUser;
    Queue->OnPromptUser = TerminalPromptUser;
    Queue->OnShowExtendedException = TerminalShowExtendedException;
    FQueues->Add(Queue);

    Terminal->OnQueryUser = TerminalQueryUser;
    Terminal->OnPromptUser = TerminalPromptUser;
    Terminal->OnShowExtendedException = TerminalShowExtendedException;
    Terminal->OnProgress = OperationProgress;
    Terminal->OnFinished = OperationFinished;
    Terminal->OnDeleteLocalFile = DeleteLocalFile;
    Terminal->OnReadDirectoryProgress = TerminalReadDirectoryProgress;

    if (!ActiveTerminal)
    {
      ActiveTerminal = Terminal;
    }
  }
  catch(...)
  {
    if (Terminal != NULL)
    {
      FreeTerminal(Terminal);
    }
    throw;
  }

  if (OnTerminalListChanged)
  {
    OnTerminalListChanged(this);
  }
  return Terminal;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::FreeActiveTerminal()
{
  if (FTerminalPendingAction == tpNull)
  {
    assert(ActiveTerminal);
    FreeTerminal(ActiveTerminal);
  }
  else
  {
    assert(FTerminalPendingAction == tpNone);
    FTerminalPendingAction = tpFree;
  }
}
//---------------------------------------------------------------------------
void TTerminalManager::ConnectTerminal(TTerminal * Terminal)
{
  TOperationStatusForm * Form = new TOperationStatusForm(Application);
  Busy(true);
  try
  {
    Form->SecureShell = Terminal;
    Form->ShowAsModal();
    Terminal->Open();
    Terminal->DoStartup();
  }
  __finally
  {
    Busy(false);
    delete Form;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TTerminalManager::ConnectActiveTerminal()
{
  TTerminalPendingAction Action;
  bool Result;
  do
  {
    Action = tpNull;
    Result = false;
    try
    {
      assert(ActiveTerminal);
      bool ShowLogPending = false;

      if (Configuration->Logging && (WinConfiguration->LogView == lvWindow))
      {
        if (WinConfiguration->LogWindowOnStartup)
        {
          RequireLogForm(LogMemo);
        }
        else
        {
          ShowLogPending = true;
        }
      }

      ConnectTerminal(ActiveTerminal);

      if (ScpExplorer)
      {
        assert(ActiveTerminal->Status == sshReady);
        TerminalReady();
      }

      WinConfiguration->ClearTemporaryLoginData();

      if (LogForm && (WinConfiguration->LogView != lvWindow))
      {
        FreeLogForm();
      }

      if (ShowLogPending)
      {
        RequireLogForm(LogMemo);
      }
      Result = true;
    }
    catch(Exception & E)
    {
      assert(FTerminalPendingAction == tpNull);
      FTerminalPendingAction = tpNone;
      try
      {
        assert(ActiveTerminal != NULL);
        ActiveTerminal->DoShowExtendedException(&E);
        Action = FTerminalPendingAction;
      }
      __finally
      {
        FTerminalPendingAction = tpNull;
      }
    }
  }
  while (Action == tpReconnect);

  if (Action == tpFree)
  {
    FreeActiveTerminal();
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::ReconnectActiveTerminal()
{
  assert(ActiveTerminal);

  TTerminal * Terminal;
  if (ScpExplorer)
  {
    TSessionData * Data = new TSessionData(ActiveTerminal->SessionData->Name);
    try
    {
      Data->Assign(ActiveTerminal->SessionData);
      if (ScpExplorer->Terminal == ActiveTerminal)
      {
        ScpExplorer->UpdateSessionData(Data);
      }
      Terminal = NewTerminal(Data);
    }
    __finally
    {
      delete Data;
    }
  }
  else
  {
    // otherwise the ScpExplorer would be created already
    assert(ActiveTerminal->Status < sshReady);
    Terminal = NewTerminal(ActiveTerminal->SessionData);
  }

  try
  {
    FreeTerminal(ActiveTerminal);
    ActiveTerminal = Terminal;
    if (FTerminalPendingAction == tpNull)
    {
      ConnectActiveTerminal();
    }
    else
    {
      FTerminalPendingAction = tpReconnect;
    }
  }
  catch(...)
  {
    FreeTerminal(Terminal);
    throw;
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::FreeAll()
{
  FDestroying = true;
  try
  {
    while (Count)
    {
      FreeTerminal(Terminals[0]);
    }
  }
  __finally
  {
    FDestroying = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::FreeTerminal(TTerminal * Terminal)
{
  try
  {
    Terminal->Active = false;

    if (OnTerminalClosed)
    {
      OnTerminalClosed(Terminal);
    }
  }
  __finally
  {
    int Index = IndexOf(Terminal);
    FTerminalList->Clear();
    Extract(Terminal);
    
    TTerminalQueue * Queue;
    Queue = reinterpret_cast<TTerminalQueue *>(FQueues->Items[Index]);
    FQueues->Delete(Index);
    delete Queue;
    
    if (ActiveTerminal && (Terminal == ActiveTerminal))
    {
      if ((Count > 0) && !FDestroying)
      {
        for (int i = 0; i < Count; i++)
        {
          if (Terminals[i]->Status == sshReady)
          {
            ActiveTerminal = Terminals[i];
            break;
          }
        }
        if (ActiveTerminal == Terminal)
        {
          ActiveTerminal = Terminals[Index];
        }
      }
      else
      {
        ActiveTerminal = NULL;
      }
    }
    else
    {
      SaveTerminal(Terminal);
    }

    delete Terminal;

    if (OnTerminalListChanged)
    {
      OnTerminalListChanged(this);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::SetScpExplorer(TCustomScpExplorerForm * value)
{
  if (ScpExplorer != value)
  {
    // changing explorer is not supported yet
    assert(!ScpExplorer || !value);
    FScpExplorer = value;
    if (FScpExplorer)
    {
      assert(!OnChangeTerminal);
      FScpExplorer->Terminal = ActiveTerminal;
      FScpExplorer->Queue = ActiveQueue;
      FOnLastTerminalClosed = FScpExplorer->LastTerminalClosed;
      FOnTerminalListChanged = FScpExplorer->TerminalListChanged;
      FOnTerminalClosed = FScpExplorer->TerminalClosed;
    }
    else
    {
      FOnLastTerminalClosed = NULL;
      FOnTerminalListChanged = NULL;
      FOnTerminalClosed = NULL;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::SetActiveTerminal(TTerminal * value)
{
  if (ActiveTerminal != value)
  {
    if (ActiveTerminal && ScpExplorer)
    {
      assert(!ScpExplorer->Terminal || (ScpExplorer->Terminal == ActiveTerminal));
      if (ScpExplorer->Terminal == ActiveTerminal)
      {
        ScpExplorer->UpdateSessionData();
      }
    }

    TTerminal * PActiveTerminal = ActiveTerminal;
    FActiveTerminal = NULL;
    if (OnChangeTerminal)
    {
      OnChangeTerminal(this);
    }
    FActiveTerminal = value;
    // moved from else block of next if (ActiveTerminal) statement
    // so ScpExplorer can update its caption
    UpdateAppTitle();
    if (ScpExplorer)
    {
      if (ActiveTerminal && (ActiveTerminal->Status == sshReady))
      {
        TerminalReady();
      }
      else
      {
        ScpExplorer->Terminal = NULL;
        ScpExplorer->Queue = NULL;
      }
    }

    if (PActiveTerminal && !PActiveTerminal->Active)
    {
      SaveTerminal(PActiveTerminal);
    }

    if (ActiveTerminal)
    {
      if (!PActiveTerminal)
      {
        CreateLogMemo();
      }
      assert(LogMemo);
      LogMemo->SessionLog = ActiveTerminal->Log;
    }
    else
    {
      if (LogForm)
      {
        FreeLogForm();
      }
      FreeLogMemo();
      if (OnLastTerminalClosed)
      {
        OnLastTerminalClosed(this);
      }
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTerminalManager::UpdateAppTitle()
{
  AnsiString NewTitle;
  if (ActiveTerminal)
  {
    NewTitle = FMTLOAD(APP_CAPTION, (ActiveTerminalTitle, AppName));
  }
  else
  {
    NewTitle = AppName;
  }

  if (!FProgressTitle.IsEmpty())
  {
    NewTitle = FProgressTitle + " - " + NewTitle;
  }
  else if (ActiveTerminal && (ScpExplorer != NULL))
  {
    AnsiString Path = ScpExplorer->PathForCaption();
    if (!Path.IsEmpty())
    {
      NewTitle = Path + " - " + NewTitle;
    }
  }

  Application->Title = NewTitle;
  return NewTitle;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::SaveTerminal(TTerminal * Terminal)
{
  if (!Terminal->SessionData->Name.IsEmpty())
  {
    TSessionData * Data;
    Data = (TSessionData *)StoredSessions->FindByName(Terminal->SessionData->Name);
    if (Data)
    {
      bool Changed = false;
      if (Terminal->SessionData->UpdateDirectories)
      {
        Data->LocalDirectory = Terminal->SessionData->LocalDirectory;
        Data->RemoteDirectory = Terminal->SessionData->RemoteDirectory;
        Changed = true;
      }

      if (Changed)
      {
        StoredSessions->Save();
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::CreateLogMemo()
{
  assert(!FLogMemo);
  assert(ActiveTerminal);
  FLogMemo = new TLogMemo(Application);
  try
  {
    FLogMemo->SessionLog = ActiveTerminal->Log;
    FLogMemo->PopupMenu = NonVisualDataModule->LogMemoPopup;
  }
  catch (...)
  {
    delete FLogMemo;
    throw;
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::FreeLogMemo()
{
  assert(LogMemo);
  LogMemo->PopupMenu = NULL;
  SAFE_DESTROY(FLogMemo);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::ApplicationException(TObject * /*Sender*/,
  Exception * E)
{
  ShowExtendedExceptionEx(ActiveTerminal, E);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::ApplicationShowHint(AnsiString & HintStr,
  bool & /*CanShow*/, THintInfo & HintInfo)
{
  TLabel * HintLabel = dynamic_cast<TLabel *>(HintInfo.HintControl);
  if ((HintLabel != NULL) && (HintLabel->Caption == HintStr))
  {
    HintInfo.HintPos = HintLabel->ClientToScreen(TPoint(0, 0));
    HintInfo.HintMaxWidth = HintLabel->Width;
    HintInfo.HideTimeout = 100000; // "almost" never
  }
  else
  {
    HintInfo.HintMaxWidth = 300;
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::DeleteLocalFile(const AnsiString FileName)
{
  if (!RecursiveDeleteFile(FileName, WinConfiguration->DeleteToRecycleBin))
  {
    throw Exception(FMTLOAD(DELETE_LOCAL_FILE_ERROR, (FileName)));
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::TerminalQueryUser(TObject * /*Sender*/,
  const AnsiString Query, TStrings * MoreMessages, int Answers,
  const TQueryParams * Params, int & Answer, TQueryType Type, void * /*Arg*/)
{
  AnsiString HelpKeyword;
  TMessageParams MessageParams(Params);
  AnsiString AQuery = Query;

  if (Params != NULL)
  {
    HelpKeyword = Params->HelpKeyword;

    if (FLAGSET(Params->Params, qpFatalAbort))
    {
      AQuery = FMTLOAD(WARN_FATAL_ERROR, (AQuery));

      if (!MessageParams.TimerMessage.IsEmpty())
      {
        MessageParams.TimerMessage = FMTLOAD(WARN_FATAL_ERROR, (MessageParams.TimerMessage));
      }
    }
  }

  if (ScpExplorer)
  {
    Answer = ScpExplorer->MoreMessageDialog(AQuery, MoreMessages, Type, Answers,
      HelpKeyword, &MessageParams);
  }
  else
  {
    Answer = MoreMessageDialog(AQuery, MoreMessages, Type, Answers, HelpKeyword,
      &MessageParams);
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::TerminalPromptUser(
  TSecureShell * /*SecureShell*/, AnsiString Prompt, TPromptKind Kind,
  AnsiString & Response, bool & Result, void * /*Arg*/)
{
  Result = DoPasswordDialog(Prompt, Kind, Response);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::TerminalShowExtendedException(
  TSecureShell * SecureShell, Exception * E, void * /*Arg*/)
{
  ShowExtendedExceptionEx(SecureShell, E);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::TerminalReadDirectoryProgress(
  TObject * /*Sender*/, int Progress)
{
  static TDateTime DirectoryReadingProgressDelay(0, 0, 1, 500);

  if (Progress == 0)
  {
    FDirectoryReadingStart = Now();
    if (!FProgressTitle.IsEmpty())
    {
      FProgressTitle = "";
      UpdateAppTitle();
    }
  }
  else if (Progress < 0)
  {
    FProgressTitle = "";
    UpdateAppTitle();
  }
  else if ((Now() - FDirectoryReadingStart) >= DirectoryReadingProgressDelay)
  {
    FProgressTitle = FMTLOAD(DIRECTORY_READING_PROGRESS, (Progress));    
    UpdateAppTitle();
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::OperationFinished(::TFileOperation Operation,
  TOperationSide Side, bool Temp, const AnsiString FileName, bool Success,
  bool & DisconnectWhenFinished)
{
  assert(ScpExplorer);
  ScpExplorer->OperationFinished(Operation, Side, Temp, FileName, Success,
    DisconnectWhenFinished);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::OperationProgress(
  TFileOperationProgressType & ProgressData, TCancelStatus & Cancel)
{
  if (ProgressData.InProgress)
  {
    FProgressTitle = FORMAT("%d%% %s",
      (ProgressData.OverallProgress(),
       TProgressForm::OperationName(ProgressData.Operation)));
  }
  else
  {
    FProgressTitle = "";
  }

  UpdateAppTitle();
  assert(ScpExplorer);
  ScpExplorer->OperationProgress(ProgressData, Cancel);
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::ConfigurationChange(TObject * /*Sender*/)
{
  assert(Configuration);
  assert(Configuration == WinConfiguration);

  if (!Application->Terminated && Configuration->Logging &&
      (WinConfiguration->LogView == lvWindow))
  {
    if (ActiveTerminal)
    {
      RequireLogForm(LogMemo);
    }
  }
    else
  {
    FreeLogForm();
  }

  TTerminalQueue * Queue;
  for (int Index = 0; Index < Count; Index++)
  {
    assert(Terminals[Index]->Log);
    Terminals[Index]->Log->ReflectSettings();
    Queue = reinterpret_cast<TTerminalQueue *>(FQueues->Items[Index]);
    Queue->TransfersLimit = GUIConfiguration->QueueTransfersLimit;
  }

  if (ScpExplorer)
  {
    ScpExplorer->ConfigurationChanged();
  }
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::TerminalReady()
{
  ScpExplorer->Terminal = ActiveTerminal;
  ScpExplorer->Queue = ActiveQueue;
  if (OnChangeTerminal)
  {
    OnChangeTerminal(this);
  }
}
//---------------------------------------------------------------------------
TStrings * __fastcall TTerminalManager::GetTerminalList()
{
  if (FTerminalList->Count != Count)
  {
    for (int i = 0; i < Count; i++)
    {
      AnsiString NameN;
      AnsiString Name = Terminals[i]->SessionData->SessionName;
      int Number = 1;
      NameN = Name;
      while (FTerminalList->IndexOf(NameN) >= 0)
      {
        Number++;
        NameN = FORMAT("%s (%d)", (Name, Number));
      }
      if (Number > 1)
      {
        Name = FORMAT("%s (%d)", (Name, Number));
      }
      FTerminalList->AddObject(Name, Terminals[i]);
    }
  }
  return FTerminalList;
}
//---------------------------------------------------------------------------
int __fastcall TTerminalManager::GetActiveTerminalIndex()
{
  return ActiveTerminal ? IndexOf(ActiveTerminal) : -1;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::SetActiveTerminalIndex(int value)
{
  ActiveTerminal = Terminals[value];
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTerminalManager::GetActiveTerminalTitle()
{
  AnsiString Result = ActiveTerminal ?
    TerminalList->Strings[IndexOf(ActiveTerminal)] : AnsiString("");
  return Result;
}
//---------------------------------------------------------------------------
TTerminalQueue * __fastcall TTerminalManager::GetActiveQueue()
{
  TTerminalQueue * Result = NULL;
  if (ActiveTerminal != NULL)
  {
    Result = reinterpret_cast<TTerminalQueue *>(FQueues->Items[ActiveTerminalIndex]);
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TTerminalManager::CycleTerminals(bool Forward)
{
  int Index = ActiveTerminalIndex;
  Index += Forward ? 1 : -1;
  if (Index < 0)
  {
    Index = Count-1;
  }
  else if (Index >= Count)
  {
    Index = 0;
  }
  ActiveTerminalIndex = Index;
}

