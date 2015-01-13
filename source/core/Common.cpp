//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop

#include "Common.h"
#include "Exceptions.h"
#include "TextsCore.h"
#include "Interface.h"
#include <StrUtils.hpp>
#include <DateUtils.hpp>
#include <math.h>
#include <shlobj.h>
#include <limits>
#include <shlwapi.h>
#include <CoreMain.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// TGuard
//---------------------------------------------------------------------------
__fastcall TGuard::TGuard(TCriticalSection * ACriticalSection) :
  FCriticalSection(ACriticalSection)
{
  assert(ACriticalSection != NULL);
  FCriticalSection->Enter();
}
//---------------------------------------------------------------------------
__fastcall TGuard::~TGuard()
{
  FCriticalSection->Leave();
}
//---------------------------------------------------------------------------
// TUnguard
//---------------------------------------------------------------------------
__fastcall TUnguard::TUnguard(TCriticalSection * ACriticalSection) :
  FCriticalSection(ACriticalSection)
{
  assert(ACriticalSection != NULL);
  FCriticalSection->Leave();
}
//---------------------------------------------------------------------------
__fastcall TUnguard::~TUnguard()
{
  FCriticalSection->Enter();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const wchar_t EngShortMonthNames[12][4] =
  {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
   L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
const char Bom[3] = "\xEF\xBB\xBF";
const wchar_t TokenPrefix = L'%';
const wchar_t NoReplacement = wchar_t(false);
const wchar_t TokenReplacement = wchar_t(true);
const UnicodeString LocalInvalidChars = L"/\\:*?\"<>|";
const UnicodeString PasswordMask = L"***";
//---------------------------------------------------------------------------
UnicodeString ReplaceChar(UnicodeString Str, wchar_t A, wchar_t B)
{
  for (Integer Index = 0; Index < Str.Length(); Index++)
    if (Str[Index+1] == A) Str[Index+1] = B;
  return Str;
}
//---------------------------------------------------------------------------
UnicodeString DeleteChar(UnicodeString Str, wchar_t C)
{
  int P;
  while ((P = Str.Pos(C)) > 0)
  {
    Str.Delete(P, 1);
  }
  return Str;
}
//---------------------------------------------------------------------------
void PackStr(UnicodeString &Str)
{
  // Following will free unnecessary bytes
  Str = Str.c_str();
}
//---------------------------------------------------------------------------
void PackStr(RawByteString &Str)
{
  // Following will free unnecessary bytes
  Str = Str.c_str();
}
//---------------------------------------------------------------------------
void __fastcall Shred(UnicodeString & Str)
{
  if (!Str.IsEmpty())
  {
    Str.Unique();
    memset(Str.c_str(), 0, Str.Length() * sizeof(*Str.c_str()));
    Str = L"";
  }
}
//---------------------------------------------------------------------------
UnicodeString MakeValidFileName(UnicodeString FileName)
{
  UnicodeString IllegalChars = L":;,=+<>|\"[] \\/?*";
  for (int Index = 0; Index < IllegalChars.Length(); Index++)
  {
    FileName = ReplaceChar(FileName, IllegalChars[Index+1], L'-');
  }
  return FileName;
}
//---------------------------------------------------------------------------
UnicodeString RootKeyToStr(HKEY RootKey)
{
  if (RootKey == HKEY_USERS) return L"HKU";
    else
  if (RootKey == HKEY_LOCAL_MACHINE) return L"HKLM";
    else
  if (RootKey == HKEY_CURRENT_USER) return L"HKCU";
    else
  if (RootKey == HKEY_CLASSES_ROOT) return L"HKCR";
    else
  if (RootKey == HKEY_CURRENT_CONFIG) return L"HKCC";
    else
  if (RootKey == HKEY_DYN_DATA) return L"HKDD";
    else
  {  Abort(); return L""; };
}
//---------------------------------------------------------------------------
UnicodeString BooleanToEngStr(bool B)
{
  if (B)
  {
    return L"Yes";
  }
  else
  {
    return L"No";
  }
}
//---------------------------------------------------------------------------
UnicodeString BooleanToStr(bool B)
{
  if (B)
  {
    return LoadStr(YES_STR);
  }
  else
  {
    return LoadStr(NO_STR);
  }
}
//---------------------------------------------------------------------------
UnicodeString DefaultStr(const UnicodeString & Str, const UnicodeString & Default)
{
  if (!Str.IsEmpty())
  {
    return Str;
  }
  else
  {
    return Default;
  }
}
//---------------------------------------------------------------------------
UnicodeString CutToChar(UnicodeString &Str, wchar_t Ch, bool Trim)
{
  Integer P = Str.Pos(Ch);
  UnicodeString Result;
  if (P)
  {
    Result = Str.SubString(1, P-1);
    Str.Delete(1, P);
  }
  else
  {
    Result = Str;
    Str = L"";
  }
  if (Trim)
  {
    Result = Result.TrimRight();
    Str = Str.TrimLeft();
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString CopyToChars(const UnicodeString & Str, int & From, UnicodeString Chs, bool Trim,
  wchar_t * Delimiter, bool DoubleDelimiterEscapes)
{
  UnicodeString Result;

  int P;
  for (P = From; P <= Str.Length(); P++)
  {
    if (IsDelimiter(Chs, Str, P))
    {
      if (DoubleDelimiterEscapes &&
          (P < Str.Length()) &&
          IsDelimiter(Chs, Str, P + 1))
      {
        Result += Str[P];
        P++;
      }
      else
      {
        break;
      }
    }
    else
    {
      Result += Str[P];
    }
  }

  if (P <= Str.Length())
  {
    if (Delimiter != NULL)
    {
      *Delimiter = Str[P];
    }
  }
  else
  {
    if (Delimiter != NULL)
    {
      *Delimiter = L'\0';
    }
  }
  // even if we reached the end, return index, as if there were the delimiter,
  // so caller can easily find index of the end of the piece by subtracting
  // 2 from From (as long as he did not asked for trimming)
  From = P+1;
  if (Trim)
  {
    Result = Result.TrimRight();
    while ((From <= Str.Length()) && (Str[From] == L' '))
    {
      From++;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString CopyToChar(const UnicodeString & Str, wchar_t Ch, bool Trim)
{
  int From = 1;
  return CopyToChars(Str, From, UnicodeString(Ch), Trim);
}
//---------------------------------------------------------------------------
UnicodeString DelimitStr(UnicodeString Str, UnicodeString Chars)
{
  for (int i = 1; i <= Str.Length(); i++)
  {
    if (Str.IsDelimiter(Chars, i))
    {
      Str.Insert(L"\\", i);
      i++;
    }
  }
  return Str;
}
//---------------------------------------------------------------------------
UnicodeString ShellDelimitStr(UnicodeString Str, wchar_t Quote)
{
  UnicodeString Chars = L"$\\";
  if (Quote == L'"')
  {
    Chars += L"`\"";
  }
  return DelimitStr(Str, Chars);
}
//---------------------------------------------------------------------------
UnicodeString ExceptionLogString(Exception *E)
{
  assert(E);
  if (E->InheritsFrom(__classid(Exception)))
  {
    UnicodeString Msg;
    Msg = FORMAT(L"(%s) %s", (E->ClassName(), E->Message));
    if (E->InheritsFrom(__classid(ExtException)))
    {
      TStrings * MoreMessages = ((ExtException*)E)->MoreMessages;
      if (MoreMessages)
      {
        Msg += L"\n" +
          ReplaceStr(MoreMessages->Text, L"\r", L"");
      }
    }
    return Msg;
  }
  else
  {
    wchar_t Buffer[1024];
    ExceptionErrorMessage(ExceptObject(), ExceptAddr(), Buffer, LENOF(Buffer));
    return UnicodeString(Buffer);
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall MainInstructions(const UnicodeString & S)
{
  UnicodeString MainMsgTag = LoadStr(MAIN_MSG_TAG);
  return MainMsgTag + S + MainMsgTag;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall MainInstructionsFirstParagraph(const UnicodeString & S)
{
  // WORKAROUND, we consider it bad practice, the highlighting should better
  // be localized (but maybe we change our mind later)
  UnicodeString Result;
  int Pos = S.Pos(L"\n\n");
  // we would not be calling this on single paragraph message
  if (ALWAYS_TRUE(Pos > 0))
  {
    Result =
      MainInstructions(S.SubString(1, Pos - 1)) +
      S.SubString(Pos, S.Length() - Pos + 1);
  }
  else
  {
    Result = MainInstructions(S);
  }
  return Result;
}
//---------------------------------------------------------------------------
bool ExtractMainInstructions(UnicodeString & S, UnicodeString & MainInstructions)
{
  bool Result = false;
  UnicodeString MainMsgTag = LoadStr(MAIN_MSG_TAG);
  if (StartsStr(MainMsgTag, S))
  {
    int EndTagPos =
      S.SubString(MainMsgTag.Length() + 1, S.Length() - MainMsgTag.Length()).Pos(MainMsgTag);
    if (EndTagPos > 0)
    {
      MainInstructions = S.SubString(MainMsgTag.Length() + 1, EndTagPos - 1);
      S.Delete(1, EndTagPos + (2 * MainMsgTag.Length()) - 1);
      Result = true;
    }
  }

  assert(MainInstructions.Pos(MainMsgTag) == 0);
  assert(S.Pos(MainMsgTag) == 0);

  return Result;
}
//---------------------------------------------------------------------------
static int FindInteractiveMsgStart(const UnicodeString & S)
{
  int Result = 0;
  UnicodeString InteractiveMsgTag = LoadStr(INTERACTIVE_MSG_TAG);
  if (EndsStr(InteractiveMsgTag, S) &&
      (S.Length() >= 2 * InteractiveMsgTag.Length()))
  {
    Result = S.Length() - 2 * InteractiveMsgTag.Length() + 1;
    while ((Result > 0) && (S.SubString(Result, InteractiveMsgTag.Length()) != InteractiveMsgTag))
    {
      Result--;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString RemoveMainInstructionsTag(UnicodeString S)
{
  UnicodeString MainInstruction;
  if (ExtractMainInstructions(S, MainInstruction))
  {
    S = MainInstruction + S;
  }
  return S;
}
//---------------------------------------------------------------------------
UnicodeString UnformatMessage(UnicodeString S)
{
  S = RemoveMainInstructionsTag(S);

  int InteractiveMsgStart = FindInteractiveMsgStart(S);
  if (InteractiveMsgStart > 0)
  {
    S = S.SubString(1, InteractiveMsgStart - 1);
  }
  return S;
}
//---------------------------------------------------------------------------
UnicodeString RemoveInteractiveMsgTag(UnicodeString S)
{
  int InteractiveMsgStart = FindInteractiveMsgStart(S);
  if (InteractiveMsgStart > 0)
  {
    UnicodeString InteractiveMsgTag = LoadStr(INTERACTIVE_MSG_TAG);
    S.Delete(InteractiveMsgStart, InteractiveMsgTag.Length());
    S.Delete(S.Length() - InteractiveMsgTag.Length() + 1, InteractiveMsgTag.Length());
  }
  return S;
}
//---------------------------------------------------------------------------
bool IsNumber(const UnicodeString Str)
{
  int Value;
  return TryStrToInt(Str, Value);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall SystemTemporaryDirectory()
{
  UnicodeString TempDir;
  TempDir.SetLength(MAX_PATH);
  TempDir.SetLength(GetTempPath(MAX_PATH, TempDir.c_str()));
  return TempDir;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall GetShellFolderPath(int CSIdl)
{
  UnicodeString Result;
  wchar_t Path[2 * MAX_PATH + 10] = L"\0";
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIdl, NULL, SHGFP_TYPE_CURRENT, Path)))
  {
    Result = Path;
  }
  return Result;
}
//---------------------------------------------------------------------------
// Particularly needed when using file name selected by TFilenameEdit,
// as it wraps a path to double-quotes, when there is a space in the path.
UnicodeString __fastcall StripPathQuotes(const UnicodeString Path)
{
  if ((Path.Length() >= 2) &&
      (Path[1] == L'\"') && (Path[Path.Length()] == L'\"'))
  {
    return Path.SubString(2, Path.Length() - 2);
  }
  else
  {
    return Path;
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall AddPathQuotes(UnicodeString Path)
{
  Path = StripPathQuotes(Path);
  if (Path.Pos(L" "))
  {
    Path = L"\"" + Path + L"\"";
  }
  return Path;
}
//---------------------------------------------------------------------------
static wchar_t * __fastcall ReplaceChar(
  UnicodeString & FileName, wchar_t * InvalidChar, wchar_t InvalidCharsReplacement)
{
  int Index = InvalidChar - FileName.c_str() + 1;
  if (InvalidCharsReplacement == TokenReplacement)
  {
    // currently we do not support unicode chars replacement
    if (FileName[Index] > 0xFF)
    {
      EXCEPTION;
    }

    FileName.Insert(ByteToHex(static_cast<unsigned char>(FileName[Index])), Index + 1);
    FileName[Index] = TokenPrefix;
    InvalidChar = FileName.c_str() + Index + 2;
  }
  else
  {
    FileName[Index] = InvalidCharsReplacement;
    InvalidChar = FileName.c_str() + Index;
  }
  return InvalidChar;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ValidLocalFileName(UnicodeString FileName)
{
  return ValidLocalFileName(FileName, L'_', L"", LocalInvalidChars);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ValidLocalFileName(
  UnicodeString FileName, wchar_t InvalidCharsReplacement,
  const UnicodeString & TokenizibleChars, const UnicodeString & LocalInvalidChars)
{
  if (InvalidCharsReplacement != NoReplacement)
  {
    bool ATokenReplacement = (InvalidCharsReplacement == TokenReplacement);
    const wchar_t * Chars =
      (ATokenReplacement ? TokenizibleChars : LocalInvalidChars).c_str();
    wchar_t * InvalidChar = FileName.c_str();
    while ((InvalidChar = wcspbrk(InvalidChar, Chars)) != NULL)
    {
      int Pos = (InvalidChar - FileName.c_str() + 1);
      wchar_t Char;
      if (ATokenReplacement &&
          (*InvalidChar == TokenPrefix) &&
          (((FileName.Length() - Pos) <= 1) ||
           (((Char = static_cast<wchar_t>(HexToByte(FileName.SubString(Pos + 1, 2)))) == L'\0') ||
            (TokenizibleChars.Pos(Char) == 0))))
      {
        InvalidChar++;
      }
      else
      {
        InvalidChar = ReplaceChar(FileName, InvalidChar, InvalidCharsReplacement);
      }
    }

    // Windows trim trailing space or dot, hence we must encode it to preserve it
    if (!FileName.IsEmpty() &&
        ((FileName[FileName.Length()] == L' ') ||
         (FileName[FileName.Length()] == L'.')))
    {
      ReplaceChar(FileName, FileName.c_str() + FileName.Length() - 1, InvalidCharsReplacement);
    }

    if (IsReservedName(FileName))
    {
      int P = FileName.Pos(".");
      if (P == 0)
      {
        P = FileName.Length() + 1;
      }
      FileName.Insert(L"%00", P);
    }
  }
  return FileName;
}
//---------------------------------------------------------------------------
void __fastcall SplitCommand(UnicodeString Command, UnicodeString &Program,
  UnicodeString & Params, UnicodeString & Dir)
{
  Command = Command.Trim();
  Params = L"";
  Dir = L"";
  if (!Command.IsEmpty() && (Command[1] == L'\"'))
  {
    Command.Delete(1, 1);
    int P = Command.Pos(L'"');
    if (P)
    {
      Program = Command.SubString(1, P-1).Trim();
      Params = Command.SubString(P + 1, Command.Length() - P).Trim();
    }
    else
    {
      throw Exception(FMTLOAD(INVALID_SHELL_COMMAND, (L"\"" + Command)));
    }
  }
  else
  {
    int P = Command.Pos(L" ");
    if (P)
    {
      Program = Command.SubString(1, P).Trim();
      Params = Command.SubString(P + 1, Command.Length() - P).Trim();
    }
    else
    {
      Program = Command;
    }
  }
  int B = Program.LastDelimiter(L"\\/");
  if (B)
  {
    Dir = Program.SubString(1, B).Trim();
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ExtractProgram(UnicodeString Command)
{
  UnicodeString Program;
  UnicodeString Params;
  UnicodeString Dir;

  SplitCommand(Command, Program, Params, Dir);

  return Program;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ExtractProgramName(UnicodeString Command)
{
  UnicodeString Name = ExtractFileName(ExtractProgram(Command));
  int Dot = Name.LastDelimiter(L".");
  if (Dot > 0)
  {
    Name = Name.SubString(1, Dot - 1);
  }
  return Name;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FormatCommand(UnicodeString Program, UnicodeString Params)
{
  Program = Program.Trim();
  Params = Params.Trim();
  if (!Params.IsEmpty()) Params = L" " + Params;
  if (Program.Pos(L" ")) Program = L"\"" + Program + L"\"";
  return Program + Params;
}
//---------------------------------------------------------------------------
const wchar_t ShellCommandFileNamePattern[] = L"!.!";
//---------------------------------------------------------------------------
void __fastcall ReformatFileNameCommand(UnicodeString & Command)
{
  if (!Command.IsEmpty())
  {
    UnicodeString Program, Params, Dir;
    SplitCommand(Command, Program, Params, Dir);
    if (Params.Pos(ShellCommandFileNamePattern) == 0)
    {
      Params = Params + (Params.IsEmpty() ? L"" : L" ") + ShellCommandFileNamePattern;
    }
    Command = FormatCommand(Program, Params);
  }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ExpandFileNameCommand(const UnicodeString Command,
  const UnicodeString FileName)
{
  return AnsiReplaceStr(Command, ShellCommandFileNamePattern,
    AddPathQuotes(FileName));
}
//---------------------------------------------------------------------------
UnicodeString __fastcall EscapePuttyCommandParam(UnicodeString Param)
{
  bool Space = false;

  for (int i = 1; i <= Param.Length(); i++)
  {
    switch (Param[i])
    {
      case L'"':
        Param.Insert(L"\\", i);
        i++;
        break;

      case L' ':
        Space = true;
        break;

      case L'\\':
        int i2 = i;
        while ((i2 <= Param.Length()) && (Param[i2] == L'\\'))
        {
          i2++;
        }
        if ((i2 <= Param.Length()) && (Param[i2] == L'"'))
        {
          while (Param[i] == L'\\')
          {
            Param.Insert(L"\\", i);
            i += 2;
          }
          i--;
        }
        break;
    }
  }

  if (Space)
  {
    Param = L"\"" + Param + L'"';
  }

  return Param;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ExpandEnvironmentVariables(const UnicodeString & Str)
{
  UnicodeString Buf;
  unsigned int Size = 1024;

  Buf.SetLength(Size);
  Buf.Unique();
  unsigned int Len = ExpandEnvironmentStrings(Str.c_str(), Buf.c_str(), Size);

  if (Len > Size)
  {
    Buf.SetLength(Len);
    Buf.Unique();
    ExpandEnvironmentStrings(Str.c_str(), Buf.c_str(), Len);
  }

  PackStr(Buf);

  return Buf;
}
//---------------------------------------------------------------------------
bool __fastcall CompareFileName(const UnicodeString & Path1, const UnicodeString & Path2)
{
  UnicodeString ShortPath1 = ExtractShortPathName(Path1);
  UnicodeString ShortPath2 = ExtractShortPathName(Path2);

  bool Result;
  // ExtractShortPathName returns empty string if file does not exist
  if (ShortPath1.IsEmpty() || ShortPath2.IsEmpty())
  {
    Result = AnsiSameText(Path1, Path2);
  }
  else
  {
    Result = AnsiSameText(ShortPath1, ShortPath2);
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall ComparePaths(const UnicodeString & Path1, const UnicodeString & Path2)
{
  // TODO: ExpandUNCFileName
  return AnsiSameText(IncludeTrailingBackslash(Path1), IncludeTrailingBackslash(Path2));
}
//---------------------------------------------------------------------------
int __fastcall CompareLogicalText(const UnicodeString & S1, const UnicodeString & S2)
{
  return StrCmpLogicalW(S1.c_str(), S2.c_str());
}
//---------------------------------------------------------------------------
bool __fastcall IsReservedName(UnicodeString FileName)
{
  int P = FileName.Pos(L".");
  int Len = (P > 0) ? P - 1 : FileName.Length();
  if ((Len == 3) || (Len == 4))
  {
    if (P > 0)
    {
      FileName.SetLength(P - 1);
    }
    static UnicodeString Reserved[] = {
      L"CON", L"PRN", L"AUX", L"NUL",
      L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9",
      L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6", L"LPT7", L"LPT8", L"LPT9" };
    for (unsigned int Index = 0; Index < LENOF(Reserved); Index++)
    {
      if (SameText(FileName, Reserved[Index]))
      {
        return true;
      }
    }
  }
  return false;
}
//---------------------------------------------------------------------------
// ApiPath support functions
// Inspired by
// http://stackoverflow.com/questions/18580945/need-clarification-for-converting-paths-into-long-unicode-paths-or-the-ones-star
// This can be reimplemented using PathCchCanonicalizeEx on Windows 8 and later
enum PATH_PREFIX_TYPE
{
  PPT_UNKNOWN,
  PPT_ABSOLUTE,           //Found absolute path that is none of the other types
  PPT_UNC,                //Found \\server\share\ prefix
  PPT_LONG_UNICODE,       //Found \\?\ prefix
  PPT_LONG_UNICODE_UNC,   //Found \\?\UNC\ prefix
};
//---------------------------------------------------------------------------
static int __fastcall PathRootLength(UnicodeString Path)
{
  // Correction for PathSkipRoot API

  // Replace all /'s with \'s because PathSkipRoot can't handle /'s
  UnicodeString Result = ReplaceChar(Path, L'/', L'\\');

  // Now call the API
  LPCTSTR Buffer = PathSkipRoot(Result.c_str());

  return (Buffer != NULL) ? (Buffer - Result.c_str()) : -1;
}
//---------------------------------------------------------------------------
static bool __fastcall PathIsRelative_CorrectedForMicrosoftStupidity(UnicodeString Path)
{
  // Correction for PathIsRelative API

  // Replace all /'s with \'s because PathIsRelative can't handle /'s
  UnicodeString Result = ReplaceChar(Path, L'/', L'\\');

  //Now call the API
  return PathIsRelative(Result.c_str());
}
//---------------------------------------------------------------------------
static int __fastcall GetOffsetAfterPathRoot(UnicodeString Path, PATH_PREFIX_TYPE & PrefixType)
{
  // Checks if 'pPath' begins with the drive, share, prefix, etc
  // EXAMPLES:
  //    Path                          Return:   Points at:                 PrefixType:
  //   Relative\Folder\File.txt        0         Relative\Folder\File.txt   PPT_UNKNOWN
  //   \RelativeToRoot\Folder          1         RelativeToRoot\Folder      PPT_ABSOLUTE
  //   C:\Windows\Folder               3         Windows\Folder             PPT_ABSOLUTE
  //   \\server\share\Desktop          15        Desktop                    PPT_UNC
  //   \\?\C:\Windows\Folder           7         Windows\Folder             PPT_LONG_UNICODE
  //   \\?\UNC\server\share\Desktop    21        Desktop                    PPT_LONG_UNICODE_UNC
  // RETURN:
  //      = Index in 'pPath' after the root, or
  //      = 0 if no root was found
  int Result = 0;

  PrefixType = PPT_UNKNOWN;

  if (!Path.IsEmpty())
  {
    int Len = Path.Length();

    bool WinXPOnly = !IsWinVista();

    // The PathSkipRoot() API doesn't work correctly on Windows XP
    if (!WinXPOnly)
    {
      // Works since Vista and up, but still needs correction :)
      int RootLength = PathRootLength(Path);
      if (RootLength >= 0)
      {
        Result = RootLength + 1;
      }
    }

    // Now determine the type of prefix
    int IndCheckUNC = -1;

    if ((Len >= 8) &&
        (Path[1] == L'\\' || Path[1] == L'/') &&
        (Path[2] == L'\\' || Path[2] == L'/') &&
        (Path[3] == L'?') &&
        (Path[4] == L'\\' || Path[4] == L'/') &&
        (Path[5] == L'U' || Path[5] == L'u') &&
        (Path[6] == L'N' || Path[6] == L'n') &&
        (Path[7] == L'C' || Path[7] == L'c') &&
        (Path[8] == L'\\' || Path[8] == L'/'))
    {
      // Found \\?\UNC\ prefix
      PrefixType = PPT_LONG_UNICODE_UNC;

      if (WinXPOnly)
      {
          //For older OS
          Result += 8;
      }

      //Check for UNC share later
      IndCheckUNC = 8;
    }
    else if ((Len >= 4) &&
        (Path[1] == L'\\' || Path[1] == L'/') &&
        (Path[2] == L'\\' || Path[2] == L'/') &&
        (Path[3] == L'?') &&
        (Path[4] == L'\\' || Path[4] == L'/'))
    {
      // Found \\?\ prefix
      PrefixType = PPT_LONG_UNICODE;

      if (WinXPOnly)
      {
          //For older OS
          Result += 4;
      }
    }
    else if ((Len >= 2) &&
        (Path[1] == L'\\' || Path[1] == L'/') &&
        (Path[2] == L'\\' || Path[2] == L'/'))
    {
      // Check for UNC share later
      IndCheckUNC = 2;
    }

    if (IndCheckUNC >= 0)
    {
      // Check for UNC, i.e. \\server\share\ part
      int Index = IndCheckUNC;
      for (int SkipSlashes = 2; SkipSlashes > 0; SkipSlashes--)
      {
        for(; Index <= Len; Index++)
        {
          TCHAR z = Path[Index];
          if ((z == L'\\') || (z == L'/') || (Index >= Len))
          {
            Index++;
            if (SkipSlashes == 1)
            {
              if (PrefixType == PPT_UNKNOWN)
              {
                PrefixType = PPT_UNC;
              }

              if (WinXPOnly)
              {
                  //For older OS
                  Result = Index;
              }
            }

            break;
          }
        }
      }
    }

    if (WinXPOnly)
    {
      // Only if we didn't determine any other type
      if (PrefixType == PPT_UNKNOWN)
      {
        if (!PathIsRelative_CorrectedForMicrosoftStupidity(Path.SubString(Result, Path.Length() - Result + 1)))
        {
          PrefixType = PPT_ABSOLUTE;
        }
      }

      // For older OS only
      int RootLength = PathRootLength(Path.SubString(Result, Path.Length() - Result + 1));
      if (RootLength >= 0)
      {
        Result = RootLength + 1;
      }
    }
    else
    {
      // Only if we didn't determine any other type
      if (PrefixType == PPT_UNKNOWN)
      {
        if (!PathIsRelative_CorrectedForMicrosoftStupidity(Path))
        {
          PrefixType = PPT_ABSOLUTE;
        }
      }
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall MakeUnicodeLargePath(UnicodeString Path)
{
  // Convert path from 'into a larger Unicode path, that allows up to 32,767 character length
  UnicodeString Result;

  if (!Path.IsEmpty())
  {
      // Determine the type of the existing prefix
      PATH_PREFIX_TYPE PrefixType;
      GetOffsetAfterPathRoot(Path, PrefixType);

      // Assume path to be without change
      Result = Path;

      switch (PrefixType)
      {
        case PPT_ABSOLUTE:
          {
            // First we need to check if its an absolute path relative to the root
            bool AddPrefix = true;
            if ((Path.Length() >= 1) &&
                ((Path[1] == L'\\') || (Path[1] == L'/')))
            {
              AddPrefix = FALSE;

              // Get current root path
              UnicodeString CurrentDir = GetCurrentDir();
              PATH_PREFIX_TYPE PrefixType2; // unused
              int Following = GetOffsetAfterPathRoot(CurrentDir, PrefixType2);
              if (Following > 0)
              {
                AddPrefix = true;
                Result = CurrentDir.SubString(1, Following - 1) + Result.SubString(2, Result.Length() - 1);
              }
            }

            if (AddPrefix)
            {
              // Add \\?\ prefix
              Result = L"\\\\?\\" + Result;
            }
          }
          break;

      case PPT_UNC:
        // First we need to remove the opening slashes for UNC share
        if ((Result.Length() >= 2) &&
            ((Result[1] == L'\\') || (Result[1] == L'/')) &&
            ((Result[2] == L'\\') || (Result[2] == L'/')))
        {
          Result = Result.SubString(3, Result.Length() - 2);
        }

        // Add \\?\UNC\ prefix
        Result = L"\\\\?\\UNC\\" + Result;
        break;
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ApiPath(UnicodeString Path)
{
  if (Path.Length() >= MAX_PATH)
  {
    if (Configuration != NULL)
    {
      Configuration->Usage->Inc(L"LongPath");
    }
    Path = MakeUnicodeLargePath(Path);
  }
  return Path;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall DisplayableStr(const RawByteString & Str)
{
  bool Displayable = true;
  int Index = 1;
  while ((Index <= Str.Length()) && Displayable)
  {
    if (((Str[Index] < '\x20') || (static_cast<unsigned char>(Str[Index]) >= static_cast<unsigned char>('\x80'))) &&
        (Str[Index] != '\n') && (Str[Index] != '\r') && (Str[Index] != '\t') && (Str[Index] != '\b'))
    {
      Displayable = false;
    }
    Index++;
  }

  UnicodeString Result;
  if (Displayable)
  {
    Result = L"\"";
    for (int Index = 1; Index <= Str.Length(); Index++)
    {
      switch (Str[Index])
      {
        case '\n':
          Result += L"\\n";
          break;

        case '\r':
          Result += L"\\r";
          break;

        case '\t':
          Result += L"\\t";
          break;

        case '\b':
          Result += L"\\b";
          break;

        case '\\':
          Result += L"\\\\";
          break;

        case '"':
          Result += L"\\\"";
          break;

        default:
          Result += wchar_t(Str[Index]);
          break;
      }
    }
    Result += L"\"";
  }
  else
  {
    Result = L"0x" + BytesToHex(Str);
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ByteToHex(unsigned char B, bool UpperCase)
{
  static wchar_t UpperDigits[] = L"0123456789ABCDEF";
  static wchar_t LowerDigits[] = L"0123456789abcdef";

  const wchar_t * Digits = (UpperCase ? UpperDigits : LowerDigits);
  UnicodeString Result;
  Result.SetLength(2);
  Result[1] = Digits[(B & 0xF0) >> 4];
  Result[2] = Digits[(B & 0x0F) >> 0];
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall BytesToHex(const unsigned char * B, size_t Length, bool UpperCase, wchar_t Separator)
{
  UnicodeString Result;
  for (size_t i = 0; i < Length; i++)
  {
    Result += ByteToHex(B[i], UpperCase);
    if ((Separator != L'\0') && (i < Length - 1))
    {
      Result += Separator;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall BytesToHex(RawByteString Str, bool UpperCase, wchar_t Separator)
{
  return BytesToHex(reinterpret_cast<const unsigned char *>(Str.c_str()), Str.Length(), UpperCase, Separator);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall CharToHex(wchar_t Ch, bool UpperCase)
{
  return BytesToHex(reinterpret_cast<const unsigned char *>(&Ch), sizeof(Ch), UpperCase);
}
//---------------------------------------------------------------------------
RawByteString __fastcall HexToBytes(const UnicodeString Hex)
{
  static UnicodeString Digits = L"0123456789ABCDEF";
  RawByteString Result;
  int L, P1, P2;
  L = Hex.Length();
  if (L % 2 == 0)
  {
    for (int i = 1; i <= Hex.Length(); i += 2)
    {
      P1 = Digits.Pos((wchar_t)toupper(Hex[i]));
      P2 = Digits.Pos((wchar_t)toupper(Hex[i + 1]));
      if (P1 <= 0 || P2 <= 0)
      {
        Result = L"";
        break;
      }
      else
      {
        Result += static_cast<char>((P1 - 1) * 16 + P2 - 1);
      }
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
unsigned char __fastcall HexToByte(const UnicodeString Hex)
{
  static UnicodeString Digits = L"0123456789ABCDEF";
  assert(Hex.Length() == 2);
  int P1 = Digits.Pos((wchar_t)toupper(Hex[1]));
  int P2 = Digits.Pos((wchar_t)toupper(Hex[2]));

  return
    static_cast<unsigned char>(((P1 <= 0) || (P2 <= 0)) ? 0 : (((P1 - 1) << 4) + (P2 - 1)));
}
//---------------------------------------------------------------------------
bool __fastcall IsLowerCaseLetter(wchar_t Ch)
{
  return (Ch >= 'a') && (Ch <= 'z');
}
//---------------------------------------------------------------------------
bool __fastcall IsUpperCaseLetter(wchar_t Ch)
{
  return (Ch >= 'A') && (Ch <= 'Z');
}
//---------------------------------------------------------------------------
bool __fastcall IsLetter(wchar_t Ch)
{
  return IsLowerCaseLetter(Ch) || IsUpperCaseLetter(Ch);
}
//---------------------------------------------------------------------------
bool __fastcall IsDigit(wchar_t Ch)
{
  return (Ch >= '0') && (Ch <= '9');
}
//---------------------------------------------------------------------------
bool __fastcall IsHex(wchar_t Ch)
{
  return
    IsDigit(Ch) ||
    ((Ch >= 'A') && (Ch <= 'F')) ||
    ((Ch >= 'a') && (Ch <= 'f'));
}
//---------------------------------------------------------------------------
int __fastcall FindCheck(int Result, const UnicodeString & Path)
{
  if ((Result != ERROR_SUCCESS) &&
      (Result != ERROR_FILE_NOT_FOUND) &&
      (Result != ERROR_NO_MORE_FILES))
  {
    throw EOSExtException(FMTLOAD(FIND_FILE_ERROR, (Path)), Result);
  }
  return Result;
}
//---------------------------------------------------------------------------
int __fastcall FindFirstUnchecked(const UnicodeString & Path, int Attr, TSearchRecChecked & F)
{
  F.Path = Path;
  return FindFirst(ApiPath(Path), Attr, F);
}
//---------------------------------------------------------------------------
int __fastcall FindFirstChecked(const UnicodeString & Path, int Attr, TSearchRecChecked & F)
{
  int Result = FindFirstUnchecked(Path, Attr, F);
  return FindCheck(Result, F.Path);
}
//---------------------------------------------------------------------------
// It can make sense to use FindNextChecked, even if unchecked FindFirst is used.
// I.e. even if we do not care that FindFirst failed, if FindNext
// failes after successfull FindFirst, it mean some terrible problem
int __fastcall FindNextChecked(TSearchRecChecked & F)
{
  return FindCheck(FindNext(F), F.Path);
}
//---------------------------------------------------------------------------
bool __fastcall FileSearchRec(const UnicodeString FileName, TSearchRec & Rec)
{
  int FindAttrs = faReadOnly | faHidden | faSysFile | faDirectory | faArchive;
  bool Result = (FindFirst(ApiPath(FileName), FindAttrs, Rec) == 0);
  if (Result)
  {
    FindClose(Rec);
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall ProcessLocalDirectory(UnicodeString DirName,
  TProcessLocalFileEvent CallBackFunc, void * Param,
  int FindAttrs)
{
  assert(CallBackFunc);
  if (FindAttrs < 0)
  {
    FindAttrs = faReadOnly | faHidden | faSysFile | faDirectory | faArchive;
  }
  TSearchRecChecked SearchRec;

  DirName = IncludeTrailingBackslash(DirName);
  if (FindFirstChecked(DirName + L"*.*", FindAttrs, SearchRec) == 0)
  {
    try
    {
      do
      {
        if ((SearchRec.Name != L".") && (SearchRec.Name != L".."))
        {
          CallBackFunc(DirName + SearchRec.Name, SearchRec, Param);
        }

      } while (FindNextChecked(SearchRec) == 0);
    }
    __finally
    {
      FindClose(SearchRec);
    }
  }
}
//---------------------------------------------------------------------------
TDateTime __fastcall EncodeDateVerbose(Word Year, Word Month, Word Day)
{
  try
  {
    TDateTime DateTime = EncodeDate(Year, Month, Day);
    return DateTime;
  }
  catch (EConvertError & E)
  {
    throw EConvertError(FORMAT(L"%s [%d-%d-%d]", (E.Message, int(Year), int(Month), int(Day))));
  }
}
//---------------------------------------------------------------------------
TDateTime __fastcall EncodeTimeVerbose(Word Hour, Word Min, Word Sec, Word MSec)
{
  try
  {
    TDateTime DateTime = EncodeTime(Hour, Min, Sec, MSec);
    return DateTime;
  }
  catch (EConvertError & E)
  {
    throw EConvertError(FORMAT(L"%s [%d:%d:%d.%d]", (E.Message, int(Hour), int(Min), int(Sec), int(MSec))));
  }
}
//---------------------------------------------------------------------------
TDateTime __fastcall SystemTimeToDateTimeVerbose(const SYSTEMTIME & SystemTime)
{
  try
  {
    TDateTime DateTime = SystemTimeToDateTime(SystemTime);
    return DateTime;
  }
  catch (EConvertError & E)
  {
    throw EConvertError(FORMAT(L"%s [%d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d]", (E.Message, int(SystemTime.wYear), int(SystemTime.wMonth), int(SystemTime.wDay), int(SystemTime.wHour), int(SystemTime.wMinute), int(SystemTime.wSecond), int(SystemTime.wMilliseconds))));
  }
}
//---------------------------------------------------------------------------
struct TDateTimeParams
{
  TDateTime UnixEpoch;
  double BaseDifference;
  long BaseDifferenceSec;
  // All Current* are actually global, not per-year and
  // are valid for Year 0 (current) only
  double CurrentDaylightDifference;
  long CurrentDaylightDifferenceSec;
  double CurrentDifference;
  long CurrentDifferenceSec;
  double StandardDifference;
  long StandardDifferenceSec;
  double DaylightDifference;
  long DaylightDifferenceSec;
  SYSTEMTIME SystemStandardDate;
  SYSTEMTIME SystemDaylightDate;
  TDateTime StandardDate;
  TDateTime DaylightDate;
  UnicodeString StandardName;
  UnicodeString DaylightName;
  // This is actually global, not per-year
  bool DaylightHack;

  bool HasDST() const
  {
    // On some systems it occurs that StandardDate is unset, while
    // DaylightDate is set. MSDN states that this is invalid and
    // should be treated as if there is no daylight saving.
    // So check both.
    return
      (SystemStandardDate.wMonth != 0) &&
      (SystemDaylightDate.wMonth != 0);
  }

  bool SummerDST() const
  {
    return HasDST() && (DaylightDate < StandardDate);
  }
};
typedef std::map<int, TDateTimeParams> TYearlyDateTimeParams;
static TYearlyDateTimeParams YearlyDateTimeParams;
static std::unique_ptr<TCriticalSection> DateTimeParamsSection(new TCriticalSection());
static void __fastcall EncodeDSTMargin(const SYSTEMTIME & Date, unsigned short Year,
  TDateTime & Result);
//---------------------------------------------------------------------------
static unsigned short __fastcall DecodeYear(const TDateTime & DateTime)
{
  unsigned short Year, Month, Day;
  DecodeDate(DateTime, Year, Month, Day);
  return Year;
}
//---------------------------------------------------------------------------
static const TDateTimeParams * __fastcall GetDateTimeParams(unsigned short Year)
{
  TGuard Guard(DateTimeParamsSection.get());

  TDateTimeParams * Result;

  TYearlyDateTimeParams::iterator i = YearlyDateTimeParams.find(Year);
  if (i != YearlyDateTimeParams.end())
  {
    Result = &(*i).second;
  }
  else
  {
    // creates new entry as a side effect
    Result = &YearlyDateTimeParams[Year];
    TIME_ZONE_INFORMATION TZI;

    unsigned long GTZI;

    HINSTANCE Kernel32 = GetModuleHandle(kernel32);
    typedef BOOL WINAPI (* TGetTimeZoneInformationForYear)(USHORT wYear, PDYNAMIC_TIME_ZONE_INFORMATION pdtzi, LPTIME_ZONE_INFORMATION ptzi);
    TGetTimeZoneInformationForYear GetTimeZoneInformationForYear =
      (TGetTimeZoneInformationForYear)GetProcAddress(Kernel32, "GetTimeZoneInformationForYear");

    if ((Year == 0) || (GetTimeZoneInformationForYear == NULL))
    {
      GTZI = GetTimeZoneInformation(&TZI);
    }
    else
    {
      GetTimeZoneInformationForYear(Year, NULL, &TZI);
      GTZI = TIME_ZONE_ID_UNKNOWN;
    }

    switch (GTZI)
    {
      case TIME_ZONE_ID_UNKNOWN:
        Result->CurrentDaylightDifferenceSec = 0;
        break;

      case TIME_ZONE_ID_STANDARD:
        Result->CurrentDaylightDifferenceSec = TZI.StandardBias;
        break;

      case TIME_ZONE_ID_DAYLIGHT:
        Result->CurrentDaylightDifferenceSec = TZI.DaylightBias;
        break;

      case TIME_ZONE_ID_INVALID:
      default:
        throw Exception(TIMEZONE_ERROR);
    }

    Result->BaseDifferenceSec = TZI.Bias;
    Result->BaseDifference = double(TZI.Bias) / MinsPerDay;
    Result->BaseDifferenceSec *= SecsPerMin;

    Result->CurrentDifferenceSec = TZI.Bias +
      Result->CurrentDaylightDifferenceSec;
    Result->CurrentDifference =
      double(Result->CurrentDifferenceSec) / MinsPerDay;
    Result->CurrentDifferenceSec *= SecsPerMin;

    Result->CurrentDaylightDifference =
      double(Result->CurrentDaylightDifferenceSec) / MinsPerDay;
    Result->CurrentDaylightDifferenceSec *= SecsPerMin;

    Result->DaylightDifferenceSec = TZI.DaylightBias * SecsPerMin;
    Result->DaylightDifference = double(TZI.DaylightBias) / MinsPerDay;
    Result->StandardDifferenceSec = TZI.StandardBias * SecsPerMin;
    Result->StandardDifference = double(TZI.StandardBias) / MinsPerDay;

    Result->SystemStandardDate = TZI.StandardDate;
    Result->SystemDaylightDate = TZI.DaylightDate;

    unsigned short AYear = (Year != 0) ? Year : DecodeYear(Now());
    if (Result->HasDST())
    {
      EncodeDSTMargin(Result->SystemStandardDate, AYear, Result->StandardDate);
      EncodeDSTMargin(Result->SystemDaylightDate, AYear, Result->DaylightDate);
    }

    Result->StandardName = TZI.StandardName;
    Result->DaylightName = TZI.DaylightName;

    Result->DaylightHack = !IsWin7();
  }

  return Result;
}
//---------------------------------------------------------------------------
static void __fastcall EncodeDSTMargin(const SYSTEMTIME & Date, unsigned short Year,
  TDateTime & Result)
{
  if (Date.wYear == 0)
  {
    TDateTime Temp = EncodeDateVerbose(Year, Date.wMonth, 1);
    Result = Temp + ((Date.wDayOfWeek - DayOfWeek(Temp) + 8) % 7) +
      (7 * (Date.wDay - 1));
    // Day 5 means, the last occurence of day-of-week in month
    if (Date.wDay == 5)
    {
      unsigned short Month = static_cast<unsigned short>(Date.wMonth + 1);
      if (Month > 12)
      {
        Month = static_cast<unsigned short>(Month - 12);
        Year++;
      }

      if (Result >= EncodeDateVerbose(Year, Month, 1))
      {
        Result -= 7;
      }
    }
    Result += EncodeTimeVerbose(Date.wHour, Date.wMinute, Date.wSecond,
      Date.wMilliseconds);
  }
  else
  {
    Result = EncodeDateVerbose(Year, Date.wMonth, Date.wDay) +
      EncodeTimeVerbose(Date.wHour, Date.wMinute, Date.wSecond, Date.wMilliseconds);
  }
}
//---------------------------------------------------------------------------
static bool __fastcall IsDateInDST(const TDateTime & DateTime)
{

  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));

  bool Result;

  // On some systems it occurs that StandardDate is unset, while
  // DaylightDate is set. MSDN states that this is invalid and
  // should be treated as if there is no daylight saving.
  // So check both.
  if (!Params->HasDST())
  {
    Result = false;
  }
  else
  {

    if (Params->SummerDST())
    {
      Result =
        (DateTime >= Params->DaylightDate) &&
        (DateTime < Params->StandardDate);
    }
    else
    {
      Result =
        (DateTime < Params->StandardDate) ||
        (DateTime >= Params->DaylightDate);
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall UsesDaylightHack()
{
  return GetDateTimeParams(0)->DaylightHack;
}
//---------------------------------------------------------------------------
TDateTime __fastcall UnixToDateTime(__int64 TimeStamp, TDSTMode DSTMode)
{
  assert(int(EncodeDateVerbose(1970, 1, 1)) == UnixDateDelta);

  TDateTime Result = UnixDateDelta + (double(TimeStamp) / SecsPerDay);

  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(Result));

  if (Params->DaylightHack)
  {
    if ((DSTMode == dstmWin) || (DSTMode == dstmUnix))
    {
      const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
      Result -= CurrentParams->CurrentDifference;
    }
    else if (DSTMode == dstmKeep)
    {
      Result -= Params->BaseDifference;
    }
  }
  else
  {
    Result -= Params->BaseDifference;
  }

  if ((DSTMode == dstmUnix) || (DSTMode == dstmKeep))
  {
    Result -= (IsDateInDST(Result) ?
      Params->DaylightDifference : Params->StandardDifference);
  }

  return Result;
}
//---------------------------------------------------------------------------
__int64 __fastcall Round(double Number)
{
  double Floor = floor(Number);
  double Ceil = ceil(Number);
  return static_cast<__int64>(((Number - Floor) > (Ceil - Number)) ? Ceil : Floor);
}
//---------------------------------------------------------------------------
bool __fastcall TryRelativeStrToDateTime(UnicodeString S, TDateTime & DateTime)
{
  S = S.Trim();
  int Index = 1;
  while ((Index <= S.Length()) && IsDigit(S[Index]))
  {
    Index++;
  }
  UnicodeString NumberStr = S.SubString(1, Index - 1);
  int Number;
  bool Result = TryStrToInt(NumberStr, Number);
  if (Result)
  {
    S.Delete(1, Index - 1);
    S = S.Trim().UpperCase();
    DateTime = Now();
    // These may not overlap with ParseSize (K, M and G)
    if (S == "S")
    {
      DateTime = IncSecond(DateTime, -Number);
    }
    else if (S == "N")
    {
      DateTime = IncMinute(DateTime, -Number);
    }
    else if (S == "H")
    {
      DateTime = IncHour(DateTime, -Number);
    }
    else if (S == "D")
    {
      DateTime = IncDay(DateTime, -Number);
    }
    else if (S == "Y")
    {
      DateTime = IncYear(DateTime, -Number);
    }
    else
    {
      Result = false;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
static __int64 __fastcall DateTimeToUnix(const TDateTime DateTime)
{
  const TDateTimeParams * CurrentParams = GetDateTimeParams(0);

  assert(int(EncodeDateVerbose(1970, 1, 1)) == UnixDateDelta);

  return Round(double(DateTime - UnixDateDelta) * SecsPerDay) +
    CurrentParams->CurrentDifferenceSec;
}
//---------------------------------------------------------------------------
FILETIME __fastcall DateTimeToFileTime(const TDateTime DateTime,
  TDSTMode /*DSTMode*/)
{
  __int64 UnixTimeStamp = ::DateTimeToUnix(DateTime);

  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));
  if (!Params->DaylightHack)
  {
    // We should probably use reversed code of FileTimeToDateTime here instead of custom implementation

    // We are incrementing and decrementing BaseDifferenceSec because it
    // can actually change between years
    // (as it did in Belarus from GMT+2 to GMT+3 between 2011 and 2012)

    UnixTimeStamp += (IsDateInDST(DateTime) ?
      Params->DaylightDifferenceSec : Params->StandardDifferenceSec) +
      Params->BaseDifferenceSec;

    const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
    UnixTimeStamp -=
      CurrentParams->CurrentDaylightDifferenceSec +
      CurrentParams->BaseDifferenceSec;

  }

  FILETIME Result;
  (*(__int64*)&(Result) = (__int64(UnixTimeStamp) + 11644473600LL) * 10000000LL);

  return Result;
}
//---------------------------------------------------------------------------
TDateTime __fastcall FileTimeToDateTime(const FILETIME & FileTime)
{
  // duplicated in DirView.pas
  TDateTime Result;
  // The 0xFFF... is sometime seen for invalid timestamps,
  // it would cause failure in SystemTimeToDateTime below
  if (FileTime.dwLowDateTime == std::numeric_limits<DWORD>::max())
  {
    Result = MinDateTime;
  }
  else
  {
    SYSTEMTIME SysTime;
    if (!UsesDaylightHack())
    {
      SYSTEMTIME UniverzalSysTime;
      FileTimeToSystemTime(&FileTime, &UniverzalSysTime);
      SystemTimeToTzSpecificLocalTime(NULL, &UniverzalSysTime, &SysTime);
    }
    else
    {
      FILETIME LocalFileTime;
      FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTime);
    }
    Result = SystemTimeToDateTimeVerbose(SysTime);
  }
  return Result;
}
//---------------------------------------------------------------------------
__int64 __fastcall ConvertTimestampToUnix(const FILETIME & FileTime,
  TDSTMode DSTMode)
{
  __int64 Result = ((*(__int64*)&(FileTime)) / 10000000LL - 11644473600LL);

  if (UsesDaylightHack())
  {
    if ((DSTMode == dstmUnix) || (DSTMode == dstmKeep))
    {
      FILETIME LocalFileTime;
      SYSTEMTIME SystemTime;
      FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SystemTime);
      TDateTime DateTime = SystemTimeToDateTimeVerbose(SystemTime);
      const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));
      Result += (IsDateInDST(DateTime) ?
        Params->DaylightDifferenceSec : Params->StandardDifferenceSec);

      if (DSTMode == dstmKeep)
      {
        const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
        Result -= CurrentParams->CurrentDaylightDifferenceSec;
      }
    }
  }
  else
  {
    if (DSTMode == dstmWin)
    {
      FILETIME LocalFileTime;
      SYSTEMTIME SystemTime;
      FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SystemTime);
      TDateTime DateTime = SystemTimeToDateTimeVerbose(SystemTime);
      const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));
      Result -= (IsDateInDST(DateTime) ?
        Params->DaylightDifferenceSec : Params->StandardDifferenceSec);
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
TDateTime __fastcall ConvertTimestampToUTC(TDateTime DateTime)
{

  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));
  DateTime +=
    (IsDateInDST(DateTime) ?
      Params->DaylightDifference : Params->StandardDifference);
  DateTime += Params->BaseDifference;

  if (Params->DaylightHack)
  {
    const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
    DateTime += CurrentParams->CurrentDaylightDifference;
  }

  return DateTime;
}
//---------------------------------------------------------------------------
TDateTime __fastcall ConvertTimestampFromUTC(TDateTime DateTime)
{

  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));
  DateTime -=
    (IsDateInDST(DateTime) ?
      Params->DaylightDifference : Params->StandardDifference);
  DateTime -= Params->BaseDifference;

  if (Params->DaylightHack)
  {
    const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
    DateTime -= CurrentParams->CurrentDaylightDifference;
  }

  return DateTime;
}
//---------------------------------------------------------------------------
__int64 __fastcall ConvertTimestampToUnixSafe(const FILETIME & FileTime,
  TDSTMode DSTMode)
{
  __int64 Result;
  if ((FileTime.dwLowDateTime == 0) &&
      (FileTime.dwHighDateTime == 0))
  {
    Result = ::DateTimeToUnix(Now());
  }
  else
  {
    Result = ConvertTimestampToUnix(FileTime, DSTMode);
  }
  return Result;
}
//---------------------------------------------------------------------------
TDateTime __fastcall AdjustDateTimeFromUnix(TDateTime DateTime, TDSTMode DSTMode)
{
  const TDateTimeParams * Params = GetDateTimeParams(DecodeYear(DateTime));

  if (Params->DaylightHack)
  {
    if ((DSTMode == dstmWin) || (DSTMode == dstmUnix))
    {
      const TDateTimeParams * CurrentParams = GetDateTimeParams(0);
      DateTime = DateTime - CurrentParams->CurrentDaylightDifference;
    }

    if (!IsDateInDST(DateTime))
    {
      if (DSTMode == dstmWin)
      {
        DateTime = DateTime - Params->DaylightDifference;
      }
    }
    else
    {
      DateTime = DateTime - Params->StandardDifference;
    }
  }
  else
  {
    if (DSTMode == dstmWin)
    {
      if (IsDateInDST(DateTime))
      {
        DateTime = DateTime + Params->DaylightDifference;
      }
      else
      {
        DateTime = DateTime + Params->StandardDifference;
      }
    }
  }

  return DateTime;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FixedLenDateTimeFormat(const UnicodeString & Format)
{
  UnicodeString Result = Format;
  bool AsIs = false;

  int Index = 1;
  while (Index <= Result.Length())
  {
    wchar_t F = Result[Index];
    if ((F == L'\'') || (F == L'\"'))
    {
      AsIs = !AsIs;
      Index++;
    }
    else if (!AsIs && ((F == L'a') || (F == L'A')))
    {
      if (Result.SubString(Index, 5).LowerCase() == L"am/pm")
      {
        Index += 5;
      }
      else if (Result.SubString(Index, 3).LowerCase() == L"a/p")
      {
        Index += 3;
      }
      else if (Result.SubString(Index, 4).LowerCase() == L"ampm")
      {
        Index += 4;
      }
      else
      {
        Index++;
      }
    }
    else
    {
      if (!AsIs && (wcschr(L"dDeEmMhHnNsS", F) != NULL) &&
          ((Index == Result.Length()) || (Result[Index + 1] != F)))
      {
        Result.Insert(F, Index);
      }

      while ((Index <= Result.Length()) && (F == Result[Index]))
      {
        Index++;
      }
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FormatTimeZone(long Sec)
{
  TTimeSpan Span = TTimeSpan::FromSeconds(Sec);
  UnicodeString Str;
  if ((Span.Seconds == 0) && (Span.Minutes == 0))
  {
    Str = FORMAT(L"%d", (-Span.Hours));
  }
  else if (Span.Seconds == 0)
  {
    Str = FORMAT(L"%d:%2.2d", (-Span.Hours, abs(Span.Minutes)));
  }
  else
  {
    Str = FORMAT(L"%d:%2.2d:%2.2d", (-Span.Hours, abs(Span.Minutes), abs(Span.Seconds)));
  }
  Str = ((Span <= TTimeSpan::Zero) ? L"+" : L"") + Str;
  return Str;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall GetTimeZoneLogString()
{
  const TDateTimeParams * CurrentParams = GetDateTimeParams(0);

  UnicodeString Result =
    FORMAT(L"Current: GMT%s", (FormatTimeZone(CurrentParams->CurrentDifferenceSec)));

  if (!CurrentParams->HasDST())
  {
    Result += FORMAT(L" (%s), No DST", (CurrentParams->StandardName));
  }
  else
  {
    Result +=
      FORMAT(L", Standard: GMT%s (%s), DST: GMT%s (%s), DST Start: %s, DST End: %s",
        (FormatTimeZone(CurrentParams->BaseDifferenceSec + CurrentParams->StandardDifferenceSec),
         CurrentParams->StandardName,
         FormatTimeZone(CurrentParams->BaseDifferenceSec + CurrentParams->DaylightDifferenceSec),
         CurrentParams->DaylightName,
         CurrentParams->DaylightDate.DateString(),
         CurrentParams->StandardDate.DateString()));
  }

  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall AdjustClockForDSTEnabled()
{
  // Windows XP deletes the DisableAutoDaylightTimeSet value when it is off
  // (the later versions set it to DynamicDaylightTimeDisabled to 0)
  bool DynamicDaylightTimeDisabled = false;
  TRegistry * Registry = new TRegistry(KEY_READ);
  try
  {
    Registry->RootKey = HKEY_LOCAL_MACHINE;
    if (Registry->OpenKey(L"SYSTEM", false) &&
        Registry->OpenKey(L"CurrentControlSet", false) &&
        Registry->OpenKey(L"Control", false) &&
        Registry->OpenKey(L"TimeZoneInformation", false))
    {
      if (Registry->ValueExists(L"DynamicDaylightTimeDisabled"))
      {
        DynamicDaylightTimeDisabled = Registry->ReadBool(L"DynamicDaylightTimeDisabled");
      }
      // WORKAROUND
      // Windows XP equivalent
      else if (Registry->ValueExists(L"DisableAutoDaylightTimeSet"))
      {
        DynamicDaylightTimeDisabled = Registry->ReadBool(L"DisableAutoDaylightTimeSet");
      }
    }
    delete Registry;
  }
  catch(...)
  {
  }
  return !DynamicDaylightTimeDisabled;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall StandardDatestamp()
{
  return FormatDateTime(L"yyyy'-'mm'-'dd", ConvertTimestampToUTC(Now()));
}
//---------------------------------------------------------------------------
UnicodeString __fastcall StandardTimestamp(const TDateTime & DateTime)
{
  return FormatDateTime(L"yyyy'-'mm'-'dd'T'hh':'nn':'ss'.'zzz'Z'", ConvertTimestampToUTC(DateTime));
}
//---------------------------------------------------------------------------
UnicodeString __fastcall StandardTimestamp()
{
  return StandardTimestamp(Now());
}
//---------------------------------------------------------------------------
static TDateTime TwoSeconds(0, 0, 2, 0);
int __fastcall CompareFileTime(TDateTime T1, TDateTime T2)
{
  // "FAT" time precision
  // (when one time is seconds-precision and other is millisecond-precision,
  // we may have times like 12:00:00.000 and 12:00:01.999, which should
  // be treated the same)
  int Result;
  if (T1 == T2)
  {
    // just optimization
    Result = 0;
  }
  else if ((T1 < T2) && (T2 - T1 >= TwoSeconds))
  {
    Result = -1;
  }
  else if ((T1 > T2) && (T1 - T2 >= TwoSeconds))
  {
    Result = 1;
  }
  else
  {
    Result = 0;
  }
  return Result;
}
//---------------------------------------------------------------------------
int __fastcall TimeToMSec(TDateTime T)
{
  return int(Round(double(T) * double(MSecsPerDay)));
}
//---------------------------------------------------------------------------
int __fastcall TimeToSeconds(TDateTime T)
{
  return TimeToMSec(T) / MSecsPerSec;
}
//---------------------------------------------------------------------------
int __fastcall TimeToMinutes(TDateTime T)
{
  return TimeToSeconds(T) / SecsPerMin;
}
//---------------------------------------------------------------------------
bool __fastcall RecursiveDeleteFile(const UnicodeString FileName, bool ToRecycleBin)
{
  SHFILEOPSTRUCT Data;

  memset(&Data, 0, sizeof(Data));
  Data.hwnd = NULL;
  Data.wFunc = FO_DELETE;
  // SHFileOperation does not support long paths anyway
  UnicodeString FileList(ApiPath(FileName));
  FileList.SetLength(FileList.Length() + 2);
  FileList[FileList.Length() - 1] = L'\0';
  FileList[FileList.Length()] = L'\0';
  Data.pFrom = FileList.c_str();
  Data.pTo = L"\0\0"; // this will actually give one null more than needed
  Data.fFlags = FOF_NOCONFIRMATION | FOF_RENAMEONCOLLISION | FOF_NOCONFIRMMKDIR |
    FOF_NOERRORUI | FOF_SILENT;
  if (ToRecycleBin)
  {
    Data.fFlags |= FOF_ALLOWUNDO;
  }
  int ErrorCode = SHFileOperation(&Data);
  bool Result = (ErrorCode == 0);
  if (!Result)
  {
    // according to MSDN, SHFileOperation may return following non-Win32
    // error codes
    if (((ErrorCode >= 0x71) && (ErrorCode <= 0x88)) ||
        (ErrorCode == 0xB7) || (ErrorCode == 0x402) || (ErrorCode == 0x10000) ||
        (ErrorCode == 0x10074))
    {
      ErrorCode = 0;
    }
    SetLastError(ErrorCode);
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall DeleteFileChecked(const UnicodeString & FileName)
{
  if (!DeleteFile(ApiPath(FileName)))
  {
    throw EOSExtException(FMTLOAD(DELETE_LOCAL_FILE_ERROR, (FileName)));
  }
}
//---------------------------------------------------------------------------
unsigned int __fastcall CancelAnswer(unsigned int Answers)
{
  unsigned int Result;
  if ((Answers & qaCancel) != 0)
  {
    Result = qaCancel;
  }
  else if ((Answers & qaNo) != 0)
  {
    Result = qaNo;
  }
  else if ((Answers & qaAbort) != 0)
  {
    Result = qaAbort;
  }
  else if ((Answers & qaOK) != 0)
  {
    Result = qaOK;
  }
  else
  {
    FAIL;
    Result = qaCancel;
  }
  return Result;
}
//---------------------------------------------------------------------------
unsigned int __fastcall AbortAnswer(unsigned int Answers)
{
  unsigned int Result;
  if (FLAGSET(Answers, qaAbort))
  {
    Result = qaAbort;
  }
  else
  {
    Result = CancelAnswer(Answers);
  }
  return Result;
}
//---------------------------------------------------------------------------
unsigned int __fastcall ContinueAnswer(unsigned int Answers)
{
  unsigned int Result;
  if (FLAGSET(Answers, qaSkip))
  {
    Result = qaSkip;
  }
  else if (FLAGSET(Answers, qaIgnore))
  {
    Result = qaIgnore;
  }
  else if (FLAGSET(Answers, qaYes))
  {
    Result = qaYes;
  }
  else if (FLAGSET(Answers, qaOK))
  {
    Result = qaOK;
  }
  else if (FLAGSET(Answers, qaRetry))
  {
    Result = qaRetry;
  }
  else
  {
    Result = CancelAnswer(Answers);
  }
  return Result;
}
//---------------------------------------------------------------------------
TLibModule * __fastcall FindModule(void * Instance)
{
  TLibModule * CurModule;
  CurModule = reinterpret_cast<TLibModule*>(LibModuleList);

  while (CurModule)
  {
    if (CurModule->Instance == (unsigned)Instance)
    {
      break;
    }
    else
    {
      CurModule = CurModule->Next;
    }
  }
  return CurModule;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall LoadStr(int Ident, unsigned int MaxLength)
{
  TLibModule * MainModule = FindModule(HInstance);
  assert(MainModule != NULL);

  UnicodeString Result;
  Result.SetLength(MaxLength);
  int Length = LoadString((HINSTANCE)MainModule->ResInstance, Ident, Result.c_str(), MaxLength);
  Result.SetLength(Length);

  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall LoadStrPart(int Ident, int Part)
{
  UnicodeString Result;
  UnicodeString Str = LoadStr(Ident);

  while (Part > 0)
  {
    Result = CutToChar(Str, L'|', false);
    Part--;
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall DecodeUrlChars(UnicodeString S)
{
  int i = 1;
  while (i <= S.Length())
  {
    switch (S[i])
    {
      case L'+':
        S[i] = ' ';
        break;

      case L'%':
        {
          UnicodeString Hex;
          while ((i + 2 <= S.Length()) && (S[i] == L'%') &&
                 IsHex(S[i + 1]) && IsHex(S[i + 2]))
          {
            Hex += S.SubString(i + 1, 2);
            S.Delete(i, 3);
          }

          if (!Hex.IsEmpty())
          {
            RawByteString Bytes = HexToBytes(Hex);
            UTF8String UTF8(Bytes.c_str(), Bytes.Length());
            UnicodeString Chars(UTF8);
            S.Insert(Chars, i);
            i += Chars.Length() - 1;
          }
        }
        break;
    }
    i++;
  }
  return S;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall DoEncodeUrl(UnicodeString S, UnicodeString Chars)
{
  int i = 1;
  while (i <= S.Length())
  {
    if (Chars.Pos(S[i]) > 0)
    {
      UnicodeString H = ByteToHex(AnsiString(UnicodeString(S[i]))[1]);
      S.Insert(H, i + 1);
      S[i] = '%';
      i += H.Length();
    }
    i++;
  }
  return S;
}
//---------------------------------------------------------------------------
// we should probably replace all uses with EncodeUrlString
UnicodeString __fastcall EncodeUrlChars(UnicodeString S)
{
  return DoEncodeUrl(S, L" /");
}
//---------------------------------------------------------------------------
UnicodeString __fastcall NonUrlChars()
{
  UnicodeString S;
  for (unsigned int I = 0; I <= 127; I++)
  {
    wchar_t C = static_cast<wchar_t>(I);
    if (IsLetter(C) ||
        IsDigit(C) ||
        (C == L'_') || (C == L'-') || (C == L'.'))
    {
      // noop
    }
    else
    {
      S += C;
    }
  }
  return S;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall EncodeUrlString(UnicodeString S)
{
  return DoEncodeUrl(S, NonUrlChars());
}
//---------------------------------------------------------------------------
UnicodeString __fastcall EncodeUrlPath(UnicodeString S)
{
  UnicodeString Ignore = NonUrlChars();
  int P = Ignore.Pos(L"/");
  if (ALWAYS_TRUE(P > 0))
  {
    Ignore.Delete(P, 1);
  }
  return DoEncodeUrl(S, Ignore);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall AppendUrlParams(UnicodeString AURL, UnicodeString Params)
{
  // see also TWebHelpSystem::ShowHelp
  const wchar_t FragmentSeparator = L'#';
  UnicodeString URL = ::CutToChar(AURL, FragmentSeparator, false);

  if (URL.Pos(L"?") == 0)
  {
    URL += L"?";
  }
  else
  {
    URL += L"&";
  }

  URL += Params;

  AddToList(URL, AURL, FragmentSeparator);

  return URL;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall EscapeHotkey(const UnicodeString & Caption)
{
  return ReplaceStr(Caption, L"&", L"&&");
}
//---------------------------------------------------------------------------
// duplicated in console's Main.cpp
bool __fastcall CutToken(UnicodeString & Str, UnicodeString & Token,
  UnicodeString * RawToken)
{
  bool Result;

  Token = L"";

  // inspired by Putty's sftp_getcmd() from PSFTP.C
  int Index = 1;
  while ((Index <= Str.Length()) &&
    ((Str[Index] == L' ') || (Str[Index] == L'\t')))
  {
    Index++;
  }

  if (Index <= Str.Length())
  {
    bool Quoting = false;

    while (Index <= Str.Length())
    {
      if (!Quoting && ((Str[Index] == L' ') || (Str[Index] == L'\t')))
      {
        break;
      }
      else if ((Str[Index] == L'"') && (Index + 1 <= Str.Length()) &&
        (Str[Index + 1] == L'"'))
      {
        Index += 2;
        Token += L'"';
      }
      else if (Str[Index] == L'"')
      {
        Index++;
        Quoting = !Quoting;
      }
      else
      {
        Token += Str[Index];
        Index++;
      }
    }

    if (RawToken != NULL)
    {
      (*RawToken) = Str.SubString(1, Index - 1);
    }

    if (Index <= Str.Length())
    {
      Index++;
    }

    Str = Str.SubString(Index, Str.Length());

    Result = true;
  }
  else
  {
    Result = false;
    Str = L"";
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall AddToList(UnicodeString & List, const UnicodeString & Value, const UnicodeString & Delimiter)
{
  if (!Value.IsEmpty())
  {
    if (!List.IsEmpty() &&
        ((List.Length() < Delimiter.Length()) ||
         (List.SubString(List.Length() - Delimiter.Length() + 1, Delimiter.Length()) != Delimiter)))
    {
      List += Delimiter;
    }
    List += Value;
  }
}
//---------------------------------------------------------------------------
bool __fastcall IsWinVista()
{
  // Vista is 6.0
  // Win XP is 5.1
  // There also 5.2, what is Windows 2003 or Windows XP 64bit
  // (we consider it WinXP for now)
  return CheckWin32Version(6, 0);
}
//---------------------------------------------------------------------------
bool __fastcall IsWin7()
{
  return CheckWin32Version(6, 1);
}
//---------------------------------------------------------------------------
bool __fastcall IsWine()
{
  HMODULE NtDll = GetModuleHandle(L"ntdll.dll");
  return
    ALWAYS_TRUE(NtDll != NULL) &&
    (GetProcAddress(NtDll, "wine_get_version") != NULL);
}
//---------------------------------------------------------------------------
LCID __fastcall GetDefaultLCID()
{
  return GetUserDefaultLCID();
}
//---------------------------------------------------------------------------
static UnicodeString ADefaultEncodingName;
UnicodeString __fastcall DefaultEncodingName()
{
  if (ADefaultEncodingName.IsEmpty())
  {
    CPINFOEX Info;
    GetCPInfoEx(CP_ACP, 0, &Info);
    ADefaultEncodingName = Info.CodePageName;
  }
  return ADefaultEncodingName;
}
//---------------------------------------------------------------------------
bool _fastcall GetWindowsProductType(DWORD & Type)
{
  bool Result;
  HINSTANCE Kernel32 = GetModuleHandle(kernel32);
  typedef BOOL WINAPI (* TGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
  TGetProductInfo GetProductInfo =
      (TGetProductInfo)GetProcAddress(Kernel32, "GetProductInfo");
  if (GetProductInfo == NULL)
  {
    Result = false;
  }
  else
  {
    GetProductInfo(Win32MajorVersion, Win32MinorVersion, 0, 0, &Type);
    Result = true;
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall WindowsProductName()
{
  UnicodeString Result;
  TRegistry * Registry = new TRegistry(KEY_READ);
  try
  {
    Registry->RootKey = HKEY_LOCAL_MACHINE;
    if (Registry->OpenKey(L"SOFTWARE", false) &&
        Registry->OpenKey(L"Microsoft", false) &&
        Registry->OpenKey(L"Windows NT", false) &&
        Registry->OpenKey(L"CurrentVersion", false))
    {
      Result = Registry->ReadString(L"ProductName");
    }
    delete Registry;
  }
  catch(...)
  {
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall IsDirectoryWriteable(const UnicodeString & Path)
{
  UnicodeString FileName =
    IncludeTrailingPathDelimiter(Path) +
    FORMAT(L"wscp_%s_%d.tmp", (FormatDateTime(L"nnzzz", Now()), int(GetCurrentProcessId())));
  HANDLE Handle = CreateFile(ApiPath(FileName).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
    CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 0);
  bool Result = (Handle != INVALID_HANDLE_VALUE);
  if (Result)
  {
    CloseHandle(Handle);
  }
  return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FormatNumber(__int64 Number)
{
  return FormatFloat(L"#,##0", Number);
}
//---------------------------------------------------------------------------
// simple alternative to FormatBytes
UnicodeString __fastcall FormatSize(__int64 Size)
{
  return FormatNumber(Size);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall ExtractFileBaseName(const UnicodeString & Path)
{
  return ChangeFileExt(ExtractFileName(Path), L"");
}
//---------------------------------------------------------------------------
TStringList * __fastcall TextToStringList(const UnicodeString & Text)
{
  std::unique_ptr<TStringList> List(new TStringList());
  List->Text = Text;
  return List.release();
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TrimVersion(UnicodeString Version)
{
  while ((Version.Pos(L".") != Version.LastDelimiter(L".")) &&
    (Version.SubString(Version.Length() - 1, 2) == L".0"))
  {
    Version.SetLength(Version.Length() - 2);
  }
  return Version;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FormatVersion(int MajovVersion, int MinorVersion, int Release)
{
  return
    TrimVersion(FORMAT(L"%d.%d.%d",
      (MajovVersion, MinorVersion, Release)));
}
//---------------------------------------------------------------------------
TFormatSettings __fastcall GetEngFormatSettings()
{
  return TFormatSettings::Create((TLocaleID)1033);
}
//---------------------------------------------------------------------------
int __fastcall ParseShortEngMonthName(const UnicodeString & MonthStr)
{
  TFormatSettings FormatSettings = GetEngFormatSettings();
  return IndexStr(MonthStr, FormatSettings.ShortMonthNames, FormatSettings.ShortMonthNames.Size()) + 1;
}
//---------------------------------------------------------------------------
TStringList * __fastcall CreateSortedStringList(bool CaseSensitive, System::Types::TDuplicates Duplicates)
{
  TStringList * Result = new TStringList();
  Result->CaseSensitive = CaseSensitive;
  Result->Sorted = true;
  Result->Duplicates = Duplicates;
  return Result;
}
//---------------------------------------------------------------------------
static UnicodeString __fastcall NormalizeIdent(UnicodeString Ident)
{
  int Index = 1;
  while (Index <= Ident.Length())
  {
    if (Ident[Index] == L'-')
    {
      Ident.Delete(Index, 1);
    }
    else
    {
      Index++;
    }
  }
  return Ident;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall FindIdent(const UnicodeString & Ident, TStrings * Idents)
{
  UnicodeString NormalizedIdent(NormalizeIdent(Ident));
  for (int Index = 0; Index < Idents->Count; Index++)
  {
    if (SameText(NormalizedIdent, NormalizeIdent(Idents->Strings[Index])))
    {
      return Idents->Strings[Index];
    }
  }
  return Ident;
}
