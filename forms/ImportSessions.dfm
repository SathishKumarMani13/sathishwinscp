object ImportSessionsDialog: TImportSessionsDialog
  Left = 362
  Top = 186
  BorderStyle = bsDialog
  Caption = 'Import sessions from PuTTY'
  ClientHeight = 293
  ClientWidth = 351
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    351
    293)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 337
    Height = 45
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 
      'Following list contains sessions stored in PuTTY SSH client. Che' +
      'ck sessions you want to import and press OK button.'
    WordWrap = True
  end
  object Label2: TLabel
    Left = 8
    Top = 52
    Width = 290
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Notice: This application always connects using SSH protocol.'
    WordWrap = True
  end
  object OKButton: TButton
    Left = 191
    Top = 262
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 3
  end
  object CancelButton: TButton
    Left = 271
    Top = 262
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 4
  end
  object SessionListView: TListView
    Left = 8
    Top = 72
    Width = 337
    Height = 160
    Anchors = [akLeft, akTop, akRight, akBottom]
    Checkboxes = True
    Columns = <
      item
        Caption = 'Session'
        Width = 240
      end
      item
        Caption = 'Protocol'
        Width = 60
      end>
    ColumnClick = False
    HideSelection = False
    ReadOnly = True
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    ViewStyle = vsReport
    OnInfoTip = SessionListViewInfoTip
    OnKeyUp = SessionListViewKeyUp
    OnMouseDown = SessionListViewMouseDown
  end
  object CheckAllButton: TButton
    Left = 8
    Top = 262
    Width = 89
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Un/check &all'
    TabOrder = 2
    OnClick = CheckAllButtonClick
  end
  object ImportKeysCheck: TCheckBox
    Left = 16
    Top = 238
    Width = 329
    Height = 17
    Caption = 'Import cached host &keys for checked sessions'
    TabOrder = 1
  end
end
