//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "GeneralSettings.h"

#include <Common.h>
#include <Configuration.h>
#include <ScpMain.h>
#include "CustomWinConfiguration.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XPThemes"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TGeneralSettingsFrame::TGeneralSettingsFrame(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TGeneralSettingsFrame::CommanderClick(TObject * /*Sender*/)
{
  CommanderInterfaceButton->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TGeneralSettingsFrame::ExplorerClick(TObject * /*Sender*/)
{
  ExplorerInterfaceButton->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TGeneralSettingsFrame::SaveConfiguration()
{
  assert(CustomWinConfiguration);
  CustomWinConfiguration->Interface = CommanderInterfaceButton->Checked ?
    ifCommander : ifExplorer;
}
//---------------------------------------------------------------------------
void __fastcall TGeneralSettingsFrame::LoadConfiguration()
{
  assert(CustomWinConfiguration);
  switch (CustomWinConfiguration->Interface) {
    case ifCommander: CommanderInterfaceButton->Checked = True; break;
    case ifExplorer: ExplorerInterfaceButton->Checked = True; break;
    default: assert(false); break;
  }
}
