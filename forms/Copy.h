//---------------------------------------------------------------------------
#ifndef CopyH
#define CopyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComboEdit.hpp>
#include <Mask.hpp>
#include <MoreButton.hpp>
#include <ExtCtrls.hpp>

#include "Rights.h"
#include "CopyParams.h"
//---------------------------------------------------------------------------
class TCopyDialog : public TForm
{
__published:
  TLabel *DirectoryLabel;
  TDirectoryEdit *LocalDirectoryEdit;
  TEdit *RemoteDirectoryEdit;
  TMoreButton *MoreButton;
  TButton *CopyButton;
  TButton *CancelButton;
  TPanel *MorePanel;
  TCheckBox *SaveSettingsCheck;
  TCopyParamsFrame *CopyParamsFrame;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:
  bool FToRemote;
  bool FDragDrop;
  TStrings * FFileList;
  bool FMove;
  AnsiString FFileMask;
  bool FAllowDirectory;
  bool __fastcall GetAllowTransferMode();
  AnsiString __fastcall GetDirectory();
  void __fastcall SetToRemote(bool value);
  TCustomEdit * __fastcall GetDirectoryEdit();
  void __fastcall SetParams(TCopyParamType value);
  TCopyParamType __fastcall GetParams();
  void __fastcall SetAllowTransferMode(Boolean value);
  void __fastcall SetDirectory(AnsiString value);
  void __fastcall SetDragDrop(Boolean value);
  void __fastcall SetFileList(TStrings * value);
  void __fastcall SetMove(bool value);
  void __fastcall SetFileMask(const AnsiString value);
  AnsiString __fastcall GetFileMask();
  void __fastcall SetAllowDirectory(bool value);
public:
  bool __fastcall Execute();
  __fastcall TCopyDialog(TComponent* Owner);
  __property bool AllowTransferMode = { read = GetAllowTransferMode, write = SetAllowTransferMode };
  __property bool ToRemote = { read = FToRemote, write = SetToRemote };
  __property AnsiString Directory = { read = GetDirectory, write = SetDirectory };
  __property TCustomEdit * DirectoryEdit = { read = GetDirectoryEdit };
  __property bool DragDrop = { read = FDragDrop, write = SetDragDrop };
  __property TStrings * FileList = { read = FFileList, write = SetFileList };
  __property TCopyParamType Params = { read = GetParams, write = SetParams };
  __property bool Move = { read = FMove, write = SetMove };
  __property bool AllowDirectory = { read = FAllowDirectory, write = SetAllowDirectory };
protected:
  void __fastcall UpdateControls();
};
//---------------------------------------------------------------------------
#endif
