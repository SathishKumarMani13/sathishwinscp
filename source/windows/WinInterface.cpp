//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <shlwapi.h>

#include <Common.h>
#include <Exceptions.h>
#include <CoreMain.h>
#include <TextsCore.h>
#include <TextsWin.h>
#include <HelpWin.h>
#include <HelpCore.h>
#include <Interface.h>
#include <VCLCommon.h>
#include <Glyphs.h>
#include <PasTools.hpp>
#include <DateUtils.hpp>

#include "WinInterface.h"
#include "CustomWinConfiguration.h"
#include "GUITools.h"
#include "JclDebug.hpp"
#include "JclHookExcept.hpp"
#include <StrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define WM_TRAY_ICON (WM_WINSCP_USER + 5)
//---------------------------------------------------------------------
TNotifyEvent GlobalOnMinimize = NULL;
//---------------------------------------------------------------------
void __fastcall FormHelp(TCustomForm * Form)
{
  InvokeHelp(Form->ActiveControl != NULL ? Form->ActiveControl : Form);
}
//---------------------------------------------------------------------------
TMessageParams::TMessageParams(unsigned int AParams)
{
  Reset();
  Params = AParams;
}
//---------------------------------------------------------------------------
TMessageParams::TMessageParams(const TQueryParams * AParams)
{
  Reset();

  if (AParams != NULL)
  {
    Aliases = AParams->Aliases;
    AliasesCount = AParams->AliasesCount;
    Timer = AParams->Timer;
    TimerEvent = AParams->TimerEvent;
    TimerMessage = AParams->TimerMessage;
    TimerAnswers = AParams->TimerAnswers;
    TimerQueryType = AParams->TimerQueryType;
    Timeout = AParams->Timeout;
    TimeoutAnswer = AParams->TimeoutAnswer;

    if (FLAGSET(AParams->Params, qpNeverAskAgainCheck))
    {
      Params |= mpNeverAskAgainCheck;
    }
    if (FLAGSET(AParams->Params, qpAllowContinueOnError))
    {
      Params |= mpAllowContinueOnError;
    }
  }
}
//---------------------------------------------------------------------------
inline void TMessageParams::Reset()
{
  Params = 0;
  Aliases = NULL;
  AliasesCount = 0;
  Timer = 0;
  TimerEvent = NULL;
  TimerMessage = L"";
  TimerAnswers = 0;
  TimerQueryType = static_cast<TQueryType>(-1);
  Timeout = 0;
  TimeoutAnswer = 0;
  NeverAskAgainTitle = L"";
  NeverAskAgainAnswer = 0;
  NeverAskAgainCheckedInitially = false;
  AllowHelp = true;
  ImageName = L"";
  MoreMessagesUrl = L"";
  MoreMessagesSize = TSize();
}
//---------------------------------------------------------------------------
static bool __fastcall IsPositiveAnswer(unsigned int Answer)
{
  return (Answer == qaYes) || (Answer == qaOK) || (Answer == qaYesToAll);
}
//---------------------------------------------------------------------------
static void __fastcall NeverAskAgainCheckClick(void * /*Data*/, TObject * Sender)
{
  TCheckBox * CheckBox = dynamic_cast<TCheckBox *>(Sender);
  assert(CheckBox != NULL);
  TForm * Dialog = dynamic_cast<TForm *>(CheckBox->Owner);
  assert(Dialog != NULL);

  unsigned int PositiveAnswer = 0;

  if (CheckBox->Checked)
  {
    if (CheckBox->Tag > 0)
    {
      PositiveAnswer = CheckBox->Tag;
    }
    else
    {
      for (int ii = 0; ii < Dialog->ControlCount; ii++)
      {
        TButton * Button = dynamic_cast<TButton *>(Dialog->Controls[ii]);
        if (Button != NULL)
        {
          if (IsPositiveAnswer(Button->ModalResult))
          {
            PositiveAnswer = Button->ModalResult;
            break;
          }
        }
      }
    }

    assert(PositiveAnswer != 0);
  }

  for (int ii = 0; ii < Dialog->ControlCount; ii++)
  {
    TButton * Button = dynamic_cast<TButton *>(Dialog->Controls[ii]);
    if (Button != NULL)
    {
      if ((Button->ModalResult != 0) && (Button->ModalResult != static_cast<int>(qaCancel)))
      {
        Button->Enabled = !CheckBox->Checked || (Button->ModalResult == static_cast<int>(PositiveAnswer));
      }

      if (Button->DropDownMenu != NULL)
      {
        for (int iii = 0; iii < Button->DropDownMenu->Items->Count; iii++)
        {
          TMenuItem * Item = Button->DropDownMenu->Items->Items[iii];
          Item->Enabled = Item->Default || !CheckBox->Checked;
        }
      }
    }
  }
}
//---------------------------------------------------------------------------
static TCheckBox * __fastcall FindNeverAskAgainCheck(TForm * Dialog)
{
  return NOT_NULL(dynamic_cast<TCheckBox *>(Dialog->FindComponent(L"NeverAskAgainCheck")));
}
//---------------------------------------------------------------------------
TForm * __fastcall CreateMessageDialogEx(const UnicodeString Msg,
  TStrings * MoreMessages, TQueryType Type, unsigned int Answers, UnicodeString HelpKeyword,
  const TMessageParams * Params, TButton *& TimeoutButton)
{
  TMsgDlgType DlgType;
  switch (Type) {
    case qtConfirmation: DlgType = mtConfirmation; break;
    case qtInformation: DlgType = mtInformation; break;
    case qtError: DlgType = mtError; break;
    case qtWarning: DlgType = mtWarning; break;
    default: FAIL;
  }

  unsigned int TimeoutAnswer = (Params != NULL) ? Params->TimeoutAnswer : 0;

  unsigned int ActualAnswers = Answers;
  if ((Params == NULL) || Params->AllowHelp)
  {
    Answers = Answers | qaHelp;
  }

  if (IsInternalErrorHelpKeyword(HelpKeyword))
  {
    Answers = Answers | qaReport;
  }

  if ((MoreMessages != NULL) && (MoreMessages->Count == 0))
  {
    MoreMessages = NULL;
  }

  UnicodeString ImageName;
  UnicodeString MoreMessagesUrl;
  TSize MoreMessagesSize;
  if (Params != NULL)
  {
    ImageName = Params->ImageName;
    MoreMessagesUrl = Params->MoreMessagesUrl;
    MoreMessagesSize = Params->MoreMessagesSize;
  }

  const TQueryButtonAlias * Aliases = (Params != NULL) ? Params->Aliases : NULL;
  unsigned int AliasesCount = (Params != NULL) ? Params->AliasesCount : 0;

  UnicodeString NeverAskAgainCaption;
  bool HasNeverAskAgain = (Params != NULL) && FLAGSET(Params->Params, mpNeverAskAgainCheck);
  if (HasNeverAskAgain)
  {
    NeverAskAgainCaption =
      !Params->NeverAskAgainTitle.IsEmpty() ?
        (UnicodeString)Params->NeverAskAgainTitle :
        // qaOK | qaIgnore is used, when custom "non-answer" button is required
        LoadStr(((ActualAnswers == qaOK) || (ActualAnswers == (qaOK | qaIgnore))) ?
          NEVER_SHOW_AGAIN : NEVER_ASK_AGAIN);
  }

  TForm * Dialog = CreateMoreMessageDialog(Msg, MoreMessages, DlgType, Answers,
    Aliases, AliasesCount, TimeoutAnswer, &TimeoutButton, ImageName, NeverAskAgainCaption,
    MoreMessagesUrl, MoreMessagesSize);

  try
  {
    if (HasNeverAskAgain && ALWAYS_TRUE(Params != NULL))
    {
      TCheckBox * NeverAskAgainCheck = FindNeverAskAgainCheck(Dialog);
      NeverAskAgainCheck->Checked = Params->NeverAskAgainCheckedInitially;
      if (Params->NeverAskAgainAnswer > 0)
      {
        NeverAskAgainCheck->Tag = Params->NeverAskAgainAnswer;
      }
      TNotifyEvent OnClick;
      ((TMethod*)&OnClick)->Code = NeverAskAgainCheckClick;
      NeverAskAgainCheck->OnClick = OnClick;
    }

    Dialog->HelpKeyword = HelpKeyword;
    if (FLAGSET(Answers, qaHelp))
    {
      Dialog->BorderIcons = Dialog->BorderIcons << biHelp;
    }
    ResetSystemSettings(Dialog);
  }
  catch(...)
  {
    delete Dialog;
    throw;
  }
  return Dialog;
}
//---------------------------------------------------------------------------
unsigned int __fastcall ExecuteMessageDialog(TForm * Dialog, unsigned int Answers, const TMessageParams * Params)
{
  FlashOnBackground();
  unsigned int Answer = Dialog->ShowModal();
  // mrCancel is returned always when X button is pressed, despite
  // no Cancel button was on the dialog. Find valid "cancel" answer.
  // mrNone is retuned when Windows session is closing (log off)
  if ((Answer == mrCancel) || (Answer == mrNone))
  {
    Answer = CancelAnswer(Answers);
  }

  if ((Params != NULL) && (Params->Params & mpNeverAskAgainCheck))
  {
    TCheckBox * NeverAskAgainCheck = FindNeverAskAgainCheck(Dialog);

    if (NeverAskAgainCheck->Checked)
    {
      bool PositiveAnswer =
        (Params->NeverAskAgainAnswer > 0) ?
          (Answer == Params->NeverAskAgainAnswer) :
          IsPositiveAnswer(Answer);
      if (PositiveAnswer)
      {
        Answer = qaNeverAskAgain;
      }
    }
  }

  return Answer;
}
//---------------------------------------------------------------------------
class TMessageTimer : public TTimer
{
public:
  TQueryParamsTimerEvent Event;
  TForm * Dialog;

  __fastcall TMessageTimer(TComponent * AOwner);

protected:
  void __fastcall DoTimer(TObject * Sender);
};
//---------------------------------------------------------------------------
__fastcall TMessageTimer::TMessageTimer(TComponent * AOwner) : TTimer(AOwner)
{
  Event = NULL;
  OnTimer = DoTimer;
  Dialog = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMessageTimer::DoTimer(TObject * /*Sender*/)
{
  if (Event != NULL)
  {
    unsigned int Result = 0;
    Event(Result);
    if (Result != 0)
    {
      Dialog->ModalResult = Result;
    }
  }
}
//---------------------------------------------------------------------------
class TMessageTimeout : public TTimer
{
public:
  __fastcall TMessageTimeout(TComponent * AOwner, unsigned int Timeout,
    TButton * Button);

  void __fastcall Suspend();
  void __fastcall Cancel();

protected:
  unsigned int FOrigTimeout;
  unsigned int FTimeout;
  TButton * FButton;
  UnicodeString FOrigCaption;

  void __fastcall DoTimer(TObject * Sender);
  void __fastcall UpdateButton();
};
//---------------------------------------------------------------------------
__fastcall TMessageTimeout::TMessageTimeout(TComponent * AOwner,
  unsigned int Timeout, TButton * Button) :
  TTimer(AOwner), FOrigTimeout(Timeout), FTimeout(Timeout), FButton(Button)
{
  OnTimer = DoTimer;
  Interval = MSecsPerSec;
  FOrigCaption = FButton->Caption;
  UpdateButton();
}
//---------------------------------------------------------------------------
void __fastcall TMessageTimeout::Suspend()
{
  const unsigned int SuspendTime = 30 * MSecsPerSec;
  FTimeout = std::max(FOrigTimeout, SuspendTime);
  UpdateButton();
}
//---------------------------------------------------------------------------
void __fastcall TMessageTimeout::Cancel()
{
  Enabled = false;
  UpdateButton();
}
//---------------------------------------------------------------------------
void __fastcall TMessageTimeout::UpdateButton()
{
  assert(FButton != NULL);
  FButton->Caption =
    !Enabled ? FOrigCaption : FMTLOAD(TIMEOUT_BUTTON, (FOrigCaption, int(FTimeout / MSecsPerSec)));
}
//---------------------------------------------------------------------------
void __fastcall TMessageTimeout::DoTimer(TObject * /*Sender*/)
{
  if (FTimeout <= Interval)
  {
    assert(FButton != NULL);
    TForm * Dialog = dynamic_cast<TForm *>(FButton->Parent);
    assert(Dialog != NULL);

    Dialog->ModalResult = FButton->ModalResult;
  }
  else
  {
    FTimeout -= Interval;
    UpdateButton();
  }
}
//---------------------------------------------------------------------
class TPublicControl : public TControl
{
friend void __fastcall MenuPopup(TObject * Sender, const TPoint & MousePos, bool & Handled);
friend void __fastcall SetTimeoutEvents(TControl * Control, TMessageTimeout * Timeout);
};
//---------------------------------------------------------------------
class TPublicWinControl : public TWinControl
{
friend void __fastcall SetTimeoutEvents(TControl * Control, TMessageTimeout * Timeout);
};
//---------------------------------------------------------------------------
static void __fastcall MessageDialogMouseMove(void * Data, TObject * /*Sender*/,
  TShiftState /*Shift*/, int /*X*/, int /*Y*/)
{
  assert(Data != NULL);
  TMessageTimeout * Timeout = static_cast<TMessageTimeout *>(Data);
  Timeout->Suspend();
}
//---------------------------------------------------------------------------
static void __fastcall MessageDialogMouseDown(void * Data, TObject * /*Sender*/,
  TMouseButton /*Button*/, TShiftState /*Shift*/, int /*X*/, int /*Y*/)
{
  assert(Data != NULL);
  TMessageTimeout * Timeout = static_cast<TMessageTimeout *>(Data);
  Timeout->Cancel();
}
//---------------------------------------------------------------------------
static void __fastcall MessageDialogKeyDownUp(void * Data, TObject * /*Sender*/,
  Word & /*Key*/, TShiftState /*Shift*/)
{
  assert(Data != NULL);
  TMessageTimeout * Timeout = static_cast<TMessageTimeout *>(Data);
  Timeout->Cancel();
}
//---------------------------------------------------------------------------
void __fastcall SetTimeoutEvents(TControl * Control, TMessageTimeout * Timeout)
{
  TPublicControl * PublicControl = reinterpret_cast<TPublicControl *>(Control);
  assert(PublicControl->OnMouseMove == NULL);
  PublicControl->OnMouseMove = MakeMethod<TMouseMoveEvent>(Timeout, MessageDialogMouseMove);
  assert(PublicControl->OnMouseDown == NULL);
  PublicControl->OnMouseDown = MakeMethod<TMouseEvent>(Timeout, MessageDialogMouseDown);

  TWinControl * WinControl = dynamic_cast<TWinControl *>(Control);
  if (WinControl != NULL)
  {
    TPublicWinControl * PublicWinControl = reinterpret_cast<TPublicWinControl *>(Control);
    assert(PublicWinControl->OnKeyDown == NULL);
    PublicWinControl->OnKeyDown = MakeMethod<TKeyEvent>(Timeout, MessageDialogKeyDownUp);
    assert(PublicWinControl->OnKeyUp == NULL);
    PublicWinControl->OnKeyUp = MakeMethod<TKeyEvent>(Timeout, MessageDialogKeyDownUp);

    for (int Index = 0; Index < WinControl->ControlCount; Index++)
    {
      SetTimeoutEvents(WinControl->Controls[Index], Timeout);
    }
  }
}
//---------------------------------------------------------------------------
unsigned int __fastcall MoreMessageDialog(const UnicodeString Message, TStrings * MoreMessages,
  TQueryType Type, unsigned int Answers, UnicodeString HelpKeyword, const TMessageParams * Params)
{
  unsigned int Result;
  TForm * Dialog = NULL;
  TMessageTimer * Timer = NULL;
  TMessageTimeout * Timeout = NULL;
  try
  {
    UnicodeString AMessage = Message;

    if ((Params != NULL) && (Params->Timer > 0))
    {
      Timer = new TMessageTimer(Application);
      Timer->Interval = Params->Timer;
      Timer->Event = Params->TimerEvent;
      if (Params->TimerAnswers > 0)
      {
        Answers = Params->TimerAnswers;
      }
      if (Params->TimerQueryType >= 0)
      {
        Type = Params->TimerQueryType;
      }
      if (!Params->TimerMessage.IsEmpty())
      {
        AMessage = Params->TimerMessage;
      }
    }

    TButton * TimeoutButton = NULL;
    Dialog = CreateMessageDialogEx(AMessage, MoreMessages, Type, Answers,
      HelpKeyword, Params, TimeoutButton);

    if (Timer != NULL)
    {
      Timer->Dialog = Dialog;
    }

    if (Params != NULL)
    {
      if (Params->Timeout > 0)
      {
        Timeout = new TMessageTimeout(Application, Params->Timeout, TimeoutButton);
        SetTimeoutEvents(Dialog, Timeout);
      }
    }

    Result = ExecuteMessageDialog(Dialog, Answers, Params);
  }
  __finally
  {
    delete Dialog;
    delete Timer;
    delete Timeout;
  }
  return Result;
}
//---------------------------------------------------------------------------
unsigned int __fastcall MessageDialog(const UnicodeString Msg, TQueryType Type,
  unsigned int Answers, UnicodeString HelpKeyword, const TMessageParams * Params)
{
  return MoreMessageDialog(Msg, NULL, Type, Answers, HelpKeyword, Params);
}
//---------------------------------------------------------------------------
unsigned int __fastcall SimpleErrorDialog(const UnicodeString Msg, const UnicodeString MoreMessages)
{
  unsigned int Result;
  TStrings * More = NULL;
  try
  {
    if (!MoreMessages.IsEmpty())
    {
      More = TextToStringList(MoreMessages);
    }
    Result = MoreMessageDialog(Msg, More, qtError, qaOK, HELP_NONE);
  }
  __finally
  {
    delete More;
  }
  return Result;
}
//---------------------------------------------------------------------------
static TStrings * __fastcall StackInfoListToStrings(
  TJclStackInfoList * StackInfoList)
{
  std::unique_ptr<TStrings> StackTrace(new TStringList());
  StackInfoList->AddToStrings(StackTrace.get(), true, false, true, true);
  for (int Index = 0; Index < StackTrace->Count; Index++)
  {
    UnicodeString Frame = StackTrace->Strings[Index];
    // get rid of declarations "flags" that are included in .map
    Frame = ReplaceStr(Frame, L"__fastcall ", L"");
    Frame = ReplaceStr(Frame, L"__linkproc__ ", L"");
    if (ALWAYS_TRUE(!Frame.IsEmpty() && (Frame[1] == L'(')))
    {
      int Start = Frame.Pos(L"[");
      int End = Frame.Pos(L"]");
      if (ALWAYS_TRUE((Start > 1) && (End > Start) && (Frame[Start - 1] == L' ')))
      {
        // remove absolute address
        Frame.Delete(Start - 1, End - Start + 2);
      }
    }
    StackTrace->Strings[Index] = Frame;
  }
  return StackTrace.release();
}
//---------------------------------------------------------------------------
static std::unique_ptr<TCriticalSection> StackTraceCriticalSection(new TCriticalSection());
typedef std::map<DWORD, TStrings *> TStackTraceMap;
static TStackTraceMap StackTraceMap;
//---------------------------------------------------------------------------
bool __fastcall AppendExceptionStackTraceAndForget(TStrings *& MoreMessages)
{
  bool Result = false;

  TGuard Guard(StackTraceCriticalSection.get());

  TStackTraceMap::iterator Iterator = StackTraceMap.find(GetCurrentThreadId());
  if (Iterator != StackTraceMap.end())
  {
    std::unique_ptr<TStrings> OwnedMoreMessages;
    if (MoreMessages == NULL)
    {
      OwnedMoreMessages.reset(new TStringList());
      MoreMessages = OwnedMoreMessages.get();
      Result = true;
    }
    if (!MoreMessages->Text.IsEmpty())
    {
      MoreMessages->Text = MoreMessages->Text + "\n";
    }
    MoreMessages->Text = MoreMessages->Text + LoadStr(STACK_TRACE) + "\n";
    MoreMessages->AddStrings(Iterator->second);

    delete Iterator->second;
    StackTraceMap.erase(Iterator);

    OwnedMoreMessages.release();
  }
  return Result;
}
//---------------------------------------------------------------------------
unsigned int __fastcall ExceptionMessageDialog(Exception * E, TQueryType Type,
  const UnicodeString MessageFormat, unsigned int Answers, UnicodeString HelpKeyword,
  const TMessageParams * Params)
{
  TStrings * MoreMessages = NULL;
  ExtException * EE = dynamic_cast<ExtException *>(E);
  if (EE != NULL)
  {
    MoreMessages = EE->MoreMessages;
  }

  UnicodeString Message;
  // this is always called from within ExceptionMessage check,
  // so it should never fail here
  CHECK(ExceptionMessageFormatted(E, Message));

  HelpKeyword = MergeHelpKeyword(HelpKeyword, GetExceptionHelpKeyword(E));

  std::unique_ptr<TStrings> OwnedMoreMessages;
  if (AppendExceptionStackTraceAndForget(MoreMessages))
  {
    OwnedMoreMessages.reset(MoreMessages);
  }

  return MoreMessageDialog(
    FORMAT(MessageFormat.IsEmpty() ? UnicodeString(L"%s") : MessageFormat, (Message)),
    MoreMessages, Type, Answers, HelpKeyword, Params);
}
//---------------------------------------------------------------------------
unsigned int __fastcall FatalExceptionMessageDialog(Exception * E, TQueryType Type,
  int SessionReopenTimeout, const UnicodeString MessageFormat, unsigned int Answers,
  UnicodeString HelpKeyword, const TMessageParams * Params)
{
  assert(FLAGCLEAR(Answers, qaRetry));
  Answers |= qaRetry;

  TQueryButtonAlias Aliases[1];
  Aliases[0].Button = qaRetry;
  Aliases[0].Alias = LoadStr(RECONNECT_BUTTON);

  TMessageParams AParams;
  if (Params != NULL)
  {
    AParams = *Params;
  }
  assert(AParams.Timeout == 0);
  // the condition is de facto excess
  if (SessionReopenTimeout > 0)
  {
    AParams.Timeout = SessionReopenTimeout;
    AParams.TimeoutAnswer = qaRetry;
  }
  assert(AParams.Aliases == NULL);
  AParams.Aliases = Aliases;
  AParams.AliasesCount = LENOF(Aliases);

  return ExceptionMessageDialog(E, Type, MessageFormat, Answers, HelpKeyword, &AParams);
}
//---------------------------------------------------------------------------
static void __fastcall DoExceptNotify(TObject * ExceptObj, void * ExceptAddr,
  bool OSException, void * BaseOfStack)
{
  if (ExceptObj != NULL)
  {
    Exception * E = dynamic_cast<Exception *>(ExceptObj);
    if ((E != NULL) && IsInternalException(E)) // optimization
    {
      DoExceptionStackTrace(ExceptObj, ExceptAddr, OSException, BaseOfStack);

      TJclStackInfoList * StackInfoList = JclLastExceptStackList();

      if (ALWAYS_TRUE(StackInfoList != NULL))
      {
        std::unique_ptr<TStrings> StackTrace(StackInfoListToStrings(StackInfoList));

        DWORD ThreadID = GetCurrentThreadId();

        TGuard Guard(StackTraceCriticalSection.get());

        TStackTraceMap::iterator Iterator = StackTraceMap.find(ThreadID);
        if (Iterator != StackTraceMap.end())
        {
          Iterator->second->Add(L"");
          Iterator->second->AddStrings(StackTrace.get());
        }
        else
        {
          StackTraceMap.insert(std::make_pair(ThreadID, StackTrace.release()));
        }

        // this chains so that JclLastExceptStackList() returns NULL the next time
        // for the current thread
        delete StackInfoList;
      }
    }
  }
}
//---------------------------------------------------------------------------
void * __fastcall BusyStart()
{
  void * Token = reinterpret_cast<void *>(Screen->Cursor);
  Screen->Cursor = crHourGlass;
  return Token;
}
//---------------------------------------------------------------------------
void __fastcall BusyEnd(void * Token)
{
  Screen->Cursor = reinterpret_cast<TCursor>(Token);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static DWORD MainThread = 0;
static TDateTime LastGUIUpdate = 0;
static double GUIUpdateIntervalFrac = static_cast<double>(OneSecond/1000*GUIUpdateInterval);  // 1/5 sec
static bool NoGUI = false;
//---------------------------------------------------------------------------
void __fastcall SetNoGUI()
{
  NoGUI = true;
}
//---------------------------------------------------------------------------
bool __fastcall ProcessGUI(bool Force)
{
  assert(MainThread != 0);
  bool Result = false;
  // Calling ProcessMessages in Azure WebJob causes access violation in VCL.
  // As we do not really need to call it in scripting/.NET, just skip it.
  if ((MainThread == GetCurrentThreadId()) && !NoGUI)
  {
    TDateTime N = Now();
    if (Force ||
        (double(N) - double(LastGUIUpdate) > GUIUpdateIntervalFrac))
    {
      LastGUIUpdate = N;
      Application->ProcessMessages();
      Result = true;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall CopyParamListButton(TButton * Button)
{
  if (!SupportsSplitButton())
  {
    MenuButton(Button);
  }
}
//---------------------------------------------------------------------------
const int cpiDefault = -1;
const int cpiConfigure = -2;
const int cpiCustom = -3;
const int cpiSaveSettings = -4;
//---------------------------------------------------------------------------
void __fastcall CopyParamListPopup(TRect Rect, TPopupMenu * Menu,
  const TCopyParamType & Param, UnicodeString Preset, TNotifyEvent OnClick,
  int Options, int CopyParamAttrs, bool SaveSettings)
{
  Menu->Items->Clear();

  TMenuItem * CustomizeItem = NULL;
  TMenuItem * Item;

  if (FLAGSET(Options, cplCustomize))
  {
    Item = new TMenuItem(Menu);
    Item->Caption = LoadStr(COPY_PARAM_CUSTOM);
    Item->Tag = cpiCustom;
    Item->Default = FLAGSET(Options, cplCustomizeDefault);
    Item->OnClick = OnClick;
    Menu->Items->Add(Item);
    CustomizeItem = Item;
  }

  if (FLAGSET(Options, cplSaveSettings))
  {
    Item = new TMenuItem(Menu);
    Item->Caption = LoadStr(COPY_PARAM_SAVE_SETTINGS);
    Item->Tag = cpiSaveSettings;
    Item->Checked = SaveSettings;
    Item->OnClick = OnClick;
    Menu->Items->Add(Item);
  }

  Item = new TMenuItem(Menu);
  Item->Caption = LoadStr(COPY_PARAM_PRESET_HEADER);
  Item->Visible = false;
  Item->Enabled = false;
  Menu->Items->Add(Item);

  bool AnyChecked = false;
  Item = new TMenuItem(Menu);
  Item->Caption = LoadStr(COPY_PARAM_DEFAULT);
  Item->Tag = cpiDefault;
  Item->Checked =
    Preset.IsEmpty() && (GUIConfiguration->CopyParamPreset[L""] == Param);
  AnyChecked = AnyChecked || Item->Checked;
  Item->OnClick = OnClick;
  Menu->Items->Add(Item);

  TCopyParamType DefaultParam;
  const TCopyParamList * CopyParamList = GUIConfiguration->CopyParamList;
  for (int i = 0; i < CopyParamList->Count; i++)
  {
    UnicodeString Name = CopyParamList->Names[i];
    TCopyParamType AParam = GUIConfiguration->CopyParamPreset[Name];
    if (AParam.AnyUsableCopyParam(CopyParamAttrs) ||
        // This makes "Binary" preset visible,
        // as long as we care about transfer mode
        ((AParam == DefaultParam) &&
         FLAGCLEAR(CopyParamAttrs, cpaIncludeMaskOnly) &&
         FLAGCLEAR(CopyParamAttrs, cpaNoTransferMode)))
    {
      Item = new TMenuItem(Menu);
      Item->Caption = Name;
      Item->Tag = i;
      Item->Checked =
        (Preset == Name) && (AParam == Param);
      AnyChecked = AnyChecked || Item->Checked;
      Item->OnClick = OnClick;
      Menu->Items->Add(Item);
    }
  }

  if (CustomizeItem != NULL)
  {
    CustomizeItem->Checked = !AnyChecked;
  }

  Item = new TMenuItem(Menu);
  Item->Caption = L"-";
  Menu->Items->Add(Item);

  Item = new TMenuItem(Menu);
  Item->Caption = LoadStr(COPY_PARAM_CONFIGURE);
  Item->Tag = cpiConfigure;
  Item->OnClick = OnClick;
  Menu->Items->Add(Item);

  MenuPopup(Menu, Rect, NULL);
}
//---------------------------------------------------------------------------
bool __fastcall CopyParamListPopupClick(TObject * Sender,
  TCopyParamType & Param, UnicodeString & Preset, int CopyParamAttrs,
  bool * SaveSettings)
{
  TComponent * Item = dynamic_cast<TComponent *>(Sender);
  assert(Item != NULL);
  assert((Item->Tag >= cpiSaveSettings) && (Item->Tag < GUIConfiguration->CopyParamList->Count));

  bool Result;
  if (Item->Tag == cpiConfigure)
  {
    bool MatchedPreset = (GUIConfiguration->CopyParamPreset[Preset] == Param);
    DoPreferencesDialog(pmPresets);
    Result = (MatchedPreset && GUIConfiguration->HasCopyParamPreset[Preset]);
    if (Result)
    {
      Param = GUIConfiguration->CopyParamPreset[Preset];
    }
  }
  else if (Item->Tag == cpiCustom)
  {
    Result = DoCopyParamCustomDialog(Param, CopyParamAttrs);
  }
  else if (Item->Tag == cpiSaveSettings)
  {
    if (ALWAYS_TRUE(SaveSettings != NULL))
    {
      *SaveSettings = !*SaveSettings;
    }
    Result = false;
  }
  else
  {
    Preset = (Item->Tag >= 0) ?
      GUIConfiguration->CopyParamList->Names[Item->Tag] : UnicodeString();
    Param = GUIConfiguration->CopyParamPreset[Preset];
    Result = true;
  }
  return Result;
}
//---------------------------------------------------------------------------
TWinInteractiveCustomCommand::TWinInteractiveCustomCommand(
  TCustomCommand * ChildCustomCommand, const UnicodeString CustomCommandName) :
  TInteractiveCustomCommand(ChildCustomCommand)
{
  FCustomCommandName = StripHotkey(CustomCommandName);
}
//---------------------------------------------------------------------------
void __fastcall TWinInteractiveCustomCommand::Prompt(
  const UnicodeString & Prompt, UnicodeString & Value)
{
  UnicodeString APrompt = Prompt;
  if (APrompt.IsEmpty())
  {
    APrompt = FMTLOAD(CUSTOM_COMMANDS_PARAM_PROMPT, (FCustomCommandName));
  }
  TStrings * History = CustomWinConfiguration->History[L"CustomCommandParam"];
  if (InputDialog(FMTLOAD(CUSTOM_COMMANDS_PARAM_TITLE, (FCustomCommandName)),
        APrompt, Value, HELP_CUSTOM_COMMAND_PARAM, History))
  {
    CustomWinConfiguration->History[L"CustomCommandParam"] = History;
  }
  else
  {
    Abort();
  }
}
//---------------------------------------------------------------------------
void __fastcall TWinInteractiveCustomCommand::Execute(
  const UnicodeString & Command, UnicodeString & Value)
{
  // inspired by
  // http://forum.codecall.net/topic/72472-execute-a-console-program-and-capture-its-output/
  HANDLE StdOutOutput;
  HANDLE StdOutInput;
  HANDLE StdInOutput;
  HANDLE StdInInput;
  SECURITY_ATTRIBUTES SecurityAttributes;
  SecurityAttributes.nLength = sizeof(SecurityAttributes);
  SecurityAttributes.lpSecurityDescriptor = NULL;
  SecurityAttributes.bInheritHandle = TRUE;
  try
  {
    if (!CreatePipe(&StdOutOutput, &StdOutInput, &SecurityAttributes, 0))
    {
      throw Exception(FMTLOAD(SHELL_PATTERN_ERROR, (Command, L"out")));
    }
    else if (!CreatePipe(&StdInOutput, &StdInInput, &SecurityAttributes, 0))
    {
      throw Exception(FMTLOAD(SHELL_PATTERN_ERROR, (Command, L"in")));
    }
    else
    {
      STARTUPINFO StartupInfo;
      PROCESS_INFORMATION ProcessInformation;

      FillMemory(&StartupInfo, sizeof(StartupInfo), 0);
      StartupInfo.cb = sizeof(StartupInfo);
      StartupInfo.wShowWindow = SW_HIDE;
      StartupInfo.hStdInput = StdInOutput;
      StartupInfo.hStdOutput = StdOutInput;
      StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

      if (!CreateProcess(NULL, Command.c_str(), &SecurityAttributes, &SecurityAttributes,
            TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInformation))
      {
        throw Exception(FMTLOAD(SHELL_PATTERN_ERROR, (Command, L"process")));
      }
      else
      {
        try
        {
          // wait until the console program terminated
          bool Running = true;
          while (Running)
          {
            switch (WaitForSingleObject(ProcessInformation.hProcess, 200))
            {
              case WAIT_TIMEOUT:
                Application->ProcessMessages();
                break;

              case WAIT_OBJECT_0:
                Running = false;
                break;

              default:
                throw Exception(FMTLOAD(SHELL_PATTERN_ERROR, (Command, L"wait")));
            }
          }

          char Buffer[1024];
          unsigned long Read;
          while (PeekNamedPipe(StdOutOutput, NULL, 0, NULL, &Read, NULL) &&
                 (Read > 0))

          {
            if (!ReadFile(StdOutOutput, &Buffer, Read, &Read, NULL))
            {
              throw Exception(FMTLOAD(SHELL_PATTERN_ERROR, (Command, L"read")));
            }
            else if (Read > 0)
            {
              Value += UnicodeString(AnsiString(Buffer, Read));
            }
          }

          // trim trailing cr/lf
          Value = TrimRight(Value);
        }
        __finally
        {
          CloseHandle(ProcessInformation.hProcess);
          CloseHandle(ProcessInformation.hThread);
        }
      }
    }
  }
  __finally
  {
    if (StdOutOutput != INVALID_HANDLE_VALUE)
    {
      CloseHandle(StdOutOutput);
    }
    if (StdOutInput != INVALID_HANDLE_VALUE)
    {
      CloseHandle(StdOutInput);
    }
    if (StdInOutput != INVALID_HANDLE_VALUE)
    {
      CloseHandle(StdInOutput);
    }
    if (StdInInput != INVALID_HANDLE_VALUE)
    {
      CloseHandle(StdInInput);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall MenuPopup(TPopupMenu * Menu, TButton * Button)
{
  MenuPopup(Menu, CalculatePopupRect(Button), Button);
}
//---------------------------------------------------------------------------
void __fastcall MenuPopup(TObject * Sender, const TPoint & MousePos, bool & Handled)
{
  TControl * Control = dynamic_cast<TControl *>(Sender);
  assert(Control != NULL);
  TPoint Point;
  if ((MousePos.x == -1) && (MousePos.y == -1))
  {
    Point = Control->ClientToScreen(TPoint(0, 0));
  }
  else
  {
    Point = Control->ClientToScreen(MousePos);
  }
  TPopupMenu * PopupMenu = (reinterpret_cast<TPublicControl *>(Control))->PopupMenu;
  assert(PopupMenu != NULL);
  TRect Rect(Point, Point);
  MenuPopup(PopupMenu, Rect, Control);
  Handled = true;
}
//---------------------------------------------------------------------------
TComponent * __fastcall GetPopupComponent(TObject * Sender)
{
  TComponent * Item = dynamic_cast<TComponent *>(Sender);
  assert(Item != NULL);
  TPopupMenu * PopupMenu = dynamic_cast<TPopupMenu *>(Item->Owner);
  assert(PopupMenu != NULL);
  assert(PopupMenu->PopupComponent != NULL);
  return PopupMenu->PopupComponent;
}
//---------------------------------------------------------------------------
void __fastcall MenuButton(TButton * Button)
{
  Button->Images = GlyphsModule->ButtonImages;
  Button->ImageIndex = 0;
  Button->DisabledImageIndex = 1;
  Button->ImageAlignment = iaRight;
}
//---------------------------------------------------------------------------
TRect __fastcall CalculatePopupRect(TButton * Button)
{
  TPoint UpPoint = Button->ClientToScreen(TPoint(0, 0));
  TPoint DownPoint = Button->ClientToScreen(TPoint(Button->Width, Button->Height));
  TRect Rect(UpPoint, DownPoint);
  // With themes enabled, button are rendered 1 pixel smaller than their actual size
  int Offset = UseThemes() ? -1 : 0;
  Rect.Inflate(Offset, Offset);
  return Rect;
}
//---------------------------------------------------------------------------
TRect __fastcall CalculatePopupRect(TControl * Control, TPoint MousePos)
{
  MousePos = Control->ClientToScreen(MousePos);
  TRect Rect(MousePos, MousePos);
  return Rect;
}
//---------------------------------------------------------------------------
void __fastcall FixButtonImage(TButton * Button)
{
  // this themes enabled, button image is by default drawn too high
  if (UseThemes())
  {
    Button->ImageMargins->Top = 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall UncenterButtonImage(TButton * Button)
{
  Button->ImageMargins->Left = 0;
  if (UseThemes())
  {
    Button->Caption = TrimLeft(Button->Caption);
  }
}
//---------------------------------------------------------------------------
void __fastcall CenterButtonImage(TButton * Button)
{
  UncenterButtonImage(Button);

  // with themes disabled, the text seems to be drawn over the icon,
  // so that the padding spaces hide away most of the icon
  if (UseThemes())
  {
    Button->ImageAlignment = iaCenter;
    int ImageWidth = Button->Images->Width;

    std::unique_ptr<TControlCanvas> Canvas(new TControlCanvas());
    Canvas->Control = Button;

    UnicodeString Caption = Button->Caption;
    UnicodeString Padding;
    while (Canvas->TextWidth(Padding) < ImageWidth)
    {
      Padding += L" ";
    }
    Caption = Padding + Caption;
    Button->Caption = Caption;

    int CaptionWidth = Canvas->TextWidth(Caption);
    // The margins seem to extend the area over which the image is centered,
    // so we have to set it to a double of desired padding.
    // Note that (CaptionWidth / 2) - (ImageWidth / 2)
    // is approximatelly same as half of caption width before padding.
    Button->ImageMargins->Left = - 2 * ((CaptionWidth / 2) - (ImageWidth / 2) + ScaleByTextHeight(Button, 2));
  }
  else
  {
    // at least do not draw it so near to the edge
    Button->ImageMargins->Left = 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall SetGlobalMinimizeHandler(TCustomForm * /*Form*/, TNotifyEvent OnMinimize)
{
  if (GlobalOnMinimize == NULL)
  {
    GlobalOnMinimize = OnMinimize;
  }
}
//---------------------------------------------------------------------------
void __fastcall ClearGlobalMinimizeHandler(TNotifyEvent OnMinimize)
{
  if (GlobalOnMinimize == OnMinimize)
  {
    GlobalOnMinimize = NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall CallGlobalMinimizeHandler(TObject * Sender)
{
  Configuration->Usage->Inc(L"OperationMinimizations");
  if (ALWAYS_TRUE(GlobalOnMinimize != NULL))
  {
    GlobalOnMinimize(Sender);
  }
}
//---------------------------------------------------------------------------
static void __fastcall DoApplicationMinimizeRestore(bool Minimize)
{
  // WORKAROUND
  // When main window is hidden (command-line operation),
  // we do not want it to be shown by TApplication.Restore,
  // so we temporarily detach it from an application.
  // Probably not really necessary for minimizing phase,
  // but we do it for consistency anyway.
  TForm * MainForm = Application->MainForm;
  bool RestoreMainForm = false;
  if (ALWAYS_TRUE(MainForm != NULL) &&
      !MainForm->Visible)
  {
    SetAppMainForm(NULL);
    RestoreMainForm = true;
  }
  try
  {
    if (Minimize)
    {
      Application->Minimize();
    }
    else
    {
      Application->Restore();
    }
  }
  __finally
  {
    if (RestoreMainForm)
    {
      SetAppMainForm(MainForm);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall ApplicationMinimize()
{
  DoApplicationMinimizeRestore(true);
}
//---------------------------------------------------------------------------
void __fastcall ApplicationRestore()
{
  DoApplicationMinimizeRestore(false);
}
//---------------------------------------------------------------------------
bool __fastcall IsApplicationMinimized()
{
  // VCL help recommends handling Application->OnMinimize/OnRestore
  // for tracking state, but OnRestore is actually not called
  // (OnMinimize is), when app is minimized from e.g. Progress window
  bool AppMinimized = IsIconic(Application->Handle);
  bool MainFormMinimized = IsIconic(Application->MainFormHandle);
  return AppMinimized || MainFormMinimized;
}
//---------------------------------------------------------------------------
void __fastcall WinInitialize()
{
  if (JclHookExceptions())
  {
    JclStackTrackingOptions << stAllModules;
    JclAddExceptNotifier(DoExceptNotify, npFirstChain);
  }

  SetErrorMode(SEM_FAILCRITICALERRORS);
  OnApiPath = ::ApiPath;
  MainThread = GetCurrentThreadId();

#pragma warn -8111
#pragma warn .8111

}
//---------------------------------------------------------------------------
void __fastcall WinFinalize()
{
  JclRemoveExceptNotifier(DoExceptNotify);
}
//---------------------------------------------------------------------------
__fastcall ::TTrayIcon::TTrayIcon(unsigned int Id)
{
  FVisible = false;
  FOnClick = NULL;
  FOnBalloonClick = NULL;
  FBalloonUserData = NULL;

  FTrayIcon = new NOTIFYICONDATA;
  memset(FTrayIcon, 0, sizeof(*FTrayIcon));
  FTrayIcon->cbSize = sizeof(*FTrayIcon);
  FTrayIcon->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

  // LoadIconMetric is available from Windows Vista only
  HMODULE ComCtl32Dll = GetModuleHandle(comctl32);
  if (ALWAYS_TRUE(ComCtl32Dll))
  {
    typedef HRESULT WINAPI (* TLoadIconMetric)(HINSTANCE hinst, PCWSTR pszName, int lims, __out HICON *phico);
    TLoadIconMetric LoadIconMetric = (TLoadIconMetric)GetProcAddress(ComCtl32Dll, "LoadIconMetric");
    if (LoadIconMetric != NULL)
    {
      // Prefer not to use Application->Icon->Handle as that shows 32x32 scaled down to 16x16 for some reason
      LoadIconMetric(MainInstance, L"MAINICON", LIM_SMALL, &FTrayIcon->hIcon);
    }
  }

  if (FTrayIcon->hIcon == 0)
  {
    FTrayIcon->hIcon = Application->Icon->Handle;
  }

  FTrayIcon->uID = Id;
  FTrayIcon->hWnd = AllocateHWnd(WndProc);
  FTrayIcon->uCallbackMessage = WM_TRAY_ICON;

  FTaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
}
//---------------------------------------------------------------------------
__fastcall ::TTrayIcon::~TTrayIcon()
{
  // make sure we hide icon even in case it was shown just to pop up the balloon
  // (in which case Visible == false)
  CancelBalloon();
  Visible = false;
  DeallocateHWnd(FTrayIcon->hWnd);
  delete FTrayIcon;
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::PopupBalloon(UnicodeString Title,
  const UnicodeString & Str, TQueryType QueryType, unsigned int Timeout,
  TNotifyEvent OnBalloonClick, TObject * BalloonUserData)
{
  if (Timeout > 30000)
  {
    // this is probably system limit, do not try more, especially for
    // the timeout-driven hiding of the tray icon (for Win2k)
    Timeout = 30000;
  }
  FTrayIcon->uFlags |= NIF_INFO;
  Title = FORMAT(L"%s - %s", (Title, AppNameString()));
  StrPLCopy(FTrayIcon->szInfoTitle, Title, LENOF(FTrayIcon->szInfoTitle) - 1);
  UnicodeString Info = Str;
  // When szInfo is empty, balloon is not shown
  // (or actually it means the balloon should be deleted, if any)
  if (Info.IsEmpty())
  {
    Info = L" ";
  }
  StrPLCopy(FTrayIcon->szInfo, Info, LENOF(FTrayIcon->szInfo) - 1);
  FTrayIcon->uTimeout = Timeout;
  switch (QueryType)
  {
    case qtError:
      FTrayIcon->dwInfoFlags = NIIF_ERROR;
      break;

    case qtInformation:
    case qtConfirmation:
      FTrayIcon->dwInfoFlags = NIIF_INFO;
      break;

    case qtWarning:
    default:
      FTrayIcon->dwInfoFlags = NIIF_WARNING;
      break;
  }

  KillTimer(FTrayIcon->hWnd, 1);
  if (Visible)
  {
    Update();
  }
  else
  {
    Notify(NIM_ADD);
  }

  FOnBalloonClick = OnBalloonClick;
  delete FBalloonUserData;
  FBalloonUserData = BalloonUserData;

  // Clearing the flag ensures that subsequent updates does not hide the baloon
  // unless CancelBalloon is called explicitly
  FTrayIcon->uFlags = FTrayIcon->uFlags & ~NIF_INFO;
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::BalloonCancelled()
{
  FOnBalloonClick = NULL;
  delete FBalloonUserData;
  FBalloonUserData = NULL;
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::CancelBalloon()
{
  KillTimer(FTrayIcon->hWnd, 1);
  if (Visible)
  {
    FTrayIcon->uFlags |= NIF_INFO;
    FTrayIcon->szInfo[0] = L'\0';
    Update();
    FTrayIcon->uFlags = FTrayIcon->uFlags & ~NIF_INFO;
  }
  else
  {
    Notify(NIM_DELETE);
  }

  BalloonCancelled();
}
//---------------------------------------------------------------------------
bool __fastcall ::TTrayIcon::Notify(unsigned int Message)
{
  bool Result = SUCCEEDED(Shell_NotifyIcon(Message, (NOTIFYICONDATA*)FTrayIcon));
  if (Result && (Message == NIM_ADD))
  {
    UINT Timeout = FTrayIcon->uTimeout;
    try
    {
      FTrayIcon->uVersion = NOTIFYICON_VERSION;
      Result = SUCCEEDED(Shell_NotifyIcon(NIM_SETVERSION, (NOTIFYICONDATA*)FTrayIcon));
    }
    __finally
    {
      FTrayIcon->uTimeout = Timeout;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::Update()
{
  if (Visible)
  {
    Notify(NIM_MODIFY);
  }
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::SetVisible(bool value)
{
  if (Visible != value)
  {
    if (value)
    {
      FVisible = Notify(NIM_ADD);
    }
    else
    {
      FVisible = false;
      KillTimer(FTrayIcon->hWnd, 1);
      Notify(NIM_DELETE);
      BalloonCancelled();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::WndProc(TMessage & Message)
{
  try
  {
    if (Message.Msg == WM_TRAY_ICON)
    {
      assert(Message.WParam == 0);
      switch (Message.LParam)
      {
        // old shell32
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        // new shell32:
        case WM_CONTEXTMENU:
          if (OnClick != NULL)
          {
            OnClick(NULL);
          }
          Message.Result = true;
          break;
      }

      if (Message.LParam == NIN_BALLOONUSERCLICK)
      {
        if (FOnBalloonClick != NULL)
        {
          // prevent the user data from being freed by possible call
          // to CancelBalloon or PopupBalloon during call to OnBalloonClick
          std::unique_ptr<TObject> UserData(FBalloonUserData);
          FBalloonUserData = NULL;
          FOnBalloonClick(UserData.get());
        }
        else if (OnClick != NULL)
        {
          OnClick(NULL);
        }
      }

      switch (Message.LParam)
      {
        case NIN_BALLOONHIDE:
        case NIN_BALLOONTIMEOUT:
        case NIN_BALLOONUSERCLICK:
          KillTimer(FTrayIcon->hWnd, 1);
          // if icon was shown just to display balloon, hide it with the balloon
          if (!Visible)
          {
            Notify(NIM_DELETE);
          }
          BalloonCancelled();
          break;
      }
    }
    else if (Message.Msg == WM_TIMER)
    {
      // sanity check
      Notify(NIM_DELETE);
      BalloonCancelled();
    }
    else if (Message.Msg == FTaskbarCreatedMsg)
    {
      if (Visible)
      {
        // force recreation
        Visible = false;
        Visible = true;
      }
    }
    else
    {
      Message.Result = DefWindowProc(FTrayIcon->hWnd, Message.Msg, Message.WParam, Message.LParam);
    }
  }
  catch(Exception & E)
  {
    Application->HandleException(&E);
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ::TTrayIcon::GetHint()
{
  return FTrayIcon->szTip;
}
//---------------------------------------------------------------------------
void __fastcall ::TTrayIcon::SetHint(UnicodeString value)
{
  if (Hint != value)
  {
    unsigned int Max = LENOF(FTrayIcon->szTip);
    StrPLCopy(FTrayIcon->szTip, value, Max - 1);
    Update();
  }
}
//---------------------------------------------------------------------------
