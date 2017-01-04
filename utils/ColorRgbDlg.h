#pragma once

#include "..\resource.h"

// CDrawColorRgbDlg dialog

class CDrawColorRgbDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawColorRgbDlg)

public:
	CDrawColorRgbDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDrawColorRgbDlg();

	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DRAW_COLOR_RGB_DLG };
	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Color_Selection_Tool_dwg.htm";}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnBnClickedForeColor();
	afx_msg void OnBnClickedBackColor();

	COLORREF m_color;
	COLORREF m_back_color;
	CString m_title;

	void set_title(CString title);
	COLORREF get_color();
	void set_color(COLORREF color);
	COLORREF get_back_color();
	void set_back_color(COLORREF color);

};