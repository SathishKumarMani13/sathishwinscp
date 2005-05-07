//---------------------------------------------------------------------------
#ifndef EditorH
#define EditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <StdActns.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include "TB2Dock.hpp"
#include "TBX.hpp"
#include "TB2Item.hpp"
#include "TB2Toolbar.hpp"
#include "TBXStatusBars.hpp"
//---------------------------------------------------------------------------
class TEditorForm : public TForm
{
__published:
  TActionList *EditorActions;
  TImageList *EditorImages;
  TAction *SaveAction;
  TTBXDock *TopDock;
  TTBXToolbar *ToolBar;
  TRichEdit *EditorMemo;
  TTBXStatusBar *StatusBar;
  TEditCut *EditCut;
  TEditCopy *EditCopy;
  TEditPaste *EditPaste;
  TEditSelectAll *EditSelectAll;
  TEditUndo *EditUndo;
  TEditDelete *EditDelete;
  TAction *PreferencesAction;
  TAction *CloseAction;
  TAction *FindAction;
  TAction *ReplaceAction;
  TAction *FindNextAction;
  TAction *GoToLineAction;
  TTBXItem *TBXItem1;
  TTBXItem *TBXItem2;
  TTBXSeparatorItem *TBXSeparatorItem1;
  TTBXItem *TBXItem3;
  TTBXItem *TBXItem4;
  TTBXItem *TBXItem5;
  TTBXItem *TBXItem6;
  TTBXItem *TBXItem7;
  TTBXItem *TBXItem8;
  TTBXSeparatorItem *TBXSeparatorItem2;
  TTBXItem *TBXItem9;
  TTBXSeparatorItem *TBXSeparatorItem3;
  TTBXItem *TBXItem10;
  TTBXItem *TBXItem11;
  TTBXItem *TBXItem12;
  TTBXSeparatorItem *TBXSeparatorItem4;
  TTBXItem *TBXItem13;
  TFindDialog *FindDialog;
  TReplaceDialog *ReplaceDialog;
  TTBXPopupMenu *EditorPopup;
  TTBXItem *Undo1;
  TTBXSeparatorItem *N1;
  TTBXItem *Cut1;
  TTBXItem *Copy1;
  TTBXItem *Paste1;
  TTBXItem *Delete1;
  TTBXSeparatorItem *N2;
  TTBXItem *SelectAll1;
  TTBXSeparatorItem *N3;
  TTBXItem *Find1;
  TTBXItem *Replace1;
  TTBXItem *Findnext1;
  TTBXItem *Gotolinenumber1;
  TTBXSeparatorItem *TBXSeparatorItem5;
  TTBXItem *TBXItem14;
  TAction *HelpAction;
  void __fastcall EditorActionsUpdate(TBasicAction *Action, bool &Handled);
  void __fastcall EditorActionsExecute(TBasicAction *Action,
          bool &Handled);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall EditorMemoMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall EditorMemoKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall EditorMemoChange(TObject *Sender);
  void __fastcall FindDialogFind(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:
  AnsiString FFileName;
  TNotifyEvent FOnFileChanged;
  TNotifyEvent FOnWindowClose;
  TCustomForm * FParentForm;
  TFindDialog * FLastFindDialog;
  TPoint FCaretPos;
  bool FShowWindowButton;
  void __fastcall SetFileName(const AnsiString value);
  void __fastcall SetParentForm(TCustomForm * value);
  void __fastcall SetShowWindowButton(bool value);
public:
  __fastcall TEditorForm(TComponent* Owner);
  virtual __fastcall ~TEditorForm();
  __property AnsiString FileName = { read = FFileName, write = SetFileName };
  __property TNotifyEvent OnFileChanged = { read = FOnFileChanged, write = FOnFileChanged };
  __property TNotifyEvent OnWindowClose = { read = FOnWindowClose, write = FOnWindowClose };
  __property TCustomForm * ParentForm = { read = FParentForm, write = SetParentForm };
  __property bool ShowWindowButton = { read = FShowWindowButton, write = SetShowWindowButton };
protected:
  void __fastcall ApplyConfiguration();
  bool __fastcall CursorInUpperPart();
  void __fastcall Find();
  void __fastcall GoToLine();
  void __fastcall LoadFile();
  void __fastcall PositionFindDialog(bool VerticalOnly);
  void __fastcall StartFind(bool Find);
  void __fastcall UpdateControls();
  virtual void __fastcall CreateParams(TCreateParams & Params);
};
//---------------------------------------------------------------------------
#endif
