//----------------------------------------------------------------------------
#ifndef PreferencesH
#define PreferencesH
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
#include <ComCtrls.hpp>
#include <Comboedit.hpp>
#include <Mask.hpp>
#include <ComboEdit.hpp>
#include <XPGroupBox.hpp>

#include "CopyParams.h"
#include "GeneralSettings.h"
#include "LogSettings.h"
#include "UpDownEdit.hpp"
#include "IEComboBox.hpp"
//----------------------------------------------------------------------------
class TCustomCommands;
//----------------------------------------------------------------------------
class TPreferencesDialog : public TForm
{
__published:
  TButton *OKButton;
  TButton *CloseButton;
  TPanel *MainPanel;
  TPageControl *PageControl;
  TTabSheet *PreferencesSheet;
  TLabel *RandomSeedFileLabel;
  TXPGroupBox *CommonPreferencesGroup;
  TCheckBox *CopyOnDoubleClickCheck;
  TCheckBox *CopyOnDoubleClickConfirmationCheck;
  TCheckBox *ConfirmOverwritingCheck;
  TCheckBox *ConfirmDeletingCheck;
  TCheckBox *ConfirmClosingSessionCheck;
  TCheckBox *DDTransferConfirmationCheck;
  TCheckBox *ContinueOnErrorCheck;
  TFilenameEdit *RandomSeedFileEdit;
  TXPGroupBox *StorageGroup;
  TRadioButton *RegistryStorageButton;
  TRadioButton *IniFileStorageButton;
  TTabSheet *LogSheet;
  TLoggingFrame *LoggingFrame;
  TTabSheet *GeneralSheet;
  TLabel *Label1;
  TGeneralSettingsFrame *GeneralSettingsFrame;
  TTabSheet *PanelsSheet;
  TXPGroupBox *PanelsRemoteDirectoryGroup;
  TCheckBox *ShowInaccesibleDirectoriesCheck;
  TXPGroupBox *PanelsCommonGroup;
  TCheckBox *ShowHiddenFilesCheck;
  TCheckBox *DefaultDirIsHomeCheck;
  TTabSheet *CommanderSheet;
  TLabel *Label3;
  TXPGroupBox *PanelsGroup;
  TCheckBox *ExplorerStyleSelectionCheck;
  TCheckBox *PreserveLocalDirectoryCheck;
  TXPGroupBox *CommanderMiscGroup;
  TCheckBox *UseLocationProfilesCheck;
  TXPGroupBox *CompareCriterionsGroup;
  TCheckBox *CompareByTimeCheck;
  TCheckBox *CompareBySizeCheck;
  TTabSheet *ExplorerSheet;
  TLabel *Label4;
  TXPGroupBox *GroupBox2;
  TCheckBox *ShowFullAddressCheck;
  TTabSheet *TransferSheet;
  TCopyParamsFrame *CopyParamsFrame;
  TXPGroupBox *ResumeBox;
  TLabel *ResumeThresholdUnitLabel;
  TRadioButton *ResumeOnButton;
  TRadioButton *ResumeSmartButton;
  TRadioButton *ResumeOffButton;
  TUpDownEdit *ResumeThresholdEdit;
  TTabSheet *EditorSheet;
  TXPGroupBox *EditorGroup;
  TRadioButton *EditorInternalButton;
  TRadioButton *EditorExternalButton;
  TFilenameEdit *ExternalEditorEdit;
  TXPGroupBox *EditorFontGroup;
  TLabel *EditorFontLabel;
  TButton *EditorFontButton;
  TXPGroupBox *EditorOptionsGroup;
  TCheckBox *EditorWordWrapCheck;
  TTabSheet *IntegrationSheet;
  TXPGroupBox *ShellIconsGroup;
  TLabel *ShellIconsLabel;
  TButton *DesktopIconButton;
  TButton *QuickLaunchIconButton;
  TButton *DesktopIconAllUsersButton;
  TButton *SendToHookButton;
  TXPGroupBox *XPGroupBox1;
  TLabel *Label2;
  TFilenameEdit *PuttyPathEdit;
  TTabSheet *CustomCommandsSheet;
  TXPGroupBox *CustomCommandsGroup;
  TLabel *LocalDirectoryLabel;
  TLabel *RemoteDirectoryLabel;
  TLabel *CustomCommandsPatternsLabel;
  TEdit *CustomCommandDescEdit;
  TEdit *CustomCommandEdit;
  TListView *CustomCommandsView;
  TButton *AddCommandButton;
  TButton *RemoveCommandButton;
  TButton *UpCommandButton;
  TButton *DownCommandButton;
  TButton *SaveCommandButton;
  TCheckBox *CustomCommandApplyToDirectoriesCheck;
  TCheckBox *CustomCommandRecursiveCheck;
  TPanel *LeftPanel;
  TTreeView *NavigationTree;
  TCheckBox *DeleteToRecycleBinCheck;
  TButton *RegisterAsUrlHandlerButton;
  TTabSheet *DragDropSheet;
  TXPGroupBox *DragDropDownloadsGroup;
  TLabel *DDExtEnabledLabel;
  TLabel *DDExtDisabledLabel;
  TRadioButton *DDExtEnabledButton;
  TRadioButton *DDExtDisabledButton;
  TPanel *DDExtDisabledPanel;
  TRadioButton *DDSystemTemporaryDirectoryButton;
  TRadioButton *DDCustomTemporaryDirectoryButton;
  TDirectoryEdit *DDTemporaryDirectoryEdit;
  TCheckBox *DDWarnLackOfTempSpaceCheck;
  TCheckBox *DDWarnOnMoveCheck;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall ControlChange(TObject *Sender);
  void __fastcall EditorFontButtonClick(TObject *Sender);
  void __fastcall ExternalEditorEditExit(TObject *Sender);
  void __fastcall ExternalEditorEditAfterDialog(TObject *Sender,
          AnsiString &Name, bool &Action);
  void __fastcall ExternalEditorEditChange(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall IconButtonClick(TObject *Sender);
  void __fastcall CustomCommandsViewData(TObject *Sender, TListItem *Item);
  void __fastcall CustomCommandsViewSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
  void __fastcall CustomCommandsViewKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall AddCommandButtonClick(TObject *Sender);
  void __fastcall SaveCommandButtonClick(TObject *Sender);
  void __fastcall RemoveCommandButtonClick(TObject *Sender);
  void __fastcall UpDownCommandButtonClick(TObject *Sender);
  void __fastcall CustomCommandsViewStartDrag(TObject *Sender,
          TDragObject *&DragObject);
  void __fastcall CustomCommandsViewDragDrop(TObject *Sender,
          TObject *Source, int X, int Y);
  void __fastcall CustomCommandsViewDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
  void __fastcall CompareByTimeCheckClick(TObject *Sender);
  void __fastcall CompareBySizeCheckClick(TObject *Sender);
  void __fastcall NavigationTreeChange(TObject *Sender, TTreeNode *Node);
  void __fastcall PageControlChange(TObject *Sender);
  void __fastcall RegisterAsUrlHandlerButtonClick(TObject *Sender);
  void __fastcall DDExtLabelClick(TObject *Sender);
private:
  TPreferencesMode FPreferencesMode;
  TFont * FEditorFont;
  TCustomCommands * FCustomCommands;
  bool FCustomCommandChanging;
  bool FAfterExternalEditorDialog;
  int FCustomCommandDragSource;
  int FCustomCommandDragDest;
  void __fastcall SetPreferencesMode(TPreferencesMode value);
  void __fastcall CMDialogKey(TWMKeyDown & Message);
public:
  virtual __fastcall ~TPreferencesDialog();
  bool __fastcall Execute();
  virtual __fastcall TPreferencesDialog(TComponent* AOwner);
  __property TPreferencesMode PreferencesMode = { read = FPreferencesMode, write = SetPreferencesMode };
protected:
  void __fastcall LoadConfiguration();
  void __fastcall LoggingGetDefaultLogFileName(System::TObject * Sender, AnsiString & DefaultLogFileName);
  void __fastcall SaveConfiguration();
  void __fastcall UpdateControls();
  void __fastcall UpdateCustomCommandsView();
  AnsiString __fastcall CustomCommandString(int Index = -1);
  int __fastcall CustomCommandParams();
  void __fastcall CustomCommandMove(int Source, int Dest);
  bool __fastcall AllowCustomCommandsDrag(int X, int Y);
  void __fastcall PrepareNavigationTree(TTreeView * Tree);
  virtual void __fastcall Dispatch(void * Message);
};
//----------------------------------------------------------------------------
#endif
