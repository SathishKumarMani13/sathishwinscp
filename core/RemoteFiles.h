//---------------------------------------------------------------------------
#ifndef RemoteFilesH
#define RemoteFilesH
//---------------------------------------------------------------------------
enum TModificationFmt { mfMDHM, mfMDY, mfFull };
//---------------------------------------------------------------------------
#define SYMLINKSTR " -> "
#define PARENTDIRECTORY ".."
#define THISDIRECTORY "."
#define ROOTDIRECTORY "/"
#define FILETYPE_SYMLINK 'L'
#define FILETYPE_DIRECTORY 'D'
//---------------------------------------------------------------------------
class TTerminal;
class TRights;
class TRemoteFileList;
//---------------------------------------------------------------------------
class TRemoteFile : public TPersistent
{
private:
  TRemoteFileList * FDirectory;
  AnsiString FOwner;
  TModificationFmt FModificationFmt;
  __int64 FSize;
  AnsiString FFileName;
  Integer FINodeBlocks;
  TDateTime FModification;
  TDateTime FLastAccess;
  AnsiString FGroup;
  Integer FIconIndex;
  Boolean FIsSymLink;
  TRemoteFile * FLinkedFile;
  TRemoteFile * FLinkedByFile;
  AnsiString FLinkTo;
  TRights *FRights;
  TTerminal *FTerminal;
  Char FType;
  bool FSelected;
  bool FCyclicLink;
  int __fastcall GetAttr();
  bool __fastcall GetBrokenLink();
  bool __fastcall GetIsDirectory() const;
  TRemoteFile * __fastcall GetLinkedFile();
  void __fastcall SetLinkedFile(TRemoteFile * value);
  AnsiString __fastcall GetModificationStr();
  void __fastcall SetModification(const TDateTime & value);
  void __fastcall SetListingStr(AnsiString value);
  AnsiString __fastcall GetListingStr();
  AnsiString __fastcall GetRightsStr();
  char __fastcall GetType() const;
  void __fastcall SetType(char AType);
  void __fastcall SetTerminal(TTerminal * value);
  void __fastcall SetRights(TRights * value);
  AnsiString __fastcall GetFullFileName();
  int __fastcall GetIconIndex();
  bool __fastcall GetIsHidden();
  bool __fastcall GetIsParentDirectory();
  bool __fastcall GetIsThisDirectory();
  bool __fastcall GetIsInaccesibleDirectory();
  AnsiString __fastcall GetExtension();
  AnsiString __fastcall GetUserModificationStr();

protected:
  void __fastcall FindLinkedFile();

public:
  __fastcall TRemoteFile(TRemoteFile * ALinkedByFile = NULL);
  virtual __fastcall ~TRemoteFile();
  TRemoteFile * __fastcall Duplicate();

  void __fastcall ShiftTime(const TDateTime & Difference);
  void __fastcall Complete();

  __property int Attr = { read = GetAttr };
  __property bool BrokenLink = { read = GetBrokenLink };
  __property TRemoteFileList * Directory = { read = FDirectory, write = FDirectory };
  __property AnsiString RightsStr = { read = GetRightsStr };
  __property __int64 Size = { read = FSize, write = FSize };
  __property AnsiString Owner = { read = FOwner, write = FOwner };
  __property AnsiString Group = { read = FGroup, write = FGroup };
  __property AnsiString FileName = { read = FFileName, write = FFileName };
  __property int INodeBlocks = { read = FINodeBlocks };
  __property TDateTime Modification = { read = FModification, write = SetModification };
  __property AnsiString ModificationStr = { read = GetModificationStr };
  __property AnsiString UserModificationStr = { read = GetUserModificationStr };
  __property TDateTime LastAccess = { read = FLastAccess, write = FLastAccess };
  __property bool IsSymLink = { read = FIsSymLink };
  __property bool IsDirectory = { read = GetIsDirectory };
  __property TRemoteFile * LinkedFile = { read = GetLinkedFile, write = SetLinkedFile };
  __property AnsiString LinkTo = { read = FLinkTo, write = FLinkTo };
  __property AnsiString ListingStr = { read = GetListingStr, write = SetListingStr };
  __property TRights * Rights = { read = FRights, write = SetRights };
  __property TTerminal * Terminal = { read = FTerminal, write = SetTerminal };
  __property Char Type = { read = GetType, write = SetType };
  __property bool Selected  = { read=FSelected, write=FSelected };
  __property AnsiString FullFileName  = { read=GetFullFileName };
  __property int IconIndex = { read = GetIconIndex };
  __property bool IsHidden = { read = GetIsHidden };
  __property bool IsParentDirectory = { read = GetIsParentDirectory };
  __property bool IsThisDirectory = { read = GetIsThisDirectory };
  __property bool IsInaccesibleDirectory  = { read=GetIsInaccesibleDirectory };
  __property AnsiString Extension  = { read=GetExtension };
};
//---------------------------------------------------------------------------
class TRemoteParentDirectory : public TRemoteFile
{
public:
  __fastcall TRemoteParentDirectory();
};
//---------------------------------------------------------------------------
class TRemoteFileList : public TObjectList
{
friend class TSCPFileSystem;
friend class TSFTPFileSystem;
protected:
  AnsiString FDirectory;
  TRemoteFile * __fastcall GetFiles(Integer Index);
  virtual void __fastcall SetDirectory(AnsiString value);
  AnsiString __fastcall GetFullDirectory();
  Boolean __fastcall GetIsRoot();
  TRemoteFile * __fastcall GetParentDirectory();
  AnsiString __fastcall GetParentPath();
  __int64 __fastcall GetTotalSize();

  virtual void __fastcall Clear();
public:
  __fastcall TRemoteFileList();
  TRemoteFile * __fastcall FindFile(const AnsiString &FileName);
  virtual void __fastcall DuplicateTo(TRemoteFileList * Copy);
  virtual void __fastcall AddFile(TRemoteFile * File);
  __property AnsiString Directory = { read = FDirectory, write = SetDirectory };
  __property TRemoteFile * Files[Integer Index] = { read = GetFiles };
  __property AnsiString FullDirectory  = { read=GetFullDirectory };
  __property Boolean IsRoot = { read = GetIsRoot };
  __property AnsiString ParentPath = { read = GetParentPath };
  __property __int64 TotalSize = { read = GetTotalSize };
};
//---------------------------------------------------------------------------
class TRemoteDirectory : public TRemoteFileList
{
friend class TSCPFileSystem;
friend class TSFTPFileSystem;
private:
  Boolean FIncludeParentDirectory;
  Boolean FIncludeThisDirectory;
  TTerminal * FTerminal;
  TStrings * FSelectedFiles;
  TRemoteFile * FParentDirectory;
  TRemoteFile * FThisDirectory;
  virtual void __fastcall SetDirectory(AnsiString value);
  TStrings * __fastcall GetSelectedFiles();
  Boolean __fastcall GetLoaded();
  void __fastcall SetIncludeParentDirectory(Boolean value);
  void __fastcall SetIncludeThisDirectory(Boolean value);
protected:
  virtual void __fastcall Clear();
public:
  __fastcall TRemoteDirectory(TTerminal * aTerminal);
  virtual void __fastcall AddFile(TRemoteFile * File);
  virtual void __fastcall DuplicateTo(TRemoteFileList * Copy);
  __property TTerminal * Terminal = { read = FTerminal, write = FTerminal };
  __property TStrings * SelectedFiles  = { read=GetSelectedFiles };
  __property Boolean IncludeParentDirectory = { read = FIncludeParentDirectory, write = SetIncludeParentDirectory };
  __property Boolean IncludeThisDirectory = { read = FIncludeThisDirectory, write = SetIncludeThisDirectory };
  __property Boolean Loaded = { read = GetLoaded };
  __property TRemoteFile * ParentDirectory = { read = FParentDirectory };
  __property TRemoteFile * ThisDirectory = { read = FThisDirectory };
};
//---------------------------------------------------------------------------
class TRemoteDirectoryCache : private TStringList
{
public:
  __fastcall TRemoteDirectoryCache();
  virtual __fastcall ~TRemoteDirectoryCache();
  TRemoteFileList * __fastcall GetFileList(const AnsiString Directory);
  void __fastcall AddFileList(TRemoteFileList * FileList);
  void __fastcall ClearFileList(AnsiString Directory, bool SubDirs);
  void __fastcall Clear();

  __property bool IsEmpty = { read = GetIsEmpty };
protected:
  virtual void __fastcall Delete(int Index);
private:
  bool __fastcall GetIsEmpty() const;
};
//---------------------------------------------------------------------------
class TRemoteDirectoryChangesCache : private TStringList
{
public:
  __fastcall TRemoteDirectoryChangesCache();

  void __fastcall AddDirectoryChange(const AnsiString SourceDir,
    const AnsiString Change, const AnsiString TargetDir);
  void __fastcall ClearDirectoryChange(AnsiString SourceDir);
  bool __fastcall GetDirectoryChange(const AnsiString SourceDir,
    const AnsiString Change, AnsiString & TargetDir);
  void __fastcall Clear();

  void __fastcall Serialize(AnsiString & Data);
  void __fastcall Deserialize(const AnsiString Data);

  __property bool IsEmpty = { read = GetIsEmpty };

private:
  static bool __fastcall DirectoryChangeKey(const AnsiString SourceDir,
    const AnsiString Change, AnsiString & Key);
  bool __fastcall GetIsEmpty() const;
};
//---------------------------------------------------------------------------
class TRights
{
public:
  static const int TextLen = 9;
  static const char UndefSymbol = '$';
  static const char UnsetSymbol = '-';
  static const char BasicSymbols[];
  static const char CombinedSymbols[];
  static const char ExtendedSymbols[];
  static const char ModeGroups[];
  enum TRight {
    rrUserIDExec, rrGroupIDExec, rrStickyBit,
    rrUserRead, rrUserWrite, rrUserExec,
    rrGroupRead, rrGroupWrite, rrGroupExec,
    rrOtherRead, rrOtherWrite, rrOtherExec,
    rrFirst = rrUserIDExec, rrLast = rrOtherExec };
  enum TFlag {
    rfSetUID =    04000, rfSetGID =      02000, rfStickyBit = 01000,
    rfUserRead =  00400, rfUserWrite =   00200, rfUserExec =  00100,
    rfGroupRead = 00040, rfGroupWrite =  00020, rfGroupExec = 00010,
    rfOtherRead = 00004, rfOtherWrite =  00002, rfOtherExec = 00001,
    rfRead =      00444, rfWrite =       00222, rfExec =      00111,
    rfNo =        00000, rfDefault =     00644, rfAll =       00777,
    rfSpecials =  07000, rfAllSpecials = 07777 };
  enum TUnsupportedFlag {
    rfDirectory  = 040000 };
  enum TState { rsNo, rsYes, rsUndef };

public:
  static TFlag __fastcall RightToFlag(TRight Right);

  __fastcall TRights();
  __fastcall TRights(const TRights & Source);
  __fastcall TRights(unsigned short Number);

  void __fastcall Assign(const TRights * Source);
  void __fastcall AddExecute();
  void __fastcall AllUndef();

  bool __fastcall operator ==(const TRights & rhr) const;
  bool __fastcall operator ==(unsigned short rhr) const;
  bool __fastcall operator !=(const TRights & rhr) const;
  TRights & __fastcall operator =(const TRights & rhr);
  TRights & __fastcall operator =(unsigned short rhr);
  TRights __fastcall operator ~() const;
  TRights __fastcall operator &(unsigned short rhr) const;
  TRights __fastcall operator &(const TRights & rhr) const;
  TRights & __fastcall operator &=(unsigned short rhr);
  TRights & __fastcall operator &=(const TRights & rhr);
  TRights __fastcall operator |(unsigned short rhr) const;
  TRights __fastcall operator |(const TRights & rhr) const;
  TRights & __fastcall operator |=(unsigned short rhr);
  TRights & __fastcall operator |=(const TRights & rhr);
  __fastcall operator unsigned short() const;
  __fastcall operator unsigned long() const;

  __property bool AllowUndef = { read = FAllowUndef, write = SetAllowUndef };
  __property bool IsUndef = { read = GetIsUndef };
  __property AnsiString ModeStr = { read = GetModeStr };
  __property AnsiString SimplestStr = { read = GetSimplestStr };
  __property AnsiString Octal = { read = GetOctal, write = SetOctal };
  __property unsigned short Number = { read = GetNumber, write = SetNumber };
  __property unsigned short NumberSet = { read = FSet };
  __property unsigned short NumberUnset = { read = FUnset };
  __property bool ReadOnly = { read = GetReadOnly, write = SetReadOnly };
  __property bool Right[TRight Right] = { read = GetRight, write = SetRight };
  __property TState RightUndef[TRight Right] = { read = GetRightUndef, write = SetRightUndef };
  __property AnsiString Text = { read = GetText, write = SetText };

private:
  bool FAllowUndef;
  unsigned short FSet;
  unsigned short FUnset;
  AnsiString FText;

  bool __fastcall GetIsUndef() const;
  AnsiString __fastcall GetModeStr() const;
  AnsiString __fastcall GetSimplestStr() const;
  void __fastcall SetNumber(unsigned short value);
  AnsiString __fastcall GetText() const;
  void __fastcall SetText(const AnsiString & value);
  void __fastcall SetOctal(AnsiString value);
  unsigned short __fastcall GetNumber() const;
  unsigned short __fastcall GetNumberSet() const;
  unsigned short __fastcall GetNumberUnset() const;
  AnsiString __fastcall GetOctal() const;
  bool __fastcall GetReadOnly();
  bool __fastcall GetRight(TRight Right) const;
  TState __fastcall GetRightUndef(TRight Right) const;
  void __fastcall SetAllowUndef(bool value);
  void __fastcall SetReadOnly(bool value);
  void __fastcall SetRight(TRight Right, bool value);
  void __fastcall SetRightUndef(TRight Right, TState value);
};
//---------------------------------------------------------------------------
enum TValidProperty { vpRights, vpGroup, vpOwner };
typedef Set<TValidProperty, vpRights, vpOwner> TValidProperties;
class TRemoteProperties
{
public:
  TValidProperties Valid;
  bool Recursive;
  TRights Rights;
  bool AddXToDirectories;
  AnsiString Group;
  AnsiString Owner;

  __fastcall TRemoteProperties();
  bool __fastcall operator ==(const TRemoteProperties & rhp) const;
  bool __fastcall operator !=(const TRemoteProperties & rhp) const;

  static TRemoteProperties __fastcall CommonProperties(TStrings * FileList);
  static TRemoteProperties __fastcall ChangedProperties(
    const TRemoteProperties & OriginalProperties, TRemoteProperties NewProperties);
};
//---------------------------------------------------------------------------
AnsiString __fastcall UnixIncludeTrailingBackslash(const AnsiString Path);
AnsiString __fastcall UnixExcludeTrailingBackslash(const AnsiString Path);
AnsiString __fastcall UnixExtractFileDir(const AnsiString Path);
AnsiString __fastcall UnixExtractFilePath(const AnsiString Path);
AnsiString __fastcall UnixExtractFileName(const AnsiString Path);
AnsiString __fastcall UnixExtractFileExt(const AnsiString Path);
Boolean __fastcall UnixComparePaths(const AnsiString Path1, const AnsiString Path2);
void __fastcall SkipPathComponent(const AnsiString & Text,
  int & SelStart, int & SelLength, bool Left, bool Unix);
//---------------------------------------------------------------------------
#endif

