//---------------------------------------------------------------------------
#ifndef ScpCommanderH
#define ScpCommanderH
//---------------------------------------------------------------------------
#include "CustomScpExplorer.h"

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <AssociatedStatusBar.hpp>
#include <CustomDirView.hpp>
#include <CustomUnixDirView.hpp>
#include <IEListView.hpp>
#include <NortonLikeListView.hpp>
#include <UnixDirView.h>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <DirView.hpp>
#include <CustomPathComboBox.hpp>
#include <IEComboBox.hpp>
#include <IEPathComboBox.hpp>
#include <PathLabel.hpp>
#include <UnixPathComboBox.h>
#include <ToolbarPanel.hpp>

#include <WinInterface.h>

#include "HistoryComboBox.hpp"
#include "CustomDriveView.hpp"
#include "DriveView.hpp"
#include "UnixDriveView.h"
//---------------------------------------------------------------------------
class TScpCommanderForm : public TCustomScpExplorerForm
{
__published:
  TSplitter *Splitter;
  TPanel *LocalPanel;
  TAssociatedStatusBar *LocalStatusBar;
  TDirView *LocalDirView;
  TPathLabel *LocalPathLabel;
  TPathLabel *RemotePathLabel;
  TCoolBar *LocalCoolBar;
  TCoolBar *RemoteCoolBar;
  TToolbarPanel *ToolbarPanel;
  TStatusBar *StatusBar;
  TToolBar *MenuToolBar;
  TToolButton *LeftMenuButton;
  TToolButton *ToolButton19;
  TToolButton *ToolButton1;
  TToolButton *RightMenuButton;
  TToolButton *ToolButton4;
  TToolBar *SelectionToolbar;
  TToolButton *ToolButton23;
  TToolButton *ToolButton24;
  TToolButton *ToolButton25;
  TToolButton *ToolButton26;
  TToolButton *ToolButton28;
  TToolButton *ToolButton29;
  TToolBar *PreferencesToolbar;
  TToolButton *ToolButton33;
  TToolButton *ToolButton35;
  TToolButton *ToolButton36;
  TToolBar *SessionToolbar;
  TToolButton *ToolButton30;
  TToolButton *ToolButton31;
  TToolButton *ToolButton32;
  TToolButton *SavedSessionsButton;
  TToolBar *CommandToolBar;
  TToolButton *ToolButton5;
  TToolButton *ToolButton6;
  TToolButton *ToolButton17;
  TToolButton *ToolButton27;
  TToolButton *ToolButton34;
  TToolButton *ToolButton16;
  TToolButton *ToolButton37;
  TToolButton *ToolButton38;
  TToolBar *ToolBar1;
  TToolButton *LocalBackButton;
  TToolButton *LocalForwardButton;
  TToolBar *ToolBar2;
  TToolButton *ToolButton57;
  TToolButton *ToolButton58;
  TToolButton *ToolButton59;
  TToolButton *ToolButton60;
  TToolButton *ToolButton61;
  TToolButton *ToolButton62;
  TToolBar *ToolBar3;
  TToolButton *RemoteBackButton;
  TToolButton *RemoteForwardButton;
  TToolBar *ToolBar4;
  TToolButton *ToolButton87;
  TToolButton *ToolButton88;
  TToolButton *ToolButton89;
  TToolButton *ToolButton90;
  TToolButton *ToolButton91;
  TToolButton *ToolButton92;
  TToolButton *ToolButton7;
  TToolBar *SortToolbar;
  TToolButton *ToolButton8;
  TToolButton *ToolButton9;
  TToolButton *ToolButton10;
  TToolButton *ToolButton11;
  TToolButton *ToolButton13;
  TToolButton *ToolButton15;
  TToolButton *ToolButton12;
  TToolButton *ToolButton14;
  TToolButton *ToolButton18;
  TToolButton *ToolButton20;
  TToolButton *ToolButton21;
  TToolButton *ToolButton22;
  TToolBar *CommandsToolbar;
  TToolButton *ToolButton39;
  TToolButton *ToolButton40;
  TToolButton *ToolButton41;
  TToolBar *ToolBar5;
  TIEPathComboBox *LocalPathComboBox;
  TToolBar *ToolBar6;
  TUnixPathComboBox *RemotePathComboBox;
  TToolButton *ToolButton42;
  TToolButton *ToolButton43;
  TToolButton *ToolButton44;
  TToolButton *ToolButton45;
  TToolButton *ToolButton46;
  TToolButton *ToolButton47;
  TToolButton *ToolButton48;
  TToolButton *ToolButton49;
  TToolButton *ToolButton51;
  TPanel *CommandLinePanel;
  THistoryComboBox *CommandLineCombo;
  TPathLabel *CommandLineLabel;
  TLabel *CommandLinePromptLabel;
  TToolButton *ToolButton50;
  TDriveView *LocalDriveView;
  TSplitter *LocalPanelSplitter;
  TToolButton *ToolButton68;
  TToolButton *ToolButton69;
  TSplitter *SessionComboResizer;
  TComboBox *SessionCombo;
  TToolButton *ToolButton2;
  void __fastcall SplitterMoved(TObject *Sender);
  void __fastcall SplitterCanResize(TObject *Sender, int &NewSize,
    bool &Accept);
  void __fastcall SplitterDblClick(TObject *Sender);
  void __fastcall PanelSplitterDblClick(TObject * Sender);
  void __fastcall PathComboBoxCloseUp(TObject *Sender,
    bool Canceled);
  void __fastcall LocalDirViewExecFile(TObject *Sender, TListItem *Item,
    bool &AllowExec);
  void __fastcall LocalFileControlDDDragEnter(TObject *Sender,
    IDataObject *DataObj, int grfKeyState, TPoint &Point,
    int &dwEffect, bool &Accept);
  void __fastcall DirViewLoaded(TObject *Sender);
  void __fastcall SessionComboCloseUp(TObject *Sender);
  void __fastcall LocalFileControlDDDragOver(TObject *Sender, int grfKeyState,
    TPoint &Point, int &dwEffect);
  void __fastcall LocalFileControlDDFileOperation(TObject *Sender,
    int dwEffect, AnsiString SourcePath, AnsiString TargetPath,
    bool &DoOperation);
  void __fastcall RemoteFileControlDDFileOperationExecuted(TObject *Sender,
    int dwEffect, AnsiString SourcePath, AnsiString TargetPath);
  void __fastcall CommandLineComboKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift);
  void __fastcall CommandLineComboDropDown(TObject *Sender);
  void __fastcall CommandLineComboEnter(TObject *Sender);
  void __fastcall CommandLineComboExit(TObject *Sender);
  void __fastcall LocalDirViewDDTargetHasDropHandler(TObject *Sender,
    TListItem *Item, int &Effect, bool &DropHandler);
  void __fastcall StatusBarDblClick(TObject *Sender);
  void __fastcall LocalFileControlDDMenuPopup(TObject *Sender, HMENU AMenu,
    IDataObject *DataObj, int AMinCustCmd, int grfKeyState, TPoint &pt);
  void __fastcall PathLabelDblClick(TObject *Sender);
  void __fastcall LocalDirViewEnter(TObject *Sender);
  void __fastcall LocalPathLabelGetStatus(TCustomPathLabel *Sender,
    bool &Active);
  void __fastcall RemotePathLabelGetStatus(TCustomPathLabel *Sender,
    bool &Active);
  void __fastcall LocalDriveViewEnter(TObject *Sender);
  void __fastcall LocalPathLabelPathClick(TCustomPathLabel *Sender,
    AnsiString Path);
  void __fastcall RemotePathLabelPathClick(TCustomPathLabel *Sender,
    AnsiString Path);
  void __fastcall LocalDirViewFileIconForName(TObject *Sender,
          TListItem *Item, AnsiString &FileName);

private:
  TCustomDirView * FDirViewToSelect;
  float FLastLeftPanelWidth;
  float FLeftPanelWidth;
  int FNormalPanelsWidth;
  int FLastWidth;
  bool FSynchronisingBrowse;
  TStrings * FInternalDDDownloadList;
  AnsiString FPrevPath[2];
  bool FFirstTerminal;
  AnsiString FDDExtTarget;
  bool FCommandLineComboPopulated;

  void __fastcall SetLeftPanelWidth(float value);
  float __fastcall GetLeftPanelWidth();
  inline TPanel * __fastcall Panel(bool Left);

protected:
  virtual bool __fastcall CopyParamDialog(TTransferDirection Direction,
    TTransferType Type, bool Temp, TStrings * FileList,
    AnsiString & TargetDirectory, TGUICopyParamType & CopyParam, bool Confirm);
  virtual TCustomDirView * __fastcall DirView(TOperationSide Side);
  TControl * __fastcall GetComponent(Byte Component);
  virtual void __fastcall RestoreFormParams();
  virtual void __fastcall RestoreParams();
  virtual void __fastcall FixControlsPlacement();
  virtual void __fastcall TerminalChanging();
  virtual void __fastcall TerminalChanged();
  virtual void __fastcall ConfigurationChanged();
  virtual bool __fastcall GetHasDirView(TOperationSide Side);
  void __fastcall UpdateControls();
  virtual void __fastcall FileOperationProgress(
    TFileOperationProgressType & ProgressData, TCancelStatus & Cancel);
  virtual void __fastcall DoOpenDirectoryDialog(TOpenDirectoryMode Mode,
    TOperationSide Side);
  bool __fastcall InternalDDDownload(AnsiString & TargetDirectory);
  virtual void __fastcall DDGetTarget(AnsiString & Directory);
  virtual void __fastcall DDExtInitDrag(TFileList * FileList, bool & Created);
  virtual void __fastcall SideEnter(TOperationSide Side);
  void __fastcall SaveCommandLine();
  void __fastcall ExecuteCommandLine();
  virtual void __fastcall PanelExportStore(TOperationSide Side,
    TPanelExport Export, TPanelExportDestination Destination,
    TStringList * ExportData);
  void __fastcall CommandLinePopulate();
  virtual int __fastcall GetStaticComponentsHeight();
  DYNAMIC void __fastcall Resize();
  DYNAMIC void __fastcall DoShow();
  virtual void __fastcall SysResizing(unsigned int Cmd);
  virtual void __fastcall BatchStart(void *& Storage);
  virtual void __fastcall BatchEnd(void * Storage);
  virtual bool __fastcall IsFileControl(TObject * Control, TOperationSide Side);
  virtual void __fastcall ReloadLocalDirectory(const AnsiString Directory = "");
  virtual bool __fastcall PanelOperation(TOperationSide Side, bool DragDrop);

public:
  __fastcall TScpCommanderForm(TComponent* Owner);
  virtual __fastcall ~TScpCommanderForm();
  
  virtual void __fastcall AddEditLink();
  virtual bool __fastcall AllowedAction(TAction * Action, TActionAllowed Allowed);
  virtual void __fastcall ChangePath(TOperationSide Side);
  virtual void __fastcall CompareDirectories();
  virtual void __fastcall UpdateSessionData(TSessionData * Data = NULL);
  virtual void __fastcall SynchronizeDirectories();
  virtual void __fastcall FullSynchronizeDirectories();
  virtual void __fastcall StoreParams();
  virtual void __fastcall ExploreLocalDirectory();
  virtual void __fastcall GoToCommandLine();
  virtual void __fastcall GoToTree();
  virtual void __fastcall OpenConsole(AnsiString Command = "");
  __property float LeftPanelWidth = { read = GetLeftPanelWidth, write = SetLeftPanelWidth };
};
//---------------------------------------------------------------------------
#endif
