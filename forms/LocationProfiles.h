//----------------------------------------------------------------------------
#ifndef LocationProfilesH
#define LocationProfilesH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <Mask.hpp>
#include <ComboEdit.hpp>

#include <FileOperationProgress.h>
#include <Terminal.h>
#include <WinInterface.h>
#include <Bookmarks.h>
#include "IEComboBox.hpp"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
//----------------------------------------------------------------------------
class TLocationProfilesDialog : public TForm
{
__published:
  TButton *OKBtn;
  TButton *CancelBtn;
  TGroupBox *BookmarksGroup;
  TTreeView *ProfilesView;
  TButton *AddBookmarkButton;
  TButton *RemoveBookmarkButton;
  TButton *DownBookmarkButton;
  TButton *UpBookmarkButton;
  TLabel *LocalDirectoryLabel;
  TIEComboBox *RemoteDirectoryEdit;
  TButton *RenameButton;
  TLabel *RemoteDirectoryLabel;
  TButton *MoveToButton;
  TImageList *BookmarkImageList;
  TIEComboBox *LocalDirectoryEdit;
  TButton *LocalDirectoryBrowseButton;
  TButton *SwitchButton;
  TButton *HelpButton;
  void __fastcall ControlChange(TObject *Sender);
  void __fastcall AddBookmarkButtonClick(TObject *Sender);
  void __fastcall RemoveBookmarkButtonClick(TObject *Sender);
  void __fastcall BookmarkButtonClick(TObject *Sender);
  void __fastcall ProfilesViewStartDrag(TObject *Sender,
          TDragObject *&DragObject);
  void __fastcall ProfilesViewDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
  void __fastcall ProfilesViewDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
  void __fastcall ProfilesViewDblClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall ProfilesViewKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall DirectoryEditChange(TObject *Sender);
  void __fastcall ProfilesViewChange(TObject *Sender, TTreeNode *Node);
  void __fastcall MoveToButtonClick(TObject *Sender);
  void __fastcall RenameButtonClick(TObject *Sender);
  void __fastcall ProfilesViewGetImageIndex(TObject *Sender,
          TTreeNode *Node);
  void __fastcall ProfilesViewGetSelectedIndex(TObject *Sender,
          TTreeNode *Node);
  void __fastcall LocalDirectoryBrowseButtonClick(TObject *Sender);
  void __fastcall SwitchButtonClick(TObject *Sender);
  void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall ProfilesViewCollapsed(TObject *Sender, TTreeNode *Node);
  void __fastcall ProfilesViewExpanded(TObject *Sender, TTreeNode *Node);
  void __fastcall ProfilesViewEdited(TObject * Sender, TTreeNode * Node,
    AnsiString & S);
  void __fastcall ProfilesViewEditing(TObject * Sender, TTreeNode * Node,
    bool & AllowEdit);

public:
  __fastcall TLocationProfilesDialog(TComponent* AOwner);
  __fastcall ~TLocationProfilesDialog();

  bool __fastcall Execute();

  __property AnsiString LocalDirectory = { read = GetLocalDirectory, write = SetLocalDirectory };
  __property AnsiString RemoteDirectory = { read = GetRemoteDirectory, write = SetRemoteDirectory };
  __property TTerminal * Terminal = { read = FTerminal, write = FTerminal };
  __property TOperationSide OperationSide = { read = FOperationSide, write = FOperationSide };
  __property TStrings * RemoteDirectories  = { read=GetRemoteDirectories, write=SetRemoteDirectories };
  __property TStrings * LocalDirectories  = { read=GetLocalDirectories, write=SetLocalDirectories };
  __property TOpenDirectoryMode Mode = { read = FMode, write = FMode };

protected:
  void __fastcall BookmarkMove(TTreeNode * Source, TTreeNode * Dest);
  void __fastcall UpdateControls();
  bool __fastcall AddAsBookmark();
  virtual void __fastcall UpdateActions();

private:
  TOpenDirectoryMode FMode;
  TTerminal * FTerminal;
  TOperationSide FOperationSide;
  TStringList * FFolders;
  bool FChanging;
  TTreeNode * FBookmarkDragSource;
  TBookmarkList * FBookmarkList;
  AnsiString FLocalDirectory;
  AnsiString FRemoteDirectory;

  void __fastcall SetLocalDirectory(AnsiString value);
  AnsiString __fastcall GetLocalDirectory();
  void __fastcall SetRemoteDirectory(AnsiString value);
  AnsiString __fastcall GetRemoteDirectory();
  void __fastcall SetOperationSide(TOperationSide value);
  void __fastcall LoadBookmarks();
  void __fastcall FindProfile();
  void __fastcall SetRemoteDirectories(TStrings * value);
  TStrings * __fastcall GetRemoteDirectories();
  void __fastcall SetLocalDirectories(TStrings * value);
  TStrings * __fastcall GetLocalDirectories();
};
//----------------------------------------------------------------------------
#endif
