//---------------------------------------------------------------------------
#ifndef GUIToolsH
#define GUIToolsH
//---------------------------------------------------------------------------
// from shlobj.h
#define CSIDL_SENDTO                    0x0009        // <user name>\SendTo
#define CSIDL_DESKTOPDIRECTORY          0x0010        // <user name>\Desktop
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019        // All Users\Desktop
#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
#define CSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
#define CSIDL_PERSONAL                  0x0005        // My Documents
//---------------------------------------------------------------------------
#include <FileMasks.H>
//---------------------------------------------------------------------------
class TSessionData;
//---------------------------------------------------------------------------
typedef void __fastcall (__closure* TProcessMessagesEvent)();
//---------------------------------------------------------------------------
bool __fastcall FindFile(AnsiString & Path);
bool __fastcall FileExistsEx(AnsiString Path);
bool __fastcall ExecuteShell(const AnsiString Path, const AnsiString Params);
bool __fastcall ExecuteShell(const AnsiString Path, const AnsiString Params,
  HANDLE & Handle);
bool __fastcall ExecuteShellAndWait(HWND Handle, const AnsiString Path, 
  const AnsiString Params, TProcessMessagesEvent ProcessMessages);
bool __fastcall ExecuteShellAndWait(HWND Handle, const AnsiString Command,
  TProcessMessagesEvent ProcessMessages);
void __fastcall OpenSessionInPutty(TSessionData * SessionData, const AnsiString Password);
bool __fastcall SpecialFolderLocation(int PathID, AnsiString & Path);
AnsiString __fastcall ItemsFormatString(const AnsiString SingleItemFormat,
  const AnsiString MultiItemsFormat, int Count, const AnsiString FirstItem);
AnsiString __fastcall ItemsFormatString(const AnsiString SingleItemFormat,
  const AnsiString MultiItemsFormat, TStrings * Items);
AnsiString __fastcall FileNameFormatString(const AnsiString SingleFileFormat,
  const AnsiString MultiFileFormat, TStrings * Files, bool Remote);
void __fastcall CopyToClipboard(AnsiString Text);
AnsiString __fastcall UniqTempDir(const AnsiString BaseDir,
  const AnsiString Identity, bool Mask = false);
bool __fastcall DeleteDirectory(const AnsiString DirName);
AnsiString __fastcall TranslateExceptionMessage(const Exception * E);
AnsiString __fastcall FormatDateTimeSpan(const AnsiString TimeFormat, TDateTime DateTime);
//---------------------------------------------------------------------------
class TLocalCustomCommand : public TFileCustomCommand
{
public:
  TLocalCustomCommand();
  TLocalCustomCommand(const AnsiString & FileName, const AnsiString & LocalFileName,
    const AnsiString & FileList);

  bool __fastcall HasLocalFileName(const AnsiString & Command);

protected:
  virtual int __fastcall PatternLen(int Index, char PatternCmd);
  virtual bool __fastcall PatternReplacement(int Index, const AnsiString & Pattern,
    AnsiString & Replacement);

private:
  AnsiString FLocalFileName;
};
//---------------------------------------------------------------------------
#endif
