object CopyParamsFrame: TCopyParamsFrame
  Left = 0
  Top = 0
  Width = 508
  Height = 150
  TabOrder = 0
  object CommonPropertiesGroup: TXPGroupBox
    Left = 331
    Top = 0
    Width = 173
    Height = 146
    Caption = 'Attributes'
    TabOrder = 4
    DesignSize = (
      173
      146)
    object CommonPreserveTimestampCheck: TCheckBox
      Left = 12
      Top = 21
      Width = 156
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Preserve timesta&mp'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = ControlChange
    end
    object CommonCalculateSizeCheck: TCheckBox
      Left = 12
      Top = 45
      Width = 156
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&Calculate total size'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = ControlChange
    end
  end
  object LocalPropertiesGroup: TXPGroupBox
    Left = 331
    Top = 0
    Width = 173
    Height = 145
    Caption = 'Attributes'
    TabOrder = 3
    DesignSize = (
      173
      145)
    object PreserveReadOnlyCheck: TCheckBox
      Left = 12
      Top = 45
      Width = 155
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Preserve rea&d-only'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object LocalPreserveTimeCheck: TCheckBox
      Left = 12
      Top = 21
      Width = 155
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Preserve timesta&mp'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
  end
  object RemotePropertiesGroup: TXPGroupBox
    Left = 331
    Top = 0
    Width = 173
    Height = 146
    Caption = 'Attributes'
    TabOrder = 2
    inline RightsFrame: TRightsFrame
      Left = 7
      Top = 36
      Width = 163
      Height = 85
      PopupMenu = RightsFrame.RightsPopup
      TabOrder = 1
      inherited OthersButton: TSpeedButton
        Width = 56
      end
      inherited GroupButton: TSpeedButton
        Width = 56
      end
      inherited OwnerButton: TSpeedButton
        Width = 56
      end
      inherited OctalLabel: TLabel
        Visible = False
      end
      inherited OwnerReadCheck: TCheckBox
        Left = 58
        Width = 31
      end
      inherited OwnerWriteCheck: TCheckBox
        Left = 95
        Width = 31
      end
      inherited OwnerExecuteCheck: TCheckBox
        Left = 132
        Width = 31
      end
      inherited GroupReadCheck: TCheckBox
        Left = 58
        Width = 31
      end
      inherited GroupWriteCheck: TCheckBox
        Left = 95
        Width = 30
      end
      inherited GroupExecuteCheck: TCheckBox
        Left = 132
        Width = 31
      end
      inherited OthersReadCheck: TCheckBox
        Left = 58
        Width = 31
      end
      inherited OthersWriteCheck: TCheckBox
        Left = 95
        Width = 30
      end
      inherited OthersExecuteCheck: TCheckBox
        Left = 132
        Width = 31
      end
      inherited OctalEdit: TEdit
        Visible = False
      end
      inherited DirectoriesXCheck: TCheckBox
        Top = 65
      end
    end
    object PreserveRightsCheck: TCheckBox
      Left = 9
      Top = 16
      Width = 156
      Height = 17
      Caption = 'Set pe&rmissions'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = ControlChange
    end
    object RemotePreserveTimeCheck: TCheckBox
      Left = 12
      Top = 121
      Width = 156
      Height = 17
      Caption = 'Preserve timesta&mp'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = ControlChange
    end
  end
  object ChangeCaseGroup: TXPGroupBox
    Left = 206
    Top = 0
    Width = 120
    Height = 146
    Caption = 'Filename modification'
    TabOrder = 1
    DesignSize = (
      120
      146)
    object CCNoChangeButton: TRadioButton
      Left = 8
      Top = 19
      Width = 107
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&No change'
      TabOrder = 0
    end
    object CCUpperCaseButton: TRadioButton
      Left = 8
      Top = 44
      Width = 107
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&Upper case'
      TabOrder = 1
    end
    object CCLowerCaseButton: TRadioButton
      Left = 8
      Top = 69
      Width = 107
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Lo&wer case'
      TabOrder = 2
    end
    object CCFirstUpperCaseButton: TRadioButton
      Left = 8
      Top = 94
      Width = 107
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&First upper case'
      TabOrder = 3
    end
    object ReplaceInvalidCharsCheck: TCheckBox
      Left = 8
      Top = 120
      Width = 105
      Height = 17
      Caption = 'Replace '#39'\:*?'#39' ...'
      TabOrder = 4
      OnClick = ControlChange
    end
  end
  object TransferModeGroup: TXPGroupBox
    Left = 3
    Top = 0
    Width = 198
    Height = 146
    Caption = 'Transfer mode'
    TabOrder = 0
    DesignSize = (
      198
      146)
    object AsciiFileMaskLabel: TLabel
      Left = 10
      Top = 99
      Width = 164
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Transfer following &files in text mode'
      FocusControl = AsciiFileMaskCombo
    end
    object TMTextButton: TRadioButton
      Left = 7
      Top = 22
      Width = 189
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&Text (plain text, html, scripts, ...)'
      TabOrder = 0
      OnClick = ControlChange
    end
    object TMBinaryButton: TRadioButton
      Left = 7
      Top = 48
      Width = 189
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&Binary (archives, doc, ...)'
      TabOrder = 1
      OnClick = ControlChange
    end
    object TMAutomaticButton: TRadioButton
      Left = 7
      Top = 74
      Width = 189
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = '&Automatic'
      TabOrder = 2
      OnClick = ControlChange
    end
    object AsciiFileMaskCombo: THistoryComboBox
      Left = 9
      Top = 115
      Width = 180
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      MaxLength = 255
      TabOrder = 3
      Text = 'AsciiFileMaskCombo'
    end
  end
end
