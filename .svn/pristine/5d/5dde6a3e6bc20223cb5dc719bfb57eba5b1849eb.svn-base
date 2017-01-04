#pragma once

#include "..\resource.h"

// PointFileInformationDialog dialog

class PointFileInformationDialog : public CDialog
{
   DECLARE_DYNAMIC(PointFileInformationDialog)

public:
   PointFileInformationDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~PointFileInformationDialog();

// Dialog Data
   enum { IDD = IDD_LOCAL_POINT_FILE_DLG };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   CString comments;
   CString edition;
   CString author;
   afx_msg void OnBnClickedFont();

   CString m_fontname;
   int m_text_size;
   int m_text_attributes;
   int m_text_fg_color;
   int m_text_bg_type;
   int m_text_bg_color;
};
