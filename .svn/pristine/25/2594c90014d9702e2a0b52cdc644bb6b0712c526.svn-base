// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// HistTransDlg.h



#pragma once

#ifdef JMPS_VARIANT
   #include "resource.h"
   #define SUCCESS 0
   #define FAILURE -1

   class CNITFOverlayVPS;
   class CNITFOverlayElement;
#else
   #include "nitf.h"
#endif

#define HISTOGRAM_BLACK_MARKER 1
#define HISTOGRAM_GRAY_MARKER  2
#define HISTOGRAM_WHITE_MARKER 3

#define NITF_TRANS_TIMER   435739
#define WM_GET_HISTOGRAM   (WM_USER + 10)


/////////////////////////////////////////////////////////////////////////////
// CHistTransDlg dialog

class CHistTransDlg : public CDialog
{
// Construction
public:
	CHistTransDlg(
#ifdef JMPS_VARIANT
      CNITFOverlayElement* pNITFOverlayElement,
      CImageCache*         pImageCache,
      CNITFOverlayVPS*     pNITFOverlayVPS,
#else
      C_nitf_obj*          pNITFObj,
#endif
      CWnd*                pParent = NULL );   // Standard constructor

   // External access to data
   LONG                 GetOpacity(){ return m_trans_level; }
   EnumEqualizationMode GetEqualizationMode() { return m_eEqualizationMode; }
   SHORT                GetMinDisplayLevel() { return m_black_val; }
   SHORT                GetMidDisplayLevel() { return m_gray_val; }
   SHORT                GetMaxDisplayLevel() { return m_white_val; }
#ifdef JMPS_VARIANT
   _bstr_t&             GetDisplayParamsXML() { return m_bstrDisplayParamsXML; }
   _bstr_t&             GetDBUpdateSQL(){ return m_bstrDBUpdateSQL; }
#endif
   
private:

// Dialog Data
	//{{AFX_DATA(CHistTransDlg)
	enum { IDD = IDD_HISTOGRAM_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Image_Enhancement_and_Transparency_Tactical_Imagery.htm";}

	CSliderCtrl m_trans_slider;
	int		   m_center;
	int		   m_maximum;
	int		   m_minimum;
	int		   m_trans_level;
	BOOL       m_is_multispectral;
#ifdef IDC_HISTOGRAM_PROGRESS
   CString     m_csHistogramProgress;
#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistTransDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHistTransDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnKillfocusTransLevel();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusCenter();
	afx_msg void OnKillfocusMaximum();
	afx_msg void OnKillfocusMinimum();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAuto();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoScreenContrast();
#if 0    // Not used
	afx_msg void OnHistogramContrast();
#endif
	afx_msg void OnEnhanceAuto();
	afx_msg void OnEnhanceHistogram();
	afx_msg void OnEnhanceNone();
	afx_msg void OnMsBands();
   afx_msg LRESULT OnGetHistogram( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   // Private data
   UINT        m_uiTransTimerID;
	CRect m_img_rect;
	CRect m_marker_rect;
#ifdef JMPS_VARIANT
   CNITFOverlayVPS* const  m_pNITFOverlayVPS;
   auto_ptr< UINT >        m_apuiFreqLum;
   _bstr_t                 m_bstrDBUpdateSQL;
#else
	C_nitf_obj* const m_pNITFObj;
#endif
   _bstr_t                 m_bstrDisplayParamsXML;
   IImageLibPtr            m_spImageLib;

   class ImageLibCallbackInterface :
      public CComObjectRootEx<CComSingleThreadModel>,
      public IDispatchImpl<IImageLibCallback, &IID_IImageLibCallback, &LIBID_IMAGELIBLib>
   {
   public:

      ImageLibCallbackInterface() : m_dProgressPercent( -1.0 ){}

      BEGIN_COM_MAP(ImageLibCallbackInterface)
         COM_INTERFACE_ENTRY(IImageLibCallback)
      END_COM_MAP()

      STDMETHOD(raw_imagelib_progress)( DOUBLE dPercent, BSTR bsLabel );
      STDMETHOD(raw_is_draw_interrupted)( VARIANT_BOOL* pvarIsDrawInterrupted )
      {
         *pvarIsDrawInterrupted = VARIANT_FALSE;
         return S_OK;
      }

      CHistTransDlg*    m_pHistTransDlg;
      DOUBLE            m_dProgressPercent;  // From last callback
   }; // End of embedded class ImageLibCallbackInterface
   CComObject<ImageLibCallbackInterface>
                        m_cbImageLibCallback; // For histogram progress
   HCURSOR              m_hWaitCursor;

   unsigned int m_hist[256];
   BOOL  m_bGetHistogramDone;
   PUINT m_puiFreqLum;
	int m_ctrval;
	int m_white_val;
	int m_gray_val;
	int m_black_val;
	int m_hit_marker;
	int m_mouse_x;
	int m_mouse_y;
	int m_mouse_off_x;
	int m_mouse_off_y;
	int m_oldmin;
	int m_oldmax;
	int m_oldctr;
	int m_first_min;
	int m_first_max;
	int m_first_ctr;
   EnumEqualizationMode m_eFirstEqualizationMode;
   _bstr_t m_bstrFirstDisplayParamsXML;
   INT m_first_trans_level;
	double m_gray_percent;
	EnumEqualizationMode m_eEqualizationMode;
	BOOL m_in_process;

   // Private methods
	void draw_display();
	void draw_page(CDC *pDC, CRect client_rect);
   BOOL GetHistogram();
	int compute_hist_data();
	void draw_black_triangle(CDC *dc, int x, int y); 
	void draw_white_triangle(CDC *dc, int x, int y); 
	void draw_gray_triangle(CDC *dc, int x, int y); 
	void draw_triangle(CDC *dc, COLORREF color, int x, int y); 
	void erase_triangle(CDC *dc, int trinum); 
	void erase_marker_rect(); 

	int marker_hit(int x, int y); 
	void drag_marker();
	void draw_markers(); 
	void calc_gray_percent();
	void adjust_gray();
	void refresh_screen();
	void show_histogram_stuff();
   VOID ShowHistogramProgress();
	void redraw_obj();
	BOOL ranges_ok();
   VOID UpdateDisplayParamsXML();

}; // class CHistTransDlg

// End of HistTransDlg.h