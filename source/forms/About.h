//----------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
#include <Configuration.h>
#include <GUITools.h>
//----------------------------------------------------------------------------
class TAboutDialog : public TForm
{
__published:
  TLabel *ApplicationLabel;
  TLabel *VersionLabel;
  TLabel *WinSCPCopyrightLabel;
  TStaticText *HomepageLabel;
  TLabel *ProductSpecificMessageLabel;
  TStaticText *ForumUrlLabel;
  TButton *OKButton;
  TButton *LicenseButton;
  TLabel *TranslatorLabel;
  TButton *HelpButton;
  TImage *Image;
  TStaticText *TranslatorUrlLabel;
  TLabel *Label3;
  TLabel *RegistrationLabel;
  TPanel *RegistrationBox;
  TLabel *RegistrationLicensesLabel;
  TStaticText *RegistrationProductIdLabel;
  TLabel *RegistrationSubjectLabel;
  TPanel *ThirdPartyPanel;
  void __fastcall LicenseButtonClick(TObject *Sender);
  void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall RegistrationProductIdLabelClick(TObject *Sender);
  void __fastcall OKButtonMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
private:
  TConfiguration * FConfiguration;
  TNotifyEvent FOnRegistrationLink;

  void __fastcall LoadData();
  void __fastcall LoadThirdParty();
  void __fastcall AddPara(UnicodeString & Text, const UnicodeString & S);
  UnicodeString __fastcall CreateLink(const UnicodeString & URL, const UnicodeString & Title = L"");
  void __fastcall ExpatLicenceHandler(TObject * Sender);

public:
  virtual __fastcall TAboutDialog(TComponent * AOwner,
    TConfiguration * Configuration, bool AllowLicense, TRegistration * Registration,
    bool ALoadThirdParty);
};
//----------------------------------------------------------------------------
#endif
