//---------------------------------------------------------------------------
#ifndef CopyH
#define CopyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include <HistoryComboBox.hpp>

#include "Rights.h"
#include "CopyParams.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TCopyDialog : public TForm
{
__published:
  TLabel *DirectoryLabel;
  THistoryComboBox *LocalDirectoryEdit;
  THistoryComboBox *RemoteDirectoryEdit;
  TButton *CopyButton;
  TButton *CancelButton;
  TButton *LocalDirectoryBrowseButton;
  TCheckBox *QueueCheck;
  TCheckBox *QueueIndividuallyCheck;
  TButton *HelpButton;
  TCheckBox *NewerOnlyCheck;
  TCheckBox *NeverShowAgainCheck;
  TButton *TransferSettingsButton;
  TGroupBox *CopyParamGroup;
  TLabel *CopyParamLabel;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall LocalDirectoryBrowseButtonClick(TObject *Sender);
  void __fastcall ControlChange(TObject *Sender);
  void __fastcall TransferSettingsButtonClick(TObject *Sender);
  void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall CopyParamGroupDblClick(TObject *Sender);
  void __fastcall CopyParamGroupContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
private:
  bool FDefaultToRemote;
  bool FToRemote;
  TStrings * FFileList;
  bool FMove;
  int FOptions;
  int FOutputOptions;
  TGUICopyParamType FParams;
  TPopupMenu * FPresetsMenu;
  AnsiString FPreset;
  TCopyParamType FCopyParams;
  int FCopyParamAttrs;
  bool FSaveSettings;
  AnsiString __fastcall GetDirectory();
  void __fastcall SetToRemote(bool value);
  THistoryComboBox * __fastcall GetDirectoryEdit();
  void __fastcall SetParams(const TGUICopyParamType & value);
  TGUICopyParamType __fastcall GetParams();
  void __fastcall SetDirectory(AnsiString value);
  void __fastcall SetFileList(TStrings * value);
  void __fastcall SetMove(bool value);
  AnsiString __fastcall GetFileMask();
  void __fastcall SetOptions(int value);
  void __fastcall SetOutputOptions(int value);
  int __fastcall GetOutputOptions();
  void __fastcall SetCopyParamAttrs(int value);
  int __fastcall GetCopyParamAttrs();
  void __fastcall CopyParamClick(TObject * Sender);
protected:
  void __fastcall UpdateControls();
  void __fastcall AdjustControls();
  void __fastcall AdjustTransferControls();
  bool __fastcall RemotePaths();
  void __fastcall CopyParamListPopup(TPoint P, int AdditionalOptions);
public:
  __fastcall TCopyDialog(TComponent* Owner);
  virtual __fastcall ~TCopyDialog();
  bool __fastcall Execute();

  __property bool ToRemote = { read = FToRemote, write = SetToRemote };
  __property AnsiString Directory = { read = GetDirectory, write = SetDirectory };
  __property THistoryComboBox * DirectoryEdit = { read = GetDirectoryEdit };
  __property TStrings * FileList = { read = FFileList, write = SetFileList };
  __property TGUICopyParamType Params = { read = GetParams, write = SetParams };
  __property bool Move = { read = FMove, write = SetMove };
  __property int Options = { read = FOptions, write = SetOptions };
  __property int CopyParamAttrs = { read = GetCopyParamAttrs, write = SetCopyParamAttrs };
  __property int OutputOptions = { read = GetOutputOptions, write = SetOutputOptions };
};
//---------------------------------------------------------------------------
#endif
