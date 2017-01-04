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



// optndlg.h : header file
//
#pragma once

#include "overlay.h"
#include "..\resource.h"
#include <set>
#include "..\overlay\OverlayCOM.h"    // for CMapChangeNotifyEventsImpl
#include "ResizableDialog.h"
#include "..\overlay\ScrollWnd.h"

// forward declarations
struct OverlayTypeDescriptor;
class CLayerOvlFactory;
class OverlayOptionsDlg;
 
/////////////////////////////////////////////////////////////////////////////
// OCXContainerPage dialog

class COCXControlWrapper : public CWnd
{
public:
   void SetPreferences(OLECHAR FAR*preference_string);
   CString GetPreferences();

private:
   bool GetIDOfName(OLECHAR FAR* MethodName, DISPID& dispid);
};

class OCXContainerPage : public CPropertyPage
{
	DECLARE_DYNCREATE(OCXContainerPage)
	DECLARE_EVENTSINK_MAP()

// Construction
public:
	OCXContainerPage();
	~OCXContainerPage();

// Dialog Data
	//{{AFX_DATA(OCXContainerPage)
	enum { IDD = IDD_OVL_OCX_CONTAINER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OCXContainerPage)
	public:
	virtual BOOL DestroyWindow();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OCXContainerPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnModified();
   afx_msg BOOL ApplyNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   virtual CString get_prog_ID() { return ""; }
   virtual CString get_preference_string() { return ""; }
   virtual void set_preference_string(CString pref) { }

   virtual BOOL HandleOnModifiedEvent();

protected:
	COCXControlWrapper m_control_wrapper;
};

// CPropertyPageEventObserver - event sink that implements various event notification interfaces
class ATL_NO_VTABLE CPropertyPageEventObserver :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CDisplayChangeNotifyEventsImpl,
   public CMapChangeNotifyEventsImpl,
   public FalconViewOverlayLib::IPropertyPageNotifyEvents
{
   OverlayOptionsDlg* m_pOverlayOptionsDlg;

public:
   CPropertyPageEventObserver() : m_pOverlayOptionsDlg(NULL)
   {
   }

   void Initialize(OverlayOptionsDlg* pOverlayOptionsDlg)
   {
      m_pOverlayOptionsDlg = pOverlayOptionsDlg;
   }

BEGIN_COM_MAP(CPropertyPageEventObserver)
	COM_INTERFACE_ENTRY(FalconViewOverlayLib::IDisplayChangeNotifyEvents)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IMapChangeNotifyEvents)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IPropertyPageNotifyEvents)
END_COM_MAP()


// override of InvalidateOverlay from CMapChangeNotifyEventsImpl
public:
   STDMETHOD(raw_InvalidateOverlay)();

   // IPropertyPageNotifyEvents
public:
   STDMETHOD(raw_OnPropertyPageModified)();
   STDMETHOD(raw_OnPropertyPageHelp)();
};

// IFvOverlayPropertyPage - 
interface IFvOverlayPropertyPage
{
   virtual HRESULT OnCreate(long hWndParent, CComObject<CPropertyPageEventObserver> *pEventObserver) = 0;
   virtual HRESULT OnApply() = 0;

   virtual ~IFvOverlayPropertyPage() { }
};

// CFvOverlayPropertySheet - 
class CFvOverlayPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFvOverlayPropertySheet)

   CComObject<CPropertyPageEventObserver> *m_pEventObserver;

// Construction
public:
	CFvOverlayPropertySheet();

   void AddPropertyPageEventObserver(CComObject<CPropertyPageEventObserver> *pEventObserver)
   {
      m_pEventObserver = pEventObserver;
   }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFvOverlayPropertySheet)
	public:
	virtual BOOL OnInitDialog();
   virtual BOOL OnApply();
   virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CFvOverlayPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// CFvOverlayPropertyPageImpl - implementation of the IFvOverlayPropertyPage interface. 
class CFvOverlayPropertyPageImpl : public IFvOverlayPropertyPage
{
   CFvOverlayPropertySheet m_propertySheet;
   CPropertyPage *m_pPropertyPage;

public:
   CFvOverlayPropertyPageImpl() : m_pPropertyPage(NULL)
   {
   }

   virtual ~CFvOverlayPropertyPageImpl()
   {
      delete m_pPropertyPage;
   }

// IFvOverlayPropertyPage implementation
   virtual HRESULT OnCreate(long hWndParent, CComObject<CPropertyPageEventObserver> *pEventObserver);
   virtual HRESULT OnApply();

protected:
   virtual CPropertyPage *CreatePropertyPage() = 0;
};

// CPropertyPageEntry -
class CPropertyPageEntry
{
   CString m_displayName;
   CRect m_placementRect;
   CWnd *m_pParentWnd;
   IFvOverlayPropertyPage *m_pOverlayPropPage;
   CComObject<CPropertyPageEventObserver> *m_pEventObserver;
   CIconImage *m_pIconImage;
   CString m_helpFileName;
   CString m_helpURI;
   long m_helpId;
   GUID m_propertyPageUid;

   std::vector<GUID> m_overlayAssocs;

   CPropertyPageEntry *m_pParentEntry;
   std::vector<CPropertyPageEntry *> m_childEntries;

   // current index into m_childEntries array.  Used by GetFirstChild and GetNextChild
   size_t m_nCrntChildIndex;

   CScrollWnd m_containerWnd;

public:
   CPropertyPageEntry(const CString& displayName, CRect placementRect, CWnd *pParentWnd,
                      IFvOverlayPropertyPage *pOverlayPropPage, CComObject<CPropertyPageEventObserver> *pEventObserver, CIconImage *pIconImage,
                      const CString& helpFileName, long helpId, const CString &helpUri, GUID propertyPageUid = GUID_NULL) : 
      m_displayName(displayName),
      m_placementRect(placementRect),
      m_pParentWnd(pParentWnd),
      m_pOverlayPropPage(pOverlayPropPage),
      m_pEventObserver(pEventObserver),
      m_pIconImage(pIconImage),
      m_helpFileName(helpFileName),
      m_helpId(helpId),
	  m_helpURI(helpUri),
      m_pParentEntry(NULL),
      m_propertyPageUid(propertyPageUid)
   {
   }

   virtual ~CPropertyPageEntry();

   void AddOverlayAssoc(GUID overlayDescGuid)
   {
      m_overlayAssocs.push_back(overlayDescGuid);
   }

   void InvalidateAssocs();

   // returns TRUE if all the overlay type's associated with this property page entry are enabled
   BOOL AllOverlayAssocsEnabled();

   // returns TRUE if the given overlay type is associated with this property page
   BOOL IsAssociatedWith(GUID overlayDescGuid);

   void AddChildEntry(CPropertyPageEntry *pChildEntry)
   {
      pChildEntry->SetParentEntry(this);
      m_childEntries.push_back(pChildEntry);
   }

   CPropertyPageEntry *GetParentEntry() { return m_pParentEntry; }

   CPropertyPageEntry *GetFirstChild();
   CPropertyPageEntry *GetNextChild();

   // used for sorting CPropertyPageEntrys
   static bool SortByDisplayName(CPropertyPageEntry *a, CPropertyPageEntry *b)
   { 
      return a->m_displayName.CompareNoCase(b->m_displayName) < 0; 
   }

   void CreatePropertyPage();
   void ShowPropertyPage();
   void HidePropertyPage();
   void OnApply();
   virtual void OnHelp();

   CString GetDisplayName() { return m_displayName; }
   CIconImage *GetIconImage() { return m_pIconImage; }

   GUID GetPropertyPageUid() { return m_propertyPageUid; }

	void MoveWindow(CRect rectPosition);

protected:
   void SetParentEntry(CPropertyPageEntry *pParentEntry) { m_pParentEntry = pParentEntry;  }
};

class CLayerOvlPropertyPageEntry : public CPropertyPageEntry
{
   CLayerOvlFactory *m_pLayerOvlFactory;

public:
   CLayerOvlPropertyPageEntry(CLayerOvlFactory *pLayerOvlFactory, const CString& displayName, CRect placementRect, CWnd *pParentWnd,
                      IFvOverlayPropertyPage *pOverlayPropPage, CComObject<CPropertyPageEventObserver> *pEventObserver, CIconImage *pIconImage,
                      const CString& helpFileName, long helpId, const CString &helpUri) : 
      CPropertyPageEntry(displayName, placementRect, pParentWnd, pOverlayPropPage, pEventObserver, pIconImage,
         helpFileName, helpId, helpUri), m_pLayerOvlFactory(pLayerOvlFactory)
   {
   }

   virtual void OnHelp();
};

/////////////////////////////////////////////////////////////////////////////
// OverlayOptionsDlg dialog

class OverlayOptionsDlg : public CResizableDialog
{
// Construction
public:
	OverlayOptionsDlg(CWnd* pParent = NULL);   // standard constructor
   ~OverlayOptionsDlg();

   //void set_property_page(UINT nIDTemplate);
   void SetInitialPropertyPage(GUID overlayDescGuid, GUID propertyPageUid);

// Dialog Data
	//{{AFX_DATA(OverlayOptionsDlg)
	enum { IDD = IDD_OVERLAY_OPTIONS };
	CTreeCtrl	m_tree_ctrl;
	CString m_title;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OverlayOptionsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OverlayOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnApply();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void InitializePropertyPageEntries();

   CComObject<CPropertyPageEventObserver> *m_pPropertyPageEventObserver;

   std::vector<CPropertyPageEntry *> m_propertyPageEntries;
   std::set<CPropertyPageEntry *> m_dirtyPropertyPageEntries;
   CPropertyPageEntry *m_pCrntSelectedEntry;

   // the initial property page can be set based on an overlay type or 
   // a specific property page uid
   GUID m_initialOverlayDescGuid;
   GUID m_initialPropertyPageUid;
   bool IsInitialPage(CPropertyPageEntry* pEntry);

	// image list for the root level items
	CImageList m_images;

	void on_help();

public:
   void InvalidateAssociatedOverlays();
   virtual HRESULT OnPropertyPageModified();
   virtual HRESULT OnPropertyPageHelp();
};
