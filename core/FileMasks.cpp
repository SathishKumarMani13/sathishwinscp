//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FileMasks.h"

namespace Masks
{
bool __fastcall MatchesMask(const AnsiString Filename, const AnsiString Mask);
}
using namespace Masks;

#include "Common.h"
#include "TextsCore.h"
//---------------------------------------------------------------------------
AnsiString __fastcall MaskFilePart(const AnsiString Part, const AnsiString Mask, bool& Masked)
{
  AnsiString Result;
  int RestStart = 1;
  bool Delim = false;
  for (int Index = 1; Index <= Mask.Length(); Index++)
  {
    switch (Mask[Index])
    {
      case '\\':
        if (!Delim)
        {
          Delim = true;
          Masked = true;
          break;
        }
    
      case '*':
        if (!Delim)
        {
          Result += Part.SubString(RestStart, Part.Length() - RestStart + 1);
          RestStart = Part.Length() + 1; 
          Masked = true;
          break;
        }

      case '?':
        if (!Delim)
        {
          if (RestStart <= Part.Length())
          {
            Result += Part[RestStart];
            RestStart++;
          }
          Masked = true;
          break;
        }

      default:
        Result += Mask[Index];
        RestStart++;
        Delim = false;
        break; 
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
AnsiString __fastcall MaskFileName(AnsiString FileName, const AnsiString Mask)
{
  if (!Mask.IsEmpty() && (Mask != "*") && (Mask != "*.*"))
  {
    bool Masked;
    int P = Mask.LastDelimiter(".");
    if (P > 0)
    {
      int P2 = FileName.LastDelimiter(".");
      // only dot at beginning of file name is not considered as
      // name/ext separator
      AnsiString FileExt = P2 > 1 ?
        FileName.SubString(P2 + 1, FileName.Length() - P2) : AnsiString();
      FileExt = MaskFilePart(FileExt, Mask.SubString(P + 1, Mask.Length() - P), Masked);
      if (P2 > 1)
      {
        FileName.SetLength(P2 - 1);
      }
      FileName = MaskFilePart(FileName, Mask.SubString(1, P - 1), Masked);
      if (!FileExt.IsEmpty())
      {
        FileName += "." + FileExt;
      }
    }
    else
    {
      FileName = MaskFilePart(FileName, Mask, Masked);
    }
  }
  return FileName;
}
//---------------------------------------------------------------------------
bool __fastcall IsFileNameMask(const AnsiString Mask)
{
  bool Masked = false;
  MaskFilePart("", Mask, Masked);
  return Masked;
}
//---------------------------------------------------------------------------
AnsiString __fastcall DelimitFileNameMask(AnsiString Mask)
{
  for (int i = 1; i <= Mask.Length(); i++)
  {
    if (strchr("\\*?", Mask[i]) != NULL)
    {
      Mask.Insert("\\", i);
      i++;
    }
  }
  return Mask;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::IsMask(const AnsiString Mask)
{
  return (Mask.LastDelimiter("?*[") > 0);
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::SingleMaskMatch(const AnsiString Mask, const AnsiString FileName)
{
  return MatchesMask(FileName, Mask);
}
//---------------------------------------------------------------------------
__fastcall TFileMasks::TFileMasks()
{
  FMasks = "";
}
//---------------------------------------------------------------------------
__fastcall TFileMasks::TFileMasks(const TFileMasks & Source)
{
  Masks = Source.Masks;
}
//---------------------------------------------------------------------------
__fastcall TFileMasks::TFileMasks(const AnsiString AMasks)
{
  FMasks = AMasks;
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::Matches(AnsiString FileName) const
{
  AnsiString S = Masks;
  // we used to extract filename here using ExtractFileName, but it fails
  // for unix filename with colon in name (:)
  while (!S.IsEmpty())
  {
    AnsiString M;
    M = CutToChar(S, ';', True);
    if (MatchesMask(FileName, M)) return true;
  }
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::IsValid()
{
  int Start, Length;
  return IsValid(Start, Length);
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::IsValid(int & Start, int & Length)
{
  AnsiString S = Masks;
  int IStart = 1;

  while (!S.IsEmpty())
  {
    AnsiString M;
    int P = S.Pos(';');
    M = CutToChar(S, ';', False);

    if (!M.IsEmpty())
    {
      try
      {
        MatchesMask("*.*", Trim(M));
      }
      catch (Exception &E)
      {
        // Ignore leading/trainling spaces
        while (!M.IsEmpty() && (M[1] == ' '))
        {
          IStart++;
          M.Delete(1, 1);
        }
        Start = IStart-1;
        Length = M.Trim().Length();
        return False;
      }
    }
    if (P) IStart += P;
  }
  return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::operator ==(const TFileMasks & rhm) const
{
  return (Masks == rhm.Masks);
}
//---------------------------------------------------------------------------
TFileMasks & __fastcall TFileMasks::operator =(const AnsiString rhs)
{
  Masks = rhs;
  return *this;
}
//---------------------------------------------------------------------------
TFileMasks & __fastcall TFileMasks::operator =(const TFileMasks & rhm)
{
  Masks = rhm.Masks;
  return *this;
}
//---------------------------------------------------------------------------
bool __fastcall TFileMasks::operator ==(const AnsiString rhs) const
{
  return (Masks == rhs);
}
//---------------------------------------------------------------------------
TFileMasks & __fastcall TFileMasks::operator =(const char * rhs)
{
  Masks = rhs;
  return *this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define TEXT_TOKEN '\255'
//---------------------------------------------------------------------------
TCustomCommand::TCustomCommand()
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomCommand::GetToken(
  const AnsiString & Command, int Index, int & Len, char & PatternCmd)
{
  assert(Index <= Command.Length());
  const char * Ptr = Command.c_str() + Index - 1;

  if (Ptr[0] == '!')
  {
    PatternCmd = Ptr[1];
    if (PatternCmd == '!')
    {
      Len = 2;
    }
    else
    {
      Len = PatternLen(Index, PatternCmd);
    }
    
    if (Len < 0)
    {
      throw Exception(FMTLOAD(CUSTOM_COMMAND_UNKNOWN, (PatternCmd, Index)));
    }
    else if (Len > 0)
    {
      if ((Command.Length() - Index + 1) < Len)
      {
        throw Exception(FMTLOAD(CUSTOM_COMMAND_UNTERMINATED, (PatternCmd, Index)));
      }
    }
    else if (Len == 0)
    {
      const char * PatternEnd = strchr(Ptr + 1, '!');
      if (PatternEnd == NULL)
      {
        throw Exception(FMTLOAD(CUSTOM_COMMAND_UNTERMINATED, (PatternCmd, Index)));
      }
      Len = PatternEnd - Ptr + 1;
    }
  }
  else
  {
    PatternCmd = TEXT_TOKEN;
    const char * NextPattern = strchr(Ptr, '!');
    if (NextPattern == NULL)
    {
      Len = Command.Length() - Index + 1;
    }
    else
    {
      Len = NextPattern - Ptr;
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCustomCommand::Complete(const AnsiString & Command,
  bool LastPass)
{
  AnsiString Result;
  int Index = 1;

  while (Index <= Command.Length())
  {
    int Len;
    char PatternCmd;
    GetToken(Command, Index, Len, PatternCmd);

    if (PatternCmd == TEXT_TOKEN)
    {
      Result += Command.SubString(Index, Len);
    }
    else if (PatternCmd == '!')
    {
      if (LastPass)
      {
        Result += '!';
      }
      else
      {
        Result += Command.SubString(Index, Len);
      }
    }
    else
    {
      AnsiString Pattern = Command.SubString(Index, Len);
      AnsiString Replacement;
      if (PatternReplacement(Index, Pattern, Replacement))
      {
        if (!LastPass)
        {
          Replacement = StringReplace(Replacement, "!", "!!",
            TReplaceFlags() << rfReplaceAll);
        }
        Result += Replacement;
      }
      else
      {
        Result += Pattern;
      }
    }

    Index += Len;
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TCustomCommand::Validate(const AnsiString & Command)
{
  CustomValidate(Command, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TCustomCommand::CustomValidate(const AnsiString & Command,
  void * Arg)
{
  int Index = 1;

  while (Index <= Command.Length())
  {
    int Len;
    char PatternCmd;
    GetToken(Command, Index, Len, PatternCmd);
    ValidatePattern(Command, Index, Len, PatternCmd, Arg);

    Index += Len;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TCustomCommand::FindPattern(const AnsiString & Command,
  char PatternCmd)
{
  bool Result = false;
  int Index = 1;

  while (!Result && (Index <= Command.Length()))
  {
    int Len;
    char APatternCmd;
    GetToken(Command, Index, Len, APatternCmd);
    if (PatternCmd == APatternCmd)
    {
      Result = true;
    }

    Index += Len;
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TCustomCommand::ValidatePattern(const AnsiString & /*Command*/,
  int /*Index*/, int /*Len*/, char /*PatternCmd*/, void * /*Arg*/)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TInteractiveCustomCommand::TInteractiveCustomCommand(
  TCustomCommand * ChildCustomCommand)
{
  FChildCustomCommand = ChildCustomCommand;
}
//---------------------------------------------------------------------------
void __fastcall TInteractiveCustomCommand::Prompt(int /*Index*/,
  const AnsiString & /*Prompt*/, AnsiString & Value)
{
  Value = "";
}
//---------------------------------------------------------------------------
int __fastcall TInteractiveCustomCommand::PatternLen(int Index, char PatternCmd)
{
  int Len;
  switch (PatternCmd)
  {
    case '?':
      Len = 0;
      break;

    default:
      Len = FChildCustomCommand->PatternLen(Index, PatternCmd);
      break;
  }
  return Len;
}
//---------------------------------------------------------------------------
bool __fastcall TInteractiveCustomCommand::PatternReplacement(int Index, const AnsiString & Pattern,
  AnsiString & Replacement)
{
  bool Result;
  if ((Pattern.Length() >= 3) && (Pattern[2] == '?'))
  {
    AnsiString PromptStr;
    int Pos = Pattern.SubString(3, Pattern.Length() - 2).Pos("?");
    if (Pos > 0)
    {
      PromptStr = Pattern.SubString(3, Pos - 1);
      Replacement = Pattern.SubString(3 + Pos, Pattern.Length() - 3 - Pos);
    }
    else
    {
      PromptStr = Pattern.SubString(3, Pattern.Length() - 3);
    }

    Prompt(Index, PromptStr, Replacement);

    Result = true;
  }
  else
  {
    Result = false; 
  }

  return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TFileCustomCommand::TFileCustomCommand() :
  TCustomCommand()
{
}
//---------------------------------------------------------------------------
TFileCustomCommand::TFileCustomCommand(const AnsiString & FileName, const AnsiString & FileList) :
  TCustomCommand()
{
  FFileName = FileName;
  FFileList = FileList;
}
//---------------------------------------------------------------------------
int __fastcall TFileCustomCommand::PatternLen(int /*Index*/, char PatternCmd)
{
  int Len;
  switch (PatternCmd)
  {
    case '&':
      Len = 2;
      break;

    default:
      Len = 1;
      break;
  }
  return Len;
}
//---------------------------------------------------------------------------
bool __fastcall TFileCustomCommand::PatternReplacement(int /*Index*/,
  const AnsiString & Pattern, AnsiString & Replacement)
{
  if (Pattern == "!&")
  {
    Replacement = FFileList;
  }
  else
  {
    assert(Pattern.Length() == 1);
    Replacement = FFileName;
  }

  return true;
}
//---------------------------------------------------------------------------
void __fastcall TFileCustomCommand::Validate(const AnsiString & Command)
{
  int Found[2] = { 0, 0 };
  CustomValidate(Command, &Found);
  if ((Found[0] > 0) && (Found[1] > 0))
  {
    throw Exception(FMTLOAD(CUSTOM_COMMAND_FILELIST_ERROR,
      (Found[1], Found[0])));
  }
}
//---------------------------------------------------------------------------
void __fastcall TFileCustomCommand::ValidatePattern(const AnsiString & /*Command*/,
  int Index, int /*Len*/, char PatternCmd, void * Arg)
{
  int * Found = static_cast<int *>(Arg);
  
  assert(Index > 0);
  
  if (PatternCmd == '&')
  {
    Found[0] = Index;
  }
  else if ((PatternCmd != TEXT_TOKEN) && (PatternLen(Index, PatternCmd) == 1))
  {
    Found[1] = Index;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TFileCustomCommand::IsFileListCommand(const AnsiString & Command)
{
  return FindPattern(Command, '&');
}
//---------------------------------------------------------------------------

