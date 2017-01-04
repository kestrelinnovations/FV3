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



#pragma once

#include "resource.h"
#include "geocx1.h"
#include <gdiplus.h>
#include "afxwin.h"
#include <afxdtctl.h>

// CGeotagPhotosDialog dialog

class CGeotagPhotosDialog : public CDialog
{
	DECLARE_DYNAMIC(CGeotagPhotosDialog)

public:
	CGeotagPhotosDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeotagPhotosDialog();

   void AddPhotoToGeotag(std::string strFilename)
   {
      m_vecPhotoFilenames.push_back(strFilename);
   }

   void UpdateLocation(double dLat, double dLon);

// Dialog Data
	enum { IDD = IDD_GEOTAG_PHOTOS };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Geotag_Photos.htm";}

   CGeocx1 m_geocx;
   CString m_strCurrentFilename;
   CButton m_btnSkip;
   CStatic m_wndPhotoPreview;
   CString m_strComment;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void PostNcDestroy();
   afx_msg void OnBnClickedGeotag();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnBnClickedGeotagAll();
   afx_msg void OnBnClickedSkip();
   afx_msg void OnBnClickedClose();
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

   std::vector<std::string> m_vecPhotoFilenames;
   unsigned int m_nCurrentPhotoIndex;
   Gdiplus::Image *m_pCurrentImage;
   BOOL m_bOldDisplayCenterCrosshair;

   BOOL GeoTagImage(CString& strJpegFilename);
   void SetCurrentPhoto(int nIndex);
   void DrawCurrentPhoto();
};

struct PHOTO_ITEM_DATA
{
   PHOTO_ITEM_DATA() 
   { 
      bIsGeoTagged = false;
      dLat = 0.0;
      dLon = 0.0;
      dt = 0;
      nImageIndex = -1;
   }

   BOOL bIsGeoTagged;
   double dLat;
   double dLon;
   std::string strFilename;
   std::string strImageDescription;
   DATE dt;   // the date/time from either the EXIF property or the file itself
   int nImageIndex;
};

// CPhotoImageCtrl

class CPhotoListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CPhotoListCtrl)

public:
	CPhotoListCtrl();
	virtual ~CPhotoListCtrl();
   void Initialize();

	void ShowFilesFromFolder(const CString& strFolderName);

protected:
	DECLARE_MESSAGE_MAP()
   afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);

   void CreateImageForPhoto(int i);

	CImageList m_imageList;	
   bool m_initialized;
   std::vector<PHOTO_ITEM_DATA *> m_vecItemData;
};

// CPhotoGeotaggingToolsDialog dialog

class CPhotoGeotaggingToolsDialog : public CDialog
{
	DECLARE_DYNAMIC(CPhotoGeotaggingToolsDialog)

public:
	CPhotoGeotaggingToolsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPhotoGeotaggingToolsDialog();

// Dialog Data
	enum { IDD = IDD_PHOTO_GEOTAGGING_TOOLS2 };

   CBrowseForFolder	m_treeBrowseCtrl;
   CPhotoListCtrl m_photoListCtrl;
   CButton m_btnAddToPointOverlay;
   CButton m_btnAddToDrawingOverlay;
   CButton m_btnAddToMovingMapOverlay;
   CButton m_btnGeotag;
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Photo_Geotagging_Tools.htm";}

protected:
   virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnBnClickedAddToPointOverlay();
   afx_msg void OnBnClickedAddToDrawingOverlay();
   afx_msg void OnBnClickedAddtoMovingMapOverlay();
   afx_msg void OnBnClickedGeotag();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);

   void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   void UpdateButtons();

	DECLARE_MESSAGE_MAP()
   DECLARE_EVENTSINK_MAP()

   CString m_strFolderName;
   int m_nWindowPosX, m_nWindowPosY;
   int m_nWindowWidth, m_nWindowHeight;
};

class CAutoLinkOffsetDialog : public CDialog
{
	DECLARE_DYNAMIC(CAutoLinkOffsetDialog)

public:
	CAutoLinkOffsetDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAutoLinkOffsetDialog();

   void GetOffset(COleDateTimeSpan& dtOffset, bool& bIsPositive);

// Dialog Data
	enum { IDD = IDD_AUTO_LINK_OFFSET };
   CDateTimeCtrl m_dtOffsetCtrl;
   COleDateTime m_dtSelectedTime;

   void SetTrailName(const CString& strTrailName) { m_trailName = strTrailName; }
   void SetOriginalDateTime(COleDateTime &dtOriginal) { m_dtOriginalDateTime = dtOriginal; }
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Time_Offset_geotag.htm";}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

   CString m_trailName;
   COleDateTime m_dtOriginalDateTime;
   CString m_strMessage;
};

class CPhotoGeotaggingOverlaySelectDialog : public CDialog
{
	DECLARE_DYNAMIC(CPhotoGeotaggingOverlaySelectDialog)

public:
   CPhotoGeotaggingOverlaySelectDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPhotoGeotaggingOverlaySelectDialog();
	
// Dialog Data
	enum { IDD = IDD_PHOTO_GEOTAGGING_OVERLAY_SELECT };
	CComboBox m_cbOverlays;

	GUID m_overlayDescGuid;
	bool m_bShowIconSelectionUi;
   C_overlay* m_pSelectedOverlay; 
   
	CIconImage *m_pIcon;
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Select_Overlay_geotag.htm";}

protected:
	virtual void DoDataExchange(CDataExchange *pDX);   // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
   afx_msg void OnPaint();
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};