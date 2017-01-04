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



// PhotoGeotaggingToolsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BrowseForFolder.h"
#include "PhotoGeotaggingToolsDialog.h"
#include "showrmk.h"
#include "fvwutil.h"
#include ".\MovingMapOverlay\factory.h"
#include "gps.h"
#include "ExifImage.h"
#include "..\Common\GdiplusUtil.h"
#include "param.h"
#include "mapview.h"
#include "getobjpr.h" // for fvw_get_frame()
#include "mapx.h"
#include "..\Utility\AlignWnd.h"
#include "Common\SafeArray.h"
#include "getobjpr.h"


#define DRAWING_OVERLAY_INITIAL_PHOTO_SIZE 100

// CGeotagPhotosDialog dialog
//

IMPLEMENT_DYNAMIC(CGeotagPhotosDialog, CDialog)

CGeotagPhotosDialog::CGeotagPhotosDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CGeotagPhotosDialog::IDD, pParent)
   , m_strCurrentFilename(_T("")),
   m_pCurrentImage(NULL),
   m_bOldDisplayCenterCrosshair(FALSE)
   , m_strComment(_T(""))
{
   // toggle the center cross-hair while the dialog is open
   MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView)
   {
      m_bOldDisplayCenterCrosshair = pView->m_bDisplayCenterCrosshair;
      pView->m_bDisplayCenterCrosshair = TRUE;
      OVL_get_overlay_manager()->invalidate_all();
   }
}

CGeotagPhotosDialog::~CGeotagPhotosDialog()
{
   delete m_pCurrentImage;
}

void CGeotagPhotosDialog::PostNcDestroy()
{
   CDialog::PostNcDestroy();

   // reset center cross-hair state
   MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView != NULL)
   {
      pView->m_bDisplayCenterCrosshair = m_bOldDisplayCenterCrosshair;
      pView->m_pGeotagPhotosDlg = NULL;

      OVL_get_overlay_manager()->invalidate_all();
   }

   // reopen the photo geotagging tools dialog
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->PostMessage(WM_COMMAND, ID_PHOTO_GEOTAGGING_TOOL, 0);

   delete this;
}

void CGeotagPhotosDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
   DDX_Control(pDX, IDC_SKIP, m_btnSkip);
   DDX_Text(pDX, IDC_FILENAME, m_strCurrentFilename);
   DDX_Control(pDX, IDC_PHOTO_PREVIEW, m_wndPhotoPreview);
   DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
}


BEGIN_MESSAGE_MAP(CGeotagPhotosDialog, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDC_GEOTAG, &CGeotagPhotosDialog::OnBnClickedGeotag)
   ON_BN_CLICKED(IDC_GEOTAG_ALL, &CGeotagPhotosDialog::OnBnClickedGeotagAll)
   ON_BN_CLICKED(IDC_SKIP, &CGeotagPhotosDialog::OnBnClickedSkip)
   ON_BN_CLICKED(IDCLOSE, &CGeotagPhotosDialog::OnBnClickedClose)
   ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CGeotagPhotosDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   if (m_vecPhotoFilenames.size() > 0)
      SetCurrentPhoto(0);

   // get the center geo-coordinates of the map
   //
   d_geo_t geoCenter = { 0.0, 0.0 };
   MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pMapView != NULL && pMapView->get_curr_map() != NULL)
      pMapView->get_curr_map()->get_actual_center(&geoCenter);

   UpdateLocation(geoCenter.lat, geoCenter.lon);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeotagPhotosDialog::SetCurrentPhoto(int nIndex)
{
   m_nCurrentPhotoIndex = nIndex;

   if (m_nCurrentPhotoIndex >= 0 && m_nCurrentPhotoIndex < m_vecPhotoFilenames.size())
   {
      CString strTitle;
      strTitle.Format("Geotag Photos (%d / %d)", m_nCurrentPhotoIndex + 1, m_vecPhotoFilenames.size());
      SetWindowText(strTitle);

      m_strCurrentFilename = CString(m_vecPhotoFilenames[m_nCurrentPhotoIndex].c_str());

      // see if current photo has any existing location or comment in the Exif header then populate the controls
      ExifImage image(_bstr_t(m_vecPhotoFilenames[m_nCurrentPhotoIndex].c_str()));

      double dLat, dLon;
      if (image.GetGpsLocation(dLat, dLon))
      {
         UpdateLocation(dLat, dLon);

         CView *pView = UTL_get_active_non_printing_view();
         if (pView != NULL)
         {
            if (UTL_change_view_map_to_best(pView, dLat, dLon) == SUCCESS)
               OVL_get_overlay_manager()->invalidate_all();
         }
      }

      CString strImageDescription;
      if (image.GetImageDescription(strImageDescription))
         m_strComment = strImageDescription;

      delete m_pCurrentImage;
      m_pCurrentImage = new Gdiplus::Image((wchar_t *)_bstr_t(m_strCurrentFilename));
      DrawCurrentPhoto();

      // can't skip the last file - disable the button
      m_btnSkip.EnableWindow(m_nCurrentPhotoIndex < m_vecPhotoFilenames.size() - 1);

      UpdateData(FALSE);
   }
}

void CGeotagPhotosDialog::UpdateLocation(double dLat, double dLon)
{
   m_geocx.SetGeocx(dLat, dLon);
}

void CGeotagPhotosDialog::OnPaint( void )
{
   CPaintDC pdc(this);
   DrawCurrentPhoto();
}

void CGeotagPhotosDialog::DrawCurrentPhoto()
{
   if (m_pCurrentImage != NULL)
   {
      Gdiplus::Graphics gc(m_wndPhotoPreview.m_hWnd);

      CRect clientRect;
      m_wndPhotoPreview.GetClientRect(&clientRect);
      gc.DrawImage(m_pCurrentImage, 0, 0, clientRect.Width(), clientRect.Height());
   }
}


// CGeotagPhotosDialog message handlers

void CGeotagPhotosDialog::OnBnClickedGeotag()
{
   // need to delete the image so the file can be written to
   delete m_pCurrentImage;
   m_pCurrentImage = NULL;

   if (GeoTagImage(m_strCurrentFilename))
   {
      CString strFilenameTmp = m_strCurrentFilename + ".tmp";
      if (!::MoveFileEx(strFilenameTmp, m_strCurrentFilename, MOVEFILE_REPLACE_EXISTING))
      {
         CString msg;
         msg.Format("Unable to write to %s (%d) - file not geotagged", m_strCurrentFilename, GetLastError());
         AfxMessageBox(msg);
      }

      OnBnClickedSkip();

      // if the last photo was geo-tagged, automatically close the dialog
      if (m_nCurrentPhotoIndex == m_vecPhotoFilenames.size())
         OnBnClickedClose();
   }
}

void CGeotagPhotosDialog::OnBnClickedGeotagAll()
{
   while (m_nCurrentPhotoIndex < m_vecPhotoFilenames.size())
      OnBnClickedGeotag();
}

void CGeotagPhotosDialog::OnBnClickedSkip()
{
   SetCurrentPhoto(m_nCurrentPhotoIndex + 1);
}

void CGeotagPhotosDialog::OnBnClickedClose()
{
   CDialog::OnClose();
   DestroyWindow();
}

BOOL CGeotagPhotosDialog::GeoTagImage(CString& strJpegFilename)
{
   double dLat = m_geocx.GetLatitude();
   double dLon = m_geocx.GetLongitude();

   ExifImage exifImage((wchar_t *)_bstr_t(strJpegFilename));

   if (exifImage.GetLastStatus() != Gdiplus::Ok)
   {
      CString msg;
      msg.Format("Unable to open %s", strJpegFilename);
      AfxMessageBox(msg);
      return FALSE;
   }

   exifImage.SetGpsLocation(dLat, dLon);

   UpdateData();
   exifImage.SetImageDescription(m_strComment);

   // Gdiplus::Image will not allow saving to file with save name - save to a 
   // temporary filename first and then rename
   CString strJpegFilenameTmp = strJpegFilename + ".tmp";

   CLSID jpgClsid;
   if (GdiPlusUtil::GetEncoderClsid(L"image/jpeg", &jpgClsid) != -1)
   {
      if (exifImage.Save((wchar_t *)_bstr_t(strJpegFilenameTmp), &jpgClsid, NULL) == Gdiplus::Ok)
      {
         return TRUE;
      }
      else
      {
         CString msg;
         msg.Format("Unable to update geotag information in %s", strJpegFilename);
         AfxMessageBox(msg);
      }
   }

   return FALSE;
}

LRESULT CGeotagPhotosDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

// CPhotoListCtrl

#define THUMBNAIL_WIDTH 151
#define THUMBNAIL_HEIGHT 115

IMPLEMENT_DYNAMIC(CPhotoListCtrl, CListCtrl)
CPhotoListCtrl::CPhotoListCtrl() :
   m_initialized(false)
{
}

CPhotoListCtrl::~CPhotoListCtrl()
{
   for(UINT i=0;i<m_vecItemData.size();++i)
      delete m_vecItemData[i];
}

BEGIN_MESSAGE_MAP(CPhotoListCtrl, CListCtrl)
   ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
END_MESSAGE_MAP()

void CPhotoListCtrl::Initialize()
{
   m_imageList.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
   SetImageList(&m_imageList, LVSIL_NORMAL);
   SetBkColor(RGB(242,242,242));

   m_initialized = true;
}

void CPhotoListCtrl::ShowFilesFromFolder(const CString& strFolderName)
{
   if (!m_initialized)
      return;

   CWaitCursor waitCursor;

   SetRedraw(FALSE);

   DeleteAllItems();

   for(UINT i=0;i<m_vecItemData.size();++i)
      delete m_vecItemData[i];
   m_vecItemData.erase(m_vecItemData.begin(), m_vecItemData.end());

   m_imageList.SetImageCount(0);

   WIN32_FIND_DATA findData;
   HANDLE findHandle = ::FindFirstFile(strFolderName + "\\*.jpg", &findData);
   if (findHandle != INVALID_HANDLE_VALUE)
   {
      do
      {
         CString strSpec(strFolderName);
         strSpec += "\\";
         strSpec += findData.cFileName;

         ExifImage gdiBitmap((wchar_t *)_bstr_t(strSpec));
         if (gdiBitmap.GetLastStatus() != Gdiplus::Ok)
            continue;

         PHOTO_ITEM_DATA *itemData = new PHOTO_ITEM_DATA;
         itemData->strFilename = strSpec;

         if (gdiBitmap.GetGpsLocation(itemData->dLat, itemData->dLon))
            itemData->bIsGeoTagged = true;

         if (!gdiBitmap.GetDateTimeOrig(itemData->dt))
         {
            // get date time from file itself
            CFileStatus fileStatus;
            CFile::GetStatus(strSpec, fileStatus);

            tm gmtTime;
            fileStatus.m_ctime.GetGmtTm(&gmtTime);
            itemData->dt = COleDateTime(1900 + gmtTime.tm_year, gmtTime.tm_mon + 1, gmtTime.tm_mday, gmtTime.tm_hour, 
               gmtTime.tm_min, gmtTime.tm_sec);
         }

         CString strImageDescription;
         if (gdiBitmap.GetImageDescription(strImageDescription))
            itemData->strImageDescription = std::string(strImageDescription);

         m_vecItemData.push_back(itemData);

         const int nItemIndex = InsertItem(m_vecItemData.size() - 1, findData.cFileName, I_IMAGECALLBACK);
         SetItemData(nItemIndex, reinterpret_cast<DWORD_PTR>(itemData));

      } while (::FindNextFile(findHandle, &findData));
      ::FindClose(findHandle);
   }

   SetRedraw(TRUE);
}

// The list control will call this when it needs to get an image index.  We 
// lazily load the images for the photos as necessary for efficiency
void CPhotoListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
   LV_ITEM *pItem = &((LV_DISPINFO*)pNMHDR)->item;

   if( pItem->mask & LVIF_IMAGE )
   {
      if (0 <= pItem->iItem && pItem->iItem < (int)m_vecItemData.size())
      {
         if (m_vecItemData[pItem->iItem]->nImageIndex == -1)
            CreateImageForPhoto(pItem->iItem);

         pItem->iImage = m_vecItemData[pItem->iItem]->nImageIndex;
      }
   }

   *pResult = 0; 
}

void CPhotoListCtrl::CreateImageForPhoto(int i)
{
   Gdiplus::Bitmap bmBlurBottom(AfxGetApp()->m_hInstance, L"BlurBottom");
   Gdiplus::Bitmap bmBlurRight(AfxGetApp()->m_hInstance, L"BlurRight");
   Gdiplus::Bitmap bmBlurTop(AfxGetApp()->m_hInstance, L"BlurTop");
   Gdiplus::Bitmap bmBlurLeft(AfxGetApp()->m_hInstance, L"BlurLeft");

   HICON hIcon = AfxGetApp()->LoadIconA(IDI_GLOBE);
   Gdiplus::Bitmap bmGeotagged(hIcon);

   ExifImage gdiBitmap((wchar_t *)_bstr_t(m_vecItemData[i]->strFilename.c_str()));
   if (gdiBitmap.GetLastStatus() != Gdiplus::Ok)
      return;

   Gdiplus::Bitmap *pThumbnail = static_cast<Gdiplus::Bitmap *>(gdiBitmap.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT));
   std::auto_ptr<Gdiplus::Bitmap> apThumbnail(pThumbnail);

   HBITMAP hBitmap;
   if (pThumbnail == NULL)
      return;

   Gdiplus::Graphics *pGraphics = Gdiplus::Graphics::FromImage(pThumbnail);
   std::auto_ptr<Gdiplus::Graphics> apGraphics(pGraphics);

   if (pGraphics != NULL)
   {
      // draw blur around edges of photo
      pGraphics->DrawImage(&bmBlurBottom, 0, THUMBNAIL_HEIGHT - 5, THUMBNAIL_WIDTH - 1, 5);
      pGraphics->DrawImage(&bmBlurRight, THUMBNAIL_WIDTH - 5, 0, 5, THUMBNAIL_HEIGHT);
      pGraphics->DrawImage(&bmBlurTop, 0, 0, THUMBNAIL_WIDTH - 1, 3);
      pGraphics->DrawImage(&bmBlurLeft, 0, 0, 3, THUMBNAIL_HEIGHT);

      // if the photo is geotagged, then draw an icon on the photo indicating this
      if (m_vecItemData[i]->bIsGeoTagged)
         pGraphics->DrawImage(&bmGeotagged, THUMBNAIL_WIDTH - 32, THUMBNAIL_HEIGHT - 32, 32, 32);
   }

   if (pThumbnail->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap) != Gdiplus::Ok)
      return;

   CBitmap *pBitmap = CBitmap::FromHandle(hBitmap);
   if (pBitmap == NULL)
      return;

   m_vecItemData[i]->nImageIndex = m_imageList.Add(pBitmap, RGB(0,0,0));
}

// CPhotoGeotaggingToolsDialog dialog

IMPLEMENT_DYNAMIC(CPhotoGeotaggingToolsDialog, CDialog)

CPhotoGeotaggingToolsDialog::CPhotoGeotaggingToolsDialog(CWnd* pParent /*=NULL*/)
: CDialog(CPhotoGeotaggingToolsDialog::IDD, pParent),
   m_nWindowPosX(0),
   m_nWindowPosY(0),
   m_nWindowWidth(0),
   m_nWindowHeight(0)
{
   m_strFolderName = PRM_get_registry_string("PhotoGeotagging", "LastFolderName", "Desktop");
}

CPhotoGeotaggingToolsDialog::~CPhotoGeotaggingToolsDialog()
{
   PRM_set_registry_string("PhotoGeotagging", "LastFolderName", m_strFolderName);
   PRM_set_registry_int("PhotoGeotagging", "PosX", m_nWindowPosX);
   PRM_set_registry_int("PhotoGeotagging", "PosY", m_nWindowPosY);
   PRM_set_registry_int("PhotoGeotagging", "Width", m_nWindowWidth);
   PRM_set_registry_int("PhotoGeotagging", "Height", m_nWindowHeight);
}

void CPhotoGeotaggingToolsDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_FOLDER_VIEW_TREE, m_treeBrowseCtrl);
   DDX_Control(pDX, IDC_PHOTO_LIST, m_photoListCtrl);
   DDX_Control(pDX, IDC_GEOTAG, m_btnGeotag);
   DDX_Control(pDX, IDC_ADD_TO_POINT_OVERLAY, m_btnAddToPointOverlay);
   DDX_Control(pDX, IDC_ADD_TO_DRAWING_OVERLAY, m_btnAddToDrawingOverlay);
   DDX_Control(pDX, IDC_ADD_TO_MOVING_MAP_OVERLAY, m_btnAddToMovingMapOverlay);
}


BEGIN_MESSAGE_MAP(CPhotoGeotaggingToolsDialog, CDialog)
   ON_NOTIFY(NM_DBLCLK, IDC_PHOTO_LIST, OnDoubleClickList)
   ON_NOTIFY(LVN_KEYDOWN, IDC_PHOTO_LIST, OnKeydown)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_PHOTO_LIST, OnListItemchanged)
   ON_BN_CLICKED(IDC_ADD_TO_POINT_OVERLAY, &CPhotoGeotaggingToolsDialog::OnBnClickedAddToPointOverlay)
   ON_BN_CLICKED(IDC_ADD_TO_DRAWING_OVERLAY, &CPhotoGeotaggingToolsDialog::OnBnClickedAddToDrawingOverlay)
   ON_BN_CLICKED(IDC_ADD_TO_MOVING_MAP_OVERLAY, &CPhotoGeotaggingToolsDialog::OnBnClickedAddtoMovingMapOverlay)
   ON_BN_CLICKED(IDC_GEOTAG, &CPhotoGeotaggingToolsDialog::OnBnClickedGeotag)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

BOOL CPhotoGeotaggingToolsDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_treeBrowseCtrl.SetTargetDir(m_strFolderName);
   m_treeBrowseCtrl.FillTree();

   m_photoListCtrl.Initialize();
   m_photoListCtrl.ShowFilesFromFolder(m_strFolderName);

   // disable tools until a selection is made
   m_btnAddToPointOverlay.EnableWindow(FALSE);
   m_btnAddToDrawingOverlay.EnableWindow(FALSE);
   m_btnGeotag.EnableWindow(FALSE);

   CRect clientRect;
   GetClientRect(&clientRect);

   // Restore the size and postition of the dialog
   const int nInitialX = PRM_get_registry_int("PhotoGeotagging", "PosX", clientRect.TopLeft().x);
   const int nInitialY = PRM_get_registry_int("PhotoGeotagging", "PosY", clientRect.TopLeft().y);
   const int nWidth = PRM_get_registry_int("PhotoGeotagging", "Width", clientRect.Width());
   const int nHeight = PRM_get_registry_int("PhotoGeotagging", "Height", clientRect.Height());
   MoveWindow(nInitialX, nInitialY, nWidth, nHeight);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// CPhotoGeotaggingToolsDialog message handlers

BEGIN_EVENTSINK_MAP(CPhotoGeotaggingToolsDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CMdmBrowse)
   ON_EVENT(CPhotoGeotaggingToolsDialog, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// The user has selected a folder

void CPhotoGeotaggingToolsDialog::OnSelectedFolderViewTree(LPCTSTR strFolderName) 
{
   try
   {
      IShellFolderObjectPtr smpShellFolderObject((IShellFolderObject*)m_treeBrowseCtrl.GetCrntShellFolder());

      if (smpShellFolderObject == NULL)
         return; // no current selected folder

      const long folderAttributes = smpShellFolderObject->m_Attributes;
      if ((folderAttributes & SFGAO_FILESYSTEM ) && (folderAttributes & SFGAO_CANLINK))
      {
         m_strFolderName = strFolderName;
         m_photoListCtrl.ShowFilesFromFolder(strFolderName);
         UpdateButtons();
      }
   }
   catch (_com_error err)
   {
      CString msg;
      msg.Format("CPhotoGeotaggingToolsDialog::OnSelectedFolderViewTree failed - %s", (char *)err.Description());
      ERR_report(msg);
   }
}

// Double-click on a photo will center the FalconView map if geotagging information is available
void CPhotoGeotaggingToolsDialog::OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (pNMListView->iItem != -1)
   {
      PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(pNMListView->iItem));
      if (pPhotoItemData != NULL && pPhotoItemData->bIsGeoTagged)
      {
         CView* pView = UTL_get_active_non_printing_view();
         if (pView != NULL && UTL_change_view_map_to_best(pView, pPhotoItemData->dLat, pPhotoItemData->dLon) == SUCCESS)
            OVL_get_overlay_manager()->invalidate_all();
      }
   }

   *pResult = 0;
}
 
// Ctrl+A selects all photos in the current directory
void CPhotoGeotaggingToolsDialog::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

   if (pLVKeyDown->wVKey == 'A' && GetKeyState(VK_CONTROL) < 0)
   {
      m_photoListCtrl.SetRedraw(FALSE);

      // set all selected, set last item selected with focus
      const int nItemCount = m_photoListCtrl.GetItemCount();
      for (int i = 0; i < nItemCount-1; i++)
         m_photoListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
      m_photoListCtrl.SetItemState(nItemCount-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

      m_photoListCtrl.SetRedraw(TRUE);
   }
}

void CPhotoGeotaggingToolsDialog::OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   UpdateButtons();
}

void CPhotoGeotaggingToolsDialog::UpdateButtons()
{
   const int nSelectedCount = m_photoListCtrl.GetSelectedCount();
   m_btnGeotag.EnableWindow(nSelectedCount > 0);

   int nSelectedGeotaggedCount = 0;
   int nItemIndex = -1;
   while ( (nItemIndex = m_photoListCtrl.GetNextItem(nItemIndex, LVNI_SELECTED) ) != -1)
   {
      PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(nItemIndex));
      if (pPhotoItemData->bIsGeoTagged)
         nSelectedGeotaggedCount++;
   };

   m_btnAddToPointOverlay.EnableWindow(nSelectedGeotaggedCount > 0);
   m_btnAddToDrawingOverlay.EnableWindow(nSelectedGeotaggedCount > 0);

   const bool bMovingMapTrailOpened = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail) != NULL;
   m_btnAddToMovingMapOverlay.EnableWindow(bMovingMapTrailOpened && nSelectedCount > 0);
}

void CPhotoGeotaggingToolsDialog::OnBnClickedAddToPointOverlay()
{
   CPhotoGeotaggingOverlaySelectDialog dlg;
   dlg.m_overlayDescGuid = FVWID_Overlay_Points;
   dlg.m_bShowIconSelectionUi = true;
   if (dlg.DoModal() != IDOK)
      return;

   C_overlay *pOverlay = dlg.m_pSelectedOverlay;
   
   if (pOverlay == nullptr)
      return;

   COverlayCOM* pOverlayCom = dynamic_cast<COverlayCOM*>(pOverlay);
   IFvDataSourceProviderPtr pProvider = pOverlayCom->GetFvOverlay();
   if( pProvider == nullptr )
      return;

   IFvDataSourcePtr pSource = pProvider->GetDataSource();
   IFvDataSetPtr pSet = pSource->GetDataSetByName(pProvider->GetDefaultDataSetName());

   if(pSet == nullptr)
   {
      //create the new set
      IFvDataSourceEditPtr pSourceEdit= pSource;
      if(pSourceEdit != nullptr)
      {
         pSourceEdit->StartEditing();
         pSet = pSourceEdit->CreateEmptyFeatureDataSet(pProvider->GetDefaultDataSetName(),"");
         pSourceEdit->FinishEditing();
      }
   }

   IFeatureDataSetEdit2Ptr pFeatureSetEdit = pSet;
   if(pFeatureSetEdit != nullptr)
   {
      pFeatureSetEdit->StartEditing();
      int nItemIndex = -1;
      while ( (nItemIndex = m_photoListCtrl.GetNextItem(nItemIndex, LVNI_SELECTED) ) != -1)
      {
         PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(nItemIndex));
         if (!pPhotoItemData->bIsGeoTagged)
            continue;

         //create the geometry
         IUtilityMethodsPtr util(CLSID_UtilityMethods);
         std::stringstream ss;
         ss.str("");
         ss << "Point(" << pPhotoItemData->dLon << " " << pPhotoItemData->dLat << ")";
         IGeometryPtr pGeometry = util->CreateGeometryFromWKT(ss.str().c_str());

         //create the feature
         IFeature2Ptr pFeature = pFeatureSetEdit->CreateProtoType(pGeometry);

         //TODO shouldn't the prototype assign the geometry we're passing in?
         pFeature->SetGeometry(pGeometry);

         //TODO the dataset should be smart enough to keep this in sync without having to set it here
         pFeature->SetFieldValueByName(L"GroupName",FIELD_TYPE_STRING,pProvider->GetDefaultDataSetName());

         //TODO get the id and style_icon_identifier alias from GeodataRenderers.RenderingOptions
         pFeature->SetFieldValueByName(L"ID",FIELD_TYPE_STRING,pProvider->GetNextFeatureId());
         std::stringstream iconName;
         iconName.str("");
         iconName << "\\icons\\" << (LPCSTR)dlg.m_pIcon->get_item_filename();
         pFeature->SetFieldValueByName(L"IconName",FIELD_TYPE_STRING,iconName.str().c_str());

         pFeature->SetFieldValueByName(L"Comment",FIELD_TYPE_STRING,pPhotoItemData->strImageDescription.substr(0, 255).c_str());

         BstrSafeArray sa;
         sa.Append(_bstr_t(pPhotoItemData->strFilename.c_str()).Detach());
         pFeature->SetFieldValueByName(L"LinkNames",FIELD_TYPE_STRING_ARRAY,(VARIANT)sa);

         pFeatureSetEdit->CopyFeature(pFeature);
      };
      pFeatureSetEdit->FinishEditing();
   }

   OVL_get_overlay_manager()->invalidate_all();
}

void CPhotoGeotaggingToolsDialog::OnBnClickedAddToDrawingOverlay()
{  
   CPhotoGeotaggingOverlaySelectDialog dlg;
   dlg.m_overlayDescGuid = FVWID_Overlay_Drawing;
   if (dlg.DoModal() != IDOK)
      return;

   C_overlay *pOverlay = dlg.m_pSelectedOverlay;
   COverlayCOM* pOverlayCom = dynamic_cast<COverlayCOM*>(pOverlay);

   if (pOverlayCom == nullptr)                          
      return;

   IDrawingOverlayPtr pDrawingOverlay = pOverlayCom->GetFvOverlay();

   if (pDrawingOverlay == nullptr)                          
      return;

   int nItemIndex = -1;
   while ( (nItemIndex = m_photoListCtrl.GetNextItem(nItemIndex, LVNI_SELECTED) ) != -1)
   {
      PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(nItemIndex));
      if (!pPhotoItemData->bIsGeoTagged)
         continue;

      pDrawingOverlay->AddPicture(pPhotoItemData->dLat, pPhotoItemData->dLon, FALSE,
         FALSE, TRUE, FALSE, UTIL_ANCHOR_LOWER_CENTER,
         _bstr_t(pPhotoItemData->strFilename.c_str()), _bstr_t(pPhotoItemData->strFilename.c_str()), 
         100, DRAWING_OVERLAY_INITIAL_PHOTO_SIZE, DRAWING_OVERLAY_INITIAL_PHOTO_SIZE, 
         _bstr_t(pPhotoItemData->strImageDescription.c_str()));
   }

   OVL_get_overlay_manager()->invalidate_all();
}

void CPhotoGeotaggingToolsDialog::OnBnClickedAddtoMovingMapOverlay()
{
   C_gps_trail *pTrail = static_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (pTrail == NULL)
      return;

   GPSPointIcon *pPoint = pTrail->get_at_index(0);
   if (pPoint == NULL)
      return;

   // obtain offset from the auto link offset dialog
   CAutoLinkOffsetDialog autoLinkOffsetDialog;
   autoLinkOffsetDialog.SetTrailName(OVL_get_overlay_manager()->GetOverlayDisplayName(pTrail));
   autoLinkOffsetDialog.SetOriginalDateTime(pPoint->get_date_time());
   if (autoLinkOffsetDialog.DoModal() != IDOK)
      return;

   COleDateTimeSpan dtOffset;
   bool bIsOffsetPositive;
   autoLinkOffsetDialog.GetOffset(dtOffset, bIsOffsetPositive);

   CString strMessage;

   int nItemIndex = -1;
   while ( (nItemIndex = m_photoListCtrl.GetNextItem(nItemIndex, LVNI_SELECTED) ) != -1)
   {
      PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(nItemIndex));
      COleDateTime dtPhoto(pPhotoItemData->dt);
      bIsOffsetPositive ? dtPhoto -= dtOffset : dtPhoto += dtOffset;

      GPSPointIcon *pPoint = pTrail->FindClosestPointByTime(dtPhoto);
      if (pPoint)
      {
         pPoint->AddLink(CString(pPhotoItemData->strFilename.c_str()));
         pTrail->set_modified(TRUE);

         // write geo-location back to the file
         ExifImage *exifImage = new ExifImage((wchar_t *)_bstr_t(pPhotoItemData->strFilename.c_str()));
         std::auto_ptr<ExifImage> apExifImage(exifImage);

         exifImage->SetGpsLocation(pPoint->get_latitude(), pPoint->get_longitude());

         // Gdiplus::Image will not allow saving to file with save name - save to a 
         // temporary filename first and then rename
         std::string strFilenameTmp = pPhotoItemData->strFilename + ".tmp";

         CLSID jpgClsid;
         if (GdiPlusUtil::GetEncoderClsid(L"image/jpeg", &jpgClsid) != -1)
         {
            if (exifImage->Save((wchar_t *)_bstr_t(strFilenameTmp.c_str()), &jpgClsid, NULL) == Gdiplus::Ok)
            {
               delete apExifImage.release();
               ::MoveFileEx(strFilenameTmp.c_str(), pPhotoItemData->strFilename.c_str(), MOVEFILE_REPLACE_EXISTING);
            }
         }
      }
      else
      {
         CString str;
         str.Format("The photo %s was not automatically linked because its time, %s, was outside the trail's bounds.\r\n\r\n",
            pPhotoItemData->strFilename.c_str(), dtPhoto.Format("%b %d %H:%M:%S %Y"));

         strMessage += str;
      }
   }

   // if one or more photos were not automatically linked to the moving map trail, explain to the user why
   if (strMessage.GetLength() > 0)
   {
      CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, strMessage, "Information", NULL);
   }

   // refresh current folder since images may have been geotagged
   m_photoListCtrl.ShowFilesFromFolder(m_strFolderName);

   OVL_get_overlay_manager()->invalidate_all();
}

void CPhotoGeotaggingToolsDialog::OnBnClickedGeotag()
{
   CGeotagPhotosDialog *pDlg = new CGeotagPhotosDialog();

   int nItemIndex = -1;
   while ( (nItemIndex = m_photoListCtrl.GetNextItem(nItemIndex, LVNI_SELECTED) ) != -1)
   {
      PHOTO_ITEM_DATA *pPhotoItemData = reinterpret_cast<PHOTO_ITEM_DATA *>(m_photoListCtrl.GetItemData(nItemIndex));
      pDlg->AddPhotoToGeotag(pPhotoItemData->strFilename);
   }

   pDlg->Create(IDD_GEOTAG_PHOTOS);

   MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView)
      pView->m_pGeotagPhotosDlg = pDlg;

   EndDialog(IDOK);
}

void CPhotoGeotaggingToolsDialog::OnSize(UINT nType, int cx, int cy)
{
   if (m_btnGeotag.m_hWnd == NULL)
      return;

   CRect dialogWindowRect;
   GetWindowRect(&dialogWindowRect);

   // expand the right-edge of the photo control to new right edge of dialog
   //
   CRect photoCtrlRect;
   m_photoListCtrl.GetWindowRect(&photoCtrlRect);
   photoCtrlRect.right = dialogWindowRect.right - 16;
   ScreenToClient(photoCtrlRect);
   m_photoListCtrl.MoveWindow(photoCtrlRect, FALSE);

   // left-align buttons
   //
   CAlignWnd buttonSet;
   buttonSet.Add(m_btnGeotag.m_hWnd);
   buttonSet.Add(m_btnAddToPointOverlay.m_hWnd);
   buttonSet.Add(m_btnAddToDrawingOverlay.m_hWnd);
   buttonSet.Add(m_btnAddToMovingMapOverlay.m_hWnd);

   CRect treeCtrlRect;
   m_treeBrowseCtrl.GetWindowRect(&treeCtrlRect);
   m_photoListCtrl.GetWindowRect(&photoCtrlRect);

   dialogWindowRect.left = treeCtrlRect.left;
   dialogWindowRect.right = photoCtrlRect.right;
   dialogWindowRect.bottom -= 10; // 10-pixel margin between buttons and bottom of dialog

   buttonSet.Align(m_hWnd, dialogWindowRect, ALIGN_LEFT|ALIGN_BOTTOM);

   // right-align OK button
   //
   CAlignWnd okButtonSet;
   okButtonSet.Add(GetDlgItem(IDOK)->m_hWnd);
   okButtonSet.Align(m_hWnd, dialogWindowRect, ALIGN_RIGHT|ALIGN_BOTTOM);

   // extend tree control down to buttons
   CRect okButtonRect;
   GetDlgItem(IDOK)->GetWindowRect(&okButtonRect);
   treeCtrlRect.bottom = okButtonRect.top - 10;
   ScreenToClient(&treeCtrlRect);
   m_treeBrowseCtrl.MoveWindow(treeCtrlRect, FALSE);

   // extend photo control down to buttons
   photoCtrlRect.bottom = okButtonRect.top - 10;
   ScreenToClient(&photoCtrlRect);
   m_photoListCtrl.MoveWindow(photoCtrlRect, FALSE);
   m_photoListCtrl.Arrange(LVA_DEFAULT);

   Invalidate();

   CRect windowRect;
   GetWindowRect(&windowRect);
   m_nWindowPosX = windowRect.TopLeft().x;
   m_nWindowPosY = windowRect.TopLeft().y;
   m_nWindowWidth = windowRect.Width();
   m_nWindowHeight = windowRect.Height();

   CDialog::OnSize(nType, cx, cy);
}

void CPhotoGeotaggingToolsDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
   lpMMI->ptMinTrackSize = CPoint(711,387);
   lpMMI->ptMaxTrackSize = CPoint(2048,2048);

   CDialog::OnGetMinMaxInfo(lpMMI);
}

LRESULT CPhotoGeotaggingToolsDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


// CAutoLinkOffsetDialog
//

IMPLEMENT_DYNAMIC(CAutoLinkOffsetDialog, CDialog)

CAutoLinkOffsetDialog::CAutoLinkOffsetDialog(CWnd* pParent /*= NULL*/)
   : CDialog(CAutoLinkOffsetDialog::IDD, pParent)
   , m_strMessage(_T(""))
{

}

CAutoLinkOffsetDialog::~CAutoLinkOffsetDialog()
{
}

void CAutoLinkOffsetDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_DATETIME, m_dtOffsetCtrl);
   DDX_DateTimeCtrl(pDX, IDC_DATETIME, m_dtSelectedTime);
   DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
}

BOOL CAutoLinkOffsetDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_strMessage.Format("The initial point of %s begins on %s at %s.  "
      "If you would like to automatically link in the selected photos relative to a different "
      "starting time you can change that here.", m_trailName, m_dtOriginalDateTime.Format("%b %d, %Y"),
      m_dtOriginalDateTime.Format("%H:%M:%S"));

   m_dtOffsetCtrl.SetFormat("MMM dd HH:mm:ss yyyy");
   m_dtSelectedTime = m_dtOriginalDateTime;

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoLinkOffsetDialog::GetOffset(COleDateTimeSpan& dtOffset, bool& bIsPositive)
{
   if (m_dtSelectedTime > m_dtOriginalDateTime)
   {
      dtOffset = m_dtSelectedTime - m_dtOriginalDateTime;
      bIsPositive = true;
   }
   else
   {
      dtOffset = m_dtOriginalDateTime - m_dtSelectedTime;
      bIsPositive = false;
   }
}

LRESULT CAutoLinkOffsetDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

BEGIN_MESSAGE_MAP(CAutoLinkOffsetDialog, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

// CPhotoGeotaggingOverlaySelectDialog
//

IMPLEMENT_DYNAMIC(CPhotoGeotaggingOverlaySelectDialog, CDialog)

CPhotoGeotaggingOverlaySelectDialog::CPhotoGeotaggingOverlaySelectDialog(CWnd* pParent /*= NULL*/)
   : CDialog(CPhotoGeotaggingOverlaySelectDialog::IDD, pParent),
    m_bShowIconSelectionUi(false),
    m_pIcon(NULL)
{

}

CPhotoGeotaggingOverlaySelectDialog::~CPhotoGeotaggingOverlaySelectDialog()
{
}

void CPhotoGeotaggingOverlaySelectDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CB_OVERLAYS, m_cbOverlays);
}

BOOL CPhotoGeotaggingOverlaySelectDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_cbOverlays.AddString("<Create New>");
   C_overlay* pOverlay = OVL_get_overlay_manager()->get_first_of_type(m_overlayDescGuid);
   while (pOverlay != NULL)
   {
      int nIndex = m_cbOverlays.AddString(OVL_get_overlay_manager()->GetOverlayDisplayName(pOverlay));
      m_cbOverlays.SetItemDataPtr(nIndex, pOverlay);

      pOverlay = OVL_get_overlay_manager()->get_next_of_type(pOverlay, m_overlayDescGuid);
   }
   m_cbOverlays.SetCurSel(0);

   if (m_bShowIconSelectionUi)
   {
      GetDlgItem(IDC_ICONUI_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ICONUI)->ShowWindow(SW_SHOW);
      m_pIcon = CIconImage::load_images("localpnt\\Photo.ico");
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPhotoGeotaggingOverlaySelectDialog::OnOK()
{
   m_pSelectedOverlay = static_cast<C_overlay *>(m_cbOverlays.GetItemDataPtr(m_cbOverlays.GetCurSel()));
   if (m_pSelectedOverlay == NULL)
      OVL_get_overlay_manager()->create(m_overlayDescGuid, &m_pSelectedOverlay);

   CDialog::OnOK();
}


LRESULT CPhotoGeotaggingOverlaySelectDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

BEGIN_MESSAGE_MAP(CPhotoGeotaggingOverlaySelectDialog, CDialog)
   ON_WM_PAINT()
   ON_WM_LBUTTONDBLCLK()
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

void CPhotoGeotaggingOverlaySelectDialog::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   if (m_pIcon != NULL)
   {
      CRect frame;
      GetDlgItem(IDC_ICONUI)->GetWindowRect(&frame);
      ScreenToClient(&frame);

      dc.DrawIcon(frame.TopLeft().x, frame.TopLeft().y, m_pIcon->get_icon(32));
   }
}

void CPhotoGeotaggingOverlaySelectDialog::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if (!m_bShowIconSelectionUi)
      return;

   CRect rect;
   GetDlgItem(IDC_ICONUI)->GetWindowRect(&rect);
   ScreenToClient(&rect);
   
   if (rect.PtInRect(point))
   {
      // get current filename
      CString initial_dir = PRM_get_registry_string("Main", "HD_DATA", "");
      initial_dir += "\\icons\\localpnt\\*.ico";
      static char szFilter[] = "Icon Files (*.ico)|*.ico||";
      CFileDialog dlg(TRUE, NULL, initial_dir, OFN_FILEMUSTEXIST, szFilter);
      dlg.m_ofn.lpstrTitle = "Select New Icon";

      if (dlg.DoModal() == IDOK)
      {
         m_pIcon = CIconImage::load_images(dlg.GetPathName(), "", 0);
         Invalidate();
      }
   }
}
