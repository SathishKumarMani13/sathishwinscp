//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Properties.h"

#include <AssociatedStatusBar.hpp> // FormatBytes()

#include <VCLCommon.h>
#include <Common.h>
#include <Terminal.h>
#include <TextsWin.h>

#include "WinInterface.h"
//---------------------------------------------------------------------
#pragma link "PathLabel"
#pragma link "Rights"
#pragma link "RightsExt"
#pragma resource "*.dfm"
//---------------------------------------------------------------------
bool __fastcall DoPropertiesDialog(TStrings * FileList,
	const AnsiString Directory, TStrings * GroupList, TStrings * UserList,
	TRemoteProperties * Properties, int AllowedChanges,
  TTerminal * Terminal)
{
	bool Result;
  TPropertiesDialog * PropertiesDialog = new TPropertiesDialog(Application);
  try
  {
    PropertiesDialog->AllowedChanges = AllowedChanges;
    PropertiesDialog->Directory = Directory;
    PropertiesDialog->FileList = FileList;
    PropertiesDialog->GroupList = GroupList;
    PropertiesDialog->UserList = UserList;
    PropertiesDialog->FileProperties = *Properties;
    PropertiesDialog->Terminal = Terminal;
    
    Result = PropertiesDialog->Execute();
    if (Result)
    {
      *Properties = PropertiesDialog->FileProperties;
    }
  }
  __finally
  {
  	delete PropertiesDialog;
  }
  return Result;
}
//---------------------------------------------------------------------
__fastcall TPropertiesDialog::TPropertiesDialog(TComponent* AOwner)
	: TForm(AOwner)
{
  RightsFrame->OnChange = ControlChange;

  FGroupsSet = False;

  TSHFileInfo FileInfo;
  FShellImageList = new TImageList(this);
  FShellImageList->Handle = SHGetFileInfo("", 0, &FileInfo, sizeof(FileInfo),
      SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
  FShellImageList->ShareImages = True;

  FFileList = new TStringList();
  FAllowCalculateSize = false;
  FSizeNotCalculated = false;

  UseSystemSettings(this);
}
//---------------------------------------------------------------------------
__fastcall TPropertiesDialog::~TPropertiesDialog()
{
  delete FFileList;
  FFileList = NULL;
  delete FShellImageList;
  FShellImageList = NULL;
}
//---------------------------------------------------------------------
bool __fastcall TPropertiesDialog::Execute()
{
  bool Result;

  FPrevTerminalClose = NULL;;
  if (FTerminal)
  {
    FPrevTerminalClose = FTerminal->OnClose;
    // used instead of previous TTerminalManager::OnChangeTerminal
    FTerminal->OnClose = TerminalClose;
  }
  
  try
  {
    if (AllowedChanges & cpGroup) ActiveControl = GroupComboBox;
      else
    if (AllowedChanges & cpOwner) ActiveControl = OwnerComboBox;
      else
    if (AllowedChanges & cpMode) ActiveControl = RightsFrame;
      else ActiveControl = OkButton;
    UpdateControls();
    
    Result = (ShowModal() == mrOk);
  }
  __finally
  {
    if (FTerminal)
    {
      assert(FTerminal->OnClose == TerminalClose);
      FTerminal->OnClose = FPrevTerminalClose;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::TerminalClose(TObject * Sender)
{
  Close();
  Terminal = NULL;
  if (FPrevTerminalClose)
  {
    FPrevTerminalClose(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetFileList(TStrings * value)
{
  if (FFileList != value)
  {
    FFileList->Assign(value);
    LoadInfo();
    FGroupsSet = false;
    FUsersSet = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::LoadInfo()
{
  if (FFileList)
  {
    assert(FFileList > 0);
    bool Multiple = (FFileList->Count != 1);
    FAllowCalculateSize = false;
    FSizeNotCalculated = false;
    FileIconImage->Picture->Bitmap = NULL;
    __int64 FilesSize;

    RightsFrame->AllowUndef = Multiple;

    if (!Multiple)
    {
      TRemoteFile * File = (TRemoteFile *)(FFileList->Objects[0]);
      assert(File && FShellImageList);
      FShellImageList->GetIcon(File->IconIndex, FileIconImage->Picture->Icon);
      FileLabel->Caption = File->FileName;
      if (!FUsersSet)
      {
        OwnerComboBox->Items->Text = File->Owner;
      }
      if (!FGroupsSet)
      {
        GroupComboBox->Items->Text = File->Group;
      }
      FilesSize = File->Size;

      LinksToLabelLabel->Visible = File->IsSymLink;
      LinksToLabel->Visible = File->IsSymLink;
      if (File->IsSymLink)
      {
        LinksToLabel->Caption = File->LinkTo;
      }
      if (File->IsDirectory)
      {
        FAllowCalculateSize = true;
        FSizeNotCalculated = true;
      }

      RightsFrame->AllowAddXToDirectories = File->IsDirectory;
      Caption = FMTLOAD(PROPERTIES_FILE_CAPTION, (File->FileName));
      RecursiveCheck->Visible = File->IsDirectory;
      RecursiveBevel->Visible = File->IsDirectory;
    }
    else
    {
      Caption = FFileList->Count ?
        FMTLOAD(PROPERTIES_FILES_CAPTION, (FFileList->Strings[0])) : AnsiString();
      LinksToLabelLabel->Hide();
      LinksToLabel->Hide();

      TStrings *GroupList = new TStringList();
      ((TStringList*)GroupList)->Duplicates = dupIgnore;
      ((TStringList*)GroupList)->Sorted = True;
      TStrings *OwnerList = new TStringList();
      ((TStringList*)OwnerList)->Duplicates = dupIgnore;
      ((TStringList*)OwnerList)->Sorted = True;

      try
      {
        int Directories = 0;
        int Files = 0;
        int SymLinks = 0;
        FilesSize = 0;

        for (int Index = 0; Index < FFileList->Count; Index++)
        {
          TRemoteFile * File = (TRemoteFile *)(FFileList->Objects[Index]);
          assert(File);
          GroupList->Add(File->Group);
          OwnerList->Add(File->Owner);
          if (File->IsDirectory)
          {
            Directories++;
            FAllowCalculateSize = true;
            FSizeNotCalculated = true;
          }
          else
          {
            Files++;
          }

          if (File->IsSymLink)
          {
            SymLinks++;
          }
          FilesSize += File->Size;
        }

        AnsiString FilesStr;
        if (Files)
        {
          FilesStr = (Files == 1) ? FMTLOAD(PROPERTIES_FILE, (Files)) :
            FMTLOAD(PROPERTIES_FILES, (Files));
          if (Directories)
          {
            FilesStr = FORMAT("%s, ", (FilesStr));
          }
        }
        if (Directories)
        {
          FilesStr += (Directories == 1) ? FMTLOAD(PROPERTIES_DIRECTORY, (Directories)) :
            FMTLOAD(PROPERTIES_DIRECTORIES, (Directories));
        }
        if (SymLinks)
        {
          AnsiString SymLinksStr;
          SymLinksStr = SymLinks == 1 ? FMTLOAD(PROPERTIES_SYMLINK, (SymLinks)) :
            FMTLOAD(PROPERTIES_SYMLINKS, (SymLinks));
          FilesStr = FORMAT("%s (%s)", (FilesStr, SymLinksStr));
        }
        FileLabel->Caption = FilesStr;

        if (!FUsersSet)
        {
          OwnerComboBox->Items = OwnerList;
        }
        if (!FGroupsSet)
        {
          GroupComboBox->Items = GroupList;
        }
        RightsFrame->AllowAddXToDirectories = (Directories > 0);
        RecursiveCheck->Visible = (Directories > 0);
        RecursiveBevel->Visible = (Directories > 0);

      }
      __finally
      {
        delete GroupList;
        delete OwnerList;
      }
    }

    LoadSize(FilesSize);

    FilesIconImage->Visible = Multiple;
    FileIconImage->Visible = !Multiple;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::LoadSize(__int64 FilesSize)
{
  AnsiString SizeStr;
  if (FSizeNotCalculated)
  {
    SizeStr = LoadStr(PROPERTIES_UNKNOWN_SIZE);
  }
  else
  {
    SizeStr = FormatBytes(FilesSize);
    if (FilesSize >= FormatBytesAbove)
    {
      __int64 PrevFormatBytesAbove = FormatBytesAbove;
      FormatBytesAbove = FilesSize + 1;
      try
      {
        SizeStr = FORMAT("%s (%s)", (SizeStr, FormatBytes(FilesSize)));
      }
      __finally
      {
        FormatBytesAbove = PrevFormatBytesAbove;
      }
    }
  }
  SizeLabel->Caption = SizeStr;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetDirectory(AnsiString value)
{
  LocationLabel->Caption = value;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPropertiesDialog::GetDirectory()
{
  return LocationLabel->Caption;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetFileProperties(TRemoteProperties value)
{
  TValidProperties Valid;
  if (value.Valid.Contains(vpRights) && FAllowedChanges & cpMode) Valid << vpRights;
  if (value.Valid.Contains(vpOwner) && FAllowedChanges & cpOwner) Valid << vpOwner;
  if (value.Valid.Contains(vpGroup) && FAllowedChanges & cpGroup) Valid << vpGroup;
  FOrigProperties = value;
  FOrigProperties.Valid = Valid;
  FOrigProperties.Recursive = false;
  
  if (value.Valid.Contains(vpRights))
  {
    RightsFrame->Rights = value.Rights;
    RightsFrame->AddXToDirectories = value.AddXToDirectories;
  }
  else
  {
    RightsFrame->Rights = TRights();
    RightsFrame->AddXToDirectories = false;
  }
  GroupComboBox->Text = value.Valid.Contains(vpGroup) ? value.Group : AnsiString();
  OwnerComboBox->Text = value.Valid.Contains(vpOwner) ? value.Owner : AnsiString();
  RecursiveCheck->Checked = value.Recursive;
}
//---------------------------------------------------------------------------
TRemoteProperties __fastcall TPropertiesDialog::GetFileProperties()
{
  TRemoteProperties Result;

  if (AllowedChanges & cpMode)
  {
    Result.Valid << vpRights;
    Result.Rights = RightsFrame->Rights;
    Result.AddXToDirectories = RightsFrame->AddXToDirectories;
  }

  #define STORE_NAME(PROPERTY) \
    if (!PROPERTY ## ComboBox->Text.Trim().IsEmpty() && \
        AllowedChanges & cp ## PROPERTY) \
    { \
      Result.Valid << vp ## PROPERTY; \
      Result.PROPERTY = PROPERTY ## ComboBox->Text.Trim(); \
    }
  STORE_NAME(Group);
  STORE_NAME(Owner);
  #undef STORE_NAME

  Result.Recursive = RecursiveCheck->Checked;

  return Result;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::ControlChange(TObject * /*Sender*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::UpdateControls()
{
  EnableControl(OkButton,
    // group name is specified or we set multiple-file properties and
    // no valid group was specified (there are at least two different groups)
    (!GroupComboBox->Text.Trim().IsEmpty() ||
     (Multiple && !FOrigProperties.Valid.Contains(vpGroup)) ||
     (FOrigProperties.Group == GroupComboBox->Text)) &&
    // same but with owner
    (!OwnerComboBox->Text.Trim().IsEmpty() ||
     (Multiple && !FOrigProperties.Valid.Contains(vpOwner)) ||
     (FOrigProperties.Owner == OwnerComboBox->Text)) &&
    ((FileProperties != FOrigProperties) || RecursiveCheck->Checked)
  );
  EnableControl(GroupComboBox, FAllowedChanges & cpGroup);
  EnableControl(OwnerComboBox, FAllowedChanges & cpOwner);
  EnableControl(RightsFrame, FAllowedChanges & cpMode);
  CalculateSizeButton->Visible = Terminal && FAllowCalculateSize;
}
//---------------------------------------------------------------------------
bool __fastcall TPropertiesDialog::GetMultiple()
{
  return (FFileList->Count != 1);
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetGroupList(TStrings * value)
{
  if (FGroupsSet || ((value != NULL) && (value->Count > 0)))
  {
    GroupComboBox->Items = value;
    FGroupsSet = true;
  }
}
//---------------------------------------------------------------------------
TStrings * __fastcall TPropertiesDialog::GetGroupList()
{
  return GroupComboBox->Items;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetUserList(TStrings * value)
{
  if (FUsersSet || ((value != NULL) && (value->Count > 0)))
  {
    OwnerComboBox->Items = value;
    FUsersSet = true;
  }
}
//---------------------------------------------------------------------------
TStrings * __fastcall TPropertiesDialog::GetUserList()
{
  return OwnerComboBox->Items;
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::FormCloseQuery(TObject * /*Sender*/,
      bool & /*CanClose*/)
{
  if (ModalResult == mrOk)
  {
    #define CHECK_VALID_NAME(Property, Message) \
      if (FOrigProperties.Valid.Contains(vp ## Property) && Property ## ComboBox->Text.Trim().IsEmpty()) { \
        Property ## ComboBox->SetFocus(); throw Exception(Message); }
    CHECK_VALID_NAME(Group, PROPERTIES_INVALID_GROUP);
    CHECK_VALID_NAME(Owner, PROPERTIES_INVALID_OWNER);
    #undef CHECK_VALID_NAME
  }
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::SetAllowedChanges(int value)
{
  if (FAllowedChanges != value)
  {
    FAllowedChanges = value;
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPropertiesDialog::CalculateSizeButtonClick(
      TObject * /*Sender*/)
{
  assert(Terminal);
  __int64 Size;
  Terminal->CalculateFilesSize(FileList, Size, 0);
  FSizeNotCalculated = false;
  LoadSize(Size);
}
//---------------------------------------------------------------------------

