object PreferencesDialog: TPreferencesDialog
  Left = 380
  Top = 111
  BorderStyle = bsDialog
  Caption = 'Preferences'
  ClientHeight = 425
  ClientWidth = 386
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poMainFormCenter
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  DesignSize = (
    386
    425)
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 386
    Height = 385
    ActivePage = IntegrationSheet
    Align = alTop
    Anchors = [akLeft, akTop, akRight, akBottom]
    MultiLine = True
    TabIndex = 8
    TabOrder = 0
    object PreferencesSheet: TTabSheet
      Caption = 'General'
      ImageIndex = 2
      DesignSize = (
        378
        339)
      object RandomSeedFileLabel: TLabel
        Left = 16
        Top = 276
        Width = 82
        Height = 13
        Caption = '&Random seed file'
      end
      object CommonPreferencesGroup: TXPGroupBox
        Left = 8
        Top = 8
        Width = 362
        Height = 181
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Confirmations'
        TabOrder = 0
        DesignSize = (
          362
          181)
        object CopyOnDoubleClickCheck: TCheckBox
          Left = 16
          Top = 109
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Copy files using double-click'
          TabOrder = 4
          OnClick = ControlChange
        end
        object CopyOnDoubleClickConfirmationCheck: TCheckBox
          Left = 32
          Top = 131
          Width = 314
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Co&nfirm copy on double-click operation'
          TabOrder = 5
          OnClick = ControlChange
        end
        object ConfirmOverwritingCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Overwriting of files'
          TabOrder = 0
          OnClick = ControlChange
        end
        object ConfirmDeletingCheck: TCheckBox
          Left = 16
          Top = 43
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Deleting of files (recommended)'
          TabOrder = 1
          OnClick = ControlChange
        end
        object ConfirmClosingSessionCheck: TCheckBox
          Left = 16
          Top = 65
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Closing &session'
          TabOrder = 2
          OnClick = ControlChange
        end
        object DDTransferConfirmationCheck: TCheckBox
          Left = 16
          Top = 87
          Width = 338
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Drag && drop operations'
          TabOrder = 3
          OnClick = ControlChange
        end
        object ContinueOnErrorCheck: TCheckBox
          Left = 16
          Top = 153
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Continue on &error (advanced users)'
          TabOrder = 6
          OnClick = ControlChange
        end
      end
      object RandomSeedFileEdit: TFilenameEdit
        Left = 128
        Top = 272
        Width = 242
        Height = 21
        AcceptFiles = True
        DefaultExt = 'log'
        Filter = 'Random seed files (*.rnd)|*.rnd|All files (*.*)|*.*'
        DialogOptions = [ofHideReadOnly, ofPathMustExist]
        DialogTitle = 'Select file for random seed'
        ClickKey = 16397
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 2
        Text = 'RandomSeedFileEdit'
        OnChange = ControlChange
      end
      object StorageGroup: TXPGroupBox
        Left = 8
        Top = 195
        Width = 362
        Height = 68
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Configuration storage'
        TabOrder = 1
        object RegistryStorageButton: TRadioButton
          Left = 16
          Top = 19
          Width = 289
          Height = 17
          Caption = 'Windows re&gistry'
          TabOrder = 0
          OnClick = ControlChange
        end
        object IniFileStorageButton: TRadioButton
          Left = 16
          Top = 43
          Width = 289
          Height = 17
          Caption = '&INI file (winscp3.ini)'
          TabOrder = 1
          OnClick = ControlChange
        end
      end
    end
    object LogSheet: TTabSheet
      Caption = 'Logging'
      ImageIndex = 4
      DesignSize = (
        378
        339)
      inline LoggingFrame: TLoggingFrame
        Left = 5
        Top = 0
        Width = 377
        Height = 212
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        DesignSize = (
          377
          212)
        inherited LoggingGroup: TXPGroupBox
          Width = 362
          DesignSize = (
            362
            177)
          inherited LogFileNameEdit: TFilenameEdit
            Width = 308
          end
          inherited LogFilePanel: TPanel
            Width = 308
          end
        end
      end
    end
    object GeneralSheet: TTabSheet
      Caption = 'Interface'
      ImageIndex = 5
      DesignSize = (
        378
        339)
      object Label1: TLabel
        Left = 8
        Top = 226
        Width = 361
        Height = 33
        AutoSize = False
        Caption = 
          'Note: a change to this setting will only take effect the next ti' +
          'me you start the application.'
        WordWrap = True
      end
      inline GeneralSettingsFrame: TGeneralSettingsFrame
        Left = 8
        Top = 8
        Width = 362
        Height = 202
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        inherited InterfaceGroup: TXPGroupBox
          Width = 362
          inherited CommanderDescriptionLabel: TLabel
            Width = 223
          end
          inherited ExplorerDescriptionLabel: TLabel
            Width = 225
          end
        end
      end
    end
    object PanelsSheet: TTabSheet
      Caption = 'Panels'
      ImageIndex = 3
      DesignSize = (
        378
        339)
      object PanelsRemoteDirectoryGroup: TXPGroupBox
        Left = 8
        Top = 88
        Width = 362
        Height = 51
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Remote directory'
        TabOrder = 1
        DesignSize = (
          362
          51)
        object ShowInaccesibleDirectoriesCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Show inaccesible directories'
          TabOrder = 0
          OnClick = ControlChange
        end
      end
      object PanelsCommonGroup: TXPGroupBox
        Left = 8
        Top = 8
        Width = 362
        Height = 72
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Common'
        TabOrder = 0
        DesignSize = (
          362
          72)
        object ShowHiddenFilesCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Show hidden files'
          TabOrder = 0
          OnClick = ControlChange
        end
        object DefaultDirIsHomeCheck: TCheckBox
          Left = 16
          Top = 42
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Default directory is &home directory'
          TabOrder = 1
          OnClick = ControlChange
        end
      end
      object DragDropPreferencesGroup: TXPGroupBox
        Left = 8
        Top = 147
        Width = 362
        Height = 166
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Drag && Drop'
        TabOrder = 2
        DesignSize = (
          362
          166)
        object Label5: TLabel
          Left = 16
          Top = 47
          Width = 313
          Height = 39
          AutoSize = False
          Caption = 
            'When downloading files using drag && drop, they are stored first' +
            ' to temporary directory.'
          WordWrap = True
        end
        object DDAllowMoveCheck: TCheckBox
          Left = 16
          Top = 23
          Width = 338
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Allow &move on remote side (not recommended)'
          TabOrder = 0
          OnClick = ControlChange
        end
        object DDSystemTemporaryDirectoryButton: TRadioButton
          Left = 32
          Top = 83
          Width = 297
          Height = 17
          Caption = '&Use temporary directory of system'
          TabOrder = 1
          OnClick = ControlChange
        end
        object DDCustomTemporaryDirectoryButton: TRadioButton
          Left = 32
          Top = 107
          Width = 129
          Height = 17
          Caption = 'Use this &directory:'
          TabOrder = 2
          OnClick = ControlChange
        end
        object DDTemporaryDirectoryEdit: TDirectoryEdit
          Left = 160
          Top = 103
          Width = 189
          Height = 21
          AcceptFiles = True
          DialogText = 'Select directory for temporary drag && drop files.'
          ClickKey = 16397
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 3
          Text = 'DDTemporaryDirectoryEdit'
          OnClick = ControlChange
        end
        object DDWarnLackOfTempSpaceCheck: TCheckBox
          Left = 32
          Top = 136
          Width = 281
          Height = 17
          Caption = '&Warn when there is not enough free space'
          TabOrder = 4
          OnClick = ControlChange
        end
      end
    end
    object CommanderSheet: TTabSheet
      Caption = 'Commander'
      ImageIndex = 3
      DesignSize = (
        378
        339)
      object Label3: TLabel
        Left = 8
        Top = 8
        Width = 366
        Height = 29
        Anchors = [akLeft, akTop, akRight]
        AutoSize = False
        Caption = 
          'Preferences on this tab applies to Norton Commander interface on' +
          'ly.'
        WordWrap = True
      end
      object PanelsGroup: TXPGroupBox
        Left = 8
        Top = 38
        Width = 362
        Height = 99
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Panels'
        TabOrder = 0
        DesignSize = (
          362
          99)
        object DeleteToRecycleBinCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Delete local files to recycle bin'
          TabOrder = 0
          OnClick = ControlChange
        end
        object ExplorerStyleSelectionCheck: TCheckBox
          Left = 16
          Top = 45
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Explorer style selection'
          TabOrder = 1
          OnClick = ControlChange
        end
        object PreserveLocalDirectoryCheck: TCheckBox
          Left = 16
          Top = 69
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Do not &change local directory when switching sessions'
          TabOrder = 2
          OnClick = ControlChange
        end
      end
      object CommanderMiscGroup: TXPGroupBox
        Left = 8
        Top = 146
        Width = 362
        Height = 53
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Miscellaneous'
        TabOrder = 1
        DesignSize = (
          362
          53)
        object UseLocationProfilesCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Use Location Profiles instead of Directory Bookmarks'
          TabOrder = 0
          OnClick = ControlChange
        end
      end
    end
    object ExplorerSheet: TTabSheet
      Caption = 'Explorer'
      ImageIndex = 5
      DesignSize = (
        378
        339)
      object Label4: TLabel
        Left = 8
        Top = 8
        Width = 366
        Height = 29
        Anchors = [akLeft, akTop, akRight]
        AutoSize = False
        Caption = 'Preferences on this tab applies to Explorer-like interface only.'
        WordWrap = True
      end
      object GroupBox2: TXPGroupBox
        Left = 8
        Top = 38
        Width = 362
        Height = 54
        Anchors = [akLeft, akTop, akRight]
        Caption = 'View'
        TabOrder = 0
        DesignSize = (
          362
          54)
        object ShowFullAddressCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Show full path on address bar'
          TabOrder = 0
          OnClick = ControlChange
        end
      end
    end
    object TransferSheet: TTabSheet
      Caption = 'Transfer'
      ImageIndex = 6
      inline CopyParamsFrame: TCopyParamsFrame
        Left = 0
        Top = 0
        Width = 529
        Height = 345
        TabOrder = 0
        inherited RemotePropertiesGroup: TXPGroupBox
          Left = 8
          Top = 156
          Width = 182
          Height = 126
          Caption = 'Upload options'
          inherited RightsFrame: TRightsFrame
            Height = 88
          end
          inherited RemotePreserveTimeCheck: TCheckBox
            Top = 161
          end
        end
        inherited CommonPropertiesGroup: TXPGroupBox
          Left = 198
          Top = 156
          Width = 172
          Height = 45
          Caption = 'Common options'
          DesignSize = (
            172
            45)
          inherited CommonPreserveTimestampCheck: TCheckBox
            Top = 18
            Width = 155
          end
        end
        inherited LocalPropertiesGroup: TXPGroupBox
          Left = 8
          Top = 284
          Width = 182
          Height = 45
          Caption = 'Download options'
          DesignSize = (
            182
            45)
          inherited PreserveReadOnlyCheck: TCheckBox
            Top = 18
            Width = 164
          end
          inherited LocalPreserveTimeCheck: TCheckBox
            Top = 92
            Width = 164
          end
        end
        inherited ChangeCaseGroup: TXPGroupBox
          Left = 247
          Top = 8
          Width = 123
          inherited CCNoChangeButton: TRadioButton
            Width = 110
          end
          inherited CCUpperCaseButton: TRadioButton
            Width = 110
          end
          inherited CCLowerCaseButton: TRadioButton
            Width = 110
          end
          inherited CCFirstUpperCaseButton: TRadioButton
            Width = 110
          end
        end
        inherited TransferModeGroup: TXPGroupBox
          Left = 8
          Top = 8
          Width = 231
          inherited TMTextButton: TRadioButton
            Width = 219
          end
          inherited TMBinaryButton: TRadioButton
            Width = 218
          end
          inherited TMAutomaticButton: TRadioButton
            Width = 218
          end
          inherited AsciiFileMaskCombo: THistoryComboBox
            Width = 213
          end
        end
      end
      object ResumeBox: TXPGroupBox
        Left = 198
        Top = 206
        Width = 172
        Height = 123
        Caption = 'Enable transfer resume for'
        TabOrder = 1
        object ResumeThresholdUnitLabel: TLabel
          Left = 136
          Top = 70
          Width = 13
          Height = 13
          Caption = 'kB'
          FocusControl = ResumeThresholdEdit
        end
        object ResumeOnButton: TRadioButton
          Left = 11
          Top = 21
          Width = 156
          Height = 17
          Caption = 'A&ll files (not recommended)'
          TabOrder = 0
          OnClick = ControlChange
        end
        object ResumeSmartButton: TRadioButton
          Left = 11
          Top = 45
          Width = 156
          Height = 17
          Caption = 'Files abo&ve'
          TabOrder = 1
          OnClick = ControlChange
        end
        object ResumeOffButton: TRadioButton
          Left = 12
          Top = 97
          Width = 156
          Height = 17
          Caption = 'Di&sable'
          TabOrder = 3
          OnClick = ControlChange
        end
        object ResumeThresholdEdit: TUpDownEdit
          Left = 45
          Top = 66
          Width = 84
          Height = 21
          Alignment = taRightJustify
          Increment = 100
          MaxValue = 4194304
          TabOrder = 2
          OnClick = ControlChange
        end
      end
    end
    object EditorSheet: TTabSheet
      Caption = 'Editor'
      ImageIndex = 7
      DesignSize = (
        378
        339)
      object EditorGroup: TXPGroupBox
        Left = 8
        Top = 8
        Width = 362
        Height = 103
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Default editor'
        TabOrder = 0
        DesignSize = (
          362
          103)
        object EditorInternalButton: TRadioButton
          Left = 16
          Top = 21
          Width = 113
          Height = 17
          Caption = '&Internal editor'
          TabOrder = 0
        end
        object EditorExternalButton: TRadioButton
          Left = 16
          Top = 45
          Width = 113
          Height = 17
          Caption = '&External editor'
          TabOrder = 1
        end
        object ExternalEditorEdit: TFilenameEdit
          Left = 32
          Top = 69
          Width = 315
          Height = 21
          OnAfterDialog = ExternalEditorEditAfterDialog
          Filter = 'Executable files (*.exe)|*.exe|All files (*.*)|*.*'
          ClickKey = 16397
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 2
          Text = 'ExternalEditorEdit'
          OnChange = ExternalEditorEditChange
          OnExit = ExternalEditorEditExit
        end
      end
      object EditorFontGroup: TXPGroupBox
        Left = 8
        Top = 117
        Width = 362
        Height = 56
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Editor font'
        TabOrder = 1
        DesignSize = (
          362
          56)
        object EditorFontLabel: TLabel
          Left = 16
          Top = 25
          Width = 249
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'EditorFontLabel'
        end
        object EditorFontButton: TButton
          Left = 271
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Select...'
          TabOrder = 0
          OnClick = EditorFontButtonClick
        end
      end
      object EditorOptionsGroup: TXPGroupBox
        Left = 8
        Top = 179
        Width = 362
        Height = 51
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Options'
        TabOrder = 2
        DesignSize = (
          362
          51)
        object EditorWordWrapCheck: TCheckBox
          Left = 16
          Top = 21
          Width = 330
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = '&Wrap long lines'
          TabOrder = 0
          OnClick = ControlChange
        end
      end
    end
    object IntegrationSheet: TTabSheet
      Caption = 'Integration'
      ImageIndex = 8
      DesignSize = (
        378
        339)
      object ShellIconsGroup: TXPGroupBox
        Left = 8
        Top = 8
        Width = 362
        Height = 209
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Shell icons'
        TabOrder = 0
        object ShellIconsLabel: TLabel
          Left = 16
          Top = 155
          Width = 329
          Height = 46
          AutoSize = False
          Caption = 
            'To add shortcuts, which directly open stored session, use button' +
            ' '#39'Shell icon'#39' on '#39'Stored sessions'#39' tab of login dialog.'
          WordWrap = True
        end
        object DesktopIconButton: TButton
          Left = 16
          Top = 24
          Width = 305
          Height = 25
          Caption = 'Create a &desktop icon'
          TabOrder = 0
          OnClick = IconButtonClick
        end
        object QuickLaunchIconButton: TButton
          Left = 16
          Top = 88
          Width = 305
          Height = 25
          Caption = 'Create a &Quick Launch icon'
          TabOrder = 1
          OnClick = IconButtonClick
        end
        object DesktopIconAllUsersButton: TButton
          Left = 16
          Top = 56
          Width = 305
          Height = 25
          Caption = 'Create a &desktop icon (all users)'
          TabOrder = 2
          OnClick = IconButtonClick
        end
        object SendToHookButton: TButton
          Left = 16
          Top = 120
          Width = 305
          Height = 25
          Caption = 'Add upload shortcut to Explorer'#39's '#39'&Send to'#39' context menu'
          TabOrder = 3
          OnClick = IconButtonClick
        end
      end
      object XPGroupBox1: TXPGroupBox
        Left = 8
        Top = 224
        Width = 362
        Height = 78
        Anchors = [akLeft, akTop, akRight]
        Caption = 'External applications'
        TabOrder = 1
        DesignSize = (
          362
          78)
        object Label2: TLabel
          Left = 16
          Top = 24
          Width = 61
          Height = 13
          Caption = '&PuTTY path:'
        end
        object PuttyPathEdit: TFilenameEdit
          Left = 16
          Top = 41
          Width = 330
          Height = 21
          OnAfterDialog = ExternalEditorEditAfterDialog
          Filter = 
            'PuTTY executable (putty.exe)|putty.exe|Executable files (*.exe)|' +
            '*.exe|All files (*.*)|*.*'
          ClickKey = 16397
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          Text = 'PuttyPathEdit'
          OnChange = ExternalEditorEditChange
          OnExit = ExternalEditorEditExit
        end
      end
    end
    object CustomCommandsSheet: TTabSheet
      Caption = 'Commands'
      ImageIndex = 9
      DesignSize = (
        378
        339)
      object CustomCommandsGroup: TXPGroupBox
        Left = 8
        Top = 8
        Width = 362
        Height = 321
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = 'Custom commands (SCP only)'
        TabOrder = 0
        DesignSize = (
          362
          321)
        object LocalDirectoryLabel: TLabel
          Left = 16
          Top = 24
          Width = 141
          Height = 13
          Caption = 'Custom command descri&ption:'
          FocusControl = CustomCommandDescEdit
        end
        object RemoteDirectoryLabel: TLabel
          Left = 16
          Top = 72
          Width = 232
          Height = 13
          Caption = '&Custom command (! is replaced with name of file):'
          FocusControl = CustomCommandEdit
        end
        object CustomCommandDescEdit: TEdit
          Left = 16
          Top = 41
          Width = 330
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          Text = 'CustomCommandDescEdit'
          OnChange = ControlChange
        end
        object CustomCommandEdit: TEdit
          Left = 16
          Top = 89
          Width = 330
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          MaxLength = 1000
          TabOrder = 1
          Text = 'CustomCommandEdit'
          OnChange = ControlChange
        end
        object CustomCommandsView: TListView
          Left = 16
          Top = 120
          Width = 238
          Height = 185
          Anchors = [akLeft, akTop, akRight, akBottom]
          Columns = <
            item
              Caption = 'Description'
              Width = 80
            end
            item
              Caption = 'Command'
              Width = 140
            end>
          ColumnClick = False
          DragMode = dmAutomatic
          HideSelection = False
          OwnerData = True
          ReadOnly = True
          RowSelect = True
          TabOrder = 2
          ViewStyle = vsReport
          OnData = CustomCommandsViewData
          OnDragDrop = CustomCommandsViewDragDrop
          OnDragOver = CustomCommandsViewDragOver
          OnKeyDown = CustomCommandsViewKeyDown
          OnSelectItem = CustomCommandsViewSelectItem
          OnStartDrag = CustomCommandsViewStartDrag
        end
        object AddCommandButton: TButton
          Left = 263
          Top = 120
          Width = 83
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Add'
          TabOrder = 3
          OnClick = AddCommandButtonClick
        end
        object RemoveCommandButton: TButton
          Left = 263
          Top = 184
          Width = 83
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Remove'
          TabOrder = 5
          OnClick = RemoveCommandButtonClick
        end
        object UpCommandButton: TButton
          Left = 263
          Top = 248
          Width = 83
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Up'
          TabOrder = 6
          OnClick = UpDownCommandButtonClick
        end
        object DownCommandButton: TButton
          Left = 263
          Top = 280
          Width = 83
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Down'
          TabOrder = 7
          OnClick = UpDownCommandButtonClick
        end
        object SaveCommandButton: TButton
          Left = 263
          Top = 152
          Width = 83
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '&Save'
          TabOrder = 4
          OnClick = SaveCommandButtonClick
        end
      end
    end
  end
  object OKButton: TButton
    Left = 217
    Top = 394
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object CloseButton: TButton
    Left = 305
    Top = 394
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
end
