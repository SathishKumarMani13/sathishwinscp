//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Common.h>

#include "CopyParams.h"

#include <VCLCommon.h>
#include <ScpMain.h>
#include "CustomWinConfiguration.h"
#include "TextsWin.h"
#include "Tools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Rights"
#pragma link "HistoryComboBox"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TCopyParamsFrame::TCopyParamsFrame(TComponent* Owner)
        : TFrame(Owner)
{
  // on start set different value than we want to allow property-setter to proceed
  FDirection = pdToLocal;
  Direction = pdToRemote;

  FOptions = cfAllowTransferMode | cfAllowExcludeMask;
  RightsFrame->AllowAddXToDirectories = True;
  FParams = new TCopyParamType();
  TCopyParamType DefParams;
  Params = DefParams;
}
//---------------------------------------------------------------------------
__fastcall TCopyParamsFrame::~TCopyParamsFrame()
{
  delete FParams;
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::SetParams(TCopyParamType value)
{
  assert((value.TransferMode == tmAscii) ||
    (value.TransferMode == tmBinary) || (value.TransferMode == tmAutomatic));
  switch (value.TransferMode) {
    case tmAscii: TMTextButton->Checked = True; break;
    case tmBinary: TMBinaryButton->Checked = True; break;
    default: TMAutomaticButton->Checked = True; break;
  }
  FOrigMasks = value.AsciiFileMask.Masks;
  AsciiFileMaskCombo->Text = value.AsciiFileMask.Masks;

  switch (value.FileNameCase) {
    case ncNoChange: CCNoChangeButton->Checked = True; break;
    case ncLowerCase: CCLowerCaseButton->Checked = True; break;
    case ncUpperCase: CCUpperCaseButton->Checked = True; break;
    case ncFirstUpperCase: CCFirstUpperCaseButton->Checked = True; break;
    case ncLowerCaseShort: CCLowerCaseShortButton->Checked = True; break;
  }

  ReplaceInvalidCharsCheck->Checked = value.ReplaceInvalidChars;

  RightsFrame->AddXToDirectories = value.AddXToDirectories;
  RightsFrame->Rights = value.Rights;
  PreserveRightsCheck->Checked = value.PreserveRights;
  PreserveReadOnlyCheck->Checked = value.PreserveReadOnly;

  assert(PreserveTimeCheck);
  PreserveTimeCheck->Checked = value.PreserveTime;

  CommonCalculateSizeCheck->Checked = value.CalculateSize;

  ExcludeFileMaskCombo->Text = value.ExcludeFileMask.Masks;

  *FParams = value;

  UpdateControls();
}
//---------------------------------------------------------------------------
TCopyParamType __fastcall TCopyParamsFrame::GetParams()
{
  TCopyParamType Result = *FParams;

  assert(TMTextButton->Checked || TMBinaryButton->Checked || TMAutomaticButton->Checked);
  if (TMTextButton->Checked) Result.TransferMode = tmAscii;
    else
  if (TMBinaryButton->Checked) Result.TransferMode = tmBinary;
    else Result.TransferMode = tmAutomatic;

  Result.AsciiFileMask.Masks = AsciiFileMaskCombo->Text;
  if (!Result.AsciiFileMask.IsValid()) Result.AsciiFileMask.Masks = FOrigMasks;

  if (CCLowerCaseButton->Checked) Result.FileNameCase = ncLowerCase;
    else
  if (CCUpperCaseButton->Checked) Result.FileNameCase = ncUpperCase;
    else
  if (CCFirstUpperCaseButton->Checked) Result.FileNameCase = ncFirstUpperCase;
    else
  if (CCLowerCaseShortButton->Checked) Result.FileNameCase = ncLowerCaseShort;
    else Result.FileNameCase = ncNoChange;

  Result.ReplaceInvalidChars = ReplaceInvalidCharsCheck->Checked;

  Result.AddXToDirectories = RightsFrame->AddXToDirectories;
  Result.Rights = RightsFrame->Rights;
  Result.PreserveRights = PreserveRightsCheck->Checked;
  Result.PreserveReadOnly = PreserveReadOnlyCheck->Checked;

  assert(PreserveTimeCheck);
  Result.PreserveTime = PreserveTimeCheck->Checked;

  Result.CalculateSize = CommonCalculateSizeCheck->Checked;

  Result.ExcludeFileMask.Masks = ExcludeFileMaskCombo->Text;

  return Result;
}
//---------------------------------------------------------------------------
TCheckBox * __fastcall TCopyParamsFrame::GetPreserveTimeCheck()
{
  switch (Direction) {
    case pdToRemote: return RemotePreserveTimeCheck;
    case pdToLocal: return LocalPreserveTimeCheck;
    case pdBoth:
    default: return CommonPreserveTimestampCheck;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::UpdateControls()
{
  EnableControl(TransferModeGroup,
    FLAGSET(Options, cfAllowTransferMode) && Enabled);
  EnableControl(AsciiFileMaskLabel,
    FLAGSET(Options, cfAllowTransferMode) && TMAutomaticButton->Checked && Enabled);
  EnableControl(AsciiFileMaskCombo,
    FLAGSET(Options, cfAllowTransferMode) && TMAutomaticButton->Checked && Enabled);
  EnableControl(RightsFrame, PreserveRightsCheck->Checked && Enabled);
  EnableControl(FilterGroup, FLAGSET(Options, cfAllowExcludeMask));
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::SetDirection(TParamsForDirection value)
{
  if (Direction != value)
  {
    Boolean APreserveTime = PreserveTimeCheck->Checked;
    FDirection = value;
    PreserveTimeCheck->Checked = APreserveTime;

    LocalPropertiesGroup->Visible = (Direction == pdToLocal || Direction == pdAll );
    RemotePropertiesGroup->Visible = (Direction == pdToRemote || Direction == pdAll );
    CommonPropertiesGroup->Visible = (Direction == pdBoth || Direction == pdAll );
    LocalPreserveTimeCheck->Visible = (Direction != pdAll);
    RemotePreserveTimeCheck->Visible = (Direction != pdAll);
    ReplaceInvalidCharsCheck->Visible =
      (Direction == pdToLocal || Direction == pdBoth || Direction == pdAll);
    CCFirstUpperCaseButton->Visible =
      (Direction == pdToLocal || Direction == pdToRemote);
    CCLowerCaseShortButton->Visible =
      (Direction == pdToRemote || Direction == pdBoth || Direction == pdAll);
    if (Direction == pdBoth || Direction == pdAll)
    {
      CCLowerCaseShortButton->Top = CCFirstUpperCaseButton->Top;
    }
    else
    {
      CCLowerCaseShortButton->Top = ReplaceInvalidCharsCheck->Top;
    }
    UpdateControls();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::ControlChange(TObject * /*Sender*/)
{
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::BeforeExecute()
{
  assert(CustomWinConfiguration);
  AsciiFileMaskCombo->Items = CustomWinConfiguration->History["Mask"];
  ExcludeFileMaskCombo->Items = CustomWinConfiguration->History["ExcludeMask"];
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::AfterExecute()
{
  assert(CustomWinConfiguration);
  AsciiFileMaskCombo->SaveToHistory();
  CustomWinConfiguration->History["Mask"] = AsciiFileMaskCombo->Items;
  ExcludeFileMaskCombo->SaveToHistory();
  CustomWinConfiguration->History["ExcludeMask"] = ExcludeFileMaskCombo->Items;
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::Validate()
{
  if (AsciiFileMaskCombo->Focused())
  {
    ValidateMaskEdit(AsciiFileMaskCombo);
  }
  if (ExcludeFileMaskCombo->Focused())
  {
    ValidateMaskEdit(ExcludeFileMaskCombo);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::SetOptions(int value)
{
  FOptions = value;
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::SetEnabled(Boolean Value)
{
  TFrame::SetEnabled(Value);
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TCopyParamsFrame::ValidateMaskComboExit(TObject * Sender)
{
  ValidateMaskEdit(dynamic_cast<TComboBox*>(Sender));
}
//---------------------------------------------------------------------------

