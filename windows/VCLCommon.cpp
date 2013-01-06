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
#include <PathLabel.hpp>
#include <PasTools.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void __fastcall AdjustListColumnsWidth(TListView* ListView, int RowCount, int RightPad)
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
  CWidth = ListView->ClientWidth - RightPad;
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
static void __fastcall SetParentColor(TControl * Control)
{
  TColor Color = clBtnFace;

  ((TEdit*)Control)->Color = Color;
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
      Control->InheritsFrom(__classid(TCustomComboBox)) ||
      Control->InheritsFrom(__classid(TCustomListView)))
  {
    if (Enable)
    {
      ((TEdit*)Control)->Color = clWindow;
    }
    else
    {
      ((TEdit*)Control)->Color = clBtnFace;
    }
  }
};
//---------------------------------------------------------------------------
void __fastcall ReadOnlyControl(TControl * Control, bool ReadOnly)
{
  if (Control->InheritsFrom(__classid(TCustomEdit)))
  {
    ((TEdit*)Control)->ReadOnly = ReadOnly;
    if (ReadOnly)
    {
      SetParentColor(Control);
    }
    else
    {
      ((TEdit*)Control)->Color = clWindow;
    }
  }
  else
  {
    assert(false);
  }
}
//---------------------------------------------------------------------------
struct TSavedSystemSettings
{
  TCustomForm * Form;
  UnicodeString FontName;
  bool Flipped;
  TWndMethod OldWndProc;
};
//---------------------------------------------------------------------------
class TPublicControl : public TWinControl
{
friend TWndMethod __fastcall ControlWndProc(TWinControl * Control);
};
//---------------------------------------------------------------------------
TWndMethod __fastcall ControlWndProc(TWinControl * Control)
{
  TPublicControl * PublicControl = static_cast<TPublicControl *>(Control);
  return &PublicControl->WndProc;
}
//---------------------------------------------------------------------------
static Forms::TMonitor * LastMonitor = NULL;
//---------------------------------------------------------------------------
inline void __fastcall DoFormWindowProc(TCustomForm * Form, TWndMethod WndProc,
  TMessage & Message)
{
  if ((Message.Msg == WM_SYSCOMMAND) &&
      (Message.WParam == SC_CONTEXTHELP))
  {
    InvokeHelp(Form->ActiveControl);
    Message.Result = 1;
  }
  else if (Message.Msg == CM_SHOWINGCHANGED)
  {
    TForm * AForm = dynamic_cast<TForm *>(Form);
    assert(AForm != NULL);
    if ((Application->MainForm == Form) ||
        // this particularly happens if error occurs while main
        // window is being shown (e.g. non existent local directory when opening
        // explorer)
        ((Application->MainForm != NULL) && !Application->MainForm->Visible))
    {
      if (Form->Showing)
      {
        SendMessage(Form->Handle, WM_SETICON, ICON_BIG, reinterpret_cast<long>(Application->Icon->Handle));
      }

      if (!Form->Showing)
      {
        // when closing main form, remember its monitor,
        // so that the next form is shown on the same one
        LastMonitor = Form->Monitor;
      }
      else if ((LastMonitor != NULL) && (LastMonitor != Form->Monitor) &&
                Form->Showing)
      {
        // would actually always be poScreenCenter, see _SafeFormCreate
        if ((AForm->Position == poMainFormCenter) ||
            (AForm->Position == poOwnerFormCenter) ||
            (AForm->Position == poScreenCenter))
        {
          // this would typically be an authentication dialog,
          // but it may as well be an message box

          // taken from TCustomForm::SetWindowToMonitor
          AForm->SetBounds(LastMonitor->Left + ((LastMonitor->Width - AForm->Width) / 2),
            LastMonitor->Top + ((LastMonitor->Height - AForm->Height) / 2),
             AForm->Width, AForm->Height);
          AForm->Position = poDesigned;
        }
        else if ((AForm->Position != poDesigned) &&
                 (AForm->Position != poDefaultPosOnly))
        {
          // we do not expect any other positioning
          assert(false);
        }
      }
      else
      {
        TForm * AForm = dynamic_cast<TForm *>(Form);
        assert(AForm != NULL);
        // otherwise it would not get centered
        if ((AForm->Position == poMainFormCenter) ||
            (AForm->Position == poOwnerFormCenter))
        {
          AForm->Position = poScreenCenter;
        }
      }
    }
    bool WasFormCenter =
      (AForm->Position == poMainFormCenter) ||
      (AForm->Position == poOwnerFormCenter);
    WndProc(Message);
    // Make sure dialogs are shown on-screen even if center of the main window
    // is off-screen. Occurs e.g. if you move the main window so that
    // only window title is visible above taksbar.
    if (Form->Showing && WasFormCenter && (AForm->Position == poDesigned))
    {
      TRect Rect;
      // Reading Form.Left/Form.Top instead here does not work, likely due to some
      // bug, when querying TProgressForm opened from TEditorForm (reloading remote file)
      GetWindowRect(Form->Handle, &Rect);

      int Left = Rect.Left;
      int Top = Rect.Top;
      TRect WorkArea = AForm->Monitor->WorkareaRect;

      if (Left + Rect.Width() > WorkArea.Right)
      {
        Left = WorkArea.Right - Rect.Width();
      }
      if (Left < WorkArea.Left)
      {
        Left = WorkArea.Left;
      }
      if (Top + Rect.Height() > WorkArea.Bottom)
      {
        Top = WorkArea.Bottom - Rect.Height();
      }
      if (Top < WorkArea.Top)
      {
        Top = WorkArea.Top;
      }
      if ((Left != Rect.Left) ||
          (Top != Rect.Top))
      {
        SetWindowPos(Form->Handle, 0, Left, Top, Rect.Width(), Rect.Height(),
          SWP_NOZORDER + SWP_NOACTIVATE);
      }
    }
  }
  else
  {
    WndProc(Message);
  }
}
//---------------------------------------------------------------------------
static void __fastcall FormWindowProc(void * Data, TMessage & Message)
{
  TCustomForm * Form = static_cast<TCustomForm *>(Data);
  DoFormWindowProc(Form, ControlWndProc(Form), Message);
}
//---------------------------------------------------------------------------
static void __fastcall FormWindowProcEx(void * Data, TMessage & Message)
{
  TSavedSystemSettings * SSettings = static_cast<TSavedSystemSettings *>(Data);
  DoFormWindowProc(SSettings->Form, SSettings->OldWndProc, Message);
}
//---------------------------------------------------------------------------
void __fastcall InitializeSystemSettings()
{
}
//---------------------------------------------------------------------------
void __fastcall FinalizeSystemSettings()
{
}
//---------------------------------------------------------------------------
// Settings that must be set as soon as possible.
void __fastcall UseSystemSettingsPre(TCustomForm * Control, void ** Settings)
{
  LocalSystemSettings(Control);

  TWndMethod WindowProc;

  if (Settings)
  {
    TSavedSystemSettings * SSettings;
    SSettings = new TSavedSystemSettings();
    *Settings = static_cast<void*>(SSettings);
    SSettings->Form = Control;
    SSettings->FontName = Control->Font->Name;
    SSettings->OldWndProc = Control->WindowProc;

    ((TMethod*)&WindowProc)->Data = SSettings;
    ((TMethod*)&WindowProc)->Code = FormWindowProcEx;
  }
  else
  {
    ((TMethod*)&WindowProc)->Data = Control;
    ((TMethod*)&WindowProc)->Code = FormWindowProc;
  }

  Control->WindowProc = WindowProc;

  assert(Control && Control->Font);
  Control->Font->Name = L"MS Shell Dlg";

  if (Control->HelpKeyword.IsEmpty())
  {
    // temporary help keyword to enable F1 key in all forms
    Control->HelpKeyword = L"start";
  }
};
//---------------------------------------------------------------------------
// Settings that must be set only after whole form is constructed
void __fastcall UseSystemSettingsPost(TCustomForm * Control, void * Settings)
{
  bool Flip;
  UnicodeString FlipStr = LoadStr(FLIP_CHILDREN);
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
void __fastcall ResetSystemSettings(TCustomForm * /*Control*/)
{
  // noop
}
//---------------------------------------------------------------------------
void __fastcall DeleteSystemSettings(TCustomForm * Control, void * Settings)
{
  assert(Settings);
  TSavedSystemSettings * SSettings = static_cast<TSavedSystemSettings *>(Settings);

  Control->WindowProc = SSettings->OldWndProc;
  delete SSettings;
}
//---------------------------------------------------------------------------
void __fastcall RevokeSystemSettings(TCustomForm * Control, void * Settings)
{
  assert(Settings);
  TSavedSystemSettings* SSettings = static_cast<TSavedSystemSettings*>(Settings);
  if (SSettings->Flipped)
  {
    Control->FlipChildren(true);
  }
  DeleteSystemSettings(Control, Settings);
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
  HWND FocusActiveWindow;
  TFocusState FocusState;
};
//---------------------------------------------------------------------------
void __fastcall ShowAsModal(TForm * Form, void *& Storage)
{
  SetCorrectFormParent(Form);
  CancelDrag();
  if (GetCapture() != 0) SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
  ReleaseCapture();
  (static_cast<TPublicForm*>(Form))->FFormState << fsModal;

  TShowAsModalStorage * AStorage = new TShowAsModalStorage;

  AStorage->FocusActiveWindow = GetActiveWindow();
  AStorage->FocusState = SaveFocusState();
  Screen->SaveFocusedList->Insert(0, Screen->FocusedForm);
  Screen->FocusedForm = Form;
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

  if (Screen->SaveFocusedList->Count > 0)
  {
    Screen->FocusedForm = static_cast<TCustomForm *>(Screen->SaveFocusedList->First());
    Screen->SaveFocusedList->Remove(Screen->FocusedForm);
  }
  else
  {
    Screen->FocusedForm = NULL;
  }

  if (AStorage->FocusActiveWindow != 0)
  {
    SetActiveWindow(AStorage->FocusActiveWindow);
  }

  RestoreFocusState(AStorage->FocusState);

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
bool __fastcall SelectDirectory(UnicodeString & Path, const UnicodeString Prompt,
  bool PreserveFileName)
{
  bool Result;
  unsigned int ErrorMode;
  ErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

  try
  {
    UnicodeString Directory;
    UnicodeString FileName;
    if (!PreserveFileName || DirectoryExists(Path))
    {
      Directory = Path;
    }
    else
    {
      Directory = ExtractFilePath(Path);
      FileName = ExtractFileName(Path);
    }
    Result = SelectDirectory(Prompt, L"", Directory);
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
void __fastcall ComboAutoSwitchInitialize(TComboBox * ComboBox)
{
  int PrevIndex = ComboBox->ItemIndex;
  ComboBox->Items->BeginUpdate();
  try
  {
    ComboBox->Clear();
    ComboBox->Items->Add(LoadStr(AUTO_SWITCH_AUTO));
    ComboBox->Items->Add(LoadStr(AUTO_SWITCH_OFF));
    ComboBox->Items->Add(LoadStr(AUTO_SWITCH_ON));
  }
  __finally
  {
    ComboBox->Items->EndUpdate();
  }
  assert(PrevIndex < ComboBox->Items->Count);
  ComboBox->ItemIndex = PrevIndex;
}
//---------------------------------------------------------------------------
void __fastcall ComboAutoSwitchLoad(TComboBox * ComboBox, TAutoSwitch Value)
{
  ComboBox->ItemIndex = 2 - Value;
  if (ComboBox->ItemIndex < 0)
  {
    ComboBox->ItemIndex = 0;
  }
}
//---------------------------------------------------------------------------
TAutoSwitch __fastcall ComboAutoSwitchSave(TComboBox * ComboBox)
{
  return (TAutoSwitch)(2 - ComboBox->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall CheckBoxAutoSwitchLoad(TCheckBox * CheckBox, TAutoSwitch Value)
{
  switch (Value)
  {
    case asOn:
      CheckBox->State = cbChecked;
      break;
    case asOff:
      CheckBox->State = cbUnchecked;
      break;
    default:
      CheckBox->State = cbGrayed;
      break;
  }
}
//---------------------------------------------------------------------------
TAutoSwitch __fastcall CheckBoxAutoSwitchSave(TCheckBox * CheckBox)
{
  switch (CheckBox->State)
  {
    case cbChecked:
      return asOn;
    case cbUnchecked:
      return asOff;
    default:
      return asAuto;
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
int CALLBACK PathWordBreakProc(wchar_t * Ch, int Current, int Len, int Code)
{
  wchar_t Delimiters[] = L"\\/ ;,.";
  int Result;
  UnicodeString ACh(Ch, Len);
  if (Code == WB_ISDELIMITER)
  {
    // we return negacy of what WinAPI docs says
    Result = (wcschr(Delimiters, ACh[Current + 1]) == NULL);
  }
  else if (Code == WB_LEFT)
  {
    Result = ACh.SubString(1, Current - 1).LastDelimiter(Delimiters);
  }
  else if (Code == WB_RIGHT)
  {
    if (Current == 0)
    {
      // will be called again with Current == 1
      Result = 0;
    }
    else
    {
      const wchar_t * P = wcspbrk(ACh.c_str() + Current - 1, Delimiters);
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
  // Since we are setting Application->ModalPopupMode = pmAuto,
  // this has to be called from OnShow, not from constructor anymore,
  // to have any effect

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
void __fastcall RepaintStatusBar(TCustomStatusBar * StatusBar)
{
  StatusBar->SimplePanel = !StatusBar->SimplePanel;
  StatusBar->SimplePanel = !StatusBar->SimplePanel;
}
//---------------------------------------------------------------------------
void __fastcall SetVerticalControlsOrder(TControl ** ControlsOrder, int Count)
{
  RemoveHiddenControlsFromOrder(ControlsOrder, Count);

  if (Count > 0)
  {
    TWinControl * CommonParent = ControlsOrder[0]->Parent;
    CommonParent->DisableAlign();
    try
    {
      int Top = 0;
      for (int Index = 0; Index < Count; Index++)
      {
        assert(ControlsOrder[Index]->Parent == CommonParent);
        if ((Index == 0) || (Top > ControlsOrder[Index]->Top))
        {
          Top = ControlsOrder[Index]->Top;
        }
      }

      for (int Index = 0; Index < Count; Index++)
      {
        TControl * Control = ControlsOrder[Index];
        Control->Top = Top;
        if (((Control->Align == alTop) || (Control->Align == alBottom)) ||
            ((Index == Count - 1) || (ControlsOrder[Index + 1]->Align == alBottom)))
        {
          Top += Control->Height;
        }
      }
    }
    __finally
    {
      CommonParent->EnableAlign();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall SetHorizontalControlsOrder(TControl ** ControlsOrder, int Count)
{
  RemoveHiddenControlsFromOrder(ControlsOrder, Count);

  if (Count > 0)
  {
    TWinControl * CommonParent = ControlsOrder[0]->Parent;
    CommonParent->DisableAlign();
    try
    {
      int Left = 0;
      for (int Index = 0; Index < Count; Index++)
      {
        assert(ControlsOrder[Index]->Parent == CommonParent);
        if ((Index == 0) || (Left > ControlsOrder[Index]->Left))
        {
          Left = ControlsOrder[Index]->Left;
        }
      }

      for (int Index = 0; Index < Count; Index++)
      {
        TControl * Control = ControlsOrder[Index];
        Control->Left = Left;
        if (((Control->Align == alLeft) || (Control->Align == alRight)) ||
            ((Index == Count - 1) || (ControlsOrder[Index + 1]->Align == alRight)))
        {
          Left += Control->Width;
        }
        // vertical alignment has priority, so alBottom-aligned controls start
        // at the very left, even if there are any alLeft/alRight controls.
        // for the reason this code is not necessary in SetVerticalControlsOrder.
        // we could exit the loop as well here.
        if ((Index == Count - 1) || (ControlsOrder[Index + 1]->Align == alBottom))
        {
          Left = 0;
        }
      }
    }
    __finally
    {
      CommonParent->EnableAlign();
    }
  }
}
//---------------------------------------------------------------------------
TPoint __fastcall GetAveCharSize(TCanvas* Canvas)
{
  Integer I;
  wchar_t Buffer[52];
  TSize Result;
  for (I = 0; I <= 25; I++) Buffer[I] = (wchar_t)(L'A' + I);
  for (I = 0; I <= 25; I++) Buffer[I+26] = (wchar_t)(L'a' + I);
  GetTextExtentPoint32(Canvas->Handle, Buffer, 52, &Result);
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
  assert(Form->Monitor != NULL);
  TRect Workarea = Form->Monitor->WorkareaRect;
  if (Form->Top + Form->Height > Workarea.Bottom)
  {
    Form->Height = Workarea.Bottom - Form->Top;
  }
  if (Form->Left + Form->Width >= Workarea.Right)
  {
    Form->Width = Workarea.Right - Form->Left;
  }
}
//---------------------------------------------------------------------------
void __fastcall UpdateFormPosition(TCustomForm * Form, TPosition Position)
{
  if ((Position == poScreenCenter) ||
      (Position == poOwnerFormCenter) ||
      (Position == poMainFormCenter))
  {
    TCustomForm * CenterForm = NULL;
    if ((Position == poOwnerFormCenter) ||
        (Position == poMainFormCenter))
    {
      CenterForm = Application->MainForm;
      if ((Position == poOwnerFormCenter) &&
          (dynamic_cast<TCustomForm*>(Form->Owner) != NULL))
      {
        CenterForm = dynamic_cast<TCustomForm*>(Form->Owner);
      }
    }

    TRect Bounds = Form->BoundsRect;
    int X, Y;
    if (CenterForm != NULL)
    {
      X = ((((TForm *)CenterForm)->Width - Bounds.Width()) / 2) +
        ((TForm *)CenterForm)->Left;
      Y = ((((TForm *)CenterForm)->Height - Bounds.Height()) / 2) +
        ((TForm *)CenterForm)->Top;
    }
    else
    {
      X = (Screen->Width - Bounds.Width()) / 2;
      Y = (Screen->Height - Bounds.Height()) / 2;
    }

    if (X < 0)
    {
      X = 0;
    }
    if (Y < 0)
    {
      Y = 0;
    }

    Form->SetBounds(X, Y, Bounds.Width(), Bounds.Height());
  }
}
//---------------------------------------------------------------------------
void __fastcall ResizeForm(TCustomForm * Form, int Width, int Height)
{
  if (Height > Screen->WorkAreaHeight)
  {
    Height = Screen->WorkAreaHeight;
  }
  if (Width > Screen->WorkAreaWidth)
  {
    Width = Screen->WorkAreaWidth;
  }
  if (Height < Form->Constraints->MinHeight)
  {
    Height = Form->Constraints->MinHeight;
  }
  if (Width < Form->Constraints->MinWidth)
  {
    Width = Form->Constraints->MinWidth;
  }
  TRect Bounds = Form->BoundsRect;
  int Top = Bounds.Top + ((Bounds.Height() - Height) / 2);
  int Left = Bounds.Left + ((Bounds.Width() - Width) / 2);
  if (Top + Height > Screen->WorkAreaTop + Screen->WorkAreaHeight)
  {
    Top = Screen->WorkAreaTop + Screen->WorkAreaHeight - Height;
  }
  if (Left + Width >= Screen->WorkAreaLeft + Screen->WorkAreaWidth)
  {
    Left = Screen->WorkAreaLeft + Screen->WorkAreaWidth - Width;
  }
  if (Top < 0)
  {
    Top = 0;
  }
  if (Left < 0)
  {
    Left = 0;
  }
  Form->SetBounds(Left, Top, Width, Height);
  Bounds = Form->BoundsRect;
  // due to constraints, form can remain larger, make sure it is centered although
  Left = Bounds.Left + ((Width - Bounds.Width()) / 2);
  Top = Bounds.Top + ((Height - Bounds.Height()) / 2);
  Form->SetBounds(Left, Top, Width, Height);
}
//---------------------------------------------------------------------------
TComponent * __fastcall GetFormOwner()
{
  if (Screen->ActiveForm != NULL)
  {
    return Screen->ActiveForm;
  }
  else
  {
    return Application;
  }
}
//---------------------------------------------------------------------------
void __fastcall SetCorrectFormParent(TForm * /*Form*/)
{
  // noop
  // remove
}
//---------------------------------------------------------------------------
void __fastcall InvokeHelp(TWinControl * Control)
{
  assert(Control != NULL);

  HELPINFO HelpInfo;
  HelpInfo.cbSize = sizeof(HelpInfo);
  HelpInfo.iContextType = HELPINFO_WINDOW;
  HelpInfo.iCtrlId = 0;
  HelpInfo.hItemHandle = Control->Handle;
  HelpInfo.dwContextId = 0;
  HelpInfo.MousePos.x = 0;
  HelpInfo.MousePos.y = 0;
  SendMessage(Control->Handle, WM_HELP, NULL, reinterpret_cast<long>(&HelpInfo));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void __fastcall FocusableLabelCanvas(TStaticText * StaticText,
  TControlCanvas ** ACanvas, TRect & R)
{
  TControlCanvas * Canvas = new TControlCanvas();
  try
  {
    Canvas->Control = StaticText;

    R = StaticText->ClientRect;

    UnicodeString Caption = StaticText->Caption;
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
    switch (StaticText->Alignment)
    {
      case taLeftJustify:
        R.Right = R.Left + TextSize.cx;
        break;

      case taRightJustify:
        R.Left = R.Right - TextSize.cx;
        break;

      case taCenter:
        {
          int Diff = R.Width() - TextSize.cx;
          R.Left += Diff / 2;
          R.Right -= Diff - (Diff / 2);
        }
        break;
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
  else if (Message.Msg == WM_RBUTTONDOWN)
  {
    StaticText->SetFocus();
    Message.Result = 1;
  }
  else if (Message.Msg == WM_CHAR)
  {
    if (reinterpret_cast<TWMChar &>(Message).CharCode == L' ')
    {
      Clicked = true;
      Message.Result = 1;
    }
    else
    {
      ControlWndProc(StaticText)(Message);
    }
  }
  else if (Message.Msg == CM_DIALOGCHAR)
  {
    if (StaticText->CanFocus() && StaticText->ShowAccelChar &&
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
      ControlWndProc(StaticText)(Message);
    }
  }
  else
  {
    ControlWndProc(StaticText)(Message);
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
  if (Application->OnShowHint != NULL)
  {
    Application->OnShowHint(HintInfo.HintStr, CanShow, HintInfo);
  }

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
  bool Cancel = false;

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

  if (Message.Msg == CM_CANCELMODE)
  {
    TCMCancelMode & CancelMessage = (TCMCancelMode&)Message;
    if ((CancelMessage.Sender != StaticText) &&
        (CancelMessage.Sender != PersistentHintWindow))
    {
      Cancel = true;
    }
  }

  if ((Message.Msg == WM_DESTROY) || (Message.Msg == WM_KILLFOCUS))
  {
    Cancel = true;
  }

  if (Cancel && (PersistentHintControl == StaticText))
  {
    CancelPersistentHint();
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
void __fastcall HintLabel(TStaticText * StaticText, UnicodeString Hint)
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
void __fastcall HintLabelRestore(TStaticText * StaticText)
{
  StaticText->WindowProc = ControlWndProc(StaticText);
  StaticText->ShowHint = false;
  StaticText->Cursor = crDefault;
}
//---------------------------------------------------------------------------
static void __fastcall ComboBoxFixWindowProc(void * Data, TMessage & Message)
{
  // it is TCustomComboxBox, but the properties are published only by TComboBox
  TComboBox * ComboBox = static_cast<TComboBox *>(Data);
  if (Message.Msg == WM_SIZE)
  {
    UnicodeString Text = ComboBox->Text;
    try
    {
      ControlWndProc(ComboBox)(Message);
    }
    __finally
    {
      // workaround for bug in combo box, that causes it to change text to any
      // item from drop down list which starts with current text,
      // after control is resized (unless the text is in drop down list as well)
      ComboBox->Text = Text;
      // hide selection, which is wrongly shown when form is resized, even when the box has not focus
      if (!ComboBox->Focused())
      {
        ComboBox->SelLength = 0;
      }
    }
  }
  else
  {
    ControlWndProc(ComboBox)(Message);
  }
}
//---------------------------------------------------------------------------
void __fastcall FixComboBoxResizeBug(TCustomComboBox * ComboBox)
{
  TWndMethod WindowProc;
  ((TMethod*)&WindowProc)->Data = ComboBox;
  ((TMethod*)&WindowProc)->Code = ComboBoxFixWindowProc;
  ComboBox->WindowProc = WindowProc;
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
    UnicodeString Url = StaticText->Caption;
    if (!SameText(Url.SubString(1, 4), L"http") && (Url.Pos(L"@") > 0))
    {
      Url = L"mailto:" + Url;
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
    if ((GetKeyState(VK_CONTROL) < 0) && (Key.CharCode == L'C'))
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
void __fastcall LinkLabel(TStaticText * StaticText, UnicodeString Url,
  TNotifyEvent OnEnter)
{
  StaticText->Transparent = false;
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
      Item->ShortCut = ShortCut(L' ', TShiftState());
      Item->OnClick = ContextMenuOnClick;
      StaticText->PopupMenu->Items->Add(Item);

      Item = new TMenuItem(StaticText->PopupMenu);
      Item->Caption = LoadStr(URL_LINK_COPY);
      Item->Tag = 1;
      Item->ShortCut = ShortCut(L'C', TShiftState() << ssCtrl);
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
//---------------------------------------------------------------------------
static void __fastcall HotTrackLabelMouseEnter(void * /*Data*/, TObject * Sender)
{
  reinterpret_cast<TLabel *>(Sender)->Font->Color = clBlue;
}
//---------------------------------------------------------------------------
static void __fastcall HotTrackLabelMouseLeave(void * /*Data*/, TObject * Sender)
{
  reinterpret_cast<TLabel *>(Sender)->ParentFont = true;
}
//---------------------------------------------------------------------------
void __fastcall HotTrackLabel(TLabel * Label)
{
  assert(Label->OnMouseEnter == NULL);
  assert(Label->OnMouseLeave == NULL);

  Label->OnMouseEnter = MakeMethod<TNotifyEvent>(NULL, HotTrackLabelMouseEnter);
  Label->OnMouseLeave = MakeMethod<TNotifyEvent>(NULL, HotTrackLabelMouseLeave);
}
//---------------------------------------------------------------------------
Forms::TMonitor *  __fastcall FormMonitor(TCustomForm * Form)
{
  Forms::TMonitor * Result;
  if ((Application->MainForm != NULL) && (Application->MainForm != Form))
  {
    Result = Application->MainForm->Monitor;
  }
  else if (LastMonitor != NULL)
  {
    Result = LastMonitor;
  }
  else
  {
    int i = 0;
    while ((i < Screen->MonitorCount) && !Screen->Monitors[i]->Primary)
    {
      i++;
    }
    assert(Screen->Monitors[i]->Primary);
    Result = Screen->Monitors[i];
  }
  return Result;
}
//---------------------------------------------------------------------------
int __fastcall GetLastMonitor()
{
  if (LastMonitor != NULL)
  {
    return LastMonitor->MonitorNum;
  }
  else
  {
    return -1;
  }
}
//---------------------------------------------------------------------------
void __fastcall SetLastMonitor(int MonitorNum)
{
  if ((MonitorNum >= 0) && (MonitorNum < Screen->MonitorCount))
  {
    LastMonitor = Screen->Monitors[MonitorNum];
  }
  else
  {
    LastMonitor = NULL;
  }
}
//---------------------------------------------------------------------------
TForm * __fastcall _SafeFormCreate(TMetaClass * FormClass, TComponent * Owner)
{
  TForm * Form;

  if (Owner == NULL)
  {
    Owner = GetFormOwner();
  }

  // if there is no main form yet, make this one main.
  // this, among other, makes other forms (dialogs invoked from this one),
  // be placed on the same monitor (otherwise all new forms get placed
  // on primary monitor)
  if (Application->MainForm == NULL)
  {
    Application->CreateForm(FormClass, &Form);
    assert(Application->MainForm == Form);
  }
  else
  {
    Form = dynamic_cast<TForm *>(Construct(FormClass, Owner));
    assert(Form != NULL);
  }

  return Form;
}
//---------------------------------------------------------------------------
TImageList * __fastcall SharedSystemImageList(bool Large)
{
  TSHFileInfo FileInfo;
  TImageList * Result = new TImageList(Application);
  int ImageListHandle = SHGetFileInfo(L"", 0, &FileInfo, sizeof(FileInfo),
    SHGFI_SYSICONINDEX | (Large ? SHGFI_LARGEICON : SHGFI_SMALLICON));
  if (ImageListHandle != 0)
  {
    Result->ShareImages = true;
    Result->Handle = ImageListHandle;
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall SupportsSplitButton()
{
  return (Win32MajorVersion >= 6);
}
