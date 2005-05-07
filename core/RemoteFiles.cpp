//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RemoteFiles.h"

#include <SysUtils.hpp>

#include "Common.h"
#include "Exceptions.h"
#include "Interface.h"
#include "Terminal.h"
#include "TextsCore.h"
/* TODO 1 : Path class instead of AnsiString (handle relativity...) */
//---------------------------------------------------------------------------
AnsiString __fastcall UnixIncludeTrailingBackslash(const AnsiString Path)
{
  // it used to return "/" when input path was empty
  if (!Path.IsEmpty() && !Path.IsDelimiter("/", Path.Length()))
  {
    return Path + "/";
  }
  else
  {
    return Path;
  }
}
//---------------------------------------------------------------------------
// Keeps "/" for root path
AnsiString __fastcall UnixExcludeTrailingBackslash(const AnsiString Path)
{
  if ((Path.Length() > 1) && Path.IsDelimiter("/", Path.Length()))
      return Path.SubString(1, Path.Length() - 1);
    else return Path;
}
//---------------------------------------------------------------------------
Boolean __fastcall ComparePaths(const AnsiString Path1, const AnsiString Path2)
{
  return AnsiSameText(IncludeTrailingBackslash(Path1), IncludeTrailingBackslash(Path2));
}
//---------------------------------------------------------------------------
Boolean __fastcall UnixComparePaths(const AnsiString Path1, const AnsiString Path2)
{
  return (UnixIncludeTrailingBackslash(Path1) == UnixIncludeTrailingBackslash(Path2));
}
//---------------------------------------------------------------------------
AnsiString __fastcall UnixExtractFileDir(const AnsiString Path)
{
  int Pos = Path.LastDelimiter('/');
  // it used to return Path when no slash was found
  if (Pos > 1)
  {
    return Path.SubString(1, Pos - 1);
  }
  else
  {
    return (Pos == 1) ? AnsiString("/") : AnsiString();
  }
}
//---------------------------------------------------------------------------
// must return trailing backslash
AnsiString __fastcall UnixExtractFilePath(const AnsiString Path)
{
  int Pos = Path.LastDelimiter('/');
  // it used to return Path when no slash was found
  return (Pos > 0) ? Path.SubString(1, Pos) : AnsiString();
}
//---------------------------------------------------------------------------
AnsiString __fastcall UnixExtractFileName(const AnsiString Path)
{
  int Pos = Path.LastDelimiter('/');
  return (Pos > 0) ? Path.SubString(Pos + 1, Path.Length() - Pos) : Path;
}
//---------------------------------------------------------------------------
AnsiString __fastcall UnixExtractFileExt(const AnsiString Path)
{
  AnsiString FileName = UnixExtractFileName(Path);
  int Pos = FileName.LastDelimiter(".");
  return (Pos > 0) ? Path.SubString(Pos, Path.Length() - Pos + 1) : AnsiString();
}
//---------------------------------------------------------------------------
bool __fastcall ExtractCommonPath(TStrings * Files, AnsiString & Path)
{
  assert(Files->Count > 0);

  Path = ExtractFilePath(Files->Strings[0]);
  bool Result = !Path.IsEmpty();
  if (Result)
  {
    for (int Index = 1; Index < Files->Count; Index++)
    {
      while (Path.IsEmpty() &&
        (Files->Strings[Index].SubString(1, Path.Length()) != Path))
      {
        int PrevLen = Path.Length();
        Path = ExtractFilePath(ExcludeTrailingBackslash(Path));
        if (Path.Length() == PrevLen)
        {
          Path = "";
        }
      }
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall UnixExtractCommonPath(TStrings * Files, AnsiString & Path)
{
  assert(Files->Count > 0);

  Path = UnixExtractFilePath(Files->Strings[0]);
  bool Result = !Path.IsEmpty();
  if (Result)
  {
    for (int Index = 1; Index < Files->Count; Index++)
    {
      while (Path.IsEmpty() &&
        (Files->Strings[Index].SubString(1, Path.Length()) != Path))
      {
        int PrevLen = Path.Length();
        Path = UnixExtractFilePath(UnixExcludeTrailingBackslash(Path));
        if (Path.Length() == PrevLen)
        {
          Path = "";
        }
      }
    }
  }

  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall IsUnixRootPath(const AnsiString Path)
{
  return Path.IsEmpty() || (Path == ROOTDIRECTORY);
}
//---------------------------------------------------------------------------
bool __fastcall IsUnixHiddenFile(const AnsiString FileName)
{
  return (FileName != ROOTDIRECTORY) && (FileName != PARENTDIRECTORY) &&
    !FileName.IsEmpty() && (FileName[1] == '.');
}
//---------------------------------------------------------------------------
AnsiString __fastcall FromUnixPath(const AnsiString Path)
{
  return StringReplace(Path, "/", "\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
AnsiString __fastcall ToUnixPath(const AnsiString Path)
{
  return StringReplace(Path, "\\", "/", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
static void __fastcall CutFirstDirectory(AnsiString & S, bool Unix)
{
  bool Root;
  int P;
  AnsiString Sep = Unix ? "/" : "\\";
  if (S == Sep)
  {
    S = "";
  }
  else
  {
    if (S[1] == Sep[1])
    {
      Root = true;
      S.Delete(1, 1);
    }
    else
    {
      Root = false;
    }
    if (S[1] == '.')
    {
      S.Delete(1, 4);
    }
    P = S.AnsiPos(Sep[1]);
    if (P)
    {
      S.Delete(1, P);
      S = "..." + Sep + S;
    }
    else
    {
      S = "";
    }
    if (Root)
    {
      S = Sep + S;
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall MinimizeName(const AnsiString FileName, int MaxLen, bool Unix)
{
  AnsiString Drive, Dir, Name, Result;
  AnsiString Sep = Unix ? "/" : "\\";

  Result = FileName;
  if (Unix)
  {
    int P = Result.LastDelimiter("/");
    if (P)
    {
      Dir = Result.SubString(1, P);
      Name = Result.SubString(P + 1, Result.Length() - P);
    }
    else
    {
      Dir = "";
      Name = Result;
    }
  }
  else
  {
    Dir = ExtractFilePath(Result);
    Name = ExtractFileName(Result);

    if (Dir.Length() >= 2 && Dir[2] == ':')
    {
      Drive = Dir.SubString(1, 2);
      Dir.Delete(1, 2);
    }
  }

  while ((!Dir.IsEmpty() || !Drive.IsEmpty()) && (Result.Length() > MaxLen))
  {
    if (Dir == Sep + "..." + Sep)
    {
      Dir = "..." + Sep;
    }
    else if (Dir == "")
    {
      Drive = "";
    }
    else
    {
      CutFirstDirectory(Dir, Unix);
    }
    Result = Drive + Dir + Name;
  }

  if (Result.Length() > MaxLen)
  {
    Result = Result.SubString(1, MaxLen);
  }
  return Result;
}
//---------------------------------------------------------------------------
AnsiString __fastcall MakeFileList(TStrings * FileList)
{
  AnsiString Result;
  for (int Index = 0; Index < FileList->Count; Index++)
  {
    if (!Result.IsEmpty())
    {
      Result += " ";
    }

    AnsiString FileName = FileList->Strings[Index];
    if (FileName.Pos(" ") > 0)
    {
      Result += "\"" + FileName + "\"";
    }
    else
    {
      Result += FileName;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
// copy from BaseUtils.pas
void __fastcall ReduceDateTimePrecision(TDateTime & DateTime,
  TModificationFmt Precision)
{
  if (Precision != mfFull)
  {
    unsigned short Y, M, D, H, N, S, MS;
  
    DecodeDate(DateTime, Y, M, D);
    DecodeTime(DateTime, H, N, S, MS);
    switch (Precision)
    {
      case mfMDHM:
        S = 0;
        MS = 0;
        break;

      case mfMDY:
        H = 0;
        N = 0;
        S = 0;
        MS = 0;

      default:
        assert(false);
    }

    DateTime = EncodeDate(Y, M, D) + EncodeTime(H, N, S, MS);
  }
}
//- TRemoteFiles ------------------------------------------------------------
__fastcall TRemoteFile::TRemoteFile(TRemoteFile * ALinkedByFile):
  TPersistent()
{
  FLinkedFile = NULL;
  FRights = new TRights();
  FIconIndex = -1;
  FCyclicLink = false;
  FModificationFmt = mfFull;
  FLinkedByFile = ALinkedByFile;
  FTerminal = NULL;
  FDirectory = NULL;
  FIsHidden = -1;
}
//---------------------------------------------------------------------------
__fastcall TRemoteFile::~TRemoteFile()
{
  delete FRights;
  delete FLinkedFile;
}
//---------------------------------------------------------------------------
TRemoteFile * __fastcall TRemoteFile::Duplicate(bool Standalone)
{
  TRemoteFile * Result;
  Result = new TRemoteFile();
  try
  {
    if (FLinkedFile)
    {
      Result->FLinkedFile = FLinkedFile->Duplicate(true);
      Result->FLinkedFile->FLinkedByFile = Result;
    }
    *Result->Rights = *FRights;
    #define COPY_FP(PROP) Result->F ## PROP = F ## PROP;
    COPY_FP(Terminal);
    COPY_FP(Owner);
    COPY_FP(ModificationFmt);
    COPY_FP(Size);
    COPY_FP(FileName);
    COPY_FP(INodeBlocks);
    COPY_FP(Modification);
    COPY_FP(LastAccess);
    COPY_FP(Group);
    COPY_FP(IconIndex);
    COPY_FP(IsSymLink);
    COPY_FP(LinkTo);
    COPY_FP(Type);
    COPY_FP(Selected);
    COPY_FP(CyclicLink);
    #undef COPY_FP
    if (Standalone && (!FFullFileName.IsEmpty() || (Directory != NULL)))
    {
      Result->FFullFileName = FullFileName;
    }
  }
  catch(...)
  {
    delete Result;
    throw;
  }
  return Result;
}
//---------------------------------------------------------------------------
Integer __fastcall TRemoteFile::GetIconIndex()
{
  assert(FIconIndex >= -1);
  if (FIconIndex < 0)
  {
    /* TODO : If file is link: Should be attributes taken from linked file? */
    unsigned long Attrs = FILE_ATTRIBUTE_NORMAL;
    if (IsDirectory) Attrs |= FILE_ATTRIBUTE_DIRECTORY;
    if (IsHidden) Attrs |= FILE_ATTRIBUTE_HIDDEN;

    TSHFileInfo SHFileInfo;
    AnsiString DumbFileName = (IsSymLink && !LinkTo.IsEmpty() ? LinkTo : FileName);
    // On Win2k we get icon of "ZIP drive" for ".." (parent directory)
    if (DumbFileName == "..") DumbFileName = "dumb";
    // this should be somewhere else, probably in TUnixDirView,
    // as the "partial" overlay is added there too
    if (AnsiSameText(UnixExtractFileExt(DumbFileName), PARTIAL_EXT))
    {
      static const size_t PartialExtLen = sizeof(PARTIAL_EXT) - 1;
      DumbFileName.SetLength(DumbFileName.Length() - PartialExtLen);
    }

    SHGetFileInfo(DumbFileName.c_str(),
      Attrs, &SHFileInfo, sizeof(SHFileInfo),
      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
    FIconIndex = SHFileInfo.iIcon;
  }
  return FIconIndex;
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFile::GetIsHidden()
{
  bool Result;
  switch (FIsHidden)
  {
    case 0:
      Result = false;
      break;

    case 1:
      Result = true;
      break;

    default:
      Result = IsUnixHiddenFile(FileName);
      break;
  }

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetIsHidden(bool value)
{
  FIsHidden = value ? 1 : 0;
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFile::GetIsDirectory() const
{
  return (toupper(Type) == FILETYPE_DIRECTORY);
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFile::GetIsParentDirectory() const
{
  return (FileName == PARENTDIRECTORY);
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFile::GetIsThisDirectory() const
{
  return (FileName == THISDIRECTORY);
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFile::GetIsInaccesibleDirectory() const
{
  Boolean Result;
  if (IsDirectory)
  {
    assert(Terminal);
    Result = !
       (((Rights->RightUndef[TRights::rrOtherExec] != TRights::rsNo)) ||
        ((Rights->Right[TRights::rrGroupExec] != TRights::rsNo) &&
         (Terminal->Groups->IndexOf(Group) >= 0)) ||
        ((Rights->Right[TRights::rrUserExec] != TRights::rsNo) &&
         (AnsiCompareText(Terminal->UserName, Owner) == 0)));
  }
    else Result = False;
  return Result;
}
//---------------------------------------------------------------------------
char __fastcall TRemoteFile::GetType() const
{
  if (IsSymLink && FLinkedFile) return FLinkedFile->Type;
    else return FType;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetType(char AType)
{
  FType = AType;
  // Allow even non-standard file types (e.g. 'S')
  // if (!AnsiString("-DL").Pos((Char)toupper(FType))) Abort();
  FIsSymLink = ((Char)toupper(FType) == FILETYPE_SYMLINK);
}
//---------------------------------------------------------------------------
TRemoteFile * __fastcall TRemoteFile::GetLinkedFile()
{
  // it would be called releatedly for broken symlinks
  //if (!FLinkedFile) FindLinkedFile();
  return FLinkedFile;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetLinkedFile(TRemoteFile * value)
{
  if (FLinkedFile != value)
  {
    if (FLinkedFile) delete FLinkedFile;
    FLinkedFile = value;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteFile::GetBrokenLink()
{
  assert(Terminal);
  // If file is symlink but we couldn't find linked file we assume broken link
  return (IsSymLink && (FCyclicLink || !FLinkedFile) &&
    Terminal->SessionData->ResolveSymlinks && Terminal->IsCapable[fcResolveSymlink]);
  // "!FLinkTo.IsEmpty()" removed because it does not work with SFTP
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::ShiftTime(const TDateTime & Difference)
{
  double D = double(Difference);
  if ((D != 0) && (FModificationFmt != mfMDY))
  {
    assert(int(FModification) != 0);
    FModification = double(FModification) + D;
    assert(int(FLastAccess) != 0);
    FLastAccess = double(FLastAccess) + D;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetModification(const TDateTime & value)
{
  if (FModification != value)
  {
    FModificationFmt = mfFull;
    FModification = value;
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetUserModificationStr()
{
  switch (FModificationFmt)
  {
    case mfMDY:
      return FormatDateTime("ddddd", Modification);
    case mfMDHM:
      return FormatDateTime("ddddd t", Modification);
    case mfFull:
    default:
      return FormatDateTime("ddddd tt", Modification);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetModificationStr()
{
  Word Year, Month, Day, Hour, Min, Sec, MSec;
  Modification.DecodeDate(&Year, &Month, &Day);
  Modification.DecodeTime(&Hour, &Min, &Sec, &MSec);
  switch (FModificationFmt)
  {
    case mfMDY:
      return FORMAT("%3s %2d %2d", (EngShortMonthNames[Month-1], Day, Year));
        
    case mfMDHM:
      return FORMAT("%3s %2d %2d:%2.2d",
        (EngShortMonthNames[Month-1], Day, Hour, Min));

    default:
      assert(false);
      // fall thru
            
    case mfFull:
      return FORMAT("%3s %2d %2d:%2.2d:%2.2d %4d", 
        (EngShortMonthNames[Month-1], Day, Hour, Min, Sec, Year));
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetExtension()
{
  return UnixExtractFileExt(FFileName);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetRights(TRights * value)
{
  FRights->Assign(value);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetRightsStr()
{
  return FRights->Text;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetListingStr(AnsiString value)
{
  // Value stored in 'value' can be used for error message
  AnsiString Line = value;
  FIconIndex = -1;
  try
  {
    AnsiString Col;

    // Do we need to do this (is ever TAB is LS output)?
    Line = ReplaceChar(Line, '\t', ' ');

    Type = Line[1];
    Line.Delete(1, 1);

    #define GETNCOL  \
      { if (Line.IsEmpty()) throw Exception(""); \
        Integer P = Line.Pos(' '); \
        if (P) { Col = Line.SubString(1, P-1); Line.Delete(1, P); } \
          else { Col = Line; Line = ""; } \
      }
    #define GETCOL { GETNCOL; Line = TrimLeft(Line); }

    // Rights string may contain special permission attributes (S,t, ...)
    Rights->AllowUndef = True;
    // On some system there is no space between permissions and node blocks count columns
    // so we get only first 9 characters and trim all following spaces (if any)
    Rights->Text = Line.SubString(1, 9);
    Line.Delete(1, 9);
    // Rights column maybe followed by '+' sign, we ignore it
    if (!Line.IsEmpty() && (Line[1] == '+')) Line.Delete(1, 1);
    Line = Line.TrimLeft();

    GETCOL;
    FINodeBlocks = StrToInt(Col);

    GETCOL;
    FOwner = Col;

    // #60 17.10.01: group name can contain space
    FGroup = "";
    GETCOL;
    __int64 ASize;
    do
    {
      FGroup += Col;
      GETCOL;
      assert(!Col.IsEmpty());
      // for devices etc.. there is additional column ending by comma, we ignore it
      if (Col[Col.Length()] == ',') GETCOL;
      ASize = StrToInt64Def(Col, -1);
      // if it's not a number (file size) we take it as part of group name
      // (at least on CygWin, there can be group with space in its name)
      if (ASize < 0) Col = " " + Col;
    }
    while (ASize < 0);

    // do not read modification time and filename if it is already set
    if (double(FModification) == 0 && FileName.IsEmpty())
    {
      FSize = ASize;

      bool FullTime = false;
      bool DayMonthFormat = false;
      Word Day, Month, Year, Hour, Min, Sec, P;

      GETCOL;
      // format dd mmm or mmm dd ?
      Day = (Word)StrToIntDef(Col, 0);
      if (Day > 0)
      {
        DayMonthFormat = true;
        GETCOL;
      }
      Month = 0;
      #define COL2MONTH \
        for (Word IMonth = 0; IMonth < 12; IMonth++) \
          if (!Col.AnsiCompareIC(EngShortMonthNames[IMonth])) { Month = IMonth; Month++; break; }
      COL2MONTH;
      // if the column is not known month name, it may have been "yyyy-mm-dd"
      // for --full-time format
      if ((Month == 0) && (Col.Length() == 10) && (Col[5] == '-') && (Col[8] == '-'))
      {
        Year = (Word)Col.SubString(1, 4).ToInt();
        Month = (Word)Col.SubString(6, 2).ToInt();
        Day = (Word)Col.SubString(9, 2).ToInt();
        GETCOL;
        Hour = (Word)Col.SubString(1, 2).ToInt();
        Min = (Word)Col.SubString(4, 2).ToInt();
        Sec = (Word)Col.SubString(7, 2).ToInt();
        FModificationFmt = mfFull;
        // skip TZ (TODO)
        // do not trim leading space of filename
        GETNCOL;
      }
      else
      {
        // or it may have been day name for another format of --full-time
        if (Month == 0)
        {
          GETCOL;
          COL2MONTH;
          // neither standard, not --full-time format
          if (Month == 0)
          {
            Abort();
          }
          else
          {
            FullTime = true;
          }
        }
        #undef COL2MONTH

        if (Day == 0)
        {
          GETNCOL;
          Day = (Word)StrToInt(Col);
        }
        if ((Day < 1) || (Day > 31)) Abort();

        // second full-time format
        // ddd mmm dd hh:nn:ss yyyy
        if (FullTime)
        {
          GETCOL;
          if (Col.Length() != 8)
          {
            Abort();
          }
          Hour = (Word)StrToInt(Col.SubString(1, 2));
          Min = (Word)StrToInt(Col.SubString(4, 2));
          Sec = (Word)StrToInt(Col.SubString(7, 2));
          FModificationFmt = mfFull;
          // do not trim leading space of filename
          GETNCOL;
          Year = (Word)StrToInt(Col);
        }
        else
        {
          // for format dd mmm the below description seems not to be true,
          // the year is not aligned to 5 characters
          if (DayMonthFormat)
          {
            GETCOL;
          }
          else
          {
            // Time/Year indicator is always 5 charactes long (???), on most
            // systems year is aligned to right (_YYYY), but on some to left (YYYY_),
            // we must ensure that trailing space is also deleted, so real
            // separator space is not treated as part of file name
            Col = Line.SubString(1, 6).Trim();
            Line.Delete(1, 6);
          }
          // GETNCOL; // We don't want to trim input strings (name with space at beginning???)
          // Check if we got time (contains :) or year
          if ((P = (Word)Col.Pos(':')) > 0)
          {
            Word CurrMonth, CurrDay;
            Hour = (Word)StrToInt(Col.SubString(1, P-1));
            Min = (Word)StrToInt(Col.SubString(P+1, Col.Length() - P));
            if (Hour > 23 || Hour > 59) Abort();
            // When we don't got year, we assume current year
            // with exception that the date would be in future
            // in this case we assume last year.
            DecodeDate(Date(), Year, CurrMonth, CurrDay);
            if ((Month > CurrMonth) ||
                (Month == CurrMonth && Day > CurrDay)) Year--;
            Sec = 0;
            FModificationFmt = mfMDHM;
          }
            else
          {
            Year = (Word)StrToInt(Col);
            if (Year > 10000) Abort();
            // When we don't got time we assume midnight
            Hour = 0; Min = 0; Sec = 0;
            FModificationFmt = mfMDY;
          }
        }
      }

      FModification = EncodeDate(Year, Month, Day) + EncodeTime(Hour, Min, Sec, 0);
      // adjust only when time is known,
      // adjusting default "midnight" time makes no sense
      if ((FModificationFmt == mfMDHM) || (FModificationFmt == mfFull))
      {
        assert(Terminal != NULL);
        FModification = AdjustDateTimeFromUnix(FModification,
          Terminal->SessionData->ConsiderDST);
      }

      if (double(FLastAccess) == 0)
      {
        FLastAccess = FModification;
      }

      // separating space is already deleted, other spaces are treated as part of name

      {
        int P;

        FLinkTo = "";
        if (IsSymLink)
        {
          P = Line.Pos(SYMLINKSTR);
          if (P)
          {
            FLinkTo = Line.SubString(
              P + strlen(SYMLINKSTR), Line.Length() - P + strlen(SYMLINKSTR) + 1);
            Line.SetLength(P - 1);
          }
          else
          {
            Abort();
          }
        }
        FFileName = UnixExtractFileName(Line);
      }
    }

    #undef GETNCOL
    #undef GETCOL
  }
  catch (Exception &E)
  {
    throw ETerminal(&E, FmtLoadStr(LIST_LINE_ERROR, ARRAYOFCONST((value))));
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::Complete()
{
  assert(Terminal != NULL);
  if (IsSymLink && Terminal->SessionData->ResolveSymlinks &&
      Terminal->IsCapable[fcResolveSymlink])
  {
    FindLinkedFile();
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::FindLinkedFile()
{
  assert(Terminal && IsSymLink);

  if (FLinkedFile) delete FLinkedFile;
  FLinkedFile = NULL;

  FCyclicLink = false;
  if (!LinkTo.IsEmpty())
  {
    // check for cyclic link
    TRemoteFile * LinkedBy = FLinkedByFile;
    while (LinkedBy)
    {
      if (LinkedBy->LinkTo == LinkTo)
      {
        // this is currenly redundant information, because it is used only to
        // detect broken symlink, which would be otherwise detected
        // by FLinkedFile == NULL
        FCyclicLink = true;
        break;
      }
      LinkedBy = LinkedBy->FLinkedByFile;
    }
  }

  if (FCyclicLink)
  {
    TRemoteFile * LinkedBy = FLinkedByFile;
    while (LinkedBy)
    {
      LinkedBy->FCyclicLink = true;
      LinkedBy = LinkedBy->FLinkedByFile;
    }
  }
  else
  {
    assert(Terminal->SessionData->ResolveSymlinks && Terminal->IsCapable[fcResolveSymlink]);
    Terminal->ExceptionOnFail = true;
    try
    {
      try
      {
        Terminal->ReadSymlink(this, FLinkedFile);
      }
      __finally
      {
        Terminal->ExceptionOnFail = false;
      }
    }
    catch (Exception &E)
    {
      if (E.InheritsFrom(__classid(EFatal))) throw;
        else Terminal->DoHandleExtendedException(&E);
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetListingStr()
{
  // note that ModificationStr is longer than 12 for mfFull
  return Format("%s%s %3s %-8s %-8s %9s %-12s %s%s", ARRAYOFCONST((
    Type, Rights->Text, IntToStr(INodeBlocks), Owner,
    Group, IntToStr(Size), ModificationStr, FileName,
    (IsSymLink ? AnsiString(SYMLINKSTR) + LinkTo : AnsiString()))));
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFile::GetFullFileName() const
{
  if (FFullFileName.IsEmpty())
  {
    assert(Terminal);
    assert(Directory != NULL);
    AnsiString Path;
    if (IsParentDirectory) Path = Directory->ParentPath;
      else
    if (IsDirectory) Path = UnixIncludeTrailingBackslash(Directory->FullDirectory + FileName);
      else Path = Directory->FullDirectory + FileName;
    return Terminal->TranslateLockedPath(Path, true);
  }
  else
  {
    return FFullFileName;
  }
}
//---------------------------------------------------------------------------
Integer __fastcall TRemoteFile::GetAttr()
{
  Integer Result = 0;
  if (Rights->ReadOnly) Result |= faReadOnly;
  if (IsHidden) Result |= faHidden;
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFile::SetTerminal(TTerminal * value)
{
  FTerminal = value;
  if (FLinkedFile)
  {
    FLinkedFile->Terminal = value;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TRemoteDirectoryFile::TRemoteDirectoryFile() : TRemoteFile()
{
  Modification = Now();
  LastAccess = Modification;
  Type = 'D';
  Size = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TRemoteParentDirectory::TRemoteParentDirectory() : TRemoteDirectoryFile()
{
  FileName = PARENTDIRECTORY;
}
//=== TRemoteFileList ------------------------------------------------------
__fastcall TRemoteFileList::TRemoteFileList():
  TObjectList()
{
  FTimestamp = Now();
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFileList::AddFile(TRemoteFile * File)
{
  Add(File);
  File->Directory = this;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFileList::DuplicateTo(TRemoteFileList * Copy)
{
  Copy->Clear();
  for (int Index = 0; Index < Count; Index++)
  {
    TRemoteFile * File = Files[Index];
    Copy->AddFile(File->Duplicate(false));
  }
  Copy->FDirectory = Directory;
  Copy->FTimestamp = FTimestamp;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFileList::Clear()
{
  FTimestamp = Now();
  TObjectList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall TRemoteFileList::SetDirectory(AnsiString value)
{
  FDirectory = UnixExcludeTrailingBackslash(value);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFileList::GetFullDirectory()
{
  return UnixIncludeTrailingBackslash(Directory);
}
//---------------------------------------------------------------------------
TRemoteFile * __fastcall TRemoteFileList::GetFiles(Integer Index)
{
  return (TRemoteFile *)Items[Index];
}
//---------------------------------------------------------------------------
Boolean __fastcall TRemoteFileList::GetIsRoot()
{
  return (Directory == ROOTDIRECTORY);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteFileList::GetParentPath()
{
  return UnixExtractFilePath(Directory);
}
//---------------------------------------------------------------------------
__int64 __fastcall TRemoteFileList::GetTotalSize()
{
  __int64 Result = 0;
  for (Integer Index = 0; Index < Count; Index++)
    if (!Files[Index]->IsDirectory) Result += Files[Index]->Size;
  return Result;
}
//---------------------------------------------------------------------------
TRemoteFile * __fastcall TRemoteFileList::FindFile(const AnsiString &FileName)
{
  for (Integer Index = 0; Index < Count; Index++)
    if (Files[Index]->FileName == FileName) return Files[Index];
  return NULL;
}
//=== TRemoteDirectory ------------------------------------------------------
__fastcall TRemoteDirectory::TRemoteDirectory(TTerminal * aTerminal):
  TRemoteFileList(), FTerminal(aTerminal)
{
  FSelectedFiles = NULL;
  FThisDirectory = NULL;
  FParentDirectory = NULL;
  FIncludeThisDirectory = false;
  FIncludeParentDirectory = true;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::Clear()
{
  if (ThisDirectory && !IncludeThisDirectory)
  {
    delete FThisDirectory;
    FThisDirectory = NULL;
  }
  if (ParentDirectory && !IncludeParentDirectory)
  {
    delete FParentDirectory;
    FParentDirectory = NULL;
  }

  TRemoteFileList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::SetDirectory(AnsiString value)
{
  TRemoteFileList::SetDirectory(value);
  //Load();
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::AddFile(TRemoteFile * File)
{
  if (File->IsThisDirectory) FThisDirectory = File;
  if (File->IsParentDirectory) FParentDirectory = File;

  if ((!File->IsThisDirectory || IncludeThisDirectory) &&
      (!File->IsParentDirectory || IncludeParentDirectory))
  {
    TRemoteFileList::AddFile(File);
  }
  File->Terminal = Terminal;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::DuplicateTo(TRemoteFileList * Copy)
{
  TRemoteFileList::DuplicateTo(Copy);
  if (ThisDirectory && !IncludeThisDirectory)
  {
    Copy->AddFile(ThisDirectory->Duplicate(false));
  }
  if (ParentDirectory && !IncludeParentDirectory)
  {
    Copy->AddFile(ParentDirectory->Duplicate(false));
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectory::GetLoaded()
{
  return ((Terminal != NULL) && Terminal->Active && !Directory.IsEmpty());
}
//---------------------------------------------------------------------------
TStrings * __fastcall TRemoteDirectory::GetSelectedFiles()
{
  if (!FSelectedFiles)
  {
    FSelectedFiles = new TStringList();
  }
  else
  {
    FSelectedFiles->Clear();
  }

  for (int Index = 0; Index < Count; Index ++)
  {
    if (Files[Index]->Selected)
    {
      FSelectedFiles->Add(Files[Index]->FullFileName);
    }
  }

  return FSelectedFiles;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::SetIncludeParentDirectory(Boolean value)
{
  if (IncludeParentDirectory != value)
  {
    FIncludeParentDirectory = value;
    if (value && ParentDirectory)
    {
      assert(IndexOf(ParentDirectory) < 0);
      Add(ParentDirectory);
    }
    else if (!value && ParentDirectory)
    {
      assert(IndexOf(ParentDirectory) >= 0);
      Extract(ParentDirectory);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectory::SetIncludeThisDirectory(Boolean value)
{
  if (IncludeThisDirectory != value)
  {
    FIncludeThisDirectory = value;
    if (value && ThisDirectory)
    {
      assert(IndexOf(ThisDirectory) < 0);
      Add(ThisDirectory);
    }
    else if (!value && ThisDirectory)
    {
      assert(IndexOf(ThisDirectory) >= 0);
      Extract(ThisDirectory);
    }
  }
}
//===========================================================================
__fastcall TRemoteDirectoryCache::TRemoteDirectoryCache(): TStringList()
{
  FSection = new TCriticalSection();
  Sorted = true;
  Duplicates = dupError;
  CaseSensitive = true;
}
//---------------------------------------------------------------------------
__fastcall TRemoteDirectoryCache::~TRemoteDirectoryCache()
{
  Clear();
  delete FSection;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryCache::Clear()
{
  TGuard Guard(FSection);

  try
  {
    for (int Index = 0; Index < Count; Index++)
    {
      delete (TRemoteFileList *)Objects[Index];
      Objects[Index] = NULL;
    }
  }
  __finally
  {
    TStringList::Clear();
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryCache::GetIsEmpty() const
{
  TGuard Guard(FSection);

  return (const_cast<TRemoteDirectoryCache*>(this)->Count == 0);
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryCache::HasFileList(const AnsiString Directory)
{
  TGuard Guard(FSection);

  int Index = IndexOf(UnixExcludeTrailingBackslash(Directory));
  return (Index >= 0);
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryCache::HasNewerFileList(const AnsiString Directory,
  TDateTime Timestamp)
{
  TGuard Guard(FSection);

  int Index = IndexOf(UnixExcludeTrailingBackslash(Directory));
  if (Index >= 0)
  {
    TRemoteFileList * FileList = dynamic_cast<TRemoteFileList *>(Objects[Index]);
    if (FileList->Timestamp <= Timestamp)
    {
      Index = -1;
    }
  }
  return (Index >= 0);
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryCache::GetFileList(const AnsiString Directory,
  TRemoteFileList * FileList)
{
  TGuard Guard(FSection);

  int Index = IndexOf(UnixExcludeTrailingBackslash(Directory));
  bool Result = (Index >= 0);
  if (Result)
  {
    assert(Objects[Index] != NULL);
    dynamic_cast<TRemoteFileList *>(Objects[Index])->DuplicateTo(FileList);
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryCache::AddFileList(TRemoteFileList * FileList)
{
  // file list cannot be cached already with only one thread, but it can be
  // when directory is loaded by secondary terminal   
  ClearFileList(FileList->Directory, false);
  
  assert(FileList);
  TRemoteFileList * Copy = new TRemoteFileList();
  FileList->DuplicateTo(Copy);

  {
    TGuard Guard(FSection);

    AddObject(Copy->Directory, Copy);
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryCache::ClearFileList(AnsiString Directory, bool SubDirs)
{
  TGuard Guard(FSection);

  Directory = UnixExcludeTrailingBackslash(Directory);
  int Index = IndexOf(Directory);
  if (Index >= 0)
  {
    Delete(Index);
  }
  if (SubDirs)
  {
    Directory = UnixIncludeTrailingBackslash(Directory);
    Index = Count-1;
    while (Index >= 0)
    {
      if (Strings[Index].SubString(1, Directory.Length()) == Directory)
      {
        Delete(Index);
      }
      Index--;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryCache::Delete(int Index)
{
  delete (TRemoteFileList *)Objects[Index];
  TStringList::Delete(Index);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TRemoteDirectoryChangesCache::TRemoteDirectoryChangesCache() :
  TStringList()
{
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryChangesCache::Clear()
{
  TStringList::Clear();
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryChangesCache::GetIsEmpty() const
{
  return (const_cast<TRemoteDirectoryChangesCache*>(this)->Count == 0);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryChangesCache::AddDirectoryChange(
  const AnsiString SourceDir, const AnsiString Change,
  const AnsiString TargetDir)
{
  assert(!TargetDir.IsEmpty());
  Values[TargetDir] = "//";
  if (TTerminal::ExpandFileName(Change, SourceDir) != TargetDir)
  {
    AnsiString Key;
    if (DirectoryChangeKey(SourceDir, Change, Key))
    {
      Values[Key] = TargetDir;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryChangesCache::ClearDirectoryChange(
  AnsiString SourceDir)
{
  for (int Index = 0; Index < Count; Index++)
  {
    if (Names[Index].SubString(1, SourceDir.Length()) == SourceDir)
    {
      Delete(Index);
      Index--;
    }
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryChangesCache::GetDirectoryChange(
  const AnsiString SourceDir, const AnsiString Change, AnsiString & TargetDir)
{
  AnsiString Key;
  bool Result;
  Key = TTerminal::ExpandFileName(Change, SourceDir);
  Result = (IndexOfName(Key) >= 0);
  if (Result)
  {
    TargetDir = Values[Key];
    // TargetDir is not "//" here only when Change is full path to symbolic link
    if (TargetDir == "//")
    {
      TargetDir = Key;
    }
  }
  else
  {
    Result = DirectoryChangeKey(SourceDir, Change, Key);
    if (Result)
    {
      AnsiString Directory = Values[Key];
      Result = !Directory.IsEmpty();
      if (Result)
      {
        TargetDir = Directory;
      }
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryChangesCache::Serialize(AnsiString & Data)
{
  Data = "A" + Text;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteDirectoryChangesCache::Deserialize(const AnsiString Data)
{
  if (Data.IsEmpty())
  {
    Text = "";
  }
  else
  {
    Text = Data.c_str() + 1;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteDirectoryChangesCache::DirectoryChangeKey(
  const AnsiString SourceDir, const AnsiString Change, AnsiString & Key)
{
  bool Result = !Change.IsEmpty();
  if (Result)
  {
    bool Absolute = TTerminal::IsAbsolutePath(Change);
    Result = !SourceDir.IsEmpty() || Absolute;
    if (Result)
    {
      Key = Absolute ? Change : SourceDir + "," + Change;
    }
  }
  return Result;
}
//=== TRights ---------------------------------------------------------------
const char TRights::BasicSymbols[] = "rwxrwxrwx";
const char TRights::CombinedSymbols[] = "--s--s--t";
const char TRights::ExtendedSymbols[] = "--S--S--T";
const char TRights::ModeGroups[] = "ugo";
//---------------------------------------------------------------------------
__fastcall TRights::TRights()
{
  FAllowUndef = false;
  FSet = 0;
  FUnset = 0;
  Number = 0;
}
//---------------------------------------------------------------------------
__fastcall TRights::TRights(unsigned short ANumber)
{
  FAllowUndef = false;
  FSet = 0;
  FUnset = 0;
  Number = ANumber;
}
//---------------------------------------------------------------------------
__fastcall TRights::TRights(const TRights & Source)
{
  Assign(&Source);
}
//---------------------------------------------------------------------------
void __fastcall TRights::Assign(const TRights * Source)
{
  FAllowUndef = Source->AllowUndef;
  FSet = Source->FSet;
  FUnset = Source->FUnset;
  FText = Source->FText;
}
//---------------------------------------------------------------------------
TRights::TFlag __fastcall TRights::RightToFlag(TRights::TRight Right)
{
  return static_cast<TFlag>(1 << (rrLast - Right));
}
//---------------------------------------------------------------------------
bool __fastcall TRights::operator ==(const TRights & rhr) const
{
  if (AllowUndef || rhr.AllowUndef)
  {
    for (int Right = rrFirst; Right <= rrLast; Right++)
    {
      if (RightUndef[static_cast<TRight>(Right)] !=
            rhr.RightUndef[static_cast<TRight>(Right)])
      {
        return false;
      }
    }
    return true;
  }
  else
  {
    return (Number == rhr.Number);
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRights::operator ==(unsigned short rhr) const
{
  return (Number == rhr);
}
//---------------------------------------------------------------------------
bool __fastcall TRights::operator !=(const TRights & rhr) const
{
  return !(*this == rhr);
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator =(unsigned short rhr)
{
  Number = rhr;
  return *this;
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator =(const TRights & rhr)
{
  Assign(&rhr);
  return *this;
}
//---------------------------------------------------------------------------
TRights __fastcall TRights::operator ~() const
{
  TRights Result(static_cast<unsigned short>(~Number));
  return Result;
}
//---------------------------------------------------------------------------
TRights __fastcall TRights::operator &(const TRights & rhr) const
{
  TRights Result(*this);
  Result &= rhr;
  return Result;
}
//---------------------------------------------------------------------------
TRights __fastcall TRights::operator &(unsigned short rhr) const
{
  TRights Result(*this);
  Result &= rhr;
  return Result;
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator &=(const TRights & rhr)
{
  if (AllowUndef || rhr.AllowUndef)
  {
    for (int Right = rrFirst; Right <= rrLast; Right++)
    {
      if (RightUndef[static_cast<TRight>(Right)] !=
            rhr.RightUndef[static_cast<TRight>(Right)])
      {
        RightUndef[static_cast<TRight>(Right)] = rsUndef;
      }
    }
  }
  else
  {
    Number &= rhr.Number;
  }
  return *this;
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator &=(unsigned short rhr)
{
  Number &= rhr;
  return *this;
}
//---------------------------------------------------------------------------
TRights __fastcall TRights::operator |(const TRights & rhr) const
{
  TRights Result(*this);
  Result |= rhr;
  return Result;
}
//---------------------------------------------------------------------------
TRights __fastcall TRights::operator |(unsigned short rhr) const
{
  TRights Result(*this);
  Result |= rhr;
  return Result;
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator |=(const TRights & rhr)
{
  Number |= rhr.Number;
  return *this;
}
//---------------------------------------------------------------------------
TRights & __fastcall TRights::operator |=(unsigned short rhr)
{
  Number |= rhr;
  return *this;
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetAllowUndef(bool value)
{
  if (FAllowUndef != value)
  {
    assert(!value || ((FSet | FUnset) == rfAllSpecials));
    FAllowUndef = value;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetText(const AnsiString & value)
{
  if (value != Text)
  {
    if ((value.Length() != TextLen) ||
        (!AllowUndef && (value.Pos(UndefSymbol) > 0)) ||
        (value.Pos(" ") > 0))
    {
      throw Exception(FMTLOAD(RIGHTS_ERROR, (value)));
    }

    FSet = 0;
    FUnset = 0;
    int Flag = 00001;
    int ExtendedFlag = 01000;
    bool KeepText = false;
    for (int i = TextLen; i >= 1; i--)
    {
      if (value[i] == UnsetSymbol)
      {
        FUnset |= static_cast<unsigned short>(Flag | ExtendedFlag);
      }
      else if (value[i] == UndefSymbol)
      {
        // do nothing
      }
      else if (value[i] == CombinedSymbols[i - 1])
      {
        FSet |= static_cast<unsigned short>(Flag | ExtendedFlag);
      }
      else if (value[i] == ExtendedSymbols[i - 1])
      {
        FSet |= static_cast<unsigned short>(ExtendedFlag);
        FUnset |= static_cast<unsigned short>(Flag);
      }
      else
      {
        if (value[i] != BasicSymbols[i - 1])
        {
          KeepText = true;
        }
        FSet |= static_cast<unsigned short>(Flag);
        FUnset |= static_cast<unsigned short>(ExtendedFlag);
      }

      Flag <<= 1;
      if (i % 3 == 1)
      {
        ExtendedFlag <<= 1;
      }
    }

    FText = KeepText ? value : AnsiString();
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRights::GetText() const
{
  if (!FText.IsEmpty())
  {
    return FText;
  }
  else
  {
    AnsiString Result;
    Result.SetLength(TextLen);

    int Flag = 00001;
    int ExtendedFlag = 01000;
    bool ExtendedPos = true;
    char Symbol;
    int i = TextLen;
    while (i >= 1)
    {
      if (ExtendedPos &&
          ((FSet & (Flag | ExtendedFlag)) == (Flag | ExtendedFlag)))
      {
        Symbol = CombinedSymbols[i - 1];
      }
      else if ((FSet & Flag) != 0)
      {
        Symbol = BasicSymbols[i - 1];
      }
      else if (ExtendedPos && ((FSet & ExtendedFlag) != 0))
      {
        Symbol = ExtendedSymbols[i - 1];
      }
      else if ((!ExtendedPos && ((FUnset & Flag) == Flag)) ||
        (ExtendedPos && ((FUnset & (Flag | ExtendedFlag)) == (Flag | ExtendedFlag))))
      {
        Symbol = UnsetSymbol;
      }
      else
      {
        Symbol = UndefSymbol;
      }

      Result[i] = Symbol;

      Flag <<= 1;
      i--;
      ExtendedPos = ((i % 3) == 0);
      if (ExtendedPos)
      {
        ExtendedFlag <<= 1;
      }
    }
    return Result;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetOctal(AnsiString value)
{
  AnsiString AValue(value);
  if (AValue.Length() == 3)
  {
    AValue = "0" + AValue;
  }

  if (Octal != AValue)
  {
    bool Correct = (AValue.Length() == 4);
    if (Correct)
    {
      for (int i = 1; (i <= AValue.Length()) && Correct; i++)
      {
        Correct = (AValue[i] >= '0') && (AValue[i] <= '7');
      }
    }

    if (!Correct)
    {
      throw Exception(FMTLOAD(INVALID_OCTAL_PERMISSIONS, (value)));
    }

    Number = static_cast<unsigned short>(
      ((AValue[1] - '0') << 9) +
      ((AValue[2] - '0') << 6) +
      ((AValue[3] - '0') << 3) +
      ((AValue[4] - '0') << 0));
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRights::GetOctal() const
{
  AnsiString Result;
  unsigned short N = NumberSet; // used to be "Number"
  Result.SetLength(4);
  Result[1] = static_cast<char>('0' + ((N & 07000) >> 9));
  Result[2] = static_cast<char>('0' + ((N & 00700) >> 6));
  Result[3] = static_cast<char>('0' + ((N & 00070) >> 3));
  Result[4] = static_cast<char>('0' + ((N & 00007) >> 0));

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetNumber(unsigned short value)
{
  if ((FSet != value) || ((FSet | FUnset) != rfAllSpecials))
  {
    FSet = value;
    FUnset = static_cast<unsigned short>(rfAllSpecials & ~FSet);
    FText = "";
  }
}
//---------------------------------------------------------------------------
unsigned short __fastcall TRights::GetNumber() const
{
  assert(!IsUndef);
  return FSet;
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetRight(TRight Right, bool value)
{
  RightUndef[Right] = (value ? rsYes : rsNo);
}
//---------------------------------------------------------------------------
bool __fastcall TRights::GetRight(TRight Right) const
{
  TState State = RightUndef[Right];
  assert(State != rsUndef);
  return (State == rsYes);
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetRightUndef(TRight Right, TState value)
{
  if (value != RightUndef[Right])
  {
    assert((value != rsUndef) || AllowUndef);

    TFlag Flag = RightToFlag(Right);

    switch (value)
    {
      case rsYes:
        FSet |= Flag;
        FUnset &= ~Flag;
        break;

      case rsNo:
        FSet &= ~Flag;
        FUnset |= Flag;
        break;

      case rsUndef:
      default:
        FSet &= ~Flag;
        FUnset &= ~Flag;
        break;
    }

    FText = "";
  }
}
//---------------------------------------------------------------------------
TRights::TState __fastcall TRights::GetRightUndef(TRight Right) const
{
  TFlag Flag = RightToFlag(Right);
  TState Result;

  if ((FSet & Flag) != 0)
  {
    Result = rsYes;
  }
  else if ((FUnset & Flag) != 0)
  {
    Result = rsNo;
  }
  else
  {
    Result = rsUndef;
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRights::SetReadOnly(bool value)
{
  Right[rrUserWrite] = !value;
  Right[rrGroupWrite] = !value;
  Right[rrOtherWrite] = !value;
}
//---------------------------------------------------------------------------
bool  __fastcall TRights::GetReadOnly()
{
  return Right[rrUserWrite] && Right[rrGroupWrite] && Right[rrOtherWrite];
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRights::GetSimplestStr() const
{
  return IsUndef ? ModeStr : Octal;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRights::GetModeStr() const
{
  AnsiString Result;
  AnsiString SetModeStr, UnsetModeStr;
  TRight Right;
  int Index;

  for (int Group = 0; Group < 3; Group++)
  {
    SetModeStr = "";
    UnsetModeStr = "";
    for (int Mode = 0; Mode < 3; Mode++)
    {
      Index = (Group * 3) + Mode;
      Right = static_cast<TRight>(rrUserRead + Index);
      switch (RightUndef[Right])
      {
        case rsYes:
          SetModeStr += BasicSymbols[Index];
          break;

        case rsNo:
          UnsetModeStr += BasicSymbols[Index];
          break;
      }
    }

    Right = static_cast<TRight>(rrUserIDExec + Group);
    Index = (Group * 3) + 2;
    switch (RightUndef[Right])
    {
      case rsYes:
        SetModeStr += CombinedSymbols[Index];
        break;

      case rsNo:
        UnsetModeStr += CombinedSymbols[Index];
        break;
    }

    if (!SetModeStr.IsEmpty() || !UnsetModeStr.IsEmpty())
    {
      if (!Result.IsEmpty())
      {
        Result += ',';
      }
      Result += ModeGroups[Group];
      if (!SetModeStr.IsEmpty())
      {
        Result += "+" + SetModeStr;
      }
      if (!UnsetModeStr.IsEmpty())
      {
        Result += "-" + UnsetModeStr;
      }
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TRights::AddExecute()
{
  for (int Group = 0; Group < 3; Group++)
  {
    if ((RightUndef[static_cast<TRight>(rrUserRead + (Group * 3))] == rsYes) ||
        (RightUndef[static_cast<TRight>(rrUserWrite + (Group * 3))] == rsYes))
    {
      Right[static_cast<TRight>(rrUserExec + (Group * 3))] = true;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TRights::AllUndef()
{
  if ((FSet != 0) || (FUnset != 0))
  {
    FSet = 0;
    FUnset = 0;
    FText = "";
  }
}
//---------------------------------------------------------------------------
bool __fastcall TRights::GetIsUndef() const
{
  return ((FSet | FUnset) != rfAllSpecials);
}
//---------------------------------------------------------------------------
__fastcall TRights::operator unsigned short() const
{
  return Number;
}
//---------------------------------------------------------------------------
__fastcall TRights::operator unsigned long() const
{
  return Number;
}
//=== TRemoteProperties -------------------------------------------------------
__fastcall TRemoteProperties::TRemoteProperties()
{
  Valid.Clear();
  AddXToDirectories = false;
  Rights.AllowUndef = false;
  Rights.Number = 0;
  Group = "";
  Owner = "";
  Recursive = false;
};
//---------------------------------------------------------------------------
bool __fastcall TRemoteProperties::operator ==(const TRemoteProperties & rhp) const
{
  bool Result = (Valid == rhp.Valid && Recursive == rhp.Recursive);

  if (Result)
  {
    if ((Valid.Contains(vpRights) &&
          (Rights != rhp.Rights || AddXToDirectories != rhp.AddXToDirectories)) ||
        (Valid.Contains(vpOwner) && Owner != rhp.Owner) ||
        (Valid.Contains(vpGroup) && Group != rhp.Group) ||
        (Valid.Contains(vpModification) && (Modification != rhp.Modification)) ||
        (Valid.Contains(vpLastAccess) && (LastAccess != rhp.LastAccess)))
    {
      Result = false;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteProperties::operator !=(const TRemoteProperties & rhp) const
{
  return !(*this == rhp);
}
//---------------------------------------------------------------------------
TRemoteProperties __fastcall TRemoteProperties::CommonProperties(TStrings * FileList)
{
  // TODO: Modification and LastAccess
  TRemoteProperties CommonProperties;
  for (int Index = 0; Index < FileList->Count; Index++)
  {
    TRemoteFile * File = (TRemoteFile *)(FileList->Objects[Index]);
    assert(File);
    if (!Index)
    {
      CommonProperties.Rights = *(File->Rights);
      // previously we allowed undef implicitly for directories,
      // now we do it explicitly in properties dialog and only in combination
      // with "recursive" option
      CommonProperties.Rights.AllowUndef = File->Rights->IsUndef;
      CommonProperties.Valid << vpRights;
      if (!File->Owner.IsEmpty())
      {
        CommonProperties.Owner = File->Owner;
        CommonProperties.Valid << vpOwner;
      }
      if (!File->Group.IsEmpty())
      {
        CommonProperties.Group = File->Group;
        CommonProperties.Valid << vpGroup;
      }
    }
    else
    {
      CommonProperties.Rights.AllowUndef = True;
      CommonProperties.Rights &= *File->Rights;
      if (CommonProperties.Owner != File->Owner)
      {
        CommonProperties.Owner = "";
        CommonProperties.Valid >> vpOwner;
      };
      if (CommonProperties.Group != File->Group)
      {
        CommonProperties.Group = "";
        CommonProperties.Valid >> vpGroup;
      };
    }
  }
  return CommonProperties;
}
//---------------------------------------------------------------------------
TRemoteProperties __fastcall TRemoteProperties::ChangedProperties(
  const TRemoteProperties & OriginalProperties, TRemoteProperties NewProperties)
{
  // TODO: Modification and LastAccess
  if (!NewProperties.Recursive)
  {
    if (NewProperties.Rights == OriginalProperties.Rights &&
        !NewProperties.AddXToDirectories)
    {
      NewProperties.Valid >> vpRights;
    }

    if (NewProperties.Group == OriginalProperties.Group)
    {
      NewProperties.Valid >> vpGroup;
    }

    if (NewProperties.Owner == OriginalProperties.Owner)
    {
      NewProperties.Valid >> vpOwner;
    }
  }
  return NewProperties;
}

