object EditorPreferencesDialog: TEditorPreferencesDialog
  Left = 303
  Top = 145
  HelpType = htKeyword
  HelpKeyword = 'ui_editor_preferences'
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  BorderStyle = bsDialog
  Caption = 'EditorPreferencesDialog'
  ClientHeight = 310
  ClientWidth = 403
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCloseQuery = FormCloseQuery
  DesignSize = (
    403
    310)
  PixelsPerInch = 96
  TextHeight = 13
  object ExternalEditorGroup: TXPGroupBox
    Left = 8
    Top = 196
    Width = 388
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Caption = 'External editor options (affects editing remote files only)'
    TabOrder = 2
    object ExternalEditorTextCheck: TCheckBox
      Left = 16
      Top = 45
      Width = 337
      Height = 17
      Caption = 'Force &text transfer mode for files edited in external editor'
      TabOrder = 1
    end
    object MDIExternalEditorCheck: TCheckBox
      Left = 16
      Top = 21
      Width = 337
      Height = 17
      Caption = 'E&xternal editor opens multiple files in one window (process)'
      TabOrder = 0
    end
  end
  object EditorGroup: TXPGroupBox
    Left = 8
    Top = 8
    Width = 388
    Height = 101
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Editor type'
    TabOrder = 0
    DesignSize = (
      388
      101)
    object EditorInternalButton: TRadioButton
      Left = 16
      Top = 21
      Width = 145
      Height = 17
      Caption = '&Internal editor'
      TabOrder = 0
      OnClick = ControlChange
    end
    object EditorExternalButton: TRadioButton
      Left = 16
      Top = 45
      Width = 145
      Height = 17
      Caption = '&External editor'
      TabOrder = 1
      OnClick = ControlChange
    end
    object ExternalEditorEdit: THistoryComboBox
      Left = 32
      Top = 69
      Width = 267
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      TabOrder = 2
      Text = 'ExternalEditorEdit'
      OnChange = ExternalEditorEditChange
      OnExit = ExternalEditorEditExit
    end
    object ExternalEditorBrowseButton: TButton
      Left = 305
      Top = 67
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'B&rowse...'
      TabOrder = 3
      OnClick = ExternalEditorBrowseButtonClick
    end
  end
  object MaskGroup: TXPGroupBox
    Left = 7
    Top = 116
    Width = 388
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Editor autoselection'
    TabOrder = 1
    DesignSize = (
      388
      73)
    object Label1: TLabel
      Left = 11
      Top = 20
      Width = 150
      Height = 13
      Caption = 'Use this editor for &following files:'
      FocusControl = MaskEdit
    end
    object MaskEdit: THistoryComboBox
      Left = 11
      Top = 39
      Width = 367
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      MaxLength = 1000
      TabOrder = 0
      Text = '*.*'
      OnExit = MaskEditExit
    end
  end
  object OkButton: TButton
    Left = 151
    Top = 277
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 3
  end
  object CancelButton: TButton
    Left = 235
    Top = 277
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 4
  end
  object HelpButton: TButton
    Left = 319
    Top = 277
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = '&Help'
    TabOrder = 5
    OnClick = HelpButtonClick
  end
end
