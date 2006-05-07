//----------------------------------------------------------------------------
#ifndef ComboInputH
#define ComboInputH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
//----------------------------------------------------------------------------
#include <SessionData.h>
//----------------------------------------------------------------------------
class TComboInputDialog : public TForm
{
__published:
  TButton * OKButton;
  TButton * CancelButton;
  TComboBox * InputCombo;
  TLabel *InputLabel;
  TButton *HelpButton;
  void __fastcall InputComboChange(TObject * Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

public:
  virtual __fastcall TComboInputDialog(TComponent * AOwner);

  void __fastcall StoredSessionsCloseQuery(TObject * Sender, bool &CanClose);

  __property TStrings * Items  = { read=GetItems, write=SetItems };
  __property AnsiString Prompt  = { read=GetPrompt, write=SetPrompt };
  __property AnsiString Text  = { read=GetText, write=SetText };
  __property bool AllowEmpty  = { read=FAllowEmpty, write=FAllowEmpty };
  __property TInputValidateEvent OnInputValidate = { read=FOnInputValidate, write=FOnInputValidate };

private:
  bool FAllowEmpty;
  TInputValidateEvent FOnInputValidate;

  void __fastcall UpdateControls();
  void __fastcall SetSessionList(TStoredSessionList * value);
  void __fastcall SetText(AnsiString value);
  AnsiString __fastcall GetText();
  void __fastcall SetPrompt(AnsiString value);
  AnsiString __fastcall GetPrompt();
  void __fastcall SetItems(TStrings * value);
  TStrings * __fastcall GetItems();
};
//----------------------------------------------------------------------------
#endif
