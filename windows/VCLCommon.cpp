//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WinInterface.h"
#include "VCLCommon.h"

#include <Common.h>
#include <TextsWin.h>
#include <RemoteFiles.h>
#include <GUITools.h>
#include <Tools.h>

#include <FileCtrl.hpp>
#include <XPThemes.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void __fastcall AdjustListColumnsWidth(TListView* ListView, int RowCount)
{
  int OriginalWidth, NewWidth, i, CWidth, LastResizible;

  OriginalWidth = 0;
  LastResizible = -1;
  for (i = 0; i < ListView->Columns->Count; i++)
  {
    OriginalWidth += ListView->Columns->Items[i]->Width;
    if (ListView->Columns->Items[i]->Tag == 0)
    {
      LastResizible = i;
    }
  }
  assert(LastResizible >= 0);

  // when listview is virtual, ListView->Items->Count seems to return invalid
  // value, thus provide a method to pass actual count explicitly
  if (RowCount < 0)
  {
    RowCount = ListView->Items->Count;
  }
  
  NewWidth = 0;
  CWidth = ListView->ClientWidth;
  if ((ListView->VisibleRowCount < RowCount) &&
      (ListView->Width - ListView->ClientWidth < GetSystemMetrics(SM_CXVSCROLL)))
  {
    CWidth -= GetSystemMetrics(SM_CXVSCROLL);
  }
  for (i = 0; i < ListView->Columns->Count; i++)
  {
    if (i != LastResizible)
    {
      if (ListView->Columns->Items[i]->Tag == 0)
      {
        ListView->Columns->Items[i]->Width =
          (CWidth * ListView->Columns->Items[i]->Width) / OriginalWidth;
      }
      NewWidth += ListView->Columns->Items[i]->Width;
    }
  }
  ListView->Columns->Items[LastResizible]->Width = CWidth-NewWidth;
}
//---------------------------------------------------------------------------
void __fastcall EnableControl(TControl * Control, bool Enable)
{
  if (Control->Enabled != Enable)
  {
    if (Control->InheritsFrom(__classid(TWinControl)) &&
        (((TWinControl*)Control)->ControlCount > 0))
    {
      for (Integer Index = 0; Index < ((TWinControl*)Control)->ControlCount; Index++)
        EnableControl(((TWinControl*)Control)->Controls[Index], Enable);
    }
    Control->Enabled = Enable;
  }
  if (Control->InheritsFrom(__classid(TCustomEdit)) ||
            Control->InheritsFrom(__classid(TCustomComboBox)))
  {
    if (Enable) ((TEdit*)Control)->Color = clWindow;
      else ((TEdit*)Control)->Color = clBtnFace;
  }
};
//---------------------------------------------------------------------------
struct TSavedSystemSettings
{
  AnsiString FontName;
  bool Flipped;
};
//---------------------------------------------------------------------------
// Settings that must be set as soon as possible.
void __fastcall UseSystemSettingsPre(TCustomForm * Control, void ** Settings)
{
  if (Settings)
  {
    TSavedSystemSettings * SSettings = new TSavedSystemSettings();
    *Settings = static_cast<void*>(SSettings);
    SSettings->FontName = Control->Font->Name;
  }
  assert(Control && Control->Font);
  Control->Font->Name = "MS Shell Dlg";

  if (Control->HelpKeyword.IsEmpty())
  {
    // temporary help keyword to enable F1 key in all forms
    Control->HelpKeyword = "start";
  }
};
//---------------------------------------------------------------------------
// Settings that must be set only after whole form is constructed
void __fastcall UseSystemSettingsPost(TCustomForm * Control, void * Settings)
{
  bool Flip;
  AnsiString FlipStr = LoadStr(FLIP_CHILDREN);
  Flip = !FlipStr.IsEmpty() && static_cast<bool>(StrToInt(FlipStr));

  if (Settings != NULL)
  {
    static_cast<TSavedSystemSettings*>(Settings)->Flipped = Flip;
  }
  
  if (Flip)
  {
    Control->FlipChildren(true);
  }

  ResetSystemSettings(Control);
};
//---------------------------------------------------------------------------
void __fastcall UseSystemSettings(TCustomForm * Control, void ** Settings)
{
  UseSystemSettingsPre(Control, Settings);
  UseSystemSettingsPost(Control, (Settings != NULL) ? *Settings : NULL);
};
//---------------------------------------------------------------------------
void __fastcall ResetSystemSettings(TCustomForm * Control)
{
  XPTheme->ShowFocus(Control);
  XPTheme->ShowAccelerators(Control);
}
//---------------------------------------------------------------------------
void __fastcall RevokeSystemSettings(TCustomForm * Control,
  void * Settings)
{
  assert(Settings);
  if (static_cast<TSavedSystemSettings*>(Settings)->Flipped)
  {
    Control->FlipChildren(true);
  }
  delete Settings;
};
//---------------------------------------------------------------------------
class TPublicForm : public TForm
{
friend void __fastcall ShowAsModal(TForm * Form, void *& Storage);
friend void __fastcall HideAsModal(TForm * Form, void *& Storage);
};
//---------------------------------------------------------------------------
struct TShowAsModalStorage
{
  void * FocusWindowList;
  void * FocusActiveWindow;
};
//---------------------------------------------------------------------------
void __fastcall ShowAsModal(TForm * Form, void *& Storage)
{
  CancelDrag();
  if (GetCapture() != 0) SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
  ReleaseCapture();
  (static_cast<TPublicForm*>(Form))->FFormState << fsModal;

  TShowAsModalStorage * AStorage = new TShowAsModalStorage;

  AStorage->FocusActiveWindow = GetActiveWindow();

  AStorage->FocusWindowList = DisableTaskWindows(0);
  Form->Show();
  SendMessage(Form->Handle, CM_ACTIVATE, 0, 0);

  Storage = AStorage;
}
//---------------------------------------------------------------------------
void __fastcall HideAsModal(TForm * Form, void *& Storage)
{
  assert((static_cast<TPublicForm*>(Form))->FFormState.Contains(fsModal));
  TShowAsModalStorage * AStorage = static_cast<TShowAsModalStorage *>(Storage);
  Storage = NULL;

  SendMessage(Form->Handle, CM_DEACTIVATE, 0, 0);
  if (GetActiveWindow() != Form->Handle)
  {
    AStorage->FocusActiveWindow = 0;
  }
  Form->Hide();

  EnableTaskWindows(AStorage->FocusWindowList);

  if (AStorage->FocusActiveWindow != 0)
  {
    SetActiveWindow(AStorage->FocusActiveWindow);
  }

  (static_cast<TPublicForm*>(Form))->FFormState >> fsModal;

  delete AStorage;
}
//---------------------------------------------------------------------------
void __fastcall ReleaseAsModal(TForm * Form, void *& Storage)
{
  if (Storage != NULL)
  {
    HideAsModal(Form, Storage);
  }
}
//---------------------------------------------------------------------------
bool __fastcall SelectDirectory(AnsiString & Path, const AnsiString Prompt,
  bool PreserveFileName)
{
  bool Result;
  unsigned int ErrorMode;
  ErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

  try
  {
    AnsiString Directory;
    AnsiString FileName;
    if (!PreserveFileName || DirectoryExists(Path))
    {
      Directory = Path;
    }
    else
    {
      Directory = ExtractFilePath(Path);
      FileName = ExtractFileName(Path);
    }
    Result = SelectDirectory(Prompt, "", Directory);
    if (Result)
    {
      Path = Directory;
      if (!FileName.IsEmpty())
      {
        Path = IncludeTrailingBackslash(Path) + FileName;
      }
    }
  }
  __finally
  {
    SetErrorMode(ErrorMode);
  }

  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall ListViewAnyChecked(TListView * ListView, bool Checked)
{
  bool AnyChecked = false;
  for (int Index = 0; Index < ListView->Items->Count; Index++)
  {
    if (ListView->Items->Item[Index]->Checked == Checked)
    {
      AnyChecked = true;
      break;
    }
  }
  return AnyChecked;
}
//---------------------------------------------------------------------------
void __fastcall ListViewCheckAll(TListView * ListView,
  TListViewCheckAll CheckAll)
{
  bool Check;

  if (CheckAll == caToggle)
  {
    Check = ListViewAnyChecked(ListView, false);
  }
  else
  {
    Check = (CheckAll == caCheck);
  }

  for (int Index = 0; Index < ListView->Items->Count; Index++)
  {
    ListView->Items->Item[Index]->Checked = Check;
  }
}
//---------------------------------------------------------------------------
// Windows algorithm is as follows (tested on W2k):
// right:
//   is_delimiter(current)
//     false:
//       right(left(current) + 1)
//     true:
//       right(right(current) + 1)
// left:
//   right(left(current) + 1)
int CALLBACK PathWordBreakProc(char * Ch, int Current, int Len, int Code)
{
  char Delimiters[] = "\\/ ;,.";
  int Result;
  AnsiString ACh;
  // stupid unicode autodetection
  // (on WinXP (or rather for RichEdit 2.0) we get unicode on input)
  if ((Len > 1) && (Ch[1] == '\0'))
  {
    // this convertes the unicode to ansi
    ACh = (wchar_t*)Ch;
  }
  else
  {
    ACh = Ch;
  }
  if (Code == WB_ISDELIMITER)
  {
    // we return negacy of what WinAPI docs says
    Result = (strchr(Delimiters, ACh[Current + 1]) == NULL);
  }
  else if (Code == WB_LEFT)
  {
    Result = ACh.SubString(1, Current - 1).LastDelimiter(Delimiters);
  }
  else if (Code == WB_RIGHT)
  {
    if (Current == 0)
    {
      // will be called gain with Current == 1
      Result = 0;
    }
    else
    {
      const char * P = strpbrk(ACh.c_str() + Current - 1, Delimiters);
      if (P == NULL)
      {
        Result = Len;
      }
      else
      {
        Result = P - ACh.c_str() + 1;
      }
    }
  }
  else
  {
    assert(false);
    Result = 0;
  }
  return Result;
}
//---------------------------------------------------------------------------
class TPublicCustomCombo : public TCustomCombo
{
friend void __fastcall InstallPathWordBreakProc(TWinControl * Control);
};
//---------------------------------------------------------------------------
void __fastcall InstallPathWordBreakProc(TWinControl * Control)
{
  HWND Wnd;
  if (dynamic_cast<TCustomCombo*>(Control) != NULL)
  {
    TPublicCustomCombo * Combo =
      static_cast<TPublicCustomCombo *>(dynamic_cast<TCustomCombo *>(Control));
    Combo->HandleNeeded();
    Wnd = Combo->EditHandle;
  }
  else
  {
    Wnd = Control->Handle;
  }
  SendMessage(Wnd, EM_SETWORDBREAKPROC, 0, (LPARAM)(EDITWORDBREAKPROC)PathWordBreakProc);
}
//---------------------------------------------------------------------------
static void __fastcall RemoveHiddenControlsFromOrder(TControl ** ControlsOrder, int & Count)
{
  int Shift = 0;
  for (int Index = 0; Index < Count; Index++)
  {
    if (ControlsOrder[Index]->Visible)
    {
      ControlsOrder[Index - Shift] = ControlsOrder[Index];
    }
    else
    {
      Shift++;
    }
  }
  Count -= Shift;
}
//---------------------------------------------------------------------------
void __fastcall SetVerticalControlsOrder(TControl ** ControlsOrder, int Count)
{
  RemoveHiddenControlsFromOrder(ControlsOrder, Count);

  for (int Index = Count - 1; Index > 0; Index--)
  {
    if (ControlsOrder[Index]->Top < ControlsOrder[Index - 1]->Top)
    {
      int NewPosition = ControlsOrder[Index - 1]->Top +
        ControlsOrder[Index - 1]->Height;
      // because of empty docks
      if (ControlsOrder[Index - 1]->Height == 0)
      {
        NewPosition++;
      }
      ControlsOrder[Index]->Top = NewPosition;
      Index = Count;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall SetHorizontalControlsOrder(TControl ** ControlsOrder, int Count)
{
  RemoveHiddenControlsFromOrder(ControlsOrder, Count);

  for (int Index = Count - 1; Index > 0; Index--)
  {
    if ((ControlsOrder[Index]->Left < ControlsOrder[Index - 1]->Left) &&
        ControlsOrder[Index - 1]->Visible)
    {
      ControlsOrder[Index]->Left = ControlsOrder[Index - 1]->Left +
        ControlsOrder[Index - 1]->Width;
      Index = Count;
    }
  }
}
//---------------------------------------------------------------------------
TPoint __fastcall GetAveCharSize(TCanvas* Canvas)
{
  Integer I;
  Char Buffer[52];
  TSize Result;
  for (I = 0; I <= 25; I++) Buffer[I] = (Char)('A' + I);
  for (I = 0; I <= 25; I++) Buffer[I+26] = (Char)('a' + I);
  GetTextExtentPoint(Canvas->Handle, Buffer, 52, &Result);
  return TPoint(Result.cx / 52, Result.cy);
}
//---------------------------------------------------------------------------
void __fastcall MakeNextInTabOrder(TWinControl * Control, TWinControl * After)
{
  if (After->TabOrder > Control->TabOrder)
  {
    After->TabOrder = Control->TabOrder;
  }
  else if (After->TabOrder < Control->TabOrder - 1)
  {
    After->TabOrder = static_cast<TTabOrder>(Control->TabOrder - 1);
  }
}
//---------------------------------------------------------------------------
void __fastcall CutFormToDesktop(TForm * Form)
{
  if (Form->Top + Form->Height > Screen->WorkAreaTop + Screen->WorkAreaHeight)
  {
    Form->Height = Screen->WorkAreaTop + Screen->WorkAreaHeight - Form->Top;
  }
  if (Form->Left + Form->Width >= Screen->WorkAreaLeft + Screen->WorkAreaWidth)
  {
    Form->Width = Screen->WorkAreaLeft + Screen->WorkAreaWidth - Form->Left;
  }
}
//---------------------------------------------------------------------------
void __fastcall SetCorrectFormParent(TForm * Form)
{
  try
  {
    // Kind of hack (i do not understand this much).
    // Rationale: for example when the preferences window is opened from login dialog
    // settings Parent to Screen->ActiveForm leads to "cannot focus disabled control",
    // so we set Parent only when absolutelly necessary 
    // (dialog opened from log window or editor)
    // TODO: does not work for dialogs opened from preferences dialog
    if ((Application->MainForm != NULL) &&
        (Application->MainForm != Screen->ActiveForm))
    {
      // this should better be check for modal form
      AnsiString C = Screen->ActiveForm->Caption;
      if ((Screen->ActiveForm != NULL) &&
          (Screen->ActiveForm->BorderStyle != bsDialog))
      {
        Form->ParentWindow = Screen->ActiveForm->Handle;
      }
    }
  }
  catch(...)
  {
    // avoid any errors, however we want to know about this in debug version.
    #ifdef _DEBUG
    throw;
    #endif
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TPublicControl : public TWinControl
{
friend void __fastcall ControlWndProc(TWinControl * Control, TMessage & Message);
};
//---------------------------------------------------------------------------
void __fastcall ControlWndProc(TWinControl * Control, TMessage & Message)
{
  TPublicControl * PublicControl = static_cast<TPublicControl *>(Control);
  PublicControl->WndProc(Message);
}
//---------------------------------------------------------------------------
static void __fastcall FocusableLabelCanvas(TStaticText * StaticText,
  TControlCanvas ** ACanvas, TRect & R)
{
  TControlCanvas * Canvas = new TControlCanvas();
  try
  {
    Canvas->Control = StaticText;

    R = StaticText->ClientRect;

    AnsiString Caption = StaticText->Caption;
    bool AccelChar = false;
    if (StaticText->ShowAccelChar)
    {
      Caption = StripHotkey(Caption);
      AccelChar = (Caption != StaticText->Caption);
    }
      
    TSize TextSize = Canvas->TextExtent(Caption);
    assert(StaticText->BorderStyle == sbsNone); // not taken into account
    if (AccelChar)
    {
      TextSize.cy += 2;
    }
  
    R.Bottom = R.Top + TextSize.cy;
    if (StaticText->Alignment == taRightJustify)
    {
      R.Left = R.Right - TextSize.cx;
    }
    else
    {
      R.Right = R.Left + TextSize.cx;
    }
  }
  __finally
  {
    if (ACanvas == NULL)
    {
      delete Canvas;
    }
  }

  if (ACanvas != NULL)
  {
    *ACanvas = Canvas;
  }
}
//---------------------------------------------------------------------------
static void __fastcall FocusableLabelWindowProc(void * Data, TMessage & Message,
  bool & Clicked)
{
  Clicked = false;
  TStaticText * StaticText = static_cast<TStaticText *>(Data);
  if (Message.Msg == WM_LBUTTONDOWN)
  {
    StaticText->SetFocus();
    // in case the action takes long, make sure focus is shown immediatelly
    UpdateWindow(StaticText->Handle);
    Clicked = true;
    Message.Result = 1;
  }
  if (Message.Msg == WM_RBUTTONDOWN)
  {
    StaticText->SetFocus();
    Message.Result = 1;
  }
  else if (Message.Msg == WM_CHAR)
  {
    if (reinterpret_cast<TWMChar &>(Message).CharCode == ' ')
    {
      Clicked = true;
      Message.Result = 1;
    }
    else
    {
      ControlWndProc(StaticText, Message);
    }
  }
  else if (Message.Msg == CM_DIALOGCHAR)
  {                                     
    if (StaticText->Enabled && StaticText->ShowAccelChar &&
        IsAccel(reinterpret_cast<TCMDialogChar &>(Message).CharCode, StaticText->Caption))
    {
      StaticText->SetFocus();
      // in case the action takes long, make sure focus is shown immediatelly
      UpdateWindow(StaticText->Handle);
      Clicked = true;
      Message.Result = 1;
    }
    else
    {
      ControlWndProc(StaticText, Message);
    }    
  }
  else
  {
    ControlWndProc(StaticText, Message);
  }

  if (Message.Msg == WM_PAINT)
  {
    TRect R;
    TControlCanvas * Canvas;
    FocusableLabelCanvas(StaticText, &Canvas, R); 
    try
    {
      if (StaticText->Focused())
      {
        Canvas->DrawFocusRect(R);
      }
      else if (!StaticText->Font->Style.Contains(fsUnderline))
      {
        Canvas->Pen->Style = psDot;
        Canvas->Brush->Style = bsClear;
        if (!StaticText->Enabled)
        {
          Canvas->Pen->Color = clBtnHighlight;
          Canvas->MoveTo(R.Left + 1 + 1, R.Bottom);
          Canvas->LineTo(R.Right + 1, R.Bottom);
          Canvas->Pen->Color = clGrayText;
        }
        Canvas->MoveTo(R.Left + 1, R.Bottom - 1);
        Canvas->LineTo(R.Right, R.Bottom - 1);
      }
    }
    __finally
    {
      delete Canvas;
    }
  }
  else if ((Message.Msg == WM_SETFOCUS) || (Message.Msg == WM_KILLFOCUS) ||
    (Message.Msg == CM_ENABLEDCHANGED))
  {
    StaticText->Invalidate();
  }
}
//---------------------------------------------------------------------------
static THintWindow * PersistentHintWindow = NULL;
static TControl * PersistentHintControl = NULL;
//---------------------------------------------------------------------------
void __fastcall CancelPersistentHint()
{
  if (PersistentHintWindow != NULL)
  {
    PersistentHintControl = NULL;
    SAFE_DESTROY(PersistentHintWindow);
  }
}
//---------------------------------------------------------------------------
void __fastcall ShowPersistentHint(TControl * Control, TPoint HintPos)
{
  CancelPersistentHint();

  THintInfo HintInfo;
  HintInfo.HintControl = Control;
  HintInfo.HintPos = HintPos;
  HintInfo.HintMaxWidth = Screen->Width;
  HintInfo.HintColor = Application->HintColor;
  HintInfo.HintStr = GetShortHint(Control->Hint);
  HintInfo.HintData = NULL;

  bool CanShow = true;
  Application->OnShowHint(HintInfo.HintStr, CanShow, HintInfo);

  if (CanShow)
  {
    PersistentHintControl = Control;
      
    PersistentHintWindow = new THintWindow(Application);
    PersistentHintWindow->BiDiMode = Control->BiDiMode;
    PersistentHintWindow->Color = HintInfo.HintColor;

    TRect HintWinRect;
    if (HintInfo.HintMaxWidth < Control->Width)
    {
      HintInfo.HintMaxWidth = Control->Width;
    }
    HintWinRect = PersistentHintWindow->CalcHintRect(
      HintInfo.HintMaxWidth, HintInfo.HintStr, HintInfo.HintData);
    OffsetRect(HintWinRect, HintInfo.HintPos.x, HintInfo.HintPos.y);
    // TODO: right align window placement for UseRightToLeftAlignment, see Forms.pas

    PersistentHintWindow->ActivateHintData(HintWinRect, HintInfo.HintStr, HintInfo.HintData);
  }
}
//---------------------------------------------------------------------------
static void __fastcall HintLabelWindowProc(void * Data, TMessage & Message)
{
  bool Clicked = false;

  TStaticText * StaticText = static_cast<TStaticText *>(Data);
  if (Message.Msg == CM_HINTSHOW)
  {
    TCMHintShow & HintShow = reinterpret_cast<TCMHintShow &>(Message);
    if (PersistentHintControl == StaticText)
    {
      // do not allow standard hint when persistent is already shown
      HintShow.Result = 1;
    }
    else
    {
      HintShow.HintInfo->HideTimeout = 100000; // never
    }
  }
  else if (Message.Msg == CN_KEYDOWN)
  {
    if ((reinterpret_cast<TWMKey &>(Message).CharCode == VK_ESCAPE) &&
        (PersistentHintControl == StaticText))
    {
      CancelPersistentHint();
      StaticText->Invalidate();
      Message.Result = 1;
    }
    else
    {
      FocusableLabelWindowProc(Data, Message, Clicked);
    }
  }
  else
  {
    FocusableLabelWindowProc(Data, Message, Clicked);
  }

  if ((Message.Msg == WM_DESTROY) || (Message.Msg == WM_KILLFOCUS))
  {
    if (PersistentHintControl == StaticText)
    {
      CancelPersistentHint();
    }
  }

  if (Clicked && (PersistentHintControl != StaticText))
  {
    TRect R;
    TPoint HintPos;

    FocusableLabelCanvas(StaticText, NULL, R); 
    HintPos.y = R.Bottom - R.Top;
    HintPos.x = R.Left;

    ShowPersistentHint(StaticText, StaticText->ClientToScreen(HintPos));
  }
}
//---------------------------------------------------------------------------
void __fastcall HintLabel(TStaticText * StaticText, AnsiString Hint)
{
  StaticText->ParentFont = true;
  if (!Hint.IsEmpty())
  {
    StaticText->Hint = Hint;
  }
  StaticText->ShowHint = true;
  StaticText->Cursor = crHandPoint;

  TWndMethod WindowProc;
  ((TMethod*)&WindowProc)->Data = StaticText;
  ((TMethod*)&WindowProc)->Code = HintLabelWindowProc;
  StaticText->WindowProc = WindowProc;
}
//---------------------------------------------------------------------------
static void __fastcall LinkLabelClick(TStaticText * StaticText)
{
  if (StaticText->OnClick != NULL)
  {
    StaticText->OnClick(StaticText);
  }
  else
  {
    AnsiString Url = StaticText->Caption;
    if (!SameText(Url.SubString(1, 4), "http") && (Url.Pos("@") > 0))
    {
      Url = "mailto:" + Url;
    }
    OpenBrowser(Url);
  }
}
//---------------------------------------------------------------------------
static void __fastcall LinkLabelWindowProc(void * Data, TMessage & Message)
{
  bool Clicked = false;

  TStaticText * StaticText = static_cast<TStaticText *>(Data);
  if (Message.Msg == WM_CONTEXTMENU)
  {
    TWMContextMenu & ContextMenu = reinterpret_cast<TWMContextMenu &>(Message);

    if ((ContextMenu.Pos.x < 0) && (ContextMenu.Pos.y < 0))
    {
      TRect R;
      FocusableLabelCanvas(StaticText, NULL, R); 
      TPoint P = StaticText->ClientToScreen(TPoint(R.Left, R.Bottom));
      ContextMenu.Pos.x = static_cast<short>(P.x);
      ContextMenu.Pos.y = static_cast<short>(P.y);
    }
  }
  else if (Message.Msg == WM_KEYDOWN)
  {
    TWMKey & Key = reinterpret_cast<TWMKey &>(Message);
    if ((GetKeyState(VK_CONTROL) < 0) && (Key.CharCode == 'C'))
    {
      CopyToClipboard(StaticText->Caption);
      Message.Result = 1;
    }
    else
    {
      FocusableLabelWindowProc(Data, Message, Clicked);
    }
  }  

  FocusableLabelWindowProc(Data, Message, Clicked);

  if (Message.Msg == WM_DESTROY)
  {
    delete StaticText->PopupMenu;
    assert(StaticText->PopupMenu == NULL);
  }

  if (Clicked)
  {
    LinkLabelClick(StaticText);
  }
}
//---------------------------------------------------------------------------
static void __fastcall LinkLabelContextMenuClick(void * Data, TObject * Sender)
{
  TStaticText * StaticText = static_cast<TStaticText *>(Data);
  TMenuItem * MenuItem = dynamic_cast<TMenuItem *>(Sender);
  assert(MenuItem != NULL);

  if (MenuItem->Tag == 0)
  {
    LinkLabelClick(StaticText);
  }
  else
  {
    CopyToClipboard(StaticText->Caption);
  }
}
//---------------------------------------------------------------------------
void __fastcall LinkLabel(TStaticText * StaticText, AnsiString Url,
  TNotifyEvent OnEnter)
{
  StaticText->ParentFont = true;
  StaticText->Font->Style = StaticText->Font->Style << fsUnderline;
  StaticText->Font->Color = clBlue;
  StaticText->Cursor = crHandPoint;
  reinterpret_cast<TButton*>(StaticText)->OnEnter = OnEnter;
  if (!Url.IsEmpty())
  {
    StaticText->Caption = Url;
  }

  if (StaticText->OnClick == NULL)
  {
    assert(StaticText->PopupMenu == NULL);
    StaticText->PopupMenu = new TPopupMenu(StaticText);
    try
    {
      TNotifyEvent ContextMenuOnClick;
      ((TMethod*)&ContextMenuOnClick)->Data = StaticText;
      ((TMethod*)&ContextMenuOnClick)->Code = LinkLabelContextMenuClick;
    
      TMenuItem * Item;

      Item = new TMenuItem(StaticText->PopupMenu);
      Item->Caption = LoadStr(URL_LINK_OPEN);
      Item->Tag = 0;
      Item->ShortCut = ShortCut(' ', TShiftState());
      Item->OnClick = ContextMenuOnClick;
      StaticText->PopupMenu->Items->Add(Item);

      Item = new TMenuItem(StaticText->PopupMenu);
      Item->Caption = LoadStr(URL_LINK_COPY);
      Item->Tag = 1;
      Item->ShortCut = ShortCut('C', TShiftState() << ssCtrl);
      Item->OnClick = ContextMenuOnClick;
      StaticText->PopupMenu->Items->Add(Item);
    }
    catch(...)
    {
      delete StaticText->PopupMenu;
      assert(StaticText->PopupMenu == NULL);
      throw;
    }
  }

  TWndMethod WindowProc;
  ((TMethod*)&WindowProc)->Data = StaticText;
  ((TMethod*)&WindowProc)->Code = LinkLabelWindowProc;
  StaticText->WindowProc = WindowProc;
}

