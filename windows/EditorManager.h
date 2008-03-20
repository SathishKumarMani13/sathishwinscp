//---------------------------------------------------------------------------
#ifndef EditorManagerH
#define EditorManagerH
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
class TTerminal;
class TTerminalQueue;
//---------------------------------------------------------------------------
struct TEditedFileData
{
  AnsiString LocalRootDirectory;
  AnsiString RemoteDirectory;
  bool ForceText;
  TTerminal * Terminal;
  TTerminalQueue * Queue;
  AnsiString SessionName;
  AnsiString OriginalFileName;
  AnsiString Command;
};
//---------------------------------------------------------------------------
typedef void __fastcall (__closure * TEditedFileChangedEvent)
  (const AnsiString FileName, const TEditedFileData & Data, HANDLE CompleteEvent);
typedef void __fastcall (__closure * TEditedFileReloadEvent)
  (const AnsiString FileName, const TEditedFileData & Data);
typedef void __fastcall (__closure * TEditedFileEarlyClosedEvent)
  (const TEditedFileData & Data, bool & KeepOpen);
//---------------------------------------------------------------------------
typedef void __fastcall (__closure * TEditedFileProcessEvent)
  (const AnsiString FileName, TEditedFileData & Data, TObject * Token, void * Arg);
//---------------------------------------------------------------------------
class TEditorManager
{
public:
  __fastcall TEditorManager();
  __fastcall ~TEditorManager();

  bool __fastcall Empty(bool IgnoreClosed);
  bool __fastcall CanAddFile(const AnsiString RemoteDirectory,
    const AnsiString OriginalFileName, const AnsiString SessionName,
    TObject *& Token, AnsiString & ExistingLocalRootDirectory,
    AnsiString & ExistingLocalDirectory);
  bool __fastcall CloseInternalEditors(TNotifyEvent CloseCallback);
  bool __fastcall CloseExternalFilesWithoutProcess();

  void __fastcall AddFileInternal(const AnsiString FileName,
    const TEditedFileData & Data, TObject * Token);
  void __fastcall AddFileExternal(const AnsiString FileName,
    const TEditedFileData & Data, HANDLE Process);

  void __fastcall Check();

  void __fastcall FileChanged(TObject * Token);
  void __fastcall FileReload(TObject * Token);
  void __fastcall FileClosed(TObject * Token);

  void __fastcall ProcessFiles(TEditedFileProcessEvent Callback, void * Arg);

  __property TEditedFileChangedEvent OnFileChange = { read = FOnFileChange, write = FOnFileChange };
  __property TEditedFileReloadEvent OnFileReload = { read = FOnFileReload, write = FOnFileReload };
  __property TEditedFileEarlyClosedEvent OnFileEarlyClosed = { read = FOnFileEarlyClosed, write = FOnFileEarlyClosed };

private:
  struct TFileData
  {
    AnsiString FileName;
    HANDLE Monitor;
    bool External;
    HANDLE Process;
    TObject * Token;
    int Timestamp;
    TEditedFileData Data;
    bool Closed;
    HANDLE UploadCompleteEvent;
    TDateTime Opened;
    bool Reupload;
  };

  std::vector<TFileData> FFiles;
  std::vector<HANDLE> FMonitors;
  std::vector<HANDLE> FProcesses;
  std::vector<HANDLE> FUploadCompleteEvents;
  TEditedFileChangedEvent FOnFileChange;
  TEditedFileReloadEvent FOnFileReload;
  TEditedFileEarlyClosedEvent FOnFileEarlyClosed;

  void __fastcall AddFile(TFileData & FileData);
  void __fastcall UploadComplete(int Index);
  void __fastcall CloseFile(int Index, bool IgnoreErrors, bool Delete);
  void __fastcall CloseProcess(int Index);
  bool __fastcall EarlyClose(int Index);
  void __fastcall CheckFileChange(int Index, bool Force);
  int __fastcall FindFile(const TObject * Token);

  enum TWaitHandle { MONITOR, PROCESS, EVENT };
  int __fastcall WaitFor(unsigned int Count, const HANDLE * Handles,
    TWaitHandle WaitFor);
};
//---------------------------------------------------------------------------
#endif
