//---------------------------------------------------------------------------
#ifndef CustomWinConfigurationH
#define CustomWinConfigurationH
//---------------------------------------------------------------------------
#include "GUIConfiguration.h"
#define WM_WINSCP_USER   (WM_USER + 0x2000)
#define WM_LOCALE_CHANGE (WM_WINSCP_USER + 1)
// WM_USER_STOP = WM_WINSCP_USER + 2 (in forms/Synchronize.cpp)
// WM_INTERUPT_IDLE = WM_WINSCP_USER + 3 (in windows/ConsoleRunner.cpp)
// WM_COMPONENT_HIDE = WM_WINSCP_USER + 4 (forms/CustomScpExplorer.cpp)
// WM_TRAY_ICON = WM_WINSCP_USER + 5 (forms/CustomScpExplorer.cpp)
// WM_LOG_UPDATE = WM_WINSCP_USER + 6 (components/LogMemo.cpp)
//---------------------------------------------------------------------------
#define C(Property) (Property != rhc.Property) ||
struct TSynchronizeChecklistConfiguration
{
  UnicodeString WindowParams;
  UnicodeString ListParams;
  bool __fastcall operator !=(TSynchronizeChecklistConfiguration & rhc)
    { return C(WindowParams) C(ListParams) 0; };
};
typedef TSynchronizeChecklistConfiguration TFindFileConfiguration;
//---------------------------------------------------------------------------
struct TConsoleWinConfiguration
{
  UnicodeString WindowSize;
  bool __fastcall operator !=(TConsoleWinConfiguration & rhc)
    { return C(WindowSize) 0; };
};
//---------------------------------------------------------------------------
class TCustomWinConfiguration : public TGUIConfiguration
{
static const int MaxHistoryCount = 50;
private:
  TLogView FLogView;
  TInterface FInterface;
  bool FShowAdvancedLoginOptions;
  TStringList * FHistory;
  TStrings * FEmptyHistory;
  TSynchronizeChecklistConfiguration FSynchronizeChecklist;
  TFindFileConfiguration FFindFile;
  TConsoleWinConfiguration FConsoleWin;
  TInterface FDefaultInterface;
  bool FDefaultShowAdvancedLoginOptions;
  bool FConfirmExitOnCompletion;
  bool FOperationProgressOnTop;
  TNotifyEvent FOnMasterPasswordRecrypt;

  void __fastcall SetInterface(TInterface value);
  void __fastcall SetLogView(TLogView value);
  void __fastcall SetShowAdvancedLoginOptions(bool value);
  void __fastcall SetHistory(const UnicodeString Index, TStrings * value);
  TStrings * __fastcall GetHistory(const UnicodeString Index);
  void __fastcall SetSynchronizeChecklist(TSynchronizeChecklistConfiguration value);
  void __fastcall SetFindFile(TFindFileConfiguration value);
  void __fastcall SetConsoleWin(TConsoleWinConfiguration value);
  void __fastcall SetConfirmExitOnCompletion(bool value);

protected:
  virtual void __fastcall SaveData(THierarchicalStorage * Storage, bool All);
  virtual void __fastcall LoadData(THierarchicalStorage * Storage);
  virtual void __fastcall LoadAdmin(THierarchicalStorage * Storage);
  virtual void __fastcall Saved();
  void __fastcall ClearHistory();
  void __fastcall DefaultHistory();
  void __fastcall RecryptPasswords();
  virtual bool __fastcall GetUseMasterPassword() = 0;

public:
  __fastcall TCustomWinConfiguration();
  virtual __fastcall ~TCustomWinConfiguration();
  virtual void __fastcall Default();
  virtual void __fastcall AskForMasterPasswordIfNotSet() = 0;

  __property TLogView LogView = { read = FLogView, write = SetLogView };
  __property TInterface Interface = { read = FInterface, write = SetInterface };
  __property bool ShowAdvancedLoginOptions = { read = FShowAdvancedLoginOptions, write = SetShowAdvancedLoginOptions};
  __property TStrings * History[UnicodeString Name] = { read = GetHistory, write = SetHistory };
  __property TSynchronizeChecklistConfiguration SynchronizeChecklist = { read = FSynchronizeChecklist, write = SetSynchronizeChecklist };
  __property TFindFileConfiguration FindFile = { read = FFindFile, write = SetFindFile };
  __property TConsoleWinConfiguration ConsoleWin = { read = FConsoleWin, write = SetConsoleWin };
  __property bool ConfirmExitOnCompletion  = { read=FConfirmExitOnCompletion, write=SetConfirmExitOnCompletion };
  __property bool OperationProgressOnTop  = { read=FOperationProgressOnTop, write=FOperationProgressOnTop };
  __property bool UseMasterPassword = { read = GetUseMasterPassword };
  __property TNotifyEvent OnMasterPasswordRecrypt = { read = FOnMasterPasswordRecrypt, write = FOnMasterPasswordRecrypt };
};
//---------------------------------------------------------------------------
#define CustomWinConfiguration \
  (dynamic_cast<TCustomWinConfiguration *>(Configuration))
//---------------------------------------------------------------------------
#endif
