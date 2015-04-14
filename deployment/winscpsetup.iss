#define AppId "winscp3"
#define ParentRegistryKey "Software\Martin Prikryl"
#define RegistryKey ParentRegistryKey+"\WinSCP 2"
#define DefaultLang "en"
#define WebRoot "http://winscp.net/"
#define WebForum WebRoot+"forum/"
#define WebDocumentation WebRoot+"eng/docs/"
#define WebReport WebRoot+"install.php"
#define WebPuTTY "http://www.chiark.greenend.org.uk/~sgtatham/putty/"
#define Year 2015
#define EnglishLang "English"
#define SetupTypeData "SetupType"
#define InnoSetupReg "Software\Microsoft\Windows\CurrentVersion\Uninstall\" + AppId + "_is1"
#define InnoSetupAppPathReg "Inno Setup: App Path"

#ifndef PuttySourceDir
  #define PuttySourceDir "c:\Program Files\PuTTY"
#endif
#ifndef Status
  #define Status "unofficial"
#endif
#ifndef SourceDir
  #define SourceDir "..\source"
#endif
#ifndef BinariesDir
  #define BinariesDir SourceDir + "\Release"
#endif
#ifndef OutputSuffix
  #define OutputSuffix
#endif
#ifndef AllTranslations
  #define AllTranslations
#endif

#define TranslationDirRel "translations"
#define TranslationDir "translations"
#define OutputDir "."

#define TranslationFileMask "WinSCP.???"
#define MainFileName "WinSCP.exe"
#define MainFileSource BinariesDir+"\"+MainFileName
#define ShellExtFileName "DragExt.dll"
#define ShellExtFileSource BinariesDir+"\"+ShellExtFileName
#define ShellExt64FileName "DragExt64.dll"
#define ShellExt64FileSource BinariesDir+"\"+ShellExt64FileName
#define ConsoleFileSource BinariesDir+"\WinSCP.com"
#define IconFileSource SourceDir+"\resource\Icon256.ico"
#define MapFileSource BinariesDir+"\WinSCP.map"

#ifdef Donations
#define PayPalCardImage "paypalcard.bmp"
#endif

#define Major
#define Minor
#define Rev
#define Build
#expr ParseVersion(MainFileSource, Major, Minor, Rev, Build)
#define VersionOnly Str(Major)+"."+Str(Minor)+(Rev > 0 ? "."+Str(Rev) : "")
#define Version VersionOnly+(Status != "" ? " "+Status : "")

#define WebArguments "ver=" +VersionOnly + "&lang={language}&utm_source=winscp&utm_medium=setup&utm_campaign=" + VersionOnly
#define WebGettingStarted WebRoot + "eng/installed.php?" + WebArguments + "&prevver="

; Some features of ISCC requires path relative to script,
; some path relative to CWD
#define MessagesPathRel(L) TranslationDirRel + "\" + "WinSCP." + L + ".isl"

#define ExplorerFile "explorer.bmp"
#define CommanderFile "commander.bmp"

[Setup]
AppId={#AppId}
AppName=WinSCP
AppMutex=WinSCP
AppPublisher=Martin Prikryl
AppPublisherURL={#WebRoot}
AppSupportURL={#WebForum}
AppUpdatesURL={#WebRoot}eng/download.php
VersionInfoCompany=Martin Prikryl
VersionInfoDescription=Setup for WinSCP {#Version} (SFTP, FTP and SCP client)
VersionInfoVersion={#Major}.{#Minor}.{#Rev}.{#Build}
VersionInfoTextVersion={#Version}
VersionInfoCopyright=(c) 2000-{#Year} Martin Prikryl
DefaultDirName={pf}\WinSCP
DefaultGroupName=WinSCP
AllowNoIcons=yes
LicenseFile=license.setup.txt
UninstallDisplayIcon={app}\WinSCP.exe
OutputDir={#OutputDir}
DisableStartupPrompt=yes
AppVersion={#Version}
AppVerName=WinSCP {#Version}
OutputBaseFilename=winscp{#Major}{#Minor}{#Rev}setup{#OutputSuffix}
SolidCompression=yes
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
ShowTasksTreeLines=yes
PrivilegesRequired=none
UsePreviousLanguage=yes
DisableProgramGroupPage=yes
MinVersion=0,5.1
SetupIconFile=winscpsetup.ico
#ifdef Sign
SignTool=sign $f "WinSCP Installer" http://winscp.net/eng/docs/installation
#endif

[Languages]
; English has to be first so that it is pre-selected
; on Setup Select Language window, when no translation matching
; Windows UI locale is available
Name: {#DefaultLang}; MessagesFile: {#MessagesPathRel(DefaultLang)}

#define FindHandle
#dim Languages[200]
#define LanguageCount 0
#define AnyLanguageComplete 0
#define LangI
#define Complete
#define DirName
#define DirNameRel

#sub ProcessTranslationFile

  #define FileName FindGetFileName(FindHandle)
  #define Lang Copy(FileName, Pos(".", FileName)+1)
  #define MessagesPath DirName + "\" + "WinSCP." + Lang + ".isl"

  #define LangNameFull ReadIni(MessagesPath, "LangOptions", "LanguageName")
  #define Sep Pos(" - ", LangNameFull)
  #if Sep > 0
    #define LangName Copy(LangNameFull, 1, Sep - 1)
  #else
    #define LangName LangNameFull
  #endif
  #define LangID ReadIni(MessagesPath, "LangOptions", "LanguageID")

  #expr Languages[LanguageCount*4] = Lang
  #expr Languages[LanguageCount*4+1] = LangName
  #expr Languages[LanguageCount*4+2] = LangID
  #expr Languages[LanguageCount*4+3] = Complete
  #expr LanguageCount++

#if Complete == 1
Name: {#Lang}; MessagesFile: {#MessagesPathRel(Lang)}
  #expr AnyLanguageComplete = 1
#endif

#endsub /* sub ProcessTranslationFile */

#sub ProcessTranslationDir

  #if FindHandle = FindFirst(DirNameRel + "\" + TranslationFileMask, 0)
    #define FResult 1
    #for {0; FResult; FResult = FindNext(FindHandle)} ProcessTranslationFile
    #expr FindClose(FindHandle)
  #endif

#endsub /* sub ProcessTranslationDir */

#expr Complete = 1
#expr DirName = TranslationDir
#expr DirNameRel = TranslationDirRel
#emit ProcessTranslationDir

#ifdef TranslationIncompleteDir
  #expr Complete = 0
  #expr DirName = TranslationIncompleteDir
  #expr DirNameRel = TranslationIncompleteDirRel
  #emit ProcessTranslationDir
#endif

; Types are not used anymore, they are preserved only to let setup
; detect previous installation type and decide between typical/custom setup
[Types]
Name: full; Description: "full"
Name: compact; Description: "compact"
Name: custom; Description: "custom"; Flags: iscustom

[Components]
Name: main; Description: {cm:ApplicationComponent}; \
  Types: full custom compact; Flags: fixed
Name: shellext; Description: {cm:ShellExtComponent}; \
  Types: full compact
Name: pageant; Description: {cm:PageantComponent}; \
  Types: full
Name: puttygen; Description: {cm:PuTTYgenComponent}; \
  Types: full
#if AnyLanguageComplete == 1
Name: transl; Description: {cm:TranslationsComponent}; \
  Types: full
#endif

[Tasks]
Name: enableupdates; Description: {cm:EnableUpdates}
Name: enableupdates\enablecollectusage; Description: {cm:EnableCollectUsage}
; Windows integration
Name: desktopicon; Description: {cm:DesktopIconTask}
Name: desktopicon\user; Description: {cm:DesktopIconUserTask}; \
  Flags: exclusive unchecked
Name: desktopicon\common; Description: {cm:DesktopIconCommonTask}; \
  Flags: exclusive
; No Quick Launch on Win7
Name: quicklaunchicon; Description: {cm:QuickLaunchIconTask}; \
  Flags: unchecked; OnlyBelowVersion: 6.1.7600
Name: sendtohook; Description: {cm:SendToHookTask}
Name: urlhandler; Description: {cm:RegisterAsUrlHandlers}
Name: searchpath; Description: {cm:AddSearchPath}; \
  Flags: unchecked; Check: IsAdminLoggedOn

[Icons]
Name: "{commonprograms}\WinSCP"; Filename: "{app}\WinSCP.exe"; Components: main; \
  Comment: "{cm:ProgramComment}"
; This is created when desktopicon task is selected
Name: "{userdesktop}\WinSCP"; Filename: "{app}\WinSCP.exe"; \
  Tasks: desktopicon\user
Name: "{commondesktop}\WinSCP"; Filename: "{app}\WinSCP.exe"; \
  Tasks: desktopicon\common
; This is created when quicklaunchicon task is selected
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\WinSCP"; \
  Filename: "{app}\WinSCP.exe"; Tasks: quicklaunchicon
; This is created when sendtohook task is selected
Name: "{sendto}\{cm:SendToHookNew}"; Filename: "{app}\WinSCP.exe"; \
  Parameters: "/upload"; Tasks: sendtohook

[InstallDelete]
; Remove pre-524 licence file (without .txt extension)
Type: files; Name: "{app}\license"
; Remove pre-520 start menu folders
Type: filesandordirs; Name: "{commonprograms}\WinSCP"
Type: filesandordirs; Name: "{userprograms}\WinSCP"

[Run]
Filename: "{app}\WinSCP.exe"; Parameters: "/RegisterForDefaultProtocols"; \
  StatusMsg: {cm:RegisteringAsUrlHandlers}; Tasks: urlhandler
Filename: "{app}\WinSCP.exe"; Parameters: "/AddSearchPath"; \
  StatusMsg: {cm:AddingSearchPath}; Tasks: searchpath
Filename: "{app}\WinSCP.exe"; Parameters: "/ImportSitesIfAny"; \
  StatusMsg: {cm:ImportSites}; Flags: skipifsilent

[UninstallDelete]
; These additional files are created by application
Type: files; Name: "{app}\WinSCP.ini"
Type: files; Name: "{app}\WinSCP.cgl"

[Files]
Source: "{#MainFileSource}"; DestDir: "{app}"; \
  Components: main; Flags: ignoreversion
Source: "{#ConsoleFileSource}"; DestDir: "{app}"; \
  Components: main; Flags: ignoreversion
Source: "{#MapFileSource}"; DestDir: "{app}"; \
  Components: main; Flags: ignoreversion
Source: "license.txt"; DestDir: "{app}"; \
  Components: main; Flags: ignoreversion
Source: "{#ShellExtFileSource}"; DestDir: "{app}"; \
  Components: shellext; \
  Flags: regserver restartreplace uninsrestartdelete; \
  Check: not IsWin64 and IsShellExtNewer(ExpandConstant('{app}\{#ShellExtFileName}'), '{#GetFileVersion(ShellExtFileSource)}')
Source: "{#ShellExt64FileSource}"; DestDir: "{app}"; \
  Components: shellext; \
  Flags: regserver restartreplace uninsrestartdelete; \
  Check: IsWin64 and IsShellExtNewer(ExpandConstant('{app}\{#ShellExt64FileName}'), '{#GetFileVersion(ShellExt64FileSource)}')
Source: "{#PuttySourceDir}\LICENCE"; DestDir: "{app}\PuTTY"; \
  Components: pageant puttygen; Flags: ignoreversion
Source: "{#PuttySourceDir}\putty.hlp"; DestDir: "{app}\PuTTY"; \
  Components: pageant puttygen; Flags: ignoreversion
Source: "{#PuttySourceDir}\pageant.exe"; DestDir: "{app}\PuTTY"; \
  Components: pageant; Flags: ignoreversion
Source: "{#PuttySourceDir}\puttygen.exe"; DestDir: "{app}\PuTTY"; \
  Components: puttygen; Flags: ignoreversion
Source: "{#ExplorerFile}"; Flags: dontcopy
Source: "{#CommanderFile}"; Flags: dontcopy
#ifdef Donations
Source: "{#PayPalCardImage}"; \
  Flags: dontcopy
#endif

[Registry]
Root: HKCU; Subkey: "{#ParentRegistryKey}"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "{#RegistryKey}"; Flags: uninsdeletekeyifempty
; Norton Commander interface
Root: HKCU; SubKey: "{#RegistryKey}\Configuration\Interface"; ValueType: dword; \
  ValueName: "Interface"; ValueData: 0; Check: UserSettings(1)
Root: HKLM; SubKey: "{#RegistryKey}"; ValueType: dword; \
  ValueName: "DefaultInterfaceInterface"; ValueData: 0; \
  Check: UserSettings(1); Flags: noerror
; Explorer-like interface
Root: HKCU; SubKey: "{#RegistryKey}\Configuration\Interface"; ValueType: dword; \
  ValueName: "Interface"; ValueData: 1; Check: not UserSettings(1)
Root: HKLM; SubKey: "{#RegistryKey}"; ValueType: dword; \
  ValueName: "DefaultInterfaceInterface"; ValueData: 1; \
  Check: not UserSettings(1); Flags: noerror
; If installer enabled ddext, let it reset the settings on uninstall,
; so the default is used on the next run
Root: HKCU; SubKey: "{#RegistryKey}\Configuration\Interface"; ValueType: dword; \
  ValueName: "DDExtEnabled"; ValueData: 1; Components: shellext; \
  Flags: uninsdeletevalue
; Updates
Root: HKCU; SubKey: "{#RegistryKey}\Configuration\Interface\Updates"; \
  ValueType: dword; ValueName: "Period"; ValueData: 7; \
  Tasks: enableupdates; Check: not UpdatesEnabled
Root: HKLM; SubKey: "{#RegistryKey}"; \
  ValueType: dword; ValueName: "DefaultUpdatesPeriod"; ValueData: 7; \
  Tasks: enableupdates; Flags: noerror
Root: HKLM; SubKey: "{#RegistryKey}"; \
  ValueType: dword; ValueName: "DefaultCollectUsage"; ValueData: 1; \
  Tasks: enableupdates\enablecollectusage; Flags: noerror

#if AnyLanguageComplete == 1

[Components]
Name: transl\eng; Description: {#EnglishLang}; Types: full custom compact; \
  Flags: fixed

#endif

#sub EmitLang

  #if Languages[LangI*4+3] == 1

[Components]
Name: transl\{#Languages[LangI*4]}; Description: {#Languages[LangI*4+1]}; \
  Types: full compact custom; Check: IsLang('{#Languages[LangI*4]}')
Name: transl\{#Languages[LangI*4]}; Description: {#Languages[LangI*4+1]}; \
  Check: not IsLang('{#Languages[LangI*4]}')

[Files]
Source: "{#TranslationDirRel}\WinSCP.{#Languages[LangI*4]}"; DestDir: "{app}"; \
  Components: transl\{#Languages[LangI*4]}; Flags: ignoreversion

[Registry]
; set program default language to setup language, but only if user installs it
Root: HKCU; SubKey: "{#RegistryKey}\Configuration\Interface"; \
  ValueType: dword; ValueName: "LocaleSafe"; ValueData: {#Languages[LangI*4+2]}; \
  Components: transl\{#Languages[LangI*4]}; Languages: {#Languages[LangI*4]}

  #endif

#endsub /* sub EmitLang */

#for {LangI = 0; LangI < LanguageCount; LangI++} EmitLang

[UninstallRun]
; Make sure no later uninstall task recreate the configuration
Filename: "{app}\WinSCP.exe"; Parameters: "/UninstallCleanup"; \
  RunOnceId: "UninstallCleanup"
Filename: "{app}\WinSCP.exe"; Parameters: "/RemoveSearchPath"; \
  RunOnceId: "RemoveSearchPath"
Filename: "{app}\WinSCP.exe"; Parameters: "/UnregisterForProtocols"; \
  RunOnceId: "UnregisterForProtocols"

[Code]
const
  wpSetupType = 100;
  wpInterface = 101;
  NewLine = #13#10;

var
  TypicalTypeButton: TRadioButton;
  CustomTypeButton: TRadioButton;
  CommanderRadioButton: TRadioButton;
  ExplorerRadioButton: TRadioButton;
  LaunchCheckbox: TCheckbox;
  OpenGettingStartedCheckbox: TCheckbox;
  AreUpdatesEnabled: Boolean;
  Upgrade: Boolean;
  MissingTranslations: string;
  PrevVersion: string;
  ShellExtNewerCacheFileName: string;
  ShellExtNewerCacheResult: Boolean;
#ifdef Donations
  DonationPanel: TPanel;
#endif
  InstallationDone: Boolean;
  LicenseAccepted: Boolean;
  InitDir: string;
  InitComponents: string;
  InitTasks: string;
  InitInterface: Integer;
  Donated: Boolean;

procedure ShowMessage(Text: string);
begin
  MsgBox(Text, mbInformation, MB_OK);
end;

function IsLang(Lang: string): Boolean;
begin
  Result := (Lang = ActiveLanguage);
end;

function IsWin8: Boolean;
var
  Version: TWindowsVersion;
begin
  GetWindowsVersionEx(Version);

  Result :=
    (Version.Major > 6) or
    ((Version.Major = 6) and (Version.Minor >= 2));
end;

procedure CutVersionPart(var VersionString: string; var VersionPart: Word);
var
  P: Integer;
begin
  P := Pos('.', VersionString);
  if P > 0 then
  begin
    VersionPart := StrToIntDef(Copy(VersionString, 1, P - 1), 0);
    Delete(VersionString, 1, P);
  end
    else
  begin
    VersionPart := StrToIntDef(VersionString, 0);
    VersionString := '';
  end;
end;

function IsShellExtNewer(FileName: string; InstalledVersion: string): Boolean;
var
  ExistingMS, ExistingLS: Cardinal;
  ExistingMajor, ExistingMinor, ExistingRev, ExistingBuild: Cardinal;
  InstalledMajor, InstalledMinor, InstalledRev, InstalledBuild: Word;
begin
  if ShellExtNewerCacheFileName = FileName then
  begin
    if ShellExtNewerCacheResult then
    begin
      Log(Format('Allowing installation of shell extension %s as already decided', [FileName]));
      Result := True;
    end
      else
    begin
      Log(Format('Skipping installation of shell extension %s as already decided', [FileName]));
      Result := False;
    end;
  end
    else
  if not FileExists(FileName) then
  begin
    Log(Format('Shell extension %s does not exist yet, allowing installation', [FileName]));
    Result := True;
  end
    else
  if not GetVersionNumbers(FileName, ExistingMS, ExistingLS) then
  begin
    Log(Format('Cannot retrieve version of existing shell extension %s, allowing installation', [FileName]));
    Result := True;
  end
    else
  begin
    ExistingMajor := ExistingMS shr 16;
    ExistingMinor := ExistingMS and $FFFF;
    ExistingRev := ExistingLS shr 16;
    ExistingBuild := ExistingLS and $FFFF;
    Log(Format('Existing shell extension %s version: %d.%d.%d[.%d]', [FileName, ExistingMajor, ExistingMinor, ExistingRev, ExistingBuild]));

    Log(Format('Installed extension version string: %s', [InstalledVersion]));
    CutVersionPart(InstalledVersion, InstalledMajor);
    CutVersionPart(InstalledVersion, InstalledMinor);
    CutVersionPart(InstalledVersion, InstalledRev);
    CutVersionPart(InstalledVersion, InstalledBuild);
    Log(Format('Installed extension version: %d.%d.%d[.%d]', [InstalledMajor, InstalledMinor, InstalledRev, InstalledBuild]));

    if ((InstalledMajor > ExistingMajor)) or
       ((InstalledMajor = ExistingMajor) and (InstalledMinor > ExistingMinor)) or
       ((InstalledMajor = ExistingMajor) and (InstalledMinor = ExistingMinor) and (InstalledRev > ExistingRev)) then
    begin
      Log('Installed extension is newer than existing extension, allowing installation');
      Result := True;
    end
      else
    begin
      Log('Installed extension is same or older than existing extension, skipping installation');
      Result := False;
    end;
  end;

  ShellExtNewerCacheFileName := FileName;
  ShellExtNewerCacheResult := Result;
end;

function UpdatesEnabled: Boolean;
begin
  Result := AreUpdatesEnabled;
end;

function UserSettings(Settings: Integer): Boolean;
begin
  case Settings of
    1: Result := CommanderRadioButton.Checked;
    else Result := False;
  end;
end;

function LanguageName(Lang: string; Unknown: string): string;
begin
  #sub EmitLang2
  if Lang = '{#Languages[LangI*4]}' then Result := '{#Languages[LangI*4+1]}'
    else
  #endsub /* sub EmitLang2 */

  #for {LangI = 0; LangI < LanguageCount; LangI++} EmitLang2

  Result := Unknown;
end;

function ContainsLanguage(Lang: string): Boolean;
begin
  #sub EmitLang3
    #if Languages[LangI*4+3] == 1
  if (Lang = '{#Languages[LangI*4]}') then Result := True
    else
    #endif
  #endsub /* sub EmitLang3 */

  #for {LangI = 0; LangI < LanguageCount; LangI++} EmitLang3

  Result := False;
end;

procedure OpenBrowser(Url: string);
var
  ErrorCode: Integer;
begin
  ShellExec('open', Url, '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure OpenHelp;
begin
  OpenBrowser('{#WebDocumentation}installation?page=' + IntToStr(WizardForm.CurPageID) + '&' + ExpandConstant('{#WebArguments}'));
end;

procedure HelpButtonClick(Sender: TObject);
begin
  OpenHelp;
end;

#ifdef Donations

procedure AboutDonationsLinkClick(Sender: TObject);
begin
  OpenBrowser('{#WebRoot}eng/donate.php?' + ExpandConstant('{#WebArguments}'));
  Donated := true;
end;

procedure DonateLinkClick(Sender: TObject);
var
  Control: TControl;
begin
  Control := TControl(Sender);
  OpenBrowser('{#WebRoot}eng/donate.php?amount=' + IntToStr(Control.Tag) + '&currency=' + ExpandConstant('{cm:Currency}') + '&' + ExpandConstant('{#WebArguments}'));
  Donated := true;
end;

#endif

procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
begin
  if Key = 112 { VK_F1 } then
  begin
    OpenHelp;
    Key := 0;
  end;
end;

procedure CaptionClick(Sender: TObject);
begin
  WizardForm.ActiveControl := TLabel(Sender).FocusControl;
end;

procedure ImageClick(Sender: TObject);
begin
  WizardForm.ActiveControl := TWinControl(TControl(Sender).Tag);
end;

type
  TProcessTranslationEvent = procedure(Lang: string; FileName: string);

procedure CollectNames(Lang: string; FileName: string);
begin
  if Length(MissingTranslations) > 0 then
    MissingTranslations := MissingTranslations + ', ';
  MissingTranslations := MissingTranslations + LanguageName(Lang, Lang);
end;

procedure DeleteTranslation(Lang: string; FileName: string);
begin
  DeleteFile(FileName);
end;

procedure ProcessMissingTranslations(OnProcessTranslation: TProcessTranslationEvent);
var
  Path: string;
  FindRec: TFindRec;
  Ext: string;
  LExt: string;
begin
  Path := AddBackslash(WizardDirValue);

  if FindFirst(ExpandConstant(Path + '{#TranslationFileMask}'), FindRec) then
  begin
    try
      repeat
        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        begin
          Ext := Uppercase(ExtractFileExt(FindRec.Name));
          if Pos('.', Ext) = 1  then
          begin
            Ext := Uppercase(Copy(Ext, 2, Length(Ext) - 1));
            LExt := Lowercase(Ext);
            if (Pos('-' + Ext + '-', '-{#AllTranslations}-') > 0) and
               not ContainsLanguage(LExt) then
              OnProcessTranslation(LExt, Path + FindRec.Name);
          end;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;
end;

function WillRestart: Boolean;
begin
  Result := WizardForm.YesRadio.Visible and WizardForm.YesRadio.Checked;
end;

procedure UpdatePostInstallRunCheckboxes(Sender: TObject);
begin
  LaunchCheckbox.Enabled := not WillRestart;
  OpenGettingStartedCheckbox.Enabled :=
    LaunchCheckbox.Enabled
end;

procedure LinkLabel(Control: TLabel);
begin
  Control.ParentFont := True;
  Control.Font.Style := Control.Font.Style + [fsUnderline];
  Control.Font.Color := clBlue;
  Control.Cursor := crHand;
end;

function IsTypicalInstallation: Boolean;
begin
  Result := TypicalTypeButton.Checked;
end;

#ifdef Donations

procedure CreateDonateLink(Amount: Integer; Row: Integer; Top: Integer);
var
  Caption: TLabel;
begin
  Caption := TLabel.Create(DonationPanel);
  Caption.Left := 0;
  Caption.Top := Top + Row * ScaleY(16);
  Caption.Tag := Amount;
  Caption.Parent := DonationPanel;
  Caption.Caption := Format(ExpandConstant('{cm:Donate}'), ['$' + IntToStr(Amount)]);
  Caption.OnClick := @DonateLinkClick;
  LinkLabel(Caption);
end;

#endif

procedure LoadEmbededBitmap(Image: TBitmapImage; Name: string);
begin
  ExtractTemporaryFile(Name);
  Image.Bitmap.LoadFromFile(ExpandConstant('{tmp}\' + Name));
  Image.AutoSize := True;
end;

// WORKAROUND
// Checkboxes and Radio buttons created on runtime do
// not scale their height automatically
procedure ScaleFixedHeightControl(Control: TButtonControl);
begin
  Control.Height := ScaleY(Control.Height);
end;

procedure InitializeWizard;
var
  DefaultLang: Boolean;
  UserInterface: Cardinal;
  UpdatesPeriod: Cardinal;
  InterfacePage: TWizardPage;
  SetupTypePage: TWizardPage;
  Caption: TLabel;
  Image: TBitmapImage;
  HelpButton: TButton;
#ifdef Donations
  P: Integer;
#endif
  S: string;
begin
  InstallationDone := False;
  LicenseAccepted := False;
  InitInterface := -1;

  DefaultLang := (ActiveLanguage = '{#DefaultLang}');

  Upgrade :=
    RegQueryStringValue(HKLM, '{#InnoSetupReg}', '{#InnoSetupAppPathReg}', S) or
    RegQueryStringValue(HKCU, '{#InnoSetupReg}', '{#InnoSetupAppPathReg}', S)

  if Upgrade and GetVersionNumbersString(AddBackslash(WizardDirValue) + '{#MainFileName}', PrevVersion) and
     (PrevVersion[2] = '.') and (PrevVersion[4] = '.') and (PrevVersion[6] = '.') then
  begin
    PrevVersion := Copy(PrevVersion, 1, 5);
  end;

  ProcessMissingTranslations(@CollectNames);

  WizardForm.KeyPreview := True;
  WizardForm.OnKeyDown := @FormKeyDown;
  // to accomodate one more task
  WizardForm.TasksList.Height := WizardForm.TasksList.Height + ScaleY(8);

  // allow installation without requiring user to accept license
  WizardForm.LicenseAcceptedRadio.Checked := True;
  WizardForm.LicenseAcceptedRadio.Visible := False;
  WizardForm.LicenseNotAcceptedRadio.Visible := False;
  WizardForm.LicenseMemo.Height :=
    WizardForm.LicenseNotAcceptedRadio.Top +
    WizardForm.LicenseNotAcceptedRadio.Height -
    WizardForm.LicenseMemo.Top - 5;

  // hide installation types combo
  WizardForm.TypesCombo.Visible := False;
  WizardForm.ComponentsList.Height :=
    WizardForm.ComponentsList.Top + WizardForm.ComponentsList.Height -
    WizardForm.TypesCombo.Top;
  WizardForm.ComponentsList.Top := WizardForm.TypesCombo.Top;

  // add help button
  HelpButton := TButton.Create(WizardForm);
  HelpButton.Parent := WizardForm;
  HelpButton.Left :=
    WizardForm.ClientWidth -
    (WizardForm.CancelButton.Left + WizardForm.CancelButton.Width);
  HelpButton.Top := WizardForm.CancelButton.Top;
  HelpButton.Width := WizardForm.CancelButton.Width;
  HelpButton.Height := WizardForm.CancelButton.Height;
  HelpButton.Caption := ExpandConstant('{cm:HelpButton}');
  HelpButton.OnClick := @HelpButtonClick;

  // installation type page

  SetupTypePage := CreateCustomPage(wpLicense,
    ExpandConstant('{cm:SetupTypeTitle}'),
    ExpandConstant('{cm:SetupTypePrompt}'));

  TypicalTypeButton := TRadioButton.Create(SetupTypePage);
  if not Upgrade then
    S := ExpandConstant('{cm:TypicalType}')
  else
    S := ExpandConstant('{cm:TypicalUpgradeType}');
  TypicalTypeButton.Caption :=
    FmtMessage(ExpandConstant('{cm:Recommended}'), [S]);
  // check typical install, if typical install was installed before or
  // when version without setup type support was installed with
  // "full" installation or when there were no installation before
  // ("full" installation is default)
  TypicalTypeButton.Checked :=
    ((GetPreviousData('{#SetupTypeData}', '') = 'typical')) or
    ((GetPreviousData('{#SetupTypeData}', '') = '') and
     (WizardSetupType(False) = 'full'));
  TypicalTypeButton.Left := ScaleX(4);
  TypicalTypeButton.Width := SetupTypePage.SurfaceWidth -
    TypicalTypeButton.Left;
  ScaleFixedHeightControl(TypicalTypeButton);
  TypicalTypeButton.Parent := SetupTypePage.Surface;

  Caption := TLabel.Create(SetupTypePage);
  Caption.WordWrap := True;
  if not Upgrade then
  begin
    if DefaultLang then
      S := ExpandConstant('{cm:TypicalType2Eng}')
    else
      S := FmtMessage(ExpandConstant('{cm:TypicalType2Intl}'), [LanguageName(ActiveLanguage, 'Unknown')]);
    Caption.Caption :=
      ExpandConstant('{cm:TypicalType1}') + NewLine +
      S + NewLine +
      ExpandConstant('{cm:TypicalType3}');
  end
    else
  begin
    if Length(MissingTranslations) > 0 then
    begin
      #if AnyLanguageComplete
        S := FmtMessage(ExpandConstant('{cm:TypicalUpgradeTypeMissingTransl}'), [MissingTranslations]);
      #else
        S := ExpandConstant('{cm:TypicalUpgradeTypeNoTransl}');
      #endif
      S := NewLine + S;
    end
      else S := '';

    Caption.Caption :=
      ExpandConstant('{cm:TypicalUpgradeType1}') + S;
  end;
  Caption.Left := ScaleX(4) + ScaleX(20);
  Caption.Width := SetupTypePage.SurfaceWidth - Caption.Left;
  Caption.Top := TypicalTypeButton.Top + TypicalTypeButton.Height + ScaleY(6);
  Caption.Parent := SetupTypePage.Surface;
  Caption.FocusControl := TypicalTypeButton;
  Caption.OnClick := @CaptionClick;

  CustomTypeButton := TRadioButton.Create(SetupTypePage);
  if not Upgrade then
    CustomTypeButton.Caption := ExpandConstant('{cm:CustomType}')
  else
    CustomTypeButton.Caption := ExpandConstant('{cm:CustomUpgradeType}');
  CustomTypeButton.Checked := (not TypicalTypeButton.Checked);
  CustomTypeButton.Left := ScaleX(4);
  CustomTypeButton.Width := SetupTypePage.SurfaceWidth -
    CustomTypeButton.Left;
  CustomTypeButton.Top := Caption.Top + Caption.Height + ScaleY(10);
  ScaleFixedHeightControl(CustomTypeButton);
  CustomTypeButton.Parent := SetupTypePage.Surface;

  Caption := TLabel.Create(SetupTypePage);
  Caption.WordWrap := True;
  if not Upgrade then
  begin
    Caption.Caption :=
      ExpandConstant('{cm:CustomType1}');
  end
    else
  begin
    Caption.Caption :=
      ExpandConstant('{cm:CustomUpgradeType1}') + NewLine +
      ExpandConstant('{cm:CustomUpgradeType2}');
  end;
  Caption.Left := ScaleX(4) + ScaleX(20);
  Caption.Width := SetupTypePage.SurfaceWidth - Caption.Left;
  Caption.Top := CustomTypeButton.Top + CustomTypeButton.Height +
    ScaleY(6);
  Caption.Parent := SetupTypePage.Surface;
  Caption.FocusControl := CustomTypeButton;
  Caption.OnClick := @CaptionClick;

  // interface page

  InterfacePage := CreateCustomPage(wpSelectTasks,
    ExpandConstant('{cm:UserSettingsTitle}'),
    ExpandConstant('{cm:UserSettingsPrompt}'));

  UpdatesPeriod := 0;
  RegQueryDWordValue(HKCU, '{#RegistryKey}\Configuration\Interface\Updates',
    'Period', UpdatesPeriod);
  AreUpdatesEnabled := (UpdatesPeriod <> 0);

  UserInterface := 0; { default is commander }
  RegQueryDWordValue(HKCU, '{#RegistryKey}\Configuration\Interface',
    'Interface', UserInterface);

  Caption := TLabel.Create(InterfacePage);
  Caption.Caption := ExpandConstant('{cm:UserInterfaceStyle}');
  Caption.Width := InterfacePage.SurfaceWidth;
  Caption.Parent := InterfacePage.Surface;

  CommanderRadioButton := TRadioButton.Create(InterfacePage);
  CommanderRadioButton.Caption := ExpandConstant('{cm:NortonCommanderInterfaceC}');
  CommanderRadioButton.Checked := (UserInterface = 0);
  CommanderRadioButton.Left := ScaleX(4);
  CommanderRadioButton.Width := ScaleX(116);
  CommanderRadioButton.Top := Caption.Top + Caption.Height + ScaleY(6);
  ScaleFixedHeightControl(CommanderRadioButton);
  CommanderRadioButton.Parent := InterfacePage.Surface;

  Image := TBitmapImage.Create(InterfacePage);
  Image.Top := CommanderRadioButton.Top + CommanderRadioButton.Height + ScaleY(6);
  Image.Left := CommanderRadioButton.Left + ScaleX(45);
  Image.Parent := InterfacePage.Surface;
  LoadEmbededBitmap(Image, '{#CommanderFile}');
  Image.ReplaceColor := $FF00FF;
  Image.ReplaceWithColor := InterfacePage.Surface.Color;
  Image.OnClick := @ImageClick;
  Image.Tag := Integer(CommanderRadioButton);

  Caption := TLabel.Create(InterfacePage);
  Caption.WordWrap := True;
  Caption.Caption :=
      ExpandConstant('{cm:NortonCommanderInterface1}') + NewLine +
      ExpandConstant('{cm:NortonCommanderInterface2}') + NewLine +
      ExpandConstant('{cm:NortonCommanderInterface3}');
  Caption.Left := CommanderRadioButton.Left + CommanderRadioButton.Width;
  Caption.Width := InterfacePage.SurfaceWidth - Caption.Left;
  Caption.Top := CommanderRadioButton.Top;
  Caption.Parent := InterfacePage.Surface;
  Caption.FocusControl := CommanderRadioButton;
  Caption.OnClick := @CaptionClick;

  ExplorerRadioButton := TRadioButton.Create(InterfacePage);
  ExplorerRadioButton.Caption := ExpandConstant('{cm:ExplorerInterfaceC}');
  ExplorerRadioButton.Checked := (UserInterface <> 0);
  ExplorerRadioButton.Left := ScaleX(4);
  ExplorerRadioButton.Width := CommanderRadioButton.Width;
  ExplorerRadioButton.Top := Caption.Top + Caption.Height + ScaleY(10);
  ScaleFixedHeightControl(ExplorerRadioButton);
  ExplorerRadioButton.Parent := InterfacePage.Surface;

  Image := TBitmapImage.Create(InterfacePage);
  Image.Top := ExplorerRadioButton.Top + ExplorerRadioButton.Height + ScaleY(6);
  Image.Left := ExplorerRadioButton.Left + ScaleX(45);
  Image.Parent := InterfacePage.Surface;
  LoadEmbededBitmap(Image, '{#ExplorerFile}');
  Image.ReplaceColor := $C020E0;
  Image.ReplaceWithColor := InterfacePage.Surface.Color;
  Image.OnClick := @ImageClick;
  Image.Tag := Integer(ExplorerRadioButton);

  Caption := TLabel.Create(InterfacePage);
  Caption.WordWrap := True;
  Caption.Caption :=
      ExpandConstant('{cm:ExplorerInterface1}') + NewLine +
      ExpandConstant('{cm:ExplorerInterface2}') + NewLine +
      ExpandConstant('{cm:ExplorerInterface3}');
  Caption.Left := ExplorerRadioButton.Left + ExplorerRadioButton.Width;
  Caption.Width := InterfacePage.SurfaceWidth - Caption.Left;
  Caption.Top := ExplorerRadioButton.Top;
  Caption.Parent := InterfacePage.Surface;
  Caption.FocusControl := ExplorerRadioButton;
  Caption.OnClick := @CaptionClick;

  // run checkbox
  LaunchCheckbox := TCheckbox.Create(WizardForm.FinishedPage);
  LaunchCheckbox.Caption := ExpandConstant('{cm:Launch}');
  LaunchCheckbox.Checked := True;
  LaunchCheckbox.Left := WizardForm.YesRadio.Left;
  LaunchCheckbox.Width := WizardForm.YesRadio.Width;
  ScaleFixedHeightControl(LaunchCheckbox);
  LaunchCheckbox.Parent := WizardForm.FinishedPage;
  OpenGettingStartedCheckbox := TCheckbox.Create(WizardForm.FinishedPage);
  OpenGettingStartedCheckbox.Caption := ExpandConstant('{cm:OpenGettingStarted}');
  OpenGettingStartedCheckbox.Checked := True;
  OpenGettingStartedCheckbox.Left := WizardForm.YesRadio.Left;
  OpenGettingStartedCheckbox.Width := WizardForm.YesRadio.Width;
  ScaleFixedHeightControl(OpenGettingStartedCheckbox);
  OpenGettingStartedCheckbox.Parent := WizardForm.FinishedPage;

#ifdef Donations

  DonationPanel := TPanel.Create(WizardForm.FinishedPage);
  DonationPanel.Left := WizardForm.YesRadio.Left;
  DonationPanel.Width := WizardForm.YesRadio.Width;
  DonationPanel.Parent := WizardForm.FinishedPage;
  DonationPanel.Top := ScaleY(190);
  DonationPanel.Height := ScaleY(110);
  DonationPanel.BevelInner := bvNone;
  DonationPanel.BevelOuter := bvNone;
  DonationPanel.Color := WizardForm.FinishedPage.Color;

  Caption := TLabel.Create(DonationPanel);
  Caption.Left := 0;
  Caption.Top := 0;
  Caption.Width := DonationPanel.Width;
  Caption.Parent := DonationPanel;
  Caption.Caption := ExpandConstant('{cm:PleaseDonate}');

  P := Caption.Top + Caption.Height + ScaleY(12);

  CreateDonateLink( 9, 0, P);
  CreateDonateLink(19, 1, P);
  CreateDonateLink(29, 2, P);
  CreateDonateLink(49, 3, P);

  Caption := TLabel.Create(DonationPanel);
  Caption.Left := 0;
  Caption.Top := P + 3 * ScaleY(16) + ScaleY(24);
  Caption.Parent := DonationPanel;
  Caption.Caption := ExpandConstant('{cm:AboutDonations}');
  Caption.OnClick := @AboutDonationsLinkClick;
  LinkLabel(Caption);

  Image := TBitmapImage.Create(DonationPanel);
  LoadEmbededBitmap(Image, '{#PayPalCardImage}');
  Image.Cursor := crHand;
  Image.Parent := DonationPanel;
  Image.Left := ScaleX(100);
  Image.Top := P + ScaleX(8);
  Image.ReplaceColor := $FCFE04;
  Image.ReplaceWithColor := WizardForm.FinishedPage.Color;
  Image.Hint := ExpandConstant('{cm:AboutDonations}');
  Image.ShowHint := True;
  Image.OnClick := @AboutDonationsLinkClick;

#endif

  WizardForm.YesRadio.OnClick := @UpdatePostInstallRunCheckboxes;
  WizardForm.NoRadio.OnClick := @UpdatePostInstallRunCheckboxes;
  UpdatePostInstallRunCheckboxes(nil);

  if IsWin8 then
  begin
    WizardForm.NoIconsCheck.Checked := True;
  end;
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
var
  S: string;
begin
  if IsTypicalInstallation then S := 'typical'
    else S := 'custom';

  SetPreviousData(PreviousDataKey, '{#SetupTypeData}', S);
end;

function SaveCheckListBoxState(ListBox: TNewCheckListBox): string;
var
  I: Integer;
begin
  for I := 0 to ListBox.Items.Count - 1 do
  begin
    Result := Result + IntToStr(Integer(ListBox.State[I]));
  end;
end;

procedure CurPageChanged(CurPageID: Integer);
var
  Delta: Integer;
  LineHeight: Integer;
  LaunchCheckboxTop: Integer;
begin
  if CurPageID = wpLicense then
  begin
    WizardForm.NextButton.Caption := ExpandConstant('{cm:AcceptButton}')
  end;

  if CurPageID = wpSelectDir then
  begin
    if InitDir = '' then
      InitDir := WizardForm.DirEdit.Text;
  end
    else
  if CurPageID = wpSelectComponents then
  begin
    if InitComponents = '' then
      InitComponents := SaveCheckListBoxState(WizardForm.ComponentsList);
  end
    else
  if CurPageID = wpSelectTasks then
  begin
    if InitTasks = '' then
      InitTasks := SaveCheckListBoxState(WizardForm.TasksList);
  end
    else
  if CurPageID = wpInterface then
  begin
    if InitInterface < 0 then
      InitInterface := Integer(CommanderRadioButton.Checked);
  end
    else
  if CurPageID = wpFinished then
  begin
    LineHeight := (WizardForm.NoRadio.Top - WizardForm.YesRadio.Top);

    // Are we at the "Restart?" screen
    if WizardForm.YesRadio.Visible then
    begin
      WizardForm.FinishedLabel.Caption :=
        ExpandConstant('{cm:FinishedRestartDragExtLabel}') + NewLine;

      Delta := WizardForm.AdjustLabelHeight(WizardForm.FinishedLabel);
      WizardForm.YesRadio.Top := WizardForm.YesRadio.Top + Delta;
      WizardForm.NoRadio.Top := WizardForm.NoRadio.Top + Delta;

      LaunchCheckboxTop := WizardForm.NoRadio.Top + LineHeight;
#ifdef Donations
      DonationPanel.Visible := False;
#endif
    end
      else
    begin
      LaunchCheckboxTop := WizardForm.RunList.Top;
    end;

    LaunchCheckbox.Top := LaunchCheckboxTop;
    OpenGettingStartedCheckbox.Top := LaunchCheckbox.Top + LineHeight;

    UpdatePostInstallRunCheckboxes(nil);
  end
    else
  if CurPageID = wpSetupType then
  begin
    Log('License accepted');
    LicenseAccepted := True;
  end
    else
  if CurPageID = wpPreparing then
  begin
    // Are we at the "Restart applications?" screen
    if WizardForm.PreparingLabel.Visible then
    begin
      WizardForm.PreparingLabel.Caption :=
        ExpandConstant('{cm:ApplicationsFoundDragExt}');
      WizardForm.IncTopDecHeight(WizardForm.PreparingMemo,
        WizardForm.AdjustLabelHeight(WizardForm.PreparingLabel));
    end;
  end;
end;

function AskedRestart: Boolean;
begin
  Result := WizardForm.YesRadio.Visible;
end;

procedure DeinitializeSetup;
var
  WinHttpReq: Variant;
  ReportUrl: string;
  ReportData: string;
begin
  // cannot send report, unless user already accepted license
  // (with privacy policy)
  if LicenseAccepted then
  begin
    Log('Preparing intallation report');

    ReportData := Format(
      'installed=%d&silent=%d&ver=%s&lang=%s&prevver=%s&', [
       Integer(InstallationDone), Integer(WizardSilent),
       ExpandConstant('{#VersionOnly}'), ActiveLanguage,
       PrevVersion]);

    try
      ReportUrl := ExpandConstant('{#WebReport}?') + ReportData;

      Log('Sending installation report: ' + ReportUrl);

      WinHttpReq := CreateOleObject('WinHttp.WinHttpRequest.5.1');
      WinHttpReq.Open('GET', ReportUrl, False);
      WinHttpReq.Send('');

      Log('Installation report send result: ' + IntToStr(WinHttpReq.Status) + ' ' + WinHttpReq.StatusText);
    except
      Log('Error sending installation report: ' + GetExceptionMessage);
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ErrorCode: Integer;
  ShowCmd: Integer;
  Path: string;
  WebGettingStarted: string;
  OpenGettingStarted: Boolean;
  UsageData: string;
  CanPostInstallRuns: Boolean;
begin
  if CurStep = ssPostInstall then
  begin
    Log('Post install');
    if Length(MissingTranslations) > 0 then
    begin
      Log('Removing obsolete translations');
      WizardForm.StatusLabel.Caption :=
        ExpandConstant('{cm:RemovingObsoleteTranslations}');
      ProcessMissingTranslations(@DeleteTranslation);
    end;
    InstallationDone := True;
  end
    else
  if CurStep = ssDone then
  begin
    Log('Done');
    // bug in InnoSetup causes it using ssDone even when
    // setup failed because machine was not restarted to complete previous
    // installation. double check that ssPostInstall was called
    if InstallationDone then
    begin
      Path := ExpandConstant('{app}\{#MainFileName}');

      CanPostInstallRuns := (not WizardSilent) and (not WillRestart);

      OpenGettingStarted :=
        OpenGettingStartedCheckbox.Enabled and
         OpenGettingStartedCheckbox.Checked;

      UsageData := '/Usage=';
      
      // old style counter
      UsageData := UsageData + Format('TypicalInstallation:%d,', [Integer(IsTypicalInstallation)]);
      
      // new style counters
      if not Upgrade then
      begin
        if IsTypicalInstallation then
          UsageData := UsageData + 'InstallationsFirstTypical+,'
        else
          UsageData := UsageData + 'InstallationsFirstCustom+,';
      end
        else
      begin
        if IsTypicalInstallation then
          UsageData := UsageData + 'InstallationsUpgradeTypical+,'
        else
          UsageData := UsageData + 'InstallationsUpgradeCustom+,';
      end;
      
      if (InitDir <> '') and (InitDir <> WizardForm.DirEdit.Text) then
        UsageData := UsageData + 'InstallationsCustomDir+,';
      if (InitComponents <> '') and (InitComponents <> SaveCheckListBoxState(WizardForm.ComponentsList)) then
        UsageData := UsageData + 'InstallationsCustomComponents+,';
      if (InitTasks <> '') and (InitTasks <> SaveCheckListBoxState(WizardForm.TasksList)) then
        UsageData := UsageData + 'InstallationsCustomTasks+,';
      if (InitInterface >= 0) and (InitInterface <> Integer(CommanderRadioButton.Checked)) then
        UsageData := UsageData + 'InstallationsCustomInterface+,';
      if CanPostInstallRuns and OpenGettingStarted then
        UsageData := UsageData + 'InstallationsGettingStarted+,';
      if CanPostInstallRuns and LaunchCheckbox.Checked then
        UsageData := UsageData + 'InstallationsLaunch+,';
      if WizardSilent then
        UsageData := UsageData + 'InstallationsSilent+,';
      if AskedRestart then
        UsageData := UsageData + 'InstallationsNeedRestart+,';
      if WillRestart then
        UsageData := UsageData + 'InstallationsRestart+,';
      if Donated then
        UsageData := UsageData + 'InstallationsDonate+,';

      // have to do this before running WinSCP GUI instance below,
      // otherwise it loads the empty/previous counters and overwrites our changes,
      // when it's closed
      Log('Recording installer usage statistics: ' + UsageData);
      // make sure we write the counters using the "normal" account
      // (the account that will be used to report the counters)
      ExecAsOriginalUser(Path, UsageData, '', SW_HIDE, ewWaitUntilTerminated, ErrorCode);

      if CanPostInstallRuns then
      begin
        if OpenGettingStarted then
        begin
          WebGettingStarted :=
            ExpandConstant('{#WebGettingStarted}') + PrevVersion;
          Log('Opening getting started page: ' + WebGettingStarted);
          OpenBrowser(WebGettingStarted);
        end;

        if LaunchCheckbox.Checked then
        begin
          if OpenGettingStarted then
          begin
            Log('Will launch WinSCP minimized');
            ShowCmd := SW_SHOWMINIMIZED
          end
            else
          begin
            ShowCmd := SW_SHOWNORMAL;
          end;

          Log('Launching WinSCP');
          ExecAsOriginalUser(Path, '', '', ShowCmd, ewNoWait, ErrorCode)
        end;
      end;
    end;
  end;
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result :=
    { Hide most pages during typical installation }
    IsTypicalInstallation and
    ((PageID = wpSelectDir) or (PageID = wpSelectComponents) or
     (PageID = wpSelectTasks) or
     { Hide Interface page for upgrades only, show for fresh installs }
     ((PageID = wpInterface) and Upgrade));
end;

function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo,
  MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: string): string;
var
  S: string;
  S2: string;
begin
  S := '';

  S := S + MemoDirInfo + NewLine + NewLine;

  if not Upgrade then
  begin
    if IsTypicalInstallation then S2 := ExpandConstant('{cm:TypicalType}')
      else S2 := ExpandConstant('{cm:CustomType}');
  end
    else
  begin
    if IsTypicalInstallation then S2 := ExpandConstant('{cm:TypicalUpgradeType}')
      else S2 := ExpandConstant('{cm:CustomUpgradeType}');
  end;
  StringChange(S2, '&', '');
  S := S + SetupMessage(msgReadyMemoType) + NewLine + Space + S2 + NewLine + NewLine;

  S := S + MemoComponentsInfo + NewLine + NewLine;

  if Length(MemoGroupInfo) > 0 then
    S := S + MemoGroupInfo + NewLine + NewLine;

  if Length(MemoTasksInfo) > 0 then
    S := S + MemoTasksInfo + NewLine + NewLine;

  S := S + ExpandConstant('{cm:UserSettingsOverview}') + NewLine;
  S := S + Space;
  if CommanderRadioButton.Checked then S2 := ExpandConstant('{cm:NortonCommanderInterfaceC}')
    else S2 := ExpandConstant('{cm:ExplorerInterfaceC}');
  StringChange(S2, '&', '');
  S := S + S2;
  S := S + NewLine;

  Result := S;
end;

function InitializeUninstall: Boolean;
begin
  // let application know that we are running silent uninstall,
  // this turns UninstallCleanup to noop
  if UninstallSilent then
    CreateMutex('WinSCPSilentUninstall');
  Result := True;
end;

#expr SaveToFile(AddBackslash(SourcePath) + "Preprocessed.iss")
