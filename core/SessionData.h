//---------------------------------------------------------------------------
#ifndef SessionDataH
#define SessionDataH

#include "FileBuffer.h"
#include "NamedObjs.h"
#include "HierarchicalStorage.h"
//---------------------------------------------------------------------------
#define SET_SESSION_PROPERTY(Property) \
  if (F##Property != value) { F##Property = value; FModified = true; }
//---------------------------------------------------------------------------
enum TCipher { cipWarn, cip3DES, cipBlowfish, cipAES, cipDES };
#define CIPHER_COUNT (cipDES+1)
enum TProtocol { ptRaw, ptTelnet, ptRLogin, ptSSH };
enum TFSProtocol { fsSCPonly, fsSFTP, fsSFTPonly };
#define FSPROTOCOL_COUNT (fsSFTPonly+1)
enum TProxyType { pxNone, pxHTTP, pxSocks, pxTelnet }; // 0.53b and older
enum TProxyMethod { pmNone, pmSocks4, pmSocks5, pmHTTP, pmTelnet, pmCmd }; // after 0.53b
enum TSshProt { ssh1only, ssh1, ssh2, ssh2only };
enum TSshBug { sbIgnore1, sbPlainPW1, sbRSA1, sbHMAC2, sbDeriveKey2, sbRSAPad2,
  sbDHGEx2, sbPKSessID2 };
#define BUG_COUNT (sbPKSessID2+1)
enum TAutoSwitch { asOn, asOff, asAuto };
enum TPingType { ptOff, ptNullPacket, ptDummyCommand };
const puRequireUsername =     0x01;
const puExcludeLeadingSlash = 0x02;
const puExtractFileName =     0x04;
//---------------------------------------------------------------------------
extern const char CipherNames[CIPHER_COUNT][10];
extern const char SshProtList[][10];
extern const char ProxyMethodList[][10];
extern const TCipher DefaultCipherList[CIPHER_COUNT];
extern const char FSProtocolNames[FSPROTOCOL_COUNT][11];
//---------------------------------------------------------------------------
class TSessionData : public TNamedObject
{
private:
  AnsiString FHostName;
  int FPortNumber;
  AnsiString FUserName;
  AnsiString FPassword;
  int FPingInterval;
  TPingType FPingType;
  bool FAgentFwd;
  bool FAliasGroupList;
  bool FAuthTIS;
  bool FAuthKI;
  bool FAuthKIPassword;
  bool FAuthGSSAPI;
  bool FCompression;
  TSshProt FSshProt;
  bool FSsh2DES;
  TCipher FCiphers[CIPHER_COUNT];
  bool FClearAliases;
  TEOLType FEOLType;
  AnsiString FPublicKeyFile;
  TProtocol FProtocol;
  TFSProtocol FFSProtocol;
  bool FModified;
  AnsiString FLocalDirectory;
  AnsiString FRemoteDirectory;
  bool FLockInHome;
  bool FSpecial;
  bool FUpdateDirectories;
  bool FCacheDirectories;
  bool FCacheDirectoryChanges;
  bool FPreserveDirectoryChanges;
  bool FSelected;
  bool FLookupUserGroups;
  AnsiString FReturnVar;
  bool FScp1Compatibility;
  AnsiString FShell;
  int FTimeout;
  bool FUnsetNationalVars;
  bool FIgnoreLsWarnings;
  bool FTcpNoDelay;
  TProxyMethod FProxyMethod;
  AnsiString FProxyHost;
  int FProxyPort;
  AnsiString FProxyUsername;
  AnsiString FProxyPassword;
  AnsiString FProxyTelnetCommand;
  TAutoSwitch FProxyDNS;
  bool FProxyLocalhost;
  TAutoSwitch FBugs[BUG_COUNT];
  AnsiString FCustomParam1;
  AnsiString FCustomParam2;
  bool FResolveSymlinks;
  TDateTime FTimeDifference;
  int FSFTPDownloadQueue;
  int FSFTPUploadQueue;
  int FSFTPListingQueue;
  bool FConsiderDST;
  TAutoSwitch FSFTPSymlinkBug;

  void __fastcall SetHostName(AnsiString value);
  void __fastcall SetPortNumber(int value);
  void __fastcall SetUserName(AnsiString value);
  void __fastcall SetPassword(AnsiString value);
  AnsiString __fastcall GetPassword();
  void __fastcall SetPingInterval(int value);
  void __fastcall SetAgentFwd(bool value);
  void __fastcall SetAuthTIS(bool value);
  void __fastcall SetAuthKI(bool value);
  void __fastcall SetAuthKIPassword(bool value);
  void __fastcall SetAuthGSSAPI(bool value);
  void __fastcall SetCompression(bool value);
  void __fastcall SetSshProt(TSshProt value);
  void __fastcall SetSsh2DES(bool value);
  void __fastcall SetCipher(int Index, TCipher value);
  TCipher __fastcall GetCipher(int Index);
  void __fastcall SetPublicKeyFile(AnsiString value);

  void __fastcall SetProtocolStr(AnsiString value);
  AnsiString __fastcall GetProtocolStr();
  bool __fastcall GetCanLogin();
  void __fastcall SetPingIntervalDT(TDateTime value);
  TDateTime __fastcall GetPingIntervalDT();
  void __fastcall SetTimeDifference(TDateTime value);
  void __fastcall SetPingType(TPingType value);
  AnsiString __fastcall GetSessionName();
  void __fastcall SetFSProtocol(TFSProtocol value);
  AnsiString __fastcall GetFSProtocolStr();
  void __fastcall SetLocalDirectory(AnsiString value);
  void __fastcall SetRemoteDirectory(AnsiString value);
  void __fastcall SetUpdateDirectories(bool value);
  void __fastcall SetCacheDirectories(bool value);
  void __fastcall SetCacheDirectoryChanges(bool value);
  void __fastcall SetPreserveDirectoryChanges(bool value);
  void __fastcall SetLockInHome(bool value);
  void __fastcall SetSpecial(bool value);
  AnsiString __fastcall GetInfoTip();
  AnsiString __fastcall GetDefaultLogFileName();
  bool __fastcall GetDefaultShell();
  void __fastcall SetDetectReturnVar(bool value);
  bool __fastcall GetDetectReturnVar();
  void __fastcall SetAliasGroupList(bool value);
  void __fastcall SetClearAliases(bool value);
  void __fastcall SetDefaultShell(bool value);
  void __fastcall SetEOLType(TEOLType value);
  void __fastcall SetLookupUserGroups(bool value);
  void __fastcall SetReturnVar(AnsiString value);
  void __fastcall SetScp1Compatibility(bool value);
  void __fastcall SetShell(AnsiString value);
  void __fastcall SetTimeout(int value);
  void __fastcall SetUnsetNationalVars(bool value);
  void __fastcall SetIgnoreLsWarnings(bool value);
  void __fastcall SetTcpNoDelay(bool value);
  AnsiString __fastcall GetSshProtStr();
  void __fastcall SetCipherList(AnsiString value);
  AnsiString __fastcall GetCipherList();
  void __fastcall SetProxyMethod(TProxyMethod value);
  void __fastcall SetProxyHost(AnsiString value);
  void __fastcall SetProxyPort(int value);
  void __fastcall SetProxyUsername(AnsiString value);
  void __fastcall SetProxyPassword(AnsiString value);
  void __fastcall SetProxyTelnetCommand(AnsiString value);
  void __fastcall SetProxyDNS(TAutoSwitch value);
  void __fastcall SetProxyLocalhost(bool value);
  AnsiString __fastcall GetProxyPassword();
  void __fastcall SetBug(TSshBug Bug, TAutoSwitch value);
  TAutoSwitch __fastcall GetBug(TSshBug Bug);
  AnsiString __fastcall GetSessionKey();
  void __fastcall SetCustomParam1(AnsiString value);
  void __fastcall SetCustomParam2(AnsiString value);
  void __fastcall SetResolveSymlinks(bool value);
  void __fastcall SetSFTPDownloadQueue(int value);
  void __fastcall SetSFTPUploadQueue(int value);
  void __fastcall SetSFTPListingQueue(int value);
  void __fastcall SetSFTPSymlinkBug(TAutoSwitch value);
  AnsiString __fastcall GetStorageKey();
  void __fastcall SetConsiderDST(bool value);

public:
  __fastcall TSessionData(AnsiString aName);
  void __fastcall Default();
  void __fastcall NonPersistant();
  virtual void __fastcall StoreToConfig(void * config);
  void __fastcall Load(THierarchicalStorage * Storage);
  void __fastcall Save(THierarchicalStorage * Storage, bool PuttyExport = false);
  void __fastcall Remove();
  virtual void __fastcall Assign(TPersistent * Source);
  bool __fastcall ParseUrl(AnsiString Url, int Params, AnsiString * FileName);
  static bool __fastcall ParseUrl(AnsiString Url, int Params,
    AnsiString * ConnectInfo, AnsiString * HostName, int * PortNumber,
    AnsiString * UserName, AnsiString * Password, AnsiString * Path,
    AnsiString * FileName);

  __property AnsiString HostName  = { read=FHostName, write=SetHostName };
  __property int PortNumber  = { read=FPortNumber, write=SetPortNumber };
  __property AnsiString UserName  = { read=FUserName, write=SetUserName };
  __property AnsiString Password  = { read=GetPassword, write=SetPassword };
  __property int PingInterval  = { read=FPingInterval, write=SetPingInterval };
  __property bool AgentFwd  = { read=FAgentFwd, write=SetAgentFwd };
  __property bool AliasGroupList = { read = FAliasGroupList, write = SetAliasGroupList };
  __property bool AuthTIS  = { read=FAuthTIS, write=SetAuthTIS };
  __property bool AuthKI  = { read=FAuthKI, write=SetAuthKI };
  __property bool AuthKIPassword  = { read=FAuthKIPassword, write=SetAuthKIPassword };
  __property bool AuthGSSAPI  = { read=FAuthGSSAPI, write=SetAuthGSSAPI };
  __property bool Compression  = { read=FCompression, write=SetCompression };
  __property TSshProt SshProt  = { read=FSshProt, write=SetSshProt };
  __property bool Ssh2DES  = { read=FSsh2DES, write=SetSsh2DES };
  __property TCipher Cipher[int Index] = { read=GetCipher, write=SetCipher };
  __property AnsiString PublicKeyFile  = { read=FPublicKeyFile, write=SetPublicKeyFile };
  __property TProtocol Protocol  = { read=FProtocol };
  __property AnsiString ProtocolStr  = { read=GetProtocolStr, write=SetProtocolStr };
  __property TFSProtocol FSProtocol  = { read=FFSProtocol, write=SetFSProtocol  };
  __property AnsiString FSProtocolStr  = { read=GetFSProtocolStr };
  __property bool Modified  = { read=FModified, write=FModified };
  __property bool CanLogin  = { read=GetCanLogin };
  __property bool ClearAliases = { read = FClearAliases, write = SetClearAliases };
  __property TDateTime PingIntervalDT = { read = GetPingIntervalDT, write = SetPingIntervalDT };
  __property TDateTime TimeDifference = { read = FTimeDifference, write = SetTimeDifference };
  __property TPingType PingType = { read = FPingType, write = SetPingType };
  __property AnsiString SessionName  = { read=GetSessionName };
  __property AnsiString LocalDirectory  = { read=FLocalDirectory, write=SetLocalDirectory };
  __property AnsiString RemoteDirectory  = { read=FRemoteDirectory, write=SetRemoteDirectory };
  __property bool UpdateDirectories = { read=FUpdateDirectories, write=SetUpdateDirectories };
  __property bool CacheDirectories = { read=FCacheDirectories, write=SetCacheDirectories };
  __property bool CacheDirectoryChanges = { read=FCacheDirectoryChanges, write=SetCacheDirectoryChanges };
  __property bool PreserveDirectoryChanges = { read=FPreserveDirectoryChanges, write=SetPreserveDirectoryChanges };
  __property bool LockInHome = { read=FLockInHome, write=SetLockInHome };
  __property bool Special = { read=FSpecial, write=SetSpecial };
  __property bool Selected  = { read=FSelected, write=FSelected };
  __property AnsiString InfoTip  = { read=GetInfoTip };
  __property AnsiString DefaultLogFileName  = { read=GetDefaultLogFileName };
  __property bool DefaultShell = { read = GetDefaultShell, write = SetDefaultShell };
  __property bool DetectReturnVar = { read = GetDetectReturnVar, write = SetDetectReturnVar };
  __property TEOLType EOLType = { read = FEOLType, write = SetEOLType };
  __property bool LookupUserGroups = { read = FLookupUserGroups, write = SetLookupUserGroups };
  __property AnsiString ReturnVar = { read = FReturnVar, write = SetReturnVar };
  __property bool Scp1Compatibility = { read = FScp1Compatibility, write = SetScp1Compatibility };
  __property AnsiString Shell = { read = FShell, write = SetShell };
  __property int Timeout = { read = FTimeout, write = SetTimeout };
  __property bool UnsetNationalVars = { read = FUnsetNationalVars, write = SetUnsetNationalVars };
  __property bool IgnoreLsWarnings  = { read=FIgnoreLsWarnings, write=SetIgnoreLsWarnings };
  __property bool TcpNoDelay  = { read=FTcpNoDelay, write=SetTcpNoDelay };
  __property AnsiString SshProtStr  = { read=GetSshProtStr };
  __property AnsiString CipherList  = { read=GetCipherList, write=SetCipherList };
  __property TProxyMethod ProxyMethod  = { read=FProxyMethod, write=SetProxyMethod };
  __property AnsiString ProxyHost  = { read=FProxyHost, write=SetProxyHost };
  __property int ProxyPort  = { read=FProxyPort, write=SetProxyPort };
  __property AnsiString ProxyUsername  = { read=FProxyUsername, write=SetProxyUsername };
  __property AnsiString ProxyPassword  = { read=GetProxyPassword, write=SetProxyPassword };
  __property AnsiString ProxyTelnetCommand  = { read=FProxyTelnetCommand, write=SetProxyTelnetCommand };
  __property TAutoSwitch ProxyDNS  = { read=FProxyDNS, write=SetProxyDNS };
  __property bool ProxyLocalhost  = { read=FProxyLocalhost, write=SetProxyLocalhost };
  __property TAutoSwitch Bug[TSshBug Bug]  = { read=GetBug, write=SetBug };
  __property AnsiString CustomParam1 = { read = FCustomParam1, write = SetCustomParam1 };
  __property AnsiString CustomParam2 = { read = FCustomParam2, write = SetCustomParam2 };
  __property AnsiString SessionKey = { read = GetSessionKey };
  __property bool ResolveSymlinks = { read = FResolveSymlinks, write = SetResolveSymlinks };
  __property int SFTPDownloadQueue = { read = FSFTPDownloadQueue, write = SetSFTPDownloadQueue };
  __property int SFTPUploadQueue = { read = FSFTPUploadQueue, write = SetSFTPUploadQueue };
  __property int SFTPListingQueue = { read = FSFTPListingQueue, write = SetSFTPListingQueue };
  __property TAutoSwitch SFTPSymlinkBug = { read = FSFTPSymlinkBug, write = SetSFTPSymlinkBug };
  __property bool ConsiderDST = { read = FConsiderDST, write = SetConsiderDST };
  __property AnsiString StorageKey = { read = GetStorageKey };
};
//---------------------------------------------------------------------------
class TStoredSessionList : public TNamedObjectList
{
public:
  __fastcall TStoredSessionList(bool aReadOnly = false);
  void __fastcall Load(AnsiString aKey);
  void __fastcall Load();
  void __fastcall Save(AnsiString aKey);
  void __fastcall Save();
  void __fastcall Load(THierarchicalStorage * Storage, bool AsModified = false);
  void __fastcall Save(THierarchicalStorage * Storage);
  void __fastcall SelectAll(bool Select);
  void __fastcall Import(TStoredSessionList * From, bool OnlySelected);
  TSessionData * __fastcall AtSession(int Index)
    { return (TSessionData*)AtObject(Index); }
  void __fastcall SelectSessionsToImport(TStoredSessionList * Dest, bool SSHOnly);
  void __fastcall Cleanup();
  int __fastcall IndexOf(TSessionData * Data);
  TSessionData * __fastcall NewSession(AnsiString SessionName, TSessionData * Session);
  virtual __fastcall ~TStoredSessionList();
  __property TSessionData * Sessions[int Index]  = { read=AtSession };
  __property TSessionData * DefaultSettings  = { read=FDefaultSettings, write=SetDefaultSettings };

  static void __fastcall ImportHostKeys(const AnsiString TargetKey,
    const AnsiString SourceKey, TStoredSessionList * Sessions,
    bool OnlySelected);

private:
  TStorage LastStorage;
  TSessionData * FDefaultSettings;
  bool FReadOnly;
  void __fastcall SetDefaultSettings(TSessionData * value);
};
//---------------------------------------------------------------------------
#endif
