//----------------------------------------------------------------------------
#ifndef LoginH
#define LoginH
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
#include <Mask.hpp>
#include <ComboEdit.hpp>
#include <ActnList.hpp>
#include <UpDownEdit.hpp>
#include <PasswordEdit.hpp>
#include <Menus.hpp>
//----------------------------------------------------------------------------
#include <Configuration.h>
#include <SessionData.h>

#include "LogSettings.h"
#include "GeneralSettings.h"
#include <ImgList.hpp>
//----------------------------------------------------------------------------
class TLoginDialog : public TForm
{
__published:
  TButton *LoginButton;
  TButton *CloseButton;
  TButton *AboutButton;
  TActionList *ActionList;
  TAction *EditSessionAction;
  TAction *SaveSessionAction;
  TAction *DeleteSessionAction;
  TAction *ImportSessionsAction;
  TAction *LoginAction;
  TAction *AboutAction;
  TAction *CleanUpAction;
  TAction *NewSessionAction;
  TPanel *MainPanel;
  TPageControl *PageControl;
  TTabSheet *SessionListSheet;
  TButton *LoadButton;
  TButton *DeleteButton;
  TListView *SessionListView;
  TButton *NewButton;
  TTabSheet *BasicSheet;
  TTabSheet *AdvancedSheet;
  TGroupBox *ProtocolGroup;
  TLabel *Label7;
  TRadioButton *SshProt1Button;
  TRadioButton *SshProt2Button;
  TCheckBox *CompressionCheck;
  TTabSheet *EnvironmentSheet;
  TTabSheet *ScpSheet;
  TGroupBox *OtherShellOptionsGroup;
  TCheckBox *LookupUserGroupsCheck;
  TCheckBox *ClearAliasesCheck;
  TCheckBox *UnsetNationalVarsCheck;
  TCheckBox *Scp1CompatibilityCheck;
  TGroupBox *ShellGroup;
  TComboBox *ShellEdit;
  TTabSheet *LogSheet;
  TLoggingFrame *LoggingFrame;
  TTabSheet *GeneralSheet;
  TLabel *Label13;
  TButton *PreferencesButton;
  TGeneralSettingsFrame *GeneralSettingsFrame;
  TPanel *LeftPanel;
  TTreeView *NavigationTree;
  TCheckBox *ShowAdvancedLoginOptionsCheck;
  TGroupBox *BasicGroup;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *PrivateKeyLabel;
  TEdit *HostNameEdit;
  TEdit *UserNameEdit;
  TPasswordEdit *PasswordEdit;
  TUpDownEdit *PortNumberEdit;
  TFilenameEdit *PrivateKeyEdit;
  TTabSheet *ConnSheet;
  TGroupBox *TimeoutGroup;
  TLabel *Label11;
  TLabel *Label12;
  TUpDownEdit *TimeoutEdit;
  TTabSheet *ProxySheet;
  TGroupBox *ProxyTypeGroup;
  TRadioButton *ProxyNoneButton;
  TRadioButton *ProxyHTTPButton;
  TRadioButton *ProxySocks4Button;
  TRadioButton *ProxyTelnetButton;
  TLabel *ProxyHostLabel;
  TLabel *ProxyPortLabel;
  TUpDownEdit *ProxyPortEdit;
  TEdit *ProxyHostEdit;
  TEdit *ProxyUsernameEdit;
  TLabel *ProxyUsernameLabel;
  TLabel *ProxyPasswordLabel;
  TPasswordEdit *ProxyPasswordEdit;
  TGroupBox *ProxySettingsGroup;
  TLabel *ProxyTelnetCommandLabel;
  TEdit *ProxyTelnetCommandEdit;
  TTabSheet *BugsSheet;
  TGroupBox *BugsGroupBox;
  TLabel *BugIgnore1Label;
  TComboBox *BugIgnore1Combo;
  TLabel *BugPlainPW1Label;
  TComboBox *BugPlainPW1Combo;
  TLabel *BugRSA1Label;
  TComboBox *BugRSA1Combo;
  TLabel *BugHMAC2Label;
  TComboBox *BugHMAC2Combo;
  TLabel *BugDeriveKey2Label;
  TComboBox *BugDeriveKey2Combo;
  TLabel *BugRSAPad2Label;
  TComboBox *BugRSAPad2Combo;
  TRadioButton *SshProt1onlyButton;
  TRadioButton *SshProt2onlyButton;
  TTabSheet *AuthSheet;
  TGroupBox *AuthenticationGroup;
  TCheckBox *AuthTISCheck;
  TCheckBox *AuthKICheck;
  TGroupBox *EncryptionGroup;
  TListBox *CipherListBox;
  TLabel *Label8;
  TCheckBox *Ssh2LegacyDESCheck;
  TButton *CipherUpButton;
  TButton *CipherDownButton;
  TButton *SetDefaultSessionButton;
  TAction *SetDefaultSessionAction;
  TButton *ToolsMenuButton;
  TPopupMenu *ToolsPopupMenu;
  TMenuItem *Import1;
  TMenuItem *Cleanup1;
  TButton *ShellIconsButton;
  TAction *DesktopIconAction;
  TGroupBox *EOLTypeGroup;
  TRadioButton *EOLTypeLFButton;
  TRadioButton *EOLTypeCRLFButton;
  TGroupBox *TransferProtocolGroup;
  TPopupMenu *IconsPopupMenu;
  TMenuItem *Desktopicon1;
  TAction *SendToHookAction;
  TMenuItem *ExplorersSendtoshortcut1;
  TLabel *BugPKSessID2Label;
  TComboBox *BugPKSessID2Combo;
  TRadioButton *ProxySocks5Button;
  TCheckBox *ProxyLocalhostCheck;
  TLabel *Label17;
  TRadioButton *ProxyDNSOffButton;
  TRadioButton *ProxyDNSAutoButton;
  TRadioButton *ProxyDNSOnButton;
  TUpDownEdit *TimeDifferenceEdit;
  TLabel *Label29;
  TLabel *Label30;
  TAction *CheckForUpdatesAction;
  TMenuItem *CheckForUpdates1;
  TButton *SaveButton;
  TButton *LanguagesButton;
  TGroupBox *PingGroup;
  TLabel *PingIntervalLabel;
  TUpDownEdit *PingIntervalSecEdit;
  TRadioButton *PingOffButton;
  TRadioButton *PingNullPacketButton;
  TRadioButton *PingDummyCommandButton;
  TUpDownEdit *TimeDifferenceMinutesEdit;
  TLabel *Label9;
  TCheckBox *AuthKIPasswordCheck;
  TTabSheet *DirectoriesSheet;
  TGroupBox *DirectoriesGroup;
  TLabel *LocalDirectoryLabel;
  TLabel *RemoteDirectoryLabel;
  TLabel *LocalDirectoryDescLabel;
  TDirectoryEdit *LocalDirectoryEdit;
  TEdit *RemoteDirectoryEdit;
  TCheckBox *UpdateDirectoriesCheck;
  TCheckBox *CacheDirectoriesCheck;
  TCheckBox *ResolveSymlinksCheck;
  TCheckBox *CacheDirectoryChangesCheck;
  TCheckBox *PreserveDirectoryChangesCheck;
  TGroupBox *DSTModeGroup;
  TRadioButton *DSTModeUnixCheck;
  TRadioButton *DSTModeWinCheck;
  TCheckBox *AuthGSSAPICheck;
  TGroupBox *RecycleBinGroup;
  TCheckBox *DeleteToRecycleBinCheck;
  TCheckBox *OverwrittenToRecycleBinCheck;
  TLabel *RecycleBinPathLabel;
  TEdit *RecycleBinPathEdit;
  TGroupBox *ScpLsOptionsGroup;
  TCheckBox *IgnoreLsWarningsCheck;
  TCheckBox *AliasGroupListCheck;
  TCheckBox *SCPLsFullTimeAutoCheck;
  TTabSheet *SftpSheet;
  TGroupBox *SFTPBugsGroupBox;
  TLabel *Label10;
  TLabel *Label36;
  TComboBox *SFTPBugSymlinkCombo;
  TTabSheet *KexSheet;
  TGroupBox *KexOptionsGroup;
  TLabel *Label28;
  TListBox *KexListBox;
  TButton *KexUpButton;
  TButton *KexDownButton;
  TGroupBox *KexReexchangeGroup;
  TLabel *Label31;
  TUpDownEdit *RekeyTimeEdit;
  TLabel *Label32;
  TEdit *RekeyDataEdit;
  TGroupBox *IPvGroup;
  TRadioButton *IPAutoButton;
  TRadioButton *IPv4Button;
  TRadioButton *IPv6Button;
  TLabel *BugRekey2Label;
  TComboBox *BugRekey2Combo;
  TGroupBox *SFTPProtocolGroup;
  TLabel *Label34;
  TLabel *Label35;
  TComboBox *SFTPMaxVersionCombo;
  TComboBox *SFTPBugUtfCombo;
  TComboBox *SFTPBugSignedTSCombo;
  TButton *HelpButton;
  TButton *ColorButton;
  TPopupMenu *ColorPopupMenu;
  TMenuItem *ColorDefaultItem;
  TMenuItem *PickColorItem;
  TImageList *ColorImageList;
  TButton *RenameButton;
  TAction *RenameSessionAction;
  TGroupBox *DirectoryOptionsGroup;
  TTabSheet *TunnelSheet;
  TGroupBox *TunnelSessionGroup;
  TLabel *Label6;
  TLabel *Label14;
  TLabel *Label15;
  TLabel *Label16;
  TLabel *Label18;
  TEdit *TunnelHostNameEdit;
  TEdit *TunnelUserNameEdit;
  TPasswordEdit *TunnelPasswordEdit;
  TUpDownEdit *TunnelPortNumberEdit;
  TFilenameEdit *TunnelPrivateKeyEdit;
  TLabel *Label19;
  TComboBox *ReturnVarEdit;
  TLabel *Label20;
  TGroupBox *AuthenticationParamsGroup;
  TCheckBox *GSSAPIFwdTGTCheck;
  TCheckBox *AgentFwdCheck;
  TLabel *GSSAPIServerRealmLabel;
  TEdit *GSSAPIServerRealmEdit;
  TCheckBox *TunnelCheck;
  TGroupBox *TunnelOptionsGroup;
  TLabel *Label21;
  TComboBox *TunnelLocalPortNumberEdit;
  TRadioButton *DSTModeKeepCheck;
  TButton *UnixEnvironmentButton;
  TButton *WindowsEnvironmentButton;
  TComboBox *TransferProtocolCombo;
  TLabel *Label22;
  TCheckBox *AllowScpFallbackCheck;
  TLabel *InsecureLabel;
  TGroupBox *ConnectionGroup;
  TCheckBox *FtpPasvModeCheck;
  TAction *ShellIconSessionAction;
  void __fastcall DataChange(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall SessionListViewSelectItem(TObject *Sender,
  TListItem *Item, bool Selected);
  void __fastcall SessionListViewDblClick(TObject *Sender);
  void __fastcall SessionListViewInfoTip(TObject *Sender,
    TListItem *Item, AnsiString &InfoTip);
  void __fastcall SessionListViewKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift);
  void __fastcall EditSessionActionExecute(TObject *Sender);
  void __fastcall SaveSessionActionExecute(TObject *Sender);
  void __fastcall DeleteSessionActionExecute(TObject *Sender);
  void __fastcall ImportSessionsActionExecute(TObject *Sender);
  void __fastcall CleanUpActionExecute(TObject *Sender);
  void __fastcall AboutActionExecute(TObject *Sender);
  void __fastcall ActionListUpdate(TBasicAction *Action,
    bool &Handled);
  void __fastcall PreferencesButtonClick(TObject *Sender);
  void __fastcall NewSessionActionExecute(TObject *Sender);
  void __fastcall NavigationTreeChange(TObject *Sender, TTreeNode *Node);
  void __fastcall PageControlChange(TObject *Sender);
  void __fastcall AlgListBoxStartDrag(TObject *Sender,
          TDragObject *&DragObject);
  void __fastcall AlgListBoxDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
  void __fastcall AlgListBoxDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
  void __fastcall CipherButtonClick(TObject *Sender);
  void __fastcall SetDefaultSessionActionExecute(TObject *Sender);
  void __fastcall ToolsMenuButtonClick(TObject *Sender);
  void __fastcall DesktopIconActionExecute(TObject *Sender);
  void __fastcall SessionListViewCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall SendToHookActionExecute(TObject *Sender);
  void __fastcall CheckForUpdatesActionExecute(TObject *Sender);
  void __fastcall LanguagesButtonClick(TObject *Sender);
  void __fastcall AuthGSSAPICheckClick(TObject *Sender);
  void __fastcall KexButtonClick(TObject *Sender);
  void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall PrivateKeyEditAfterDialog(TObject *Sender,
          AnsiString &Name, bool &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall ColorButtonClick(TObject *Sender);
  void __fastcall ColorDefaultItemClick(TObject *Sender);
  void __fastcall PickColorItemClick(TObject *Sender);
  void __fastcall SessionListViewEditing(TObject * Sender, TListItem * Item,
    bool & AllowEdit);
  void __fastcall RenameSessionActionExecute(TObject * Sender);
  void __fastcall SessionListViewEdited(TObject * Sender, TListItem * Item,
    AnsiString & S);
  void __fastcall SessionListViewCompare(TObject * Sender, TListItem * Item1,
    TListItem * Item2, int Data, int & Compare);
  void __fastcall UnixEnvironmentButtonClick(TObject *Sender);
  void __fastcall WindowsEnvironmentButtonClick(TObject *Sender);
  void __fastcall PathEditBeforeDialog(TObject *Sender, AnsiString &Name,
          bool &Action);
  void __fastcall TransferProtocolComboChange(TObject *Sender);
  void __fastcall NavigationTreeCollapsing(TObject *Sender,
          TTreeNode *Node, bool &AllowCollapse);
  void __fastcall ShellIconSessionActionExecute(TObject *Sender);

private:
  int NoUpdate;
  TSessionData * FSessionData;
  TSessionData * FEditingSessionData;
  TStoredSessionList * FStoredSessions;
  int FAlgDragSource, FAlgDragDest;
  int FOptions;
  TPopupMenu * FLanguagesPopupMenu;
  AnsiString FOrigCaption;
  bool FInitialized;
  TTabSheet * FSavedTab;
  int FSavedSession;
  bool FLocaleChanging;
  void * FSystemSettings;
  AnsiString FCurrentSessionName;
  TColor FColor;
  AnsiString FBeforeDialogPath;
  TStringList * FTreeLabels;
  TFSProtocol FFSProtocol;

  void __fastcall LoadSession(TSessionData * aSessionData);
  void __fastcall UpdateControls();
  void __fastcall SetSessionData(TSessionData * value);
  TSessionData * __fastcall GetSessionData();
  void __fastcall SaveSession(TSessionData * aStoredSession);
  void __fastcall SetStoredSessions(TStoredSessionList * value);
  void __fastcall LoadSessions();
  void __fastcall LoadSessionItem(TListItem * Item);
  void __fastcall SetSelectedSession(TSessionData * value);
  TSessionData * __fastcall GetSelectedSession();
  void __fastcall CMDialogKey(TWMKeyDown & Message);
  void __fastcall WMHelp(TWMHelp & Message);
  void __fastcall InitializeBugsCombo(TComboBox * BugsCombo);
  int __fastcall FSProtocolToIndex(TFSProtocol FSProtocol, bool & AllowScpFallback);
  TFSProtocol __fastcall IndexToFSProtocol(int Index, bool AllowScpFallback);
  void __fastcall UpdateNavigationTree();
  void __fastcall LoadPing(TSessionData * aSessionData);
  void __fastcall SavePing(TSessionData * aSessionData);

protected:
  void __fastcall Default();
  void __fastcall LoadConfiguration();
  void __fastcall SaveConfiguration();
  void __fastcall ShowPreferencesDialog();
  void __fastcall ChangePage(TTabSheet * Tab);
  virtual void __fastcall Dispatch(void * Message);
  bool __fastcall AllowAlgDrag(TListBox * AlgListBox, int X, int Y);
  void __fastcall AlgMove(TListBox * AlgListBox, int Source, int Dest);
  void __fastcall SetOptions(int value);
  void __fastcall LocaleClick(TObject * Sender);
  void __fastcall LocaleGetClick(TObject * Sender);
  void __fastcall Init();
  void __fastcall InitControls();
  void __fastcall ShowTabs(bool Show);
  void __fastcall VerifyKey(AnsiString FileName, bool TypeOnly);
  void __fastcall EditSession();

public:
  virtual __fastcall TLoginDialog(TComponent* AOwner);
  __fastcall ~TLoginDialog();
  bool __fastcall Execute();

  __property TSessionData * SessionData  = { read=GetSessionData, write=SetSessionData };
  __property TStoredSessionList * StoredSessions  = { read=FStoredSessions, write=SetStoredSessions };
  __property TSessionData * SelectedSession  = { read=GetSelectedSession, write=SetSelectedSession };
  __property int Options = { read=FOptions, write=SetOptions };
};
//----------------------------------------------------------------------------
#endif
