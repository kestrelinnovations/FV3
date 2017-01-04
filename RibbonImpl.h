// Copyright (c) 1994-2009, 2012 Georgia Tech Research Corporation, Atlanta, GA
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

// RibbonImpl.h

/// <summary>
/// This header file defines a set of COM objects that wrap the various
/// MFC C++ Ribbon classes (CMFCRibbonBar, CMFCRibbonCategory, CMFCRibbonPanel
///
///   COM Class                  Contained MFC Class
///   ============================================
///   CRibbonImpl                CMFCRibbonBar
///   CRibbonTabImpl             CMFCRibbonCategory
///   CRibbonPanelImpl           CMFCRibbonPanel
///   CRibbonElementImpl         CMFCRibbonBaseElement
///   
/// Buttons, CheckBoxes, ComboBoxes are all derived from CMFCRibbonBaseElement
/// (and CRibbonElementImpl for the COM classes.)
///
/// The MFC classes are arranged in a hierarchy that manages object lifetimes;
/// in other words CMFCRibbonBar containes a collection of CMFCRibbonCategory (the latter
/// represent different ribbon tabs). CMFCRibbonCategory manages a collection of CMFCRibbonPanel, etc.
/// The idea is that, after constructing a ribbon bar full of UI elements, one can just destroy
/// The CMFCRibbonBar; it will in turn, destroy all the tabs it contains, these will destroy the panels
/// which will eventually destroy the buttons, checkboxes, etc.
///
/// The COM clases manage lifetimes from the bottom up.  Each button holds a reference to its
/// containing panel (for example), which holds a reference to the containing tab (category), etc.  When the
/// last UI object in a panel is destroyed, the panel will be destroyed.  When the last panel on
/// a tab (category) is destroyed, the tab will destroy itself, etc.  This means that users of the COM
/// classes need to clean up by releasing references to the UI elements and let the deletions cascade
/// from there.
///
/// </summary>

// forward declarations
class CFvManagedDockablePane;

class CRibbonImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbon3, &FalconViewOverlayLib::IID_IRibbon3, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CMFCRibbonBar *m_pRibbonBar;
public:
   HRESULT CreateRibbonButton( __in LPCTSTR ptszText, __in HICON hIconLarge,
      __in BOOL bAlwaysShowDescription, __in HICON hIconSmall,
      __out PLONG plElementID, __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton );

   CMFCRibbonCategory* FindTab(BSTR TabName);

   CRibbonImpl() : m_pRibbonBar(nullptr)
   {
   }
   VOID FinalRelease()
   {
   }

BEGIN_COM_MAP(CRibbonImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbon)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbon)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbon2)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbon3)
END_COM_MAP()

   void SetRibbonBar(CMFCRibbonBar *pRibbonBar)
   {
      m_pRibbonBar = pRibbonBar;
   }
   CMFCRibbonBar* GetRibbonBar()
   {
      return m_pRibbonBar;
   }

   // IRibbon
public:
   STDMETHOD(raw_RecalcLayout)();
   STDMETHOD(raw_AddTab)(BSTR tabName, FalconViewOverlayLib::IRibbonTab** ppRibbonTab);

   STDMETHOD(raw_AddContextualTab)(BSTR tabName, BSTR contextualTabSetName, long* puiContextID,
      FalconViewOverlayLib::IRibbonTab** ppRibbonTab);
   STDMETHOD(raw_ShowContextualTabs)(long uiContextID, long bShow);

   STDMETHOD(raw_SetActiveTab)(FalconViewOverlayLib::IRibbonTab* pRibbonTab);

   STDMETHOD(raw_RemoveTab)(FalconViewOverlayLib::IRibbonTab* pRibbonTab);
   STDMETHOD(raw_RemoveContexualTabs)(long uiContextID);

   STDMETHOD(raw_CreateRibbonButton)(BSTR text, long hIcon, long bAlwaysShowDescription, 
      long hIconSmall, long* elementID, FalconViewOverlayLib::IRibbonButton** ppRibbonButton);

   /// <summary>
   /// This function works out-of-process.  Icon handles don't marshall across process
   /// boundaries.
   /// </summary>
   STDMETHOD(raw_CreateRibbonButton2)( __in BSTR bsText, __in BSTR bsIconFile, __in long bAlwaysShowDescription, 
      __out long* plElementID, __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton );

   STDMETHOD(raw_CreateRibbonCheckBox)( __in BSTR bsText,
      __out long* plElementID, __out FalconViewOverlayLib::IRibbonCheckBox** ppRibbonCheckBox );

   STDMETHOD(raw_CreateRibbonComboBox)(
      __out long* plElementID, __out FalconViewOverlayLib::IRibbonComboBox** ppRibbonComboBox );

   STDMETHOD(raw_CreateSeparator)(long bHorz, FalconViewOverlayLib::IRibbonSeparator** ppSeparator);

   /// <summary>
   /// IRibbon2 is derived from IRibbon (which has already been released to the world).  IRibbon2
   /// supports button groups and radio button groups.
   /// </summary>
   STDMETHOD(raw_CreateRibbonButtonsGroup)(FalconViewOverlayLib::IRibbonButtonsGroup** ppRibbonButtonsGroup);
   STDMETHOD(raw_CreateRibbonRadioButtonsGroup)(FalconViewOverlayLib::IRibbonRadioButtonsGroup** ppRibbonRadioButtonsGroup);
   STDMETHOD(raw_AddPanelToStaticTab)(BSTR PanelName, BSTR TabName, FalconViewOverlayLib::IRibbonPanel** ppRibbonPanel);
   STDMETHOD(raw_SetActiveStaticTab)(BSTR TabName);
   STDMETHOD(raw_RemovePanelFromStaticTab)(BSTR PanelName, BSTR TabName);
   STDMETHOD(raw_AddTab2)(BSTR TabName, FalconViewOverlayLib::IRibbonTab2** ppRibbonTab2);
   STDMETHOD(raw_CreateRibbonRadioButtonsGroup2)(FalconViewOverlayLib::IRibbonRadioButtonsGroup2** ppRibbonRadioButtonsGroup);
   STDMETHOD(raw_GetTabNames)(SAFEARRAY** tabNames);
   STDMETHOD(raw_CreateRibbonButton3)( __in BSTR bsText, __in BSTR bsIconFile, __in BSTR bsTooltip,
      __in long bAlwaysShowDescription, __out long* plElementID,
      __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton );
   STDMETHOD(raw_AddRibbonBarElementToStaticPanel)(BSTR PanelName, BSTR TabName, FalconViewOverlayLib::IRibbonElement* pRibbonElement, BSTR InsertAfter);
   STDMETHOD(raw_RemoveRibbonBarElementFromStaticPanel)(BSTR PanelName, BSTR TabName, FalconViewOverlayLib::IRibbonElement* pRibbonElement);

   // IRibbon3
   STDMETHOD(raw_GetPanelNames)(BSTR TabName, SAFEARRAY** PanelNames);
   STDMETHOD(raw_GetElementNames)(BSTR TabName, BSTR PanelName, SAFEARRAY** ElementNames);

   static HRESULT GetNextRibbonElementId(UINT* nextId);
};


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonTabImpl
//
////////////////////////////////////////////////////////////////////////////////

class CRibbonTabImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonTab2, &FalconViewOverlayLib::IID_IRibbonTab2, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CMFCRibbonCategory *m_pRibbonCategory;
   CRibbonImpl* m_pRibbon; // Parent ribbon

public:
   CRibbonTabImpl() : m_pRibbonCategory(nullptr)
   {
   }
   void FinalRelease();

BEGIN_COM_MAP(CRibbonTabImpl)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonTab)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonTab2)
END_COM_MAP()

   void SetCategory(CMFCRibbonCategory *pRibbonCategory)
   {
      m_pRibbonCategory = pRibbonCategory;
   }
   CMFCRibbonCategory* GetCategory()
   {
      return m_pRibbonCategory;
   }
   void SetRibbon( CRibbonImpl* pRibbon )
   {
      m_pRibbon = pRibbon;
   }
   CRibbonImpl* GetRibbon()
   {
       return m_pRibbon;
   }

   // IRibbonTab
public:  
   STDMETHOD(put_TabName)(BSTR tabName);
   STDMETHOD(get_TabName)(BSTR* tabName);
   STDMETHOD(raw_AddPanel)(BSTR panelName, FalconViewOverlayLib::IRibbonPanel** ppRibbonPanel);
   STDMETHOD(raw_RemovePanel)(FalconViewOverlayLib::IRibbonPanel* pRibbonPanel);

   // IRibbonTab2
   STDMETHOD(raw_AddPanel2)(BSTR panelName, FalconViewOverlayLib::IRibbonPanel2** ppRibbonPanel);
   STDMETHOD(raw_GetPanelNames)(SAFEARRAY** panelNames);
};


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonPanelImpl
//
////////////////////////////////////////////////////////////////////////////////

class CRibbonPanelImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonPanel2, &FalconViewOverlayLib::IID_IRibbonPanel2,
            &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CMFCRibbonPanel *m_pRibbonPanel;
   CRibbonTabImpl* m_pRibbonTab;

public:
   CRibbonPanelImpl() : m_pRibbonPanel(nullptr){}

BEGIN_COM_MAP(CRibbonPanelImpl)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonPanel)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonPanel2)
END_COM_MAP()

   VOID FinalRelease();
   void SetPanel(CMFCRibbonPanel *pRibbonPanel)
   {
      m_pRibbonPanel = pRibbonPanel;
   }
   CMFCRibbonPanel* GetPanel()
   {
      return m_pRibbonPanel;
   }
   void SetRibbonTab( CRibbonTabImpl* pRibbonTab )
   {
      m_pRibbonTab = pRibbonTab;
   }
   CRibbonTabImpl* GetRibbonTab()
   {
      return m_pRibbonTab;
   }

public:  
   // IRibbonPanel
   STDMETHOD(raw_AddRibbonBarElement)(FalconViewOverlayLib::IRibbonElement* pRibbonElement);
   STDMETHOD(raw_RemoveRibbonBarElement)(FalconViewOverlayLib::IRibbonElement* pRibbonElement);
   
   // IRibbonPanel2
   STDMETHOD(raw_AddRibbonBarElement2)(FalconViewOverlayLib::IRibbonElement* pRibbonElement, BSTR InsertAfter);
};


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonElementImpl
//
////////////////////////////////////////////////////////////////////////////////
#define GETPUT_ELEMENT_PROP( type, prop ) \
   STDMETHOD(put_##prop)( type prop ){ return CRibbonElementImpl::put_##prop( prop ); } \
   STDMETHOD(get_##prop)( type* p##prop){ return CRibbonElementImpl::get_##prop( p##prop ); }
#define GETPUT_ELEMENT_L_PROP( prop ) GETPUT_ELEMENT_PROP( long, prop )
#define GETPUT_ELEMENT_BS_PROP( prop ) GETPUT_ELEMENT_PROP( BSTR, prop )
#define GETPUT_ELEMENT_COMMON_PROPS()  \
   GETPUT_ELEMENT_BS_PROP( Text )      \
   GETPUT_ELEMENT_L_PROP( Visible )    \
   GETPUT_ELEMENT_L_PROP( Checked )    \
   GETPUT_ELEMENT_L_PROP( Enabled )

class CRibbonElementImpl :
   public IDispatchImpl<FalconViewOverlayLib::IRibbonElement,
                     &FalconViewOverlayLib::IID_IRibbonElement,
                     &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
protected:
   CMFCRibbonBaseElement* m_pRibbonBaseElement;
   UINT m_uiID;
   CRibbonPanelImpl* m_pRibbonPanel;
   BOOL m_enabled;
   long m_checked;      // 0 = Unchecked, 1 = Checked, 2 = Indeterminate

   VOID ElementFinalRelease();

public:

   CRibbonElementImpl() : m_pRibbonBaseElement(nullptr), m_pRibbonPanel(nullptr),
      m_enabled(TRUE), m_checked(BST_UNCHECKED), m_uiID( 0 )
   {
   }
   void SetRibbonBaseElement(CMFCRibbonBaseElement* pRibbonBaseElement)
   {
      m_pRibbonBaseElement = pRibbonBaseElement;
      if (m_pRibbonBaseElement)
         m_pRibbonBaseElement->SetData(reinterpret_cast<DWORD_PTR>(this));
   }
   CMFCRibbonBaseElement* GetRibbonBaseElement()
   {
      return m_pRibbonBaseElement;
   }
   VOID SetRibbonPanel( CRibbonPanelImpl* pRibbonPanel )
   {
      m_pRibbonPanel = pRibbonPanel;
   }
   CRibbonPanelImpl* GetRibbonPanel()
   {
      return m_pRibbonPanel;
   }
   void SetID( UINT uiID )
   {
      m_uiID = uiID;
   }
   UINT GetID(void)
   {
      return m_uiID;
   }

   virtual void OnClicked(long nID) { }      // Ignore if not implemented in derived class
   virtual BOOL GetEnabled() { return m_enabled; }
   virtual long GetChecked() { return m_checked; }

   // IRibbonElement
   STDMETHOD(put_Text)(BSTR text)
   {
      if (m_pRibbonBaseElement)
         m_pRibbonBaseElement->SetText(_bstr_t(text));
      return S_OK;
   }
   STDMETHOD(get_Text)(BSTR* text)
   {
      if (m_pRibbonBaseElement)
         *text = _bstr_t(m_pRibbonBaseElement->GetText()).Detach();
      else
         *text = ::SysAllocString(L"");

      return S_OK;
   }
   STDMETHOD(put_Visible)(long lVisible)
   {
      if (m_pRibbonBaseElement)
         m_pRibbonBaseElement->SetVisible(lVisible);

      return S_OK;

   }
   STDMETHOD(get_Visible)(long* pVisible)
   {
      *pVisible = 0;
      if (m_pRibbonBaseElement)
         *pVisible = m_pRibbonBaseElement->IsVisible();

      return S_OK;
   }
   STDMETHOD(put_Enabled)(long lEnabled)
   {
      m_enabled = lEnabled;
      return S_OK;

   }
   STDMETHOD(get_Enabled)(long* pEnabled)
   {
      *pEnabled = TRUE;
      if (m_pRibbonBaseElement)
         *pEnabled = !m_pRibbonBaseElement->IsDisabled();

      return S_OK;
   }
   STDMETHOD(put_Checked)(long lChecked)
   {
      // This check is due to the original IDL documentation that stated lChecked was a bool.
      // NOTE:  We don't handle other BST states for now.
      if ((lChecked < BST_UNCHECKED) || (lChecked > BST_INDETERMINATE))
         m_checked = BST_CHECKED;
      else
         m_checked = lChecked;
      return S_OK;
   }
   STDMETHOD(get_Checked)(long* pChecked)
   {
      *pChecked = FALSE;
      if (m_pRibbonBaseElement)
         *pChecked = m_pRibbonBaseElement->IsChecked();

      return S_OK;
   }

}; // class CRibbonElementImpl


////////////////////////////////////////////////////////////////////////////////
//
// CFvManagedDockablePaneButtonImpl
//
////////////////////////////////////////////////////////////////////////////////
class CFvManagedDockablePaneButtonImpl : public CRibbonElementImpl
{
public:
   virtual void OnClicked(long nID);
   virtual long GetChecked();

   STDMETHODIMP_(ULONG)   AddRef(void);
   STDMETHODIMP_(ULONG)   Release(void);
   STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
   CFvManagedDockablePane* GetDockablePane();
   void SetDockablePane(CFvManagedDockablePane* pane);
   CMFCRibbonPanel* GetParentPanel();
   void SetParentPanel(CMFCRibbonPanel* parentPanel);
   CMFCRibbonBar* GetRibbonBar();
   void SetRibbonBar(CMFCRibbonBar* ribbonBar);

   BOOL OnClose();

private:
   int   m_cRef;
   CFvManagedDockablePane* m_pane;
   CMFCRibbonPanel* m_parentPanel;
   CMFCRibbonBar* m_ribbonBar;

}; // class CFvManagedDockablePaneButtonImpl

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonButton
//
////////////////////////////////////////////////////////////////////////////////

template<class T>
class CProxyIRibbonButtonEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::IID_IRibbonButtonEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnButtonPressed(long nID)
   {
      T* pT = static_cast<T*>(this);
      int nConnections = m_vec.GetSize();
      HRESULT hr = S_OK;

      for ( int iConnectionIndex = 0; iConnectionIndex < nConnections; iConnectionIndex++ )
      {
         pT->Lock();
         CComPtr<IUnknown> spu = m_vec.GetAt( iConnectionIndex );
         pT->Unlock();

         IDispatch* pd = reinterpret_cast< IDispatch* >( spu.p );
         if ( pd != nullptr )
         {
            // Direct COM callback
            FalconViewOverlayLib::IRibbonButtonEventsPtr sprbe = pd;
            if ( sprbe != nullptr )
               hr = sprbe->raw_OnButtonPressed( nID );
         }
      }
      return hr;
   }
}; // class CProxyIRibbonButtonEvents


template<class T>
class CProxy_IRibbonButtonEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::DIID__IRibbonButtonEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnButtonPressed(long nID)
   {
      CComVariant varResult;
      T* pT = static_cast<T*>(this);
      int nConnectionIndex;
      CComVariant* pvars = new CComVariant[1];
      int nConnections = m_vec.GetSize();
      
      for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
      {
         pT->Lock();
         CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
         pT->Unlock();
         IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
         if (pDispatch != NULL)
         {
            VariantClear(&varResult);
            pvars[0] = nID;
            DISPPARAMS disp = { pvars, NULL, 1, 0 };
            pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
         }
      }
      delete[] pvars;
      return varResult.scode;
   }

}; // class CProxy_IRibbonButtonEvents


class CRibbonButtonImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IConnectionPointContainerImpl<CRibbonButtonImpl>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonButton2, &FalconViewOverlayLib::IID_IRibbonButton2,
               &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public CProxyIRibbonButtonEvents< CRibbonButtonImpl >,
   public CProxy_IRibbonButtonEvents< CRibbonButtonImpl >
{
   DWORD m_dwPrimaryConnectionPointCookie;
   DWORD m_dwPrimaryDispConnectionPointCookie;

   long m_toggle; // Indicates momentary or stateful button ( 0 => momentary )
   FalconViewOverlayLib::IRibbonRadioButtonsGroup*  m_pRadioGroup; // Pointer to parent radio group;

public:
   bool m_OneSiblingButtonEnabled;
   //FalconViewOverlayLib::IRibbonRadioButtonsGroup2*  m_pRadioGroup2; // Pointer to parent radio group 2;

   CRibbonButtonImpl() :
      m_dwPrimaryConnectionPointCookie( 0 ),
      m_dwPrimaryDispConnectionPointCookie( 0 ),
      m_toggle( 0 ),
      m_pRadioGroup ( nullptr ),
      m_OneSiblingButtonEnabled( false )
      //m_pRadioGroup2 ( nullptr )
   {
   }

   VOID FinalRelease()
   {
      put_EventConnection( nullptr );  // Any primary connection
      ElementFinalRelease();
   }

BEGIN_COM_MAP(CRibbonButtonImpl)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonButton)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonButton)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonButton2)
   COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CRibbonButtonImpl)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::IID_IRibbonButtonEvents)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::DIID__IRibbonButtonEvents)
END_CONNECTION_POINT_MAP()

   // IRibbonButton
   GETPUT_ELEMENT_COMMON_PROPS()
   STDMETHOD(put_Toggle)(long lToggle)
   {
      m_toggle = lToggle;
      return S_OK;
   }

   STDMETHOD(get_Toggle)(long* lToggle)
   {
      *lToggle = m_toggle;
      return S_OK;
   }

   STDMETHOD(put_RadioGroup)(FalconViewOverlayLib::IRibbonRadioButtonsGroup * pRadioGroup)
   {
      m_pRadioGroup = pRadioGroup;
      //m_pRadioGroup2 = static_cast<FalconViewOverlayLib::IRibbonRadioButtonsGroup2*>(pRadioGroup);
      return S_OK;
   }

   STDMETHOD(get_RadioGroup)(FalconViewOverlayLib::IRibbonRadioButtonsGroup ** ppRadioGroup)
   {
      *ppRadioGroup = m_pRadioGroup;
      return S_OK;
   }

   STDMETHOD(put_EventConnection)(__in IUnknown* piu );

   void OnClicked(long nID)
   {
      if (m_OneSiblingButtonEnabled)
      {
         int x = 3;
      }
      else 
      {
         int x = 3;
      }

      if ( m_toggle != 0 ) // If this is a stateful button
      {
         if (!m_OneSiblingButtonEnabled) 
         {
            const long CheckedState = !CRibbonElementImpl::m_checked;   // Toggle
            if ( m_pRadioGroup && CheckedState )
            {
               const int nButtons = m_pRadioGroup->GetButtonCount();
               for ( int iButton = 0; iButton < nButtons; ++iButton )
               {
                  //FalconViewOverlayLib::IRibbonButton2* pButton;
                  //m_pRadioGroup->raw_GetButton(iButton, &pButton);
                  //pButton->put_Checked(0);
                  FalconViewOverlayLib::IRibbonButton2Ptr pButton = m_pRadioGroup->GetButton(iButton);
                  pButton->put_Checked(BST_UNCHECKED);
               }
            }  
            this->put_Checked(CheckedState);
         }
         else if (m_OneSiblingButtonEnabled)// if disabling a button, at least one button must remain on
         {
            const long CheckedState = !CRibbonElementImpl::m_checked;   // Toggle
            if ( m_pRadioGroup && CheckedState) // Either turning on first button or changing the single enabled button to current button.
            {
               const int nButtons = m_pRadioGroup->GetButtonCount();
               for ( int iButton = 0; iButton < nButtons; ++iButton )
               {
                  //FalconViewOverlayLib::IRibbonButton2* pButton;
                  //m_pRadioGroup->raw_GetButton(iButton, &pButton);
                  //pButton->put_Checked(0);
                  FalconViewOverlayLib::IRibbonButton2Ptr pButton = m_pRadioGroup->GetButton(iButton);

                  pButton->put_Checked(BST_UNCHECKED);
               }
               this->put_Checked(CheckedState);
            } 
            else if (m_pRadioGroup && !CheckedState) //trying to disable a button
            {
               // Cannot disable only enabled button. Do nothing here.
            }
         }
      }

      CProxyIRibbonButtonEvents::Fire_OnButtonPressed(nID);
      CProxy_IRibbonButtonEvents::Fire_OnButtonPressed(nID);
   }
}; // class CRibbonButtonImpl


//////////////////////////////////////////////////////////////////////////////////
//
// CRibbonCheckBox
//
////////////////////////////////////////////////////////////////////////////////

template<class T>
class CProxyIRibbonCheckBoxEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::IID_IRibbonCheckBoxEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnCheckBoxClicked(long nID)
   {
      T* pT = static_cast<T*>(this);
      int nConnections = m_vec.GetSize();
      HRESULT hr = S_OK;

      for ( int iConnectionIndex = 0; iConnectionIndex < nConnections; iConnectionIndex++ )
      {
         pT->Lock();
         CComPtr<IUnknown> spu = m_vec.GetAt( iConnectionIndex );
         pT->Unlock();

         IDispatch* pd = reinterpret_cast< IDispatch* >( spu.p );
         if ( pd != nullptr )
         {
            // Direct COM callback
            FalconViewOverlayLib::IRibbonCheckBoxEventsPtr sprbe = pd;
            if ( sprbe != nullptr )
               hr = sprbe->raw_OnCheckBoxClicked( nID );
         }
      }
      return hr;
   }
}; // class CProxyIRibbonCheckBoxEvents


template<class T>
class CProxy_IRibbonCheckBoxEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::DIID__IRibbonCheckBoxEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnCheckBoxClicked(long nID)
   {
      CComVariant varResult;
      T* pT = static_cast<T*>(this);
      int nConnectionIndex;
      CComVariant* pvars = new CComVariant[1];
      int nConnections = m_vec.GetSize();
      
      for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
      {
         pT->Lock();
         CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
         pT->Unlock();
         IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
         if (pDispatch != NULL)
         {
            VariantClear(&varResult);
            pvars[0] = nID;
            DISPPARAMS disp = { pvars, NULL, 1, 0 };
            pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
         }
      }
      delete[] pvars;
      return varResult.scode;
   }

}; // class CProxy_IRibbonCheckBoxEvents


class CRibbonCheckBoxImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IConnectionPointContainerImpl<CRibbonCheckBoxImpl>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonCheckBox, &FalconViewOverlayLib::IID_IRibbonCheckBox,
               &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public CProxyIRibbonCheckBoxEvents< CRibbonCheckBoxImpl >,
   public CProxy_IRibbonCheckBoxEvents< CRibbonCheckBoxImpl >
{
   DWORD m_dwPrimaryConnectionPointCookie;
   DWORD m_dwPrimaryDispConnectionPointCookie;

public:
   CRibbonCheckBoxImpl() :
      m_dwPrimaryConnectionPointCookie( 0 ),
      m_dwPrimaryDispConnectionPointCookie( 0 )
   {
   }
   VOID FinalRelease()
   {
      put_EventConnection( nullptr );  // Any primary connection
      ElementFinalRelease();
   }

BEGIN_COM_MAP(CRibbonCheckBoxImpl)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonCheckBox)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonCheckBox)
   COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CRibbonCheckBoxImpl)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::IID_IRibbonCheckBoxEvents)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::DIID__IRibbonCheckBoxEvents)
END_CONNECTION_POINT_MAP()

   // IRibbonCheckBox
   GETPUT_ELEMENT_COMMON_PROPS()
   STDMETHOD(put_EventConnection)(__in IUnknown* piu );

   void OnClicked(long nID)
   {
      CRibbonElementImpl::m_checked = !CRibbonElementImpl::m_checked;   // Toggle
      CProxyIRibbonCheckBoxEvents::Fire_OnCheckBoxClicked(nID);
      CProxy_IRibbonCheckBoxEvents::Fire_OnCheckBoxClicked(nID);
   }
}; // class CRibbonCheckBoxImpl



//////////////////////////////////////////////////////////////////////////////////
//
// CRibbonComboBox
//
////////////////////////////////////////////////////////////////////////////////

template<class T>
class CProxyIRibbonComboBoxEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::IID_IRibbonComboBoxEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnComboBoxClicked(long nID)
   {
      T* pT = static_cast<T*>(this);
      int nConnections = m_vec.GetSize();
      HRESULT hr = S_OK;

      for ( int iConnectionIndex = 0; iConnectionIndex < nConnections; iConnectionIndex++ )
      {
         pT->Lock();
         CComPtr<IUnknown> spu = m_vec.GetAt( iConnectionIndex );
         pT->Unlock();

         IDispatch* pd = reinterpret_cast< IDispatch* >( spu.p );
         if ( pd != nullptr )
         {
            // Direct COM callback
            FalconViewOverlayLib::IRibbonComboBoxEventsPtr sprbe = pd;
            if ( sprbe != nullptr )
               hr = sprbe->raw_OnComboBoxClicked( nID );
         }
      }
      return hr;
   }
}; // class CProxyIRibbonComboBoxEvents


template<class T>
class CProxy_IRibbonComboBoxEvents :
   public IConnectionPointImpl<T, &FalconViewOverlayLib::DIID__IRibbonComboBoxEvents, CComDynamicUnkArray>
{
public:
   HRESULT Fire_OnComboBoxClicked(long nID)
   {
      CComVariant varResult;
      T* pT = static_cast<T*>(this);
      int nConnectionIndex;
      CComVariant* pvars = new CComVariant[1];
      int nConnections = m_vec.GetSize();
      
      for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
      {
         pT->Lock();
         CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
         pT->Unlock();
         IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
         if (pDispatch != NULL)
         {
            VariantClear(&varResult);
            pvars[0] = nID;
            DISPPARAMS disp = { pvars, NULL, 1, 0 };
            pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
         }
      }
      delete[] pvars;
      return varResult.scode;
   }

}; // class CProxy_IRibbonComboBoxEvents


class CRibbonComboBoxImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IConnectionPointContainerImpl<CRibbonComboBoxImpl>,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonComboBox, &FalconViewOverlayLib::IID_IRibbonComboBox,
               &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public CProxyIRibbonComboBoxEvents< CRibbonComboBoxImpl >,
   public CProxy_IRibbonComboBoxEvents< CRibbonComboBoxImpl >
{
   DWORD m_dwPrimaryConnectionPointCookie;
   DWORD m_dwPrimaryDispConnectionPointCookie;

public:
   CRibbonComboBoxImpl() :
      m_dwPrimaryConnectionPointCookie( 0 ),
      m_dwPrimaryDispConnectionPointCookie( 0 )
   {
   }
   VOID FinalRelease()
   {
      put_EventConnection( nullptr );  // Any primary connection
      ElementFinalRelease();
   }

BEGIN_COM_MAP(CRibbonComboBoxImpl)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonComboBox)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonComboBox)
   COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CRibbonComboBoxImpl)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::IID_IRibbonComboBoxEvents)
   CONNECTION_POINT_ENTRY(FalconViewOverlayLib::DIID__IRibbonComboBoxEvents)
END_CONNECTION_POINT_MAP()

   // IRibbonComboBox
   GETPUT_ELEMENT_COMMON_PROPS()
   STDMETHOD(put_EventConnection)(__in IUnknown* piu );
   STDMETHOD(raw_AddItem)( BSTR ItemText, DWORD ItemData, long* Index);
   STDMETHOD(raw_RemoveItem)( BSTR ItemText );
   STDMETHOD(raw_GetCurSel)( BSTR* ItemText, unsigned long* ItemData, long* Index );
   STDMETHOD(raw_SelectItem)( BSTR ItemText );

   void OnClicked(long nID)
   {
      CProxyIRibbonComboBoxEvents::Fire_OnComboBoxClicked(nID);
      CProxy_IRibbonComboBoxEvents::Fire_OnComboBoxClicked(nID);
   }
}; // class CRibbonComboBoxImpl


////////////////////////////////////////////////////////////////////////////////
//
// Ribbon Separator
//
////////////////////////////////////////////////////////////////////////////////

class CRibbonSeparatorImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonSeparator, &FalconViewOverlayLib::IID_IRibbonSeparator, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   CRibbonSeparatorImpl()
   {
   }

BEGIN_COM_MAP(CRibbonSeparatorImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonSeparator)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonSeparator)
END_COM_MAP()

   // IRibbonSeparator
public:
   GETPUT_ELEMENT_COMMON_PROPS()
   STDMETHOD(put_EventConnection)(__in IUnknown* piu ){ return S_OK; }

}; // class CRibbonSeparatorImpl

////////////////////////////////////////////////////////////////////////////////
//
// Ribbon Buttons Group
//
////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 
/// </summary>
class CRibbonButtonsGroupImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonButtonsGroup, &FalconViewOverlayLib::IID_IRibbonButtonsGroup, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   CRibbonButtonsGroupImpl()
   {
   }

BEGIN_COM_MAP(CRibbonButtonsGroupImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonButtonsGroup)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonButtonsGroup)
END_COM_MAP()

   virtual VOID FinalRelease();

   // IRibbonButtonsGroup
public:
   GETPUT_ELEMENT_COMMON_PROPS()
   // This ribbon element will not generate events
   STDMETHOD(put_EventConnection)(__in IUnknown* piu ){ return S_OK; }
   STDMETHOD(raw_AddButton)(IRibbonElement* pRibbonElement);
   STDMETHOD(raw_RemoveAll)();
}; // class CRibbonButtonsGroupImpl

////////////////////////////////////////////////////////////////////////////////
//
// Ribbon Radio Buttons Group
//
////////////////////////////////////////////////////////////////////////////////

typedef std::vector<FalconViewOverlayLib::IRibbonButton2*> BUTTON_LIST;
typedef BUTTON_LIST::iterator BUTTON_ITERATOR;

class CRibbonRadioButtonsGroupImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CRibbonElementImpl,
   public IDispatchImpl<FalconViewOverlayLib::IRibbonRadioButtonsGroup2, 
            &FalconViewOverlayLib::IID_IRibbonRadioButtonsGroup2, 
            &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   BUTTON_LIST m_aRadioButtons;

public:
   bool m_canDisableAll;

   CRibbonRadioButtonsGroupImpl()
   {
   }

BEGIN_COM_MAP(CRibbonRadioButtonsGroupImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IRibbonRadioButtonsGroup)
   COM_INTERFACE_ENTRY2(FalconViewOverlayLib::IRibbonElement, CRibbonElementImpl)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonRadioButtonsGroup)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRibbonRadioButtonsGroup2)
END_COM_MAP()

   virtual VOID FinalRelease();

   // IRibbonRadioButtonsGroup
public:
   GETPUT_ELEMENT_COMMON_PROPS()
   // This ribbon element will not generate events
   STDMETHOD(put_EventConnection)(__in IUnknown* piu ){ return S_OK; }
   // Only buttons can be added to a radio group
   STDMETHOD(raw_AddButton)(FalconViewOverlayLib::IRibbonButton2* pRibbonButton);
   STDMETHOD(raw_RemoveAll)();
   STDMETHOD(raw_GetButton)( long Index, FalconViewOverlayLib::IRibbonButton2** ppRibbonButton );
   STDMETHOD(raw_GetButtonCount)( long* pCount );

   // IRibbonRadioButtonsGroup2
   STDMETHOD(raw_AddButton2)(FalconViewOverlayLib::IRibbonButton2* pRibbonButton);
}; // class CRibbonRadioButtonsGroupImpl

//
// CFvRibbonBar command target
//
class CFvRibbonBar : public CCmdTarget
{
   DECLARE_DYNCREATE(CFvRibbonBar)
   DECLARE_OLECREATE(CFvRibbonBar)

   CFvRibbonBar();    // protected constructor used by dynamic creation

public:
   virtual void OnFinalRelease();

protected:
   virtual ~CFvRibbonBar(){}

   DECLARE_MESSAGE_MAP()
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()

   IDispatch* GetMainRibbon(void);

   enum 
   {
      dispidGetMainRibbon = 1L
   };
};

// End of RibbonImpl.h
