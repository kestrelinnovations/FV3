// DrawColorRgbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColorRgbDlg.h"
//#include "afxdialogex.h"
#include "colordlg.h"
#include "..\getobjpr.h"



// CDrawColorRgbDlg dialog

IMPLEMENT_DYNAMIC(CDrawColorRgbDlg, CDialogEx)

// **************************************************************************
// **************************************************************************

CDrawColorRgbDlg::CDrawColorRgbDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrawColorRgbDlg::IDD, pParent)
{
	m_title = "Colors";

}

// **************************************************************************
// **************************************************************************

CDrawColorRgbDlg::~CDrawColorRgbDlg()
{
}

// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// **************************************************************************
// **************************************************************************

BEGIN_MESSAGE_MAP(CDrawColorRgbDlg, CDialogEx)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_FORE_COLOR, &CDrawColorRgbDlg::OnBnClickedForeColor)
	ON_BN_CLICKED(IDC_BACK_COLOR, &CDrawColorRgbDlg::OnBnClickedBackColor)
END_MESSAGE_MAP()


// **************************************************************************
// **************************************************************************

BOOL CDrawColorRgbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	return TRUE;
}
// CDrawColorRgbDlg message handlers


// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::OnBnClickedForeColor()
{
	CColorDialog dlg(m_color);
	int rslt;

	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		m_color = dlg.GetColor();
		CDrawColorDlg::draw_color_button_rgb(this, IDC_FORE_COLOR, m_back_color);	
		Invalidate();
	}
}


// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::OnBnClickedBackColor()
{
	CColorDialog dlg(m_back_color);
	int rslt;

	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		m_back_color = dlg.GetColor();
		CDrawColorDlg::draw_color_button_rgb(this, IDC_BACK_COLOR, m_back_color);	
		Invalidate();
	}
}

// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    switch (nIDCtl) 
    {
    	case IDC_FORE_COLOR:
        	CDrawColorDlg::draw_color_button_rgb(this, IDC_FORE_COLOR, m_color);
        	break;
	    case IDC_BACK_COLOR:
			CDrawColorDlg::draw_color_button_rgb(this, IDC_BACK_COLOR, m_back_color);
        	break;
			
	    default:
    	    return;
    }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
// end of OnDrawItem

// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::set_title(CString title) 
{
	m_title = title;
}
// end of set_title

// **************************************************************************
// **************************************************************************

COLORREF CDrawColorRgbDlg::get_color()
{
	return m_color;
}
// end of get_color
	
// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::set_color(COLORREF color)
{
	m_color = color;
}
// end of set_color
	
// **************************************************************************
// **************************************************************************

COLORREF CDrawColorRgbDlg::get_back_color()
{
	return m_back_color;
}
// end of get_back_color
	
// **************************************************************************
// **************************************************************************

void CDrawColorRgbDlg::set_back_color(COLORREF color)
{
	m_back_color = color;
}
// end of set_back_color
	
// **************************************************************************
// **************************************************************************
LRESULT CDrawColorRgbDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

