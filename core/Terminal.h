//---------------------------------------------------------------------------
#ifndef TerminalH
#define TerminalH

#include <Classes.hpp>

#include "FileOperationProgress.h"
#include "FileMasks.h"
#include "SecureShell.h"
//---------------------------------------------------------------------------
class TCopyParamType;
class TFileOperationProgressType;
class TRemoteDirectory;
class TRemoteFile;
class TCustomFileSystem;
struct TCalculateSizeParams;
struct TOverwriteFileParams;
typedef TStringList TUserGroupsList;
typedef void __fastcall (__closure *TReadDirectoryEvent)(System::TObject* Sender, Boolean ReloadOnly);
typedef void __fastcall (__closure *TProcessFileEvent)
  (const AnsiString FileName, const TRemoteFile * File, void * Param);
typedef int __fastcall (__closure *TFileOperationEvent)
  (void * Param1, void * Param2);
//---------------------------------------------------------------------------
/*#define SUSPEND_OPERATION(Command)    \
  {                                   \
    OperationProgress->Suspend();     \
    try {                             \
      Command                         \
    } __finally {                     \
      OperationProgress->Resume();    \
    }                                 \
  }
 */

#define SUSPEND_OPERATION(Command)                            \
  {                                                           \
    TSuspendFileOperationProgress Suspend(OperationProgress); \
    Command                                                   \
  }
 

#define THROW_SKIP_FILE(EXCEPTION, MESSAGE) \
  throw EScpSkipFile(EXCEPTION, MESSAGE)

/* TODO : Better user interface (query to user) */
#define FILE_OPERATION_LOOP_CUSTOM(TERMINAL, ALLOW_SKIP, MESSAGE, OPERATION) { \
  bool DoRepeat; \
  do { \
    DoRepeat = false; \
    try { \
      OPERATION;                                                            \
    }                                                                       \
    catch (EAbort & E)                                                      \
    {                                                                       \
      throw;                                                                \
    }                                                                       \
    catch (EScpSkipFile & E)                                                \
    {                                                                       \
      throw;                                                                \
    }                                                                       \
    catch (EFatal & E)                                                \
    {                                                                       \
      throw;                                                                \
    }                                                                       \
    catch (Exception & E)                                                   \
    { \
      HandleExtendedException(&E); \
      int Answers = qaRetry | qaAbort | ((ALLOW_SKIP) ? qaSkip : 0); \
      int Answer; \
      int Params = qpAllowContinueOnError | (!(ALLOW_SKIP) ? qpFatalAbort : 0); \
      SUSPEND_OPERATION ( \
        Answer = TERMINAL->DoQueryUser(MESSAGE, &E, Answers, Params); \
      ); \
      DoRepeat = (Answer == qaRetry); \
      if (Answer == qaAbort) OperationProgress->Cancel = csCancel; \
      if (!DoRepeat && ALLOW_SKIP) THROW_SKIP_FILE(&E, MESSAGE); \
        else \
      if (!DoRepeat && !ALLOW_SKIP) throw; \
    } \
  } while (DoRepeat); }

#define FILE_OPERATION_LOOP(MESSAGE, OPERATION) \
  FILE_OPERATION_LOOP_EX(True, MESSAGE, OPERATION)
//---------------------------------------------------------------------------
enum TFSCapability { fcUserGroupListing, fcModeChanging, fcGroupChanging,
  fcOwnerChanging, fcAnyCommand, fcHardLink, fcSymbolicLink, fcResolveSymlink,
  fcTextMode, fcRename, fcNativeTextMode };
//---------------------------------------------------------------------------
const cpDelete = 0x01;
const cpDragDrop = 0x04;
const cpTemporary = 0x04; // alias to cpDragDrop
const cpNoConfirmation = 0x08;
//---------------------------------------------------------------------------
const ccApplyToDirectories = 0x01;
const ccRecursive = 0x02;
//---------------------------------------------------------------------------
const csIgnoreErrors = 0x01;
//---------------------------------------------------------------------------
class TTerminal : public TSecureShell
{
// for TranslateLockedPath()
friend class TRemoteFile;
// for ReactOnCommand()
friend class TSCPFileSystem;
friend class TSFTPFileSystem;
private:
  AnsiString FCurrentDirectory;
  AnsiString FLockDirectory;
  Integer FExceptionOnFail;
  TRemoteDirectory * FFiles;
  int FInTransaction;
  TNotifyEvent FOnChangeDirectory;
  TReadDirectoryEvent FOnReadDirectory;
  TNotifyEvent FOnStartReadDirectory;
  bool FReadCurrentDirectoryPending;
  bool FReadDirectoryPending;
  TUserGroupsList * FUserGroups;
  bool FUserGroupsLookedup;
  TFileOperationProgressEvent FOnProgress;
  TFileOperationFinished FOnFinished;
  TFileOperationProgressType * FOperationProgress;
  bool FUseBusyCursor;
  TRemoteDirectoryCache * FDirectoryCache;
  TRemoteDirectoryChangesCache * FDirectoryChangesCache;
  TCustomFileSystem * FFileSystem;
  TStrings * FAdditionalInfo;
  AnsiString FLastDirectoryChange;
  void __fastcall CommandError(Exception * E, const AnsiString Msg);
  int __fastcall CommandError(Exception * E, const AnsiString Msg, int Answers);
  AnsiString __fastcall PeekCurrentDirectory();
  AnsiString __fastcall GetCurrentDirectory();
  bool __fastcall GetExceptionOnFail() const;
  AnsiString __fastcall GetProtocolName();
  TUserGroupsList * __fastcall GetUserGroups();
  void __fastcall SetCurrentDirectory(AnsiString value);
  void __fastcall SetExceptionOnFail(bool value);
  void __fastcall ReactOnCommand(int /*TFSCommand*/ Cmd);
  AnsiString __fastcall GetUserName() const;
  bool __fastcall GetAreCachesEmpty() const;

protected:
  void __fastcall DoStartReadDirectory();
  void __fastcall DoReadDirectory(bool ReloadOnly);
  void __fastcall DoCreateDirectory(const AnsiString DirName,
    const TRemoteProperties * Properties);
  void __fastcall DoDeleteFile(const AnsiString FileName,
    const TRemoteFile * File, void * Param);
  void __fastcall DoCustomCommandOnFile(AnsiString FileName,
    const TRemoteFile * File, AnsiString Command, int Params);
  void __fastcall DoRenameFile(const AnsiString FileName, const AnsiString NewName);
  void __fastcall DoChangeFileProperties(const AnsiString FileName,
    const TRemoteFile * File, const TRemoteProperties * Properties);
  void __fastcall DoChangeDirectory();
  void __fastcall EnsureNonExistence(const AnsiString FileName);
  void __fastcall LookupUserGroups();
  void __fastcall FileModified(const TRemoteFile * File);
  int __fastcall FileOperationLoop(TFileOperationEvent CallBackFunc,
    TFileOperationProgressType * OperationProgress, bool AllowSkip,
    const AnsiString Message, void * Param1 = NULL, void * Param2 = NULL);
  bool __fastcall GetIsCapable(TFSCapability Capability) const;
  void __fastcall ProcessFiles(TStrings * FileList,
    TFileOperation Operation, TProcessFileEvent ProcessFile, void * Param = NULL);
  void __fastcall ProcessDirectory(const AnsiString DirName,
    TProcessFileEvent CallBackFunc, void * Param = NULL);
  AnsiString __fastcall TranslateLockedPath(AnsiString Path, bool Lock);
  void __fastcall ReadDirectory(TRemoteFileList * FileList);
  void __fastcall CustomReadDirectory(TRemoteFileList * FileList);
  void __fastcall DoCreateLink(const AnsiString FileName, const AnsiString PointTo, bool Symbolic);
  bool __fastcall CreateLocalFile(const AnsiString FileName,
    TFileOperationProgressType * OperationProgress, HANDLE * AHandle);
  void __fastcall OpenLocalFile(const AnsiString FileName, int Access,
    int * Attrs, HANDLE * Handle, unsigned long * ACTime, unsigned long * MTime,
    unsigned long * ATime, __int64 * Size, bool TryWriteReadOnly = true);
  TRemoteFileList * ReadDirectoryListing(AnsiString Directory);
  bool __fastcall HandleException(Exception * E);
  void __fastcall CalculateFileSize(AnsiString FileName,
    const TRemoteFile * File, /*TCalculateSizeParams*/ void * Size);
  void __fastcall DoCalculateDirectorySize(const AnsiString FileName,
    const TRemoteFile * File, TCalculateSizeParams * Params);
  void __fastcall CalculateLocalFileSize(const AnsiString FileName,
    const TSearchRec Rec, /*__int64*/ void * Size);
  void __fastcall CalculateLocalFilesSize(TStrings * FileList, __int64 & Size);
  TStrings * __fastcall GetAdditionalInfo();
  int __fastcall ConfirmFileOverwrite(const AnsiString FileName,
    const TOverwriteFileParams * FileParams, int Answers, int Params);

  __property TFileOperationProgressType * OperationProgress = { read=FOperationProgress };

public:
  __fastcall TTerminal();
  __fastcall ~TTerminal();
  virtual void __fastcall Open();
  virtual void __fastcall Close();
  void __fastcall DirectoryModified(const AnsiString Path, bool SubDirs);
  void __fastcall AnyCommand(const AnsiString Command);
  void __fastcall CloseOnCompletion(const AnsiString Message = "");
  AnsiString __fastcall AbsolutePath(AnsiString Path);
  void __fastcall BeginTransaction();
  void __fastcall ReadCurrentDirectory();
  void __fastcall ReadDirectory(bool ReloadOnly);
  void __fastcall ReadFile(const AnsiString FileName, TRemoteFile *& File);
  void __fastcall ReadSymlink(TRemoteFile * SymlinkFile, TRemoteFile *& File);
  void __fastcall CopyToLocal(TStrings * FilesToCopy,
    const AnsiString TargetDir, const TCopyParamType * CopyParam, int Params);
  void __fastcall CopyToRemote(TStrings * FilesToCopy,
    const AnsiString TargetDir, const TCopyParamType * CopyParam, int Params);
  void __fastcall CreateDirectory(const AnsiString DirName,
    const TRemoteProperties * Properties = NULL);
  void __fastcall CreateLink(const AnsiString FileName, const AnsiString PointTo, bool Symbolic);
  void __fastcall DeleteFile(AnsiString FileName,
    const TRemoteFile * File = NULL, void * Recursive = NULL);
  void __fastcall DeleteFiles(TStrings * FilesToDelete, bool * Recursive = NULL);
  void __fastcall CustomCommandOnFile(AnsiString FileName,
    const TRemoteFile * File, void * AParams);
  void __fastcall CustomCommandOnFiles(AnsiString Command, int Params, TStrings * Files);
  void __fastcall ChangeDirectory(const AnsiString Directory);
  void __fastcall DoStartup();
  void __fastcall EndTransaction();
  void __fastcall HomeDirectory();
  void __fastcall ChangeFileProperties(AnsiString FileName,
    const TRemoteFile * File, /*const TRemoteProperties */ void * Properties);
  void __fastcall ChangeFilesProperties(TStrings * FileList,
    const TRemoteProperties * Properties);
  void __fastcall TerminalError(AnsiString Msg);
  void __fastcall TerminalError(Exception * E, AnsiString Msg);
  void __fastcall ReloadDirectory();
  void __fastcall RenameFile(const AnsiString FileName, const AnsiString NewName);
  void __fastcall RenameFile(const TRemoteFile * File, const AnsiString NewName, bool CheckExistence);
  void __fastcall CalculateFilesSize(TStrings * FileList, __int64 & Size, int Params);
  void __fastcall ClearCaches();
  static bool __fastcall IsAbsolutePath(const AnsiString Path);
  static AnsiString __fastcall ExpandFileName(AnsiString Path,
    const AnsiString BasePath);

  __property AnsiString CurrentDirectory = { read = GetCurrentDirectory, write = SetCurrentDirectory };
  __property bool ExceptionOnFail = { read = GetExceptionOnFail, write = SetExceptionOnFail };
  __property TRemoteDirectory * Files = { read = FFiles };
  __property TNotifyEvent OnChangeDirectory = { read = FOnChangeDirectory, write = FOnChangeDirectory };
  __property TReadDirectoryEvent OnReadDirectory = { read = FOnReadDirectory, write = FOnReadDirectory };
  __property TNotifyEvent OnStartReadDirectory = { read = FOnStartReadDirectory, write = FOnStartReadDirectory };
  __property TUserGroupsList * UserGroups = { read = GetUserGroups };
  __property TFileOperationProgressEvent OnProgress  = { read=FOnProgress, write=FOnProgress };
  __property TFileOperationFinished OnFinished  = { read=FOnFinished, write=FOnFinished };
  __property AnsiString ProtocolName = { read = GetProtocolName };
  __property bool UseBusyCursor = { read = FUseBusyCursor, write = FUseBusyCursor };
  __property AnsiString UserName  = { read=GetUserName };
  __property bool IsCapable[TFSCapability Capability] = { read = GetIsCapable };
  __property TStrings * AdditionalInfo = { read = GetAdditionalInfo };
  __property bool AreCachesEmpty = { read = GetAreCachesEmpty };
};
//---------------------------------------------------------------------------
class TTerminalList : public TObjectList
{
public:
  __fastcall TTerminalList(TConfiguration * AConfiguration);
  __fastcall ~TTerminalList();

  virtual TTerminal * __fastcall NewTerminal(TSessionData * Data);
  virtual void __fastcall FreeTerminal(TTerminal * Terminal);
  void __fastcall FreeAndNullTerminal(TTerminal * & Terminal);
  virtual void __fastcall Idle();

  __property TTerminal * Terminals[int Index]  = { read=GetTerminal };

private:
  TConfiguration * FConfiguration;

  TTerminal * __fastcall GetTerminal(int Index);
};
//---------------------------------------------------------------------------
struct TCustomCommandParams
{
  AnsiString Command;
  int Params;
};
//---------------------------------------------------------------------------
struct TCalculateSizeParams
{
  __int64 Size;
  int Params;
};
//---------------------------------------------------------------------------
struct TOverwriteFileParams
{
  __int64 SourceSize;
  __int64 DestSize;
  TDateTime SourceTimestamp;
  TDateTime DestTimestamp;
};
//---------------------------------------------------------------------------
#endif
