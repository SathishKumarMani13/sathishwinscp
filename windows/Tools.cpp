//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop

#include <shlobj.h>

#include <Common.h>
#include <TextsWin.h>

#include "GUITools.h"
#include "Tools.h"
#include "TB2Dock.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TFontStyles __fastcall IntToFontStyles(int value)
{
  TFontStyles Result;
  for (int i = fsBold; i <= fsStrikeOut; i++)
  {
    if (value & 1)
    {
      Result << (TFontStyle)i;
    }
    value >>= 1;
  }
  return Result;
}
//---------------------------------------------------------------------------
int __fastcall FontStylesToInt(const TFontStyles value)
{
  int Result = 0;
  for (int i = fsStrikeOut; i >= fsBold; i--)
  {
    Result <<= 1;
    if (value.Contains((TFontStyle)i))
    {
      Result |= 1;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall CenterFormOn(TForm * Form, TControl * CenterOn)
{
  TPoint ScreenPoint = CenterOn->ClientToScreen(TPoint(0, 0));
  Form->Left = ScreenPoint.x + (CenterOn->Width / 2) - (Form->Width / 2);
  Form->Top = ScreenPoint.y + (CenterOn->Height / 2) - (Form->Height / 2);
}
//---------------------------------------------------------------------------
AnsiString __fastcall GetListViewStr(TListView * ListView)
{
  AnsiString Result;
  for (int Index = 0; Index < ListView->Columns->Count; Index++)
  {
    if (!Result.IsEmpty())
    {
      Result += ",";
    }
    Result += IntToStr(ListView->Column[Index]->Width);
  }
  return Result;
}
//---------------------------------------------------------------------------
void __fastcall LoadListViewStr(TListView * ListView, AnsiString LayoutStr)
{
  int Index = 0;
  while (!LayoutStr.IsEmpty() && (Index < ListView->Columns->Count))
  {
    ListView->Column[Index]->Width = StrToIntDef(
      CutToChar(LayoutStr, ',', true), ListView->Column[Index]->Width);
    Index++;  
  }
}
//---------------------------------------------------------------------------
bool __fastcall ExecuteShellAndWait(const AnsiString Path, const AnsiString Params)
{
  return ExecuteShellAndWait(Application->Handle, Path, Params, 
    &Application->ProcessMessages);
}
//---------------------------------------------------------------------------
bool __fastcall ExecuteShellAndWait(const AnsiString Command)
{
  return ExecuteShellAndWait(Application->Handle, Command, 
    &Application->ProcessMessages);
} 
//---------------------------------------------------------------------------
void __fastcall CreateDesktopShortCut(const AnsiString &Name,
  const AnsiString &File, const AnsiString & Params, const AnsiString & Description,
  int SpecialFolder)
{
  IShellLink* pLink;
  IPersistFile* pPersistFile;
  LPMALLOC      ShellMalloc;
  LPITEMIDLIST  DesktopPidl;
  char DesktopDir[MAX_PATH];

  if (SpecialFolder < 0)
  {
    SpecialFolder = CSIDL_DESKTOPDIRECTORY;
  }

  try
  {
    if (FAILED(SHGetMalloc(&ShellMalloc))) throw Exception("");

    if (FAILED(SHGetSpecialFolderLocation(NULL, SpecialFolder, &DesktopPidl)))
    {
      throw Exception("");
    }

    if (!SHGetPathFromIDList(DesktopPidl, DesktopDir))
    {
      ShellMalloc->Free(DesktopPidl);
      ShellMalloc->Release();
      throw Exception("");
    }

    ShellMalloc->Free(DesktopPidl);
    ShellMalloc->Release();

    if (SUCCEEDED(CoInitialize(NULL)))
    {
      if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
          IID_IShellLink, (void **) &pLink)))
      {
        try
        {
          pLink->SetPath(File.c_str());
          pLink->SetDescription(Description.c_str());
          pLink->SetArguments(Params.c_str());
          pLink->SetShowCmd(SW_SHOW);

          if (SUCCEEDED(pLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile)))
          {
            try
            {
              WideString strShortCutLocation(DesktopDir);
              // Name can contain even path (e.g. to create quick launch icon)
              strShortCutLocation += AnsiString("\\") + Name + ".lnk";
              if (!SUCCEEDED(pPersistFile->Save(strShortCutLocation.c_bstr(), TRUE)))
              {
                throw Exception("");
              }
            }
            __finally
            {
              pPersistFile->Release();
            }
          }
        }
        __finally
        {
          pLink->Release();
        }
      }
      CoUninitialize();
    }
  }
  catch(...)
  {
    throw Exception(CREATE_SHORTCUT_ERROR);
  }
}
//---------------------------------------------------------------------------
void __fastcall ValidateMaskEdit(TComboBox * Edit)
{
  assert(Edit != NULL);
  TFileMasks Masks = Edit->Text;
  int Start, Length;
  if (!Masks.IsValid(Start, Length))
  {
    SimpleErrorDialog(FMTLOAD(MASK_ERROR, (Masks.Masks.SubString(Start + 1, Length))));
    Edit->SetFocus();
    Edit->SelStart = Start;
    Edit->SelLength = Length;
    Abort();
  }
}
//---------------------------------------------------------------------------
void __fastcall OpenBrowser(AnsiString URL)
{
  ShellExecute(Application->Handle, "open", URL.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
bool __fastcall IsFormatInClipboard(unsigned int Format)
{
  bool Result = OpenClipboard(0);
  if (Result)
  {
    Result = IsClipboardFormatAvailable(Format);
    CloseClipboard();
  }
  return Result;
}
//---------------------------------------------------------------------------
bool __fastcall TextFromClipboard(AnsiString & Text)
{
  bool Result = OpenClipboard(0);
  if (Result)
  {
    HANDLE Handle = NULL;
    try
    {
      Handle = GetClipboardData(CF_TEXT);
      Result = (Handle != NULL);
      if (Result)
      {
        Text = static_cast<const char*>(GlobalLock(Handle));
      }
    }
    __finally
    {
      if (Handle != NULL)
      {
        GlobalUnlock(Handle);
      }
      CloseClipboard();
    }
  }
  return Result;
}
