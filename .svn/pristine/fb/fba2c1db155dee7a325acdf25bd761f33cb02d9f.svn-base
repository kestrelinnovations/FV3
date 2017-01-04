#pragma once
#include "afxwin.h"


// CVVODCurrencyDlg dialog

class CVVODCurrencyDlg : public CDialog
{
	DECLARE_DYNAMIC(CVVODCurrencyDlg)

public:
	CVVODCurrencyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVVODCurrencyDlg();

// Dialog Data
	enum { IDD = IDD_VVOD_CURRENCY };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Vector_Vertical_Obstruction_Data_Manager.htm";}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
   enum eUpdateTypes { MDM_FULL, MDM_UPD_LIBS, UPD_PATCH_FILES };

   bool TestVVODDataPath();                     // Check and verify if the (control) specified VVOD path is accessible
   void set_vvod_currency_state();              // Set the state of the Icons on the dialog (i.e. the Green Checks)
   void PopulatePaths(eUpdateTypes updType);    // Add online data paths to the dialog
   bool CopyVVODData(bool UpdateDataOnly);      // Performs the copy of VVOD data
   void CopyVVODUpdate();                       // Copies a VVOD update set onto the system
   void RemoveVVODUpdate(bool Silent=false);    // Removes the current update database

   CString m_UpdateDataPath;
   CStatic m_vvod_currency_cm;
   CStatic m_VVODUpdateStatusIcon;
   CStatic m_VVODOverallStatusIcon;
   CComboBox m_SourcePathsLB;
   CComboBox m_DestPathsLB;
   CString m_userSelectedPath;
   bool m_BlockHandlerMsg;                      // An internal flag to block Dialog created chnges from being handled

   bool m_VVODIsCurrent;                        // Internal flag as to whether VVOD data is current

public:
   afx_msg void OnBnClickedOk();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnEnKillfocusEditVvodUpdateDataPath();
   afx_msg void OnBnClickedVvodCurrencyUpdate();
   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
   afx_msg void OnBnClickedUpdateCopyVvodButton();
   afx_msg void OnBnClickedFullCopyVvodButton();
   afx_msg void OnCbnSelchangeSourceDataPath();
   afx_msg void OnBnClickedCopyVvodUpdButton();
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnBnClickedHelp();
};