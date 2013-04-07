//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Consts.hpp>
#include <GUITools.h>

#include <Common.h>
#include <VCLCommon.h>
#include <WinInterface.h>
#include <TextsWin.h>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
class TMessageForm : public TForm
{
public:
  static TForm * __fastcall Create(const UnicodeString & Msg, TStrings * MoreMessages,
    TMsgDlgType DlgType, TMsgDlgButtons Buttons,
    TQueryButtonAlias * Aliases, unsigned int AliasesCount,
    TMsgDlgBtn TimeoutResult, TButton ** TimeoutButton, const UnicodeString & ImageName);

protected:
  __fastcall TMessageForm(TComponent * AOwner);

  DYNAMIC void __fastcall KeyDown(Word & Key, TShiftState Shift);
  UnicodeString __fastcall GetFormText();
  virtual void __fastcall CreateParams(TCreateParams & Params);
  DYNAMIC void __fastcall DoShow();
  virtual void __fastcall Dispatch(void * Message);

private:
  TLabel * Message;
  TMemo * MessageMemo;

  void __fastcall HelpButtonClick(TObject * Sender);
  void __fastcall CMDialogKey(TWMKeyDown & Message);
};
//---------------------------------------------------------------------------
__fastcall TMessageForm::TMessageForm(TComponent * AOwner) : TForm(AOwner, 0)
{
  Message = NULL;
  MessageMemo = NULL;
  TNonClientMetrics NonClientMetrics;
  NonClientMetrics.cbSize = sizeof(NonClientMetrics);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &NonClientMetrics, 0))
  {
    Font->Handle = CreateFontIndirect(&NonClientMetrics.lfMessageFont);
  }
  Position = poOwnerFormCenter;
  UseSystemSettingsPre(this);
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::HelpButtonClick(TObject * /*Sender*/)
{
  if (HelpKeyword != HELP_NONE)
  {
    FormHelp(this);
  }
  else
  {
    UnicodeString Text = Message->Caption;
    if (MessageMemo != NULL)
    {
      Text += L"\n" + MessageMemo->Text;
    }
    MessageWithNoHelp(Text);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::KeyDown(Word & Key, TShiftState Shift)
{
  if (Shift.Contains(ssCtrl) && (Key == L'C'))
  {
    CopyToClipboard(GetFormText());
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMessageForm::GetFormText()
{
  UnicodeString DividerLine, ButtonCaptions;

  DividerLine = UnicodeString::StringOfChar(L'-', 27) + sLineBreak;
  for (int i = 0; i < ComponentCount - 1; i++)
  {
    if (dynamic_cast<TButton*>(Components[i]) != NULL)
    {
      ButtonCaptions += dynamic_cast<TButton*>(Components[i])->Caption +
        UnicodeString::StringOfChar(L' ', 3);
    }
  }
  ButtonCaptions = StringReplace(ButtonCaptions, L"&", L"",
    TReplaceFlags() << rfReplaceAll);
  UnicodeString MoreMessages;
  if (MessageMemo != NULL)
  {
    MoreMessages = MessageMemo->Text + DividerLine;
  }
  UnicodeString MessageCaption;
  MessageCaption = StringReplace(Message->Caption, L"\r", L"", TReplaceFlags() << rfReplaceAll);
  MessageCaption = StringReplace(MessageCaption, L"\n", L"\r\n", TReplaceFlags() << rfReplaceAll);
  UnicodeString Result = FORMAT(L"%s%s%s%s%s%s%s%s%s%s%s", (DividerLine, Caption, sLineBreak,
    DividerLine, MessageCaption, sLineBreak, DividerLine, MoreMessages,
    ButtonCaptions, sLineBreak, DividerLine));
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::CMDialogKey(TWMKeyDown & Message)
{
  // this gets used in WinInterface.cpp SetTimeoutEvents
  if (OnKeyDown != NULL)
  {
    OnKeyDown(this, Message.CharCode, KeyDataToShiftState(Message.KeyData));
  }
  TForm::Dispatch(&Message);
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::Dispatch(void * Message)
{
  TMessage * M = reinterpret_cast<TMessage*>(Message);
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
void __fastcall TMessageForm::CreateParams(TCreateParams & Params)
{
  TForm::CreateParams(Params);
  if ((Screen != NULL) && (Screen->ActiveForm != NULL) &&
      Screen->ActiveForm->HandleAllocated())
  {
    Params.WndParent = Screen->ActiveForm->Handle;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::DoShow()
{
  UseSystemSettingsPost(this);

  TForm::DoShow();
}
//---------------------------------------------------------------------------
const ResourceString * Captions[] = { &_SMsgDlgWarning, &_SMsgDlgError, &_SMsgDlgInformation,
  &_SMsgDlgConfirm, NULL };
const wchar_t * IconIDs[] = { IDI_EXCLAMATION, IDI_HAND, IDI_ASTERISK,
  IDI_QUESTION, NULL };
const int ButtonCount = 11;
const UnicodeString ButtonNames[ButtonCount] = {
  L"Yes", L"No", L"OK", L"Cancel", L"Abort", L"Retry", L"Ignore", L"All", L"NoToAll",
  L"YesToAll", L"Help" };
const ResourceString * ButtonCaptions[ButtonCount] = {
  &_SMsgDlgYes, &_SMsgDlgNo, &_SMsgDlgOK, &_SMsgDlgCancel, &_SMsgDlgAbort,
  &_SMsgDlgRetry, &_SMsgDlgIgnore, &_SMsgDlgAll, &_SMsgDlgNoToAll, &_SMsgDlgYesToAll,
  &_SMsgDlgHelp };
extern const int ModalResults[ButtonCount] = {
  mrYes, mrNo, mrOk, mrCancel, mrAbort, mrRetry, mrIgnore, mrAll, mrNoToAll,
  mrYesToAll, 0 };
const int mcHorzMargin = 8;
const int mcVertMargin = 8;
const int mcHorzSpacing = 10;
const int mcVertSpacing = 10;
const int mcButtonWidth = 50;
const int mcButtonHeight = 14;
const int mcButtonSpacing = 4;
const int mcMoreMessageWidth = 320;
const int mcMoreMessageHeight = 80;
//---------------------------------------------------------------------------
TForm * __fastcall TMessageForm::Create(const UnicodeString & Msg,
  TStrings * MoreMessages, TMsgDlgType DlgType, TMsgDlgButtons Buttons,
  TQueryButtonAlias * Aliases, unsigned int AliasesCount,
  TMsgDlgBtn TimeoutResult, TButton ** TimeoutButton, const UnicodeString & ImageName)
{
  TRect TextRect;

  TMsgDlgBtn DefaultButton, CancelButton;
  if (Buttons.Contains(mbOK))
  {
    DefaultButton = mbOK;
  }
  else if (Buttons.Contains(mbYes))
  {
    DefaultButton = mbYes;
  }
  else
  {
    DefaultButton = mbRetry;
  }

  if (Buttons.Contains(mbCancel))
  {
    CancelButton = mbCancel;
  }
  else if (Buttons.Contains(mbNo))
  {
    CancelButton = mbNo;
  }
  else if (Buttons.Contains(mbAbort))
  {
    CancelButton = mbAbort;
  }
  else
  {
    CancelButton = mbOK;
  }

  if (TimeoutButton != NULL)
  {
    *TimeoutButton = NULL;
  }

  TMessageForm * Result = SafeFormCreate<TMessageForm>();

  Result->BiDiMode = Application->BiDiMode;
  Result->BorderStyle = bsDialog;
  Result->Canvas->Font = Result->Font;
  Result->KeyPreview = true;
  TPoint DialogUnits = GetAveCharSize(Result->Canvas);
  int HorzMargin = MulDiv(mcHorzMargin, DialogUnits.x, 4);
  int VertMargin = MulDiv(mcVertMargin, DialogUnits.y, 8);
  int HorzSpacing = MulDiv(mcHorzSpacing, DialogUnits.x, 4);
  int VertSpacing = MulDiv(mcVertSpacing, DialogUnits.y, 8);
  int ButtonWidth = MulDiv(mcButtonWidth, DialogUnits.x, 4);
  TButton * ButtonControls[ButtonCount + 1];
  int ButtonControlsCount = 0;
  for (unsigned int B = mbYes; B <= mbHelp; B++)
  {
    assert(B < ButtonCount);
    if (Buttons.Contains(TMsgDlgBtn(B)))
    {
      TextRect = Rect(0,0,0,0);
      UnicodeString Caption = LoadResourceString(ButtonCaptions[B]);

      // temporary fix of accelerators (&Abort vs. &All/Yes to &All)
      // must be removed
      if (Caption == L"&All")
      {
        Caption = L"A&ll";
      }
      else if (Caption == L"Yes to &All")
      {
        Caption = L"Yes to A&ll";
      }

      TNotifyEvent OnClick = NULL;
      if (Aliases != NULL)
      {
        for (unsigned int i = 0; i < AliasesCount; i++)
        {
          if (B == Aliases[i].Button)
          {
            Caption = Aliases[i].Alias;
            OnClick = Aliases[i].OnClick;
            break;
          }
        }
      }

      TButton * Button = new TButton(Result);

      UnicodeString MeasureCaption = Caption;
      if ((TimeoutButton != NULL) && (B == static_cast<unsigned int>(TimeoutResult)))
      {
        MeasureCaption = FMTLOAD(TIMEOUT_BUTTON, (MeasureCaption, 99));
        *TimeoutButton = Button;
      }

      DrawText(Result->Canvas->Handle,
        UnicodeString(MeasureCaption).c_str(), -1,
        &TextRect, DT_CALCRECT | DT_LEFT | DT_SINGLELINE |
        Result->DrawTextBiDiModeFlagsReadingOnly());
      int CurButtonWidth = TextRect.Right - TextRect.Left + 8;
      if (CurButtonWidth > ButtonWidth)
      {
        ButtonWidth = CurButtonWidth;
      }

      Button->Name = ButtonNames[TMsgDlgBtn(B)];
      Button->Parent = Result;
      Button->Caption = Caption;
      if (OnClick != NULL)
      {
        Button->OnClick = OnClick;
      }
      else
      {
        Button->ModalResult = ModalResults[B];
        Button->Default = (B == static_cast<unsigned int>(DefaultButton));
        Button->Cancel = (B == static_cast<unsigned int>(CancelButton));
      }
      if (MoreMessages != NULL)
      {
        Button->Anchors = TAnchors() << akBottom << akLeft;
      }
      if (B == mbHelp)
      {
        Button->OnClick = Result->HelpButtonClick;
      }

      ButtonControls[ButtonControlsCount] = Button;
      ButtonControlsCount++;
    }
  }

  int ButtonHeight = MulDiv(mcButtonHeight, DialogUnits.y, 8);
  int ButtonSpacing = MulDiv(mcButtonSpacing, DialogUnits.x, 4);
  SetRect(&TextRect, 0, 0, Screen->Width / 2, 0);
  DrawText(Result->Canvas->Handle, Msg.c_str(), Msg.Length() + 1, &TextRect,
    DT_EXPANDTABS | DT_CALCRECT | DT_WORDBREAK |
    Result->DrawTextBiDiModeFlagsReadingOnly());
  int MaxWidth = Screen->Width - HorzMargin * 2 - 32 - HorzSpacing - 30;
  if (TextRect.right > MaxWidth)
  {
    // this will truncate the text, we should implement something smarter eventually
    TextRect.right = MaxWidth;
  }
  const wchar_t * IconID = IconIDs[DlgType];
  int IconTextWidth = TextRect.Right;
  int IconTextHeight = TextRect.Bottom;
  if ((IconID != NULL) || !ImageName.IsEmpty())
  {
    IconTextWidth += 32 + HorzSpacing;
    if (IconTextHeight < 32)
    {
      IconTextHeight = 32;
    }
  }

  if (MoreMessages != NULL)
  {
    TMemo * MessageMemo = new TMemo(Result);
    MessageMemo->Parent = Result;
    MessageMemo->ReadOnly = true;
    MessageMemo->WantReturns = False;
    MessageMemo->ScrollBars = ssVertical;
    MessageMemo->Anchors = TAnchors() << akLeft << akRight << akTop; //akBottom;
    MessageMemo->Color = clBtnFace;
    MessageMemo->Lines->Text = MoreMessages->Text;

    Result->MessageMemo = MessageMemo;
  }

  int ButtonGroupWidth = 0;
  if (ButtonControlsCount > 0)
  {
    ButtonGroupWidth = ButtonWidth * ButtonControlsCount +
      ButtonSpacing * (ButtonControlsCount - 1);
  }

  int MoreMessageWidth = (MoreMessages != NULL ?
    MulDiv(mcMoreMessageWidth, DialogUnits.x, 4) : 0);
  int MoreMessageHeight = (MoreMessages != NULL ?
    MulDiv(mcMoreMessageHeight, DialogUnits.y, 12) : 0);

  int AClientWidth =
    (IconTextWidth > ButtonGroupWidth ? IconTextWidth : ButtonGroupWidth) +
    HorzMargin * 2;
  Result->ClientWidth = (AClientWidth > MoreMessageWidth ?
    AClientWidth : MoreMessageWidth);
  Result->ClientHeight = IconTextHeight + ButtonHeight + VertSpacing +
    VertMargin * 2 + MoreMessageHeight;
  Result->Left = (Screen->Width / 2) - (Result->Width / 2);
  Result->Top = (Screen->Height / 2) - (Result->Height / 2);
  if (DlgType != mtCustom)
  {
    Result->Caption = LoadResourceString(Captions[DlgType]);
  }
  else
  {
    Result->Caption = Application->Title;
  }

  if ((IconID != NULL) || !ImageName.IsEmpty())
  {
    TImage * Image = new TImage(Result);
    Image->Name = L"Image";
    Image->Parent = Result;
    if (!ImageName.IsEmpty())
    {
      std::auto_ptr<TPngImage> Png(new TPngImage());
      Png->LoadFromResourceName(0, ImageName);
      Image->Picture->Assign(Png.get());
    }
    else
    {
      Image->Picture->Icon->Handle = LoadIcon(0, IconID);
    }
    Image->SetBounds(HorzMargin, VertMargin, 32, 32);
  }

  TLabel * Message = new TLabel(Result);
  Result->Message = Message;
  Message->Name = L"Message";
  Message->Parent = Result;
  Message->WordWrap = true;
  Message->Caption = Msg;
  Message->BoundsRect = TextRect;
  Message->BiDiMode = Result->BiDiMode;
  // added to show & as & for messages containing !& pattern of custom commands
  // (suppose that we actually never want to use & as accel in message text)
  Message->ShowAccelChar = false;
  int ALeft = IconTextWidth - TextRect.Right + HorzMargin;
  Message->SetBounds(ALeft, VertMargin, TextRect.Right, TextRect.Bottom);
  int ButtonTop = IconTextHeight + VertMargin + VertSpacing + MoreMessageHeight;

  if (Result->MessageMemo != NULL)
  {
    Result->MessageMemo->BoundsRect = TRect(Message->Left,
      Message->Top + Message->Height + VertSpacing,
      Result->ClientWidth - HorzMargin,
      Message->Top + Message->Height + VertSpacing + MoreMessageHeight);
    // rather hack, whole control positioning is wrong
    if (Result->MessageMemo->Top + Result->MessageMemo->Height > ButtonTop - VertSpacing)
    {
      Result->MessageMemo->Height =
        (ButtonTop - VertSpacing) - Result->MessageMemo->Top;
    }
  }

  int X = (Result->ClientWidth - ButtonGroupWidth) / 2;
  for (int i = 0; i < ButtonControlsCount; i++)
  {
    ButtonControls[i]->SetBounds(X,
      ButtonTop, ButtonWidth, ButtonHeight);
    X += ButtonWidth + ButtonSpacing;
  }

  return Result;
}
//---------------------------------------------------------------------------
TForm * __fastcall CreateMoreMessageDialog(const UnicodeString & Msg,
  TStrings * MoreMessages, TMsgDlgType DlgType, TMsgDlgButtons Buttons,
  TQueryButtonAlias * Aliases, unsigned int AliasesCount,
  TMsgDlgBtn TimeoutResult, TButton ** TimeoutButton, const UnicodeString & ImageName)
{
  return TMessageForm::Create(Msg, MoreMessages, DlgType, Buttons,
    Aliases, AliasesCount, TimeoutResult, TimeoutButton, ImageName);
}
