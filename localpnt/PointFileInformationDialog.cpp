// PointFileInformationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PointFileInformationDialog.h"

#import "FvCommonDialogs.tlb" no_namespace, named_guids

// PointFileInformationDialog dialog

IMPLEMENT_DYNAMIC(PointFileInformationDialog, CDialog)

PointFileInformationDialog::PointFileInformationDialog(CWnd* pParent /*=NULL*/)
: CDialog(PointFileInformationDialog::IDD, pParent)
, comments(_T(""))
, edition(_T(""))
, author(_T(""))
{

}

PointFileInformationDialog::~PointFileInformationDialog()
{
}

void PointFileInformationDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT_COMMENTS, comments);
   DDX_Text(pDX, IDC_EDIT_EDITION, edition);
   DDV_MaxChars(pDX, edition, 10);
   DDX_Text(pDX, IDC_EDIT_AUTHOR, author);
   DDV_MaxChars(pDX, author, 50);
   DDV_MaxChars(pDX, comments, 255);
}


BEGIN_MESSAGE_MAP(PointFileInformationDialog, CDialog)
   ON_BN_CLICKED(IDC_FONT, &PointFileInformationDialog::OnBnClickedFont)
END_MESSAGE_MAP()


// PointFileInformationDialog message handlers

void PointFileInformationDialog::OnBnClickedFont()
{
   IFVFontDialogPtr m_font_dialog(__uuidof(FVFontDialog));

   m_font_dialog->SetColor(m_text_fg_color);
   m_font_dialog->SetFont(_bstr_t(m_fontname));
   m_font_dialog->SetPointSize(m_text_size);
   m_font_dialog->SetAttributes(m_text_attributes);
   m_font_dialog->SetBackground(m_text_bg_type);
   m_font_dialog->SetBackgroundColor(m_text_bg_color);

   m_font_dialog->EnableRgb(1);

   if ( m_font_dialog->DoModal() == IDOK )
   {
      m_font_dialog->GetColor(&m_text_fg_color);
      m_font_dialog->GetPointSize(&m_text_size);
      m_font_dialog->GetAttributes(&m_text_attributes);
      m_font_dialog->GetBackground(&m_text_bg_type);
      m_font_dialog->GetBackgroundColor(&m_text_bg_color);

      BSTR bstr_font = NULL;
      m_font_dialog->GetFont(&bstr_font);
      AfxBSTR2CString(&m_fontname, bstr_font);
      SysFreeString(bstr_font);
   }

   m_font_dialog.Release();
}
