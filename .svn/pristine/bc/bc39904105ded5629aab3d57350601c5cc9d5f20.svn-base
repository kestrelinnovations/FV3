// Copyright (c) 1994-2009,2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// RibbonImpl.cpp

#include "stdafx.h"
#include "resource.h"
#include "RibbonImpl.h"
#include "FvDockablePane.h"
#include "err.h"
#include "overlay.h"
#include "getobjpr.h"
#include <set>
#include "ComErrorHandler.h"

using namespace FalconViewOverlayLib;

static UINT next_ribbon_element_id = ID_FIRST_RIBBON_ELEMENT;
static std::set< UINT > sUnusedElementIDs;


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonImpl
//
////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CRibbonImpl::raw_RecalcLayout()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      m_pRibbonBar->ForceRecalcLayout();    
      return S_OK;
   }
   CATCH_BLOCK_RET
}


STDMETHODIMP CRibbonImpl::raw_AddTab(BSTR tabName, IRibbonTab** ppRibbonTab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CMFCRibbonCategory* pCategory = m_pRibbonBar->AddCategory(_bstr_t(tabName), 0, 0 );

      CComObject<CRibbonTabImpl>* pRibbonTab;
      CComObject<CRibbonTabImpl>::CreateInstance(&pRibbonTab);
   
#ifdef REF_CNT_TRACE
      IRibbonPtr pt( this );
      ASSERT( pt != nullptr );
      pt->AddRef();
      long c = pt->Release();
      TRACE( _T("In AddTab(): original ribbon ref cnt = %d\n"), c );
#endif

      pRibbonTab->SetRibbon( this );
      IRibbonPtr( this )->AddRef();    // Must be released by tab

      pRibbonTab->SetCategory(pCategory);

      pRibbonTab->AddRef();
      *ppRibbonTab = pRibbonTab;

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_AddTab2(BSTR tabName, IRibbonTab2** ppRibbonTab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CMFCRibbonCategory* pCategory = m_pRibbonBar->AddCategory(_bstr_t(tabName), 0, 0 );

      CComObject<CRibbonTabImpl>* pRibbonTab;
      CComObject<CRibbonTabImpl>::CreateInstance(&pRibbonTab);
   
#ifdef REF_CNT_TRACE
      IRibbonPtr pt( this );
      ASSERT( pt != nullptr );
      pt->AddRef();
      long c = pt->Release();
      TRACE( _T("In AddTab(): original ribbon ref cnt = %d\n"), c );
#endif

      pRibbonTab->SetRibbon( this );
      IRibbonPtr( this )->AddRef();    // Must be released by tab

      pRibbonTab->SetCategory(pCategory);

      pRibbonTab->AddRef();
      *ppRibbonTab = pRibbonTab;

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_GetTabNames(SAFEARRAY** tabNames)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Parameter validation
      {
         if (tabNames == NULL)
            return E_POINTER;
         else
            *tabNames = NULL;
      }

      // Setup the SAFEARRAY to be returned
      const long num_dimensions = 1;
      SAFEARRAYBOUND array_bounds[num_dimensions];

      array_bounds[0].lLbound = 0;
      array_bounds[0].cElements = (ULONG) m_pRibbonBar->GetCategoryCount();

      *tabNames = SafeArrayCreate(VT_BSTR, num_dimensions, array_bounds);

      // Add each path to the SAFEARRAY
      long index = 0;
      for (ULONG i = 0; i < array_bounds[0].cElements; i++)
      {
         _bstr_t tabName = m_pRibbonBar->GetCategory(i)->GetName();
         ::SafeArrayPutElement(*tabNames, &index, tabName.Detach());
         index++;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_AddContextualTab(BSTR tabName, BSTR contextualTabSetName, 
   long* puiContextID, IRibbonTab** ppRibbonTab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      static long next_uiContextID = 8000;

      *puiContextID = 0;

      // contextual tabs with the same names will have the same context IDs
      //
      const int num_categories = m_pRibbonBar->GetCategoryCount();
      for (int i=0; i<num_categories; ++i)
      {
         CMFCRibbonCategory* p = m_pRibbonBar->GetCategory(i);
         const UINT context_id = p->GetContextID();
         if (context_id)    // non-zero context ID
         {
            CString tabSetName;
            m_pRibbonBar->GetContextName(context_id, tabSetName);
            if (tabSetName == (char *)_bstr_t(contextualTabSetName))
            {
               *puiContextID = context_id;
            }
         }
      }
      if (!*puiContextID)
         *puiContextID = next_uiContextID++;

      CMFCRibbonCategory* pCategory = m_pRibbonBar->AddContextCategory(
         _bstr_t(tabName), _bstr_t(contextualTabSetName), *puiContextID, 
         AFX_CategoryColor_Yellow, 0, 0);

      CComObject<CRibbonTabImpl>* pRibbonTab;
      CComObject<CRibbonTabImpl>::CreateInstance(&pRibbonTab);
      pRibbonTab->AddRef();
      pRibbonTab->SetCategory(pCategory);

      *ppRibbonTab = pRibbonTab;
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_ShowContextualTabs(long uiContextID, long bShow)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   TRY_BLOCK
   {
      m_pRibbonBar->ShowContextCategories(uiContextID, bShow);
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_SetActiveTab(IRibbonTab* pRibbonTab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonTabImpl* p = static_cast<CRibbonTabImpl *>(pRibbonTab);
      CMFCRibbonCategory* pCategory = p->GetCategory();

      // The category name must be non empty
      if (pCategory && pCategory->IsVisible() && strlen(pCategory->GetName()) > 0)
         m_pRibbonBar->SetActiveCategory(pCategory);
      else
      {
         CString msg;
         msg.Format("Tab will not be changed to active: category = 0x%X (must be non-NULL), "
            "visible: %d (must be non-zero), name: %s (must be non-empty)", pCategory,
            pCategory ? 0 : pCategory->IsVisible(),
            pCategory ? "" : pCategory->GetName());
         INFO_report(msg);
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_RemoveTab(IRibbonTab* pRibbonTab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonTabImpl* p = static_cast<CRibbonTabImpl *>(pRibbonTab);

      int cat_idx = m_pRibbonBar->GetCategoryIndex(p->GetCategory());
      if ( cat_idx < 0 )
         return S_FALSE;

      m_pRibbonBar->RemoveCategory(cat_idx);
      p->SetCategory( nullptr );

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_RemoveContexualTabs(long uiContextID)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      const int num_categories = m_pRibbonBar->GetCategoryCount();
      for (int i=num_categories - 1; i>=0; ++i)
      {
         CMFCRibbonCategory* pCategory = m_pRibbonBar->GetCategory(i);
         if (uiContextID && pCategory->GetContextID() == uiContextID)
         {
            m_pRibbonBar->RemoveCategory(i);
         }
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonButton(BSTR text, long hIcon, long bAlwaysShowDescription, 
   long hIconSmall, long* elementID, IRibbonButton** ppRibbonButton)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      return CreateRibbonButton( (LPCTSTR) _bstr_t( text ),
         reinterpret_cast< HICON >( hIcon ), bAlwaysShowDescription,
         reinterpret_cast< HICON >( hIconSmall ), elementID, ppRibbonButton );
   }
   CATCH_BLOCK_RET
}


STDMETHODIMP CRibbonImpl::raw_CreateRibbonButton2(
         __in BSTR bsText, __in BSTR bsIconFile, __in long bAlwaysShowDescription, 
         __out PLONG plElementID, __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CIconImage* pII = CIconImage::load_images( (LPCTSTR) _bstr_t( bsIconFile ) );

      _bstr_t bstrText( bsText );
      if ( bsText == nullptr )
         bstrText = pII->get_item_title();

      return CreateRibbonButton( (LPCTSTR) bstrText, CopyIcon( pII->get_icon( 32 ) ),
         bAlwaysShowDescription, CopyIcon( pII->get_icon( 16 ) ), plElementID, ppRibbonButton );
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonButton3(
         __in BSTR bsText, __in BSTR bsIconFile, __in BSTR bsTooltip,
         __in long bAlwaysShowDescription, __out PLONG plElementID,
         __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CIconImage* pII = CIconImage::load_images( (LPCTSTR) _bstr_t( bsIconFile ) );

      _bstr_t bstrText( bsText );
      if ( bsText == nullptr )
         bstrText = pII->get_item_title();
      _bstr_t bstrTooltip( bsTooltip );
      if ( bsTooltip == nullptr )
         bstrTooltip = pII->get_item_title();

      HRESULT hr = CreateRibbonButton( (LPCTSTR) bstrText, CopyIcon( pII->get_icon( 32 ) ),
         bAlwaysShowDescription, CopyIcon( pII->get_icon( 16 ) ), plElementID, ppRibbonButton );
      if (SUCCEEDED(hr))
      {
         CComObject<CRibbonButtonImpl>* p =
            static_cast<CComObject<CRibbonButtonImpl>*>(*ppRibbonButton);
         CMFCRibbonButton* button = static_cast<CMFCRibbonButton*>(p->GetRibbonBaseElement());
         button->SetToolTipText(bstrTooltip);
         p->SetRibbonBaseElement(button);
         *ppRibbonButton = p;
         return hr;
      }
      else
      {
         return E_FAIL;
      }
   }
   CATCH_BLOCK_RET
}
   
HRESULT CRibbonImpl::CreateRibbonButton(
            __in LPCTSTR ptszText, __in HICON hIconLarge,
            __in BOOL bAlwaysShowDescription, __in HICON hIconSmall,
            __out PLONG plElementID, __out FalconViewOverlayLib::IRibbonButton** ppRibbonButton )
{
   if ( !sUnusedElementIDs.empty() )
   {
      *plElementID = *sUnusedElementIDs.begin();
      sUnusedElementIDs.erase( sUnusedElementIDs.begin() );
   }
   else if ( next_ribbon_element_id >= ID_LAST_RIBBON_ELEMENT )
   {
      AfxMessageBox( _T("Too many ribbon button IDs used") );
      return E_FAIL;
   }
   else
      *plElementID = next_ribbon_element_id++;

   CComObject<CRibbonButtonImpl>* p;
   CComObject<CRibbonButtonImpl>::CreateInstance(&p);
   p->AddRef();

   // Copy icons since the ribbon bar will take ownership
   p->SetRibbonBaseElement( new CMFCRibbonButton( *plElementID, ptszText,
      CopyIcon( hIconLarge ), bAlwaysShowDescription, CopyIcon( hIconSmall ), TRUE ) );
   p->SetID( *plElementID );

   *ppRibbonButton = p;
   return S_OK;
}
  
STDMETHODIMP CRibbonImpl::raw_CreateRibbonCheckBox(
            __in BSTR bsText,
            __out long* plElementID,
            __out FalconViewOverlayLib::IRibbonCheckBox** ppRibbonCheckBox )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if ( !sUnusedElementIDs.empty() )
      {
         *plElementID = *sUnusedElementIDs.begin();
         sUnusedElementIDs.erase( sUnusedElementIDs.begin() );
      }
      else if ( next_ribbon_element_id >= ID_LAST_RIBBON_ELEMENT )
      {
         AfxMessageBox( _T("Too many ribbon button IDs used") );
         return E_FAIL;
      }
      else
         *plElementID = next_ribbon_element_id++;

      CComObject<CRibbonCheckBoxImpl>* p;
      CComObject<CRibbonCheckBoxImpl>::CreateInstance(&p);
      p->AddRef();

      p->SetRibbonBaseElement( new CMFCRibbonCheckBox( *plElementID, (LPCTSTR) _bstr_t( bsText ) ) );
      p->SetID( *plElementID );

      *ppRibbonCheckBox = p;
      return S_OK;
   }
   CATCH_BLOCK_RET
}

HRESULT CRibbonImpl::GetNextRibbonElementId(UINT *nextId)
{
   if ( next_ribbon_element_id >= ID_LAST_RIBBON_ELEMENT )
   {
      AfxMessageBox( _T("Too many ribbon button IDs used") );
      return E_FAIL;
   }
   else
      *nextId = next_ribbon_element_id++;

   return S_OK;
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonComboBox(
            __out long* plElementID,
            __out FalconViewOverlayLib::IRibbonComboBox** ppRibbonComboBox )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if ( !sUnusedElementIDs.empty() )
      {
         *plElementID = *sUnusedElementIDs.begin();
         sUnusedElementIDs.erase( sUnusedElementIDs.begin() );
      }
      else if ( next_ribbon_element_id >= ID_LAST_RIBBON_ELEMENT )
      {
         AfxMessageBox( _T("Too many ribbon button IDs used") );
         return E_FAIL;
      }
      else
         *plElementID = next_ribbon_element_id++;

      CComObject<CRibbonComboBoxImpl>* p;
      CComObject<CRibbonComboBoxImpl>::CreateInstance(&p);
      p->AddRef();

      p->SetRibbonBaseElement( new CMFCRibbonComboBox( *plElementID, TRUE ) );
      p->SetID( *plElementID );

      *ppRibbonComboBox = p;
      return S_OK;
   }
   CATCH_BLOCK_RET
}


STDMETHODIMP CRibbonImpl::raw_CreateSeparator(long bHorz, IRibbonSeparator** ppSeparator)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CComObject<CRibbonSeparatorImpl>* p;
      CComObject<CRibbonSeparatorImpl>::CreateInstance(&p);
      p->AddRef();
      p->SetRibbonBaseElement(new CMFCRibbonSeparator(bHorz));

      *ppSeparator = p;
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonButtonsGroup(IRibbonButtonsGroup** ppRibbonButtonsGroup)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CComObject<CRibbonButtonsGroupImpl>* p;
      CComObject<CRibbonButtonsGroupImpl>::CreateInstance(&p);
      p->AddRef();
      p->SetRibbonBaseElement(new CMFCRibbonButtonsGroup());

      *ppRibbonButtonsGroup = p;
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonRadioButtonsGroup(IRibbonRadioButtonsGroup** ppRibbonRadioButtonsGroup)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CComObject<CRibbonRadioButtonsGroupImpl>* p;
      CComObject<CRibbonRadioButtonsGroupImpl>::CreateInstance(&p);
      p->AddRef();
      p->SetRibbonBaseElement(new CMFCRibbonButtonsGroup());

      p->m_canDisableAll = true;
      *ppRibbonRadioButtonsGroup = p;
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_CreateRibbonRadioButtonsGroup2(IRibbonRadioButtonsGroup2** ppRibbonRadioButtonsGroup)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CComObject<CRibbonRadioButtonsGroupImpl>* p;
      CComObject<CRibbonRadioButtonsGroupImpl>::CreateInstance(&p);
      p->AddRef();
      p->SetRibbonBaseElement(new CMFCRibbonButtonsGroup());

      p->m_canDisableAll = false;
      *ppRibbonRadioButtonsGroup = p;
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_AddPanelToStaticTab(BSTR PanelName, BSTR TabName, IRibbonPanel** ppRibbonPanel)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Find the matching tab.  If not found, this function will return
      // a null panel pointer.
      const int tabCount = m_pRibbonBar->GetCategoryCount();
      CMFCRibbonCategory* pCategory;
      bool found = false;
      for ( int iter = 0; iter < tabCount; ++iter )
      {
         pCategory= m_pRibbonBar->GetCategory(iter);
         _bstr_t tabName = pCategory->GetName();
         if ( tabName == _bstr_t(TabName) )
         {
            found = true;
            break;
         }
      }

      // Found it - add the panel
      if (found)
      {
         CMFCRibbonPanel* pPanel = pCategory->AddPanel(_bstr_t(PanelName));
   
         CComObject<CRibbonPanelImpl>* p;
         CComObject<CRibbonPanelImpl>::CreateInstance(&p);

         p->SetPanel(pPanel);

         // Important - do not connect to COM tab wrapper class - we do NOT want
         // to destroy the tab when the panel is destroyed.
         p->SetRibbonTab( nullptr );

         p->AddRef();
         *ppRibbonPanel = p;   
      }
      else
         *ppRibbonPanel = nullptr;

#ifdef REF_CNT_TRACE
      IRibbonPanelPtr sprb( *ppRibbonPanel );
      sprb->AddRef();
      long c = sprb->Release();
      TRACE( _T("AddPanelToStaticTab, panel ref cnt = %d\n"), c );
#endif

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_SetActiveStaticTab(BSTR TabName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Get the category from the tab name
      CMFCRibbonCategory* pCategory = FindTab(TabName);

      // The category name must be non empty
      if (pCategory && pCategory->IsVisible() && strlen(pCategory->GetName()) > 0)
         m_pRibbonBar->SetActiveCategory(pCategory);
      else
      {
         CString msg;
         msg.Format("Tab will not be changed to active: category = 0x%X (must be non-NULL), "
            "visible: %d (must be non-zero), name: %s (must be non-empty)", pCategory,
            pCategory ? 0 : pCategory->IsVisible(),
            pCategory ? "" : pCategory->GetName());
         INFO_report(msg);
      }
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_RemovePanelFromStaticTab(BSTR PanelName, BSTR TabName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Find the matching tab.
      const int tabCount = m_pRibbonBar->GetCategoryCount();
      int panelCount;
      CMFCRibbonCategory* pCategory;
      CMFCRibbonPanel* pPanel;
      bool foundTab = false;
      bool foundPanel = false;
      for ( int iter = 0; iter < tabCount; ++iter )
      {
         pCategory= m_pRibbonBar->GetCategory(iter);
         _bstr_t tabName = pCategory->GetName();
         if ( tabName == _bstr_t(TabName) )
         {
            foundTab = true;
            panelCount = pCategory->GetPanelCount();
            break;
         }
      }

      // Found it. Attempt to find and remove panel.
      if (foundTab)
      {
         for ( int iter = 0; iter < panelCount; ++iter )
         {
            pPanel = pCategory->GetPanel(iter);
            _bstr_t panelName = pPanel->GetName();
            if ( panelName == _bstr_t(PanelName) )
            {
               foundPanel = true;
               //IRibbonPanelPtr(pPanel);
               pCategory->RemovePanel(iter, 1);
               pPanel = nullptr;
               break;
            }
         }
         if (!foundPanel) {return E_FAIL;} // Panel not found error. Nonsilent failure.
      }
      else 
      {
         return E_FAIL; // Tab not found error. Nonsilent failure.
      }

#ifdef REF_CNT_TRACE
      IRibbonPanelPtr sprb( *ppRibbonPanel );
      sprb->AddRef();
      long c = sprb->Release();
      TRACE( _T("AddPanelToStaticTab, panel ref cnt = %d\n"), c );
#endif

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_AddRibbonBarElementToStaticPanel(BSTR PanelName, BSTR TabName, FalconViewOverlayLib::IRibbonElement* pRibbonElement, BSTR InsertAfter)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
       // Find the matching tab.
       const int tabCount = m_pRibbonBar->GetCategoryCount();
       int panelCount;
       CMFCRibbonCategory* pCategory;
       CMFCRibbonPanel* pPanel;
       bool foundTab = false;
       bool foundPanel = false;
       for ( int iter = 0; iter < tabCount; ++iter )
       {
           pCategory= m_pRibbonBar->GetCategory(iter);
           _bstr_t tabName = pCategory->GetName();
           if ( tabName == _bstr_t(TabName) )
           {
               foundTab = true;
               panelCount = pCategory->GetPanelCount();
               break;
           }
       }

       if (foundTab)
       {
           for ( int iter = 0; iter < panelCount; ++iter )
           {
               pPanel = pCategory->GetPanel(iter);
               _bstr_t panelName = pPanel->GetName();
               if ( panelName == _bstr_t(PanelName) )
               {
                   foundPanel = true;
                
                   // Add element to static panel.
                   CComObject<CRibbonPanelImpl>* p;
                   CComObject<CRibbonPanelImpl>::CreateInstance(&p);
                
                   // We aren't using the tab from the panel, so we just null it out here
                   p->SetRibbonTab(nullptr);
                
                   p->SetPanel(pPanel);
                   IRibbonPanel2Ptr sprb(p);
                   sprb->raw_AddRibbonBarElement2(pRibbonElement, InsertAfter);
                   p->SetPanel(nullptr);
                   p = nullptr;
                   break;
               }
           }
           if (!foundPanel) {return E_FAIL;} // Panel not found error. Nonsilent failure.
       }
       else 
       {
           return E_FAIL; // Tab not found error. Nonsilent failure.
       }

       return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_RemoveRibbonBarElementFromStaticPanel(BSTR PanelName, BSTR TabName, FalconViewOverlayLib::IRibbonElement* pRibbonElement)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
       // Find the matching tab.
       const int tabCount = m_pRibbonBar->GetCategoryCount();
       int panelCount;
       CMFCRibbonCategory* pCategory;
       CMFCRibbonPanel* pPanel;
       bool foundTab = false;
       bool foundPanel = false;
       for ( int iter = 0; iter < tabCount; ++iter )
       {
           pCategory= m_pRibbonBar->GetCategory(iter);
           _bstr_t tabName = pCategory->GetName();
           if ( tabName == _bstr_t(TabName) )
           {
               foundTab = true;
               panelCount = pCategory->GetPanelCount();
               break;
           }
       }

       // Found it. Attempt to find and remove panel.
       if (foundTab)
       {
           for ( int iter = 0; iter < panelCount; ++iter )
           {
               pPanel = pCategory->GetPanel(iter);
               _bstr_t panelName = pPanel->GetName();
               if ( panelName == _bstr_t(PanelName) )
               {
                   foundPanel = true;
                   // Add element to static panel.
                   CComObject<CRibbonPanelImpl>* p;
                   CComObject<CRibbonPanelImpl>::CreateInstance(&p);

                   // We aren't using the tab from the panel, so we just null it out here
                   p->SetRibbonTab(nullptr);

                   p->SetPanel(pPanel);
                   IRibbonPanel2Ptr sprb(p);
                   sprb->RemoveRibbonBarElement(pRibbonElement);
                   p->SetPanel(nullptr);
                   p = nullptr;
                   break;
               }
           }
           if (!foundPanel) {return E_FAIL;} // Panel not found error. Nonsilent failure.
       }
       else 
       {
           return E_FAIL; // Tab not found error. Nonsilent failure.
       }

       return S_OK;
   }
   CATCH_BLOCK_RET
}

CMFCRibbonCategory* CRibbonImpl::FindTab(BSTR TabName)
{
   // Find the matching tab.  If not found, this function will return
   // a null pointer.
   const int tabCount = m_pRibbonBar->GetCategoryCount();
   CMFCRibbonCategory* pCategory;
   bool found = false;
   for ( int iter = 0; iter < tabCount; ++iter )
   {
      pCategory= m_pRibbonBar->GetCategory(iter);
      _bstr_t tabName = pCategory->GetName();
      if ( tabName == _bstr_t(TabName) )
      {
         found = true;
         break;
      }
   }
   if (found)
      return pCategory;
   else
      return nullptr;
}


////////////////////////////////////////////////////////////////////////////////
//
// IRibbon3
//
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CRibbonImpl::raw_GetPanelNames(BSTR TabName, SAFEARRAY** PanelNames)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Parameter validation
      {
         if (PanelNames == NULL)
            return E_POINTER;
         else
            *PanelNames = NULL;
      }

      CMFCRibbonCategory* pCat = FindTab(TabName);

      if (pCat == nullptr)
         return E_FAIL;

      // Setup the SAFEARRAY to be returned
      const long num_dimensions = 1;
      SAFEARRAYBOUND array_bounds[num_dimensions];

      array_bounds[0].lLbound = 0;
      array_bounds[0].cElements = (ULONG) pCat->GetPanelCount();

      *PanelNames = SafeArrayCreate(VT_BSTR, num_dimensions, array_bounds);

      // Add each path to the SAFEARRAY
      long index = 0;
      for (ULONG i = 0; i < array_bounds[0].cElements; i++)
      {
         _bstr_t panelName = pCat->GetPanel(i)->GetName();
         ::SafeArrayPutElement(*PanelNames, &index, panelName.Detach());
         index++;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonImpl::raw_GetElementNames(BSTR TabName, BSTR PanelName, SAFEARRAY** ElementNames)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Parameter validation
      {
         if (ElementNames == NULL)
            return E_POINTER;
         else
            *ElementNames = NULL;
      }

      CMFCRibbonCategory* pCat = FindTab(TabName);

      if (pCat == nullptr)
         return E_FAIL;

      int panelCount = pCat->GetPanelCount();
      CMFCRibbonPanel* pPanel = nullptr;
      bool foundPanel = false;
      for ( int iter = 0; iter < panelCount; ++iter )
      {
         pPanel = pCat->GetPanel(iter);
         _bstr_t panelName = pPanel->GetName();
         if ( panelName == _bstr_t(PanelName) )
         {
            foundPanel = true;
            break;
         }
      }

      if (!foundPanel) {return E_FAIL;} // Panel not found error. Nonsilent failure.

      // Setup the SAFEARRAY to be returned
      const long num_dimensions = 1;
      SAFEARRAYBOUND array_bounds[num_dimensions];

      array_bounds[0].lLbound = 0;
      array_bounds[0].cElements = (ULONG) pPanel->GetCount();

      *ElementNames = SafeArrayCreate(VT_BSTR, num_dimensions, array_bounds);

      // Add each path to the SAFEARRAY
      long index = 0;
      for (ULONG i = 0; i < array_bounds[0].cElements; i++)
      {
         _bstr_t elementName = pPanel->GetElement(i)->GetText();
         ::SafeArrayPutElement(*ElementNames, &index, elementName.Detach());
         index++;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonTabImpl
//
////////////////////////////////////////////////////////////////////////////////

void CRibbonTabImpl::FinalRelease()
{
   if ( m_pRibbonCategory != nullptr )
      m_pRibbon->raw_RemoveTab( this );
   IRibbonPtr( m_pRibbon )->Release(); // Release delete protection

#ifdef REF_CNT_TRACE
   IRibbonPtr sprb( m_pRibbon );
   sprb->AddRef();
   long c = sprb->Release();
   TRACE( _T("Tab final release, ribbon ref cnt = %d\n"), c );
#endif
}

STDMETHODIMP CRibbonTabImpl::put_TabName(BSTR tabName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if (m_pRibbonCategory)
         m_pRibbonCategory->SetName(_bstr_t(tabName));

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonTabImpl::get_TabName(BSTR* tabName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if (m_pRibbonCategory)
         *tabName = _bstr_t(m_pRibbonCategory->GetName()).Detach();
      else
         *tabName = ::SysAllocString(L"");

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonTabImpl::raw_AddPanel(BSTR panelName, IRibbonPanel** ppRibbonPanel)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CMFCRibbonPanel* pPanel = m_pRibbonCategory->AddPanel(_bstr_t(panelName));
   
      CComObject<CRibbonPanelImpl>* p;
      CComObject<CRibbonPanelImpl>::CreateInstance(&p);

      p->SetPanel(pPanel);
      p->SetRibbonTab( this );
      IRibbonTabPtr( this )->AddRef();    // Must be released by panel

#ifdef REF_CNT_TRACE
      IRibbonTabPtr sprt( this );
      sprt->AddRef();
      long c = sprt->Release();
      TRACE( _T("AddPanel(): tab ref cnt = %d\n"), c );
#endif
      p->AddRef();
      *ppRibbonPanel = p;

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonTabImpl::raw_AddPanel2(BSTR panelName, IRibbonPanel2** ppRibbonPanel2)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CMFCRibbonPanel* pPanel = m_pRibbonCategory->AddPanel(_bstr_t(panelName));
   
      CComObject<CRibbonPanelImpl>* p;
      CComObject<CRibbonPanelImpl>::CreateInstance(&p);

      p->SetPanel(pPanel);
      p->SetRibbonTab( this );
      IRibbonTabPtr( this )->AddRef();    // Must be released by panel

#ifdef REF_CNT_TRACE
      IRibbonTabPtr sprt( this );
      sprt->AddRef();
      long c = sprt->Release();
      TRACE( _T("AddPanel(): tab ref cnt = %d\n"), c );
#endif
      p->AddRef();
      *ppRibbonPanel2 = p;

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonTabImpl::raw_GetPanelNames(SAFEARRAY** panelNames)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      // Parameter validation
      {
         if (panelNames == NULL)
            return E_POINTER;
         else
            *panelNames = NULL;
      }

      // Setup the SAFEARRAY to be returned
      const long num_dimensions = 1;
      SAFEARRAYBOUND array_bounds[num_dimensions];

      array_bounds[0].lLbound = 0;
      array_bounds[0].cElements = (ULONG) m_pRibbonCategory->GetPanelCount();

      *panelNames = SafeArrayCreate(VT_BSTR, num_dimensions, array_bounds);

      // Add each path to the SAFEARRAY
      long index = 0;
      for (ULONG i = 0; i < array_bounds[0].cElements; i++)
      {
         _bstr_t panelName = m_pRibbonCategory->GetPanel(i)->GetName();
         ::SafeArrayPutElement(*panelNames, &index, panelName.Detach());
         index++;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonTabImpl::raw_RemovePanel(IRibbonPanel* pRibbonPanel)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonPanelImpl* p = static_cast<CRibbonPanelImpl *>(pRibbonPanel);

      CMFCRibbonPanel* pPanel = p->GetPanel();
      if ( pPanel != nullptr )
      {
         const int panel_idx = pPanel->GetParentCategory()->GetPanelIndex(pPanel);
         if (panel_idx != -1)
            pPanel->GetParentCategory()->RemovePanel(panel_idx);
         p->SetPanel(nullptr);
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonPanelImpl
//
////////////////////////////////////////////////////////////////////////////////

void CRibbonPanelImpl::FinalRelease()
{
   // This code repeats the code to remove the panel from the underlying
   // MFCCategory class without using a RibbonTabImpl wrapper (which
   // doesn't exist in the case of panels added to a static tab)
   if ( m_pRibbonPanel != nullptr )
   {
      const int panel_idx = m_pRibbonPanel->GetParentCategory()->GetPanelIndex(m_pRibbonPanel);
      if (panel_idx != -1)
         m_pRibbonPanel->GetParentCategory()->RemovePanel(panel_idx);
      m_pRibbonPanel = nullptr;
   }

   // If there is a tab wrapper class, release it so it can be deleted when the
   // last panel on it is destroyed.
   if ( m_pRibbonTab != nullptr )
   {
      IRibbonTabPtr( m_pRibbonTab )->Release(); // Release parent delete protection
   }
}

STDMETHODIMP CRibbonPanelImpl::raw_AddRibbonBarElement(IRibbonElement* pRibbonElement)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonElementImpl* p = static_cast<CRibbonElementImpl *>(pRibbonElement);
   
      if (m_pRibbonPanel)
      {
         m_pRibbonPanel->Add(p->GetRibbonBaseElement());

         p->SetRibbonPanel( this );
         IRibbonPanelPtr( this )->AddRef();  // Must be released by element
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonPanelImpl::raw_AddRibbonBarElement2(IRibbonElement* pRibbonElement, BSTR InsertAfter)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      bool foundElement = false;
      CMFCRibbonBaseElement* pElement;
      CMFCRibbonPanel* pPanel = this->GetPanel();
      const int elementCount = pPanel->GetCount();

      _bstr_t bstrInsertAfter(InsertAfter);

      if (bstrInsertAfter == _bstr_t(L"*First"))
      {
         foundElement = true;

         // Add as the first element in the panel.
         CRibbonElementImpl* p = static_cast<CRibbonElementImpl *>(pRibbonElement);
         pPanel->Insert(p->GetRibbonBaseElement(), 0); // Want to insert at the front of the panel.
         p->SetRibbonPanel(this);
         IRibbonPanelPtr( this )->AddRef();  // Must be released by element     
      }
      else
      {
         for (int i=0; i<elementCount; i++) 
         {
            pElement = pPanel->GetElement(i);
            _bstr_t buttonName = pElement->GetText();
            if ( buttonName == bstrInsertAfter )
            {
               foundElement = true;

               CRibbonElementImpl* p = static_cast<CRibbonElementImpl *>(pRibbonElement);
               pPanel->Insert(p->GetRibbonBaseElement(), (i+1)); // Want to insert after current item
               p->SetRibbonPanel(this);
               IRibbonPanelPtr( this )->AddRef();  // Must be released by element
               break;
            }
         }
      }

      if (!foundElement) 
      {
         return raw_AddRibbonBarElement(pRibbonElement);
      }
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonPanelImpl::raw_RemoveRibbonBarElement(IRibbonElement* pRibbonElement)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonElementImpl* p = static_cast<CRibbonElementImpl *>(pRibbonElement);
      UINT iD = 0;
   
      CMFCRibbonBaseElement* pBaseElement = p->GetRibbonBaseElement();
      if ( pBaseElement != nullptr )
      {
         if (m_pRibbonPanel != nullptr)
         {
            int element_idx = m_pRibbonPanel->GetIndex(p->GetRibbonBaseElement());
            if (element_idx != -1)
               m_pRibbonPanel->Remove(element_idx);

            iD = p->GetID();
            p->SetRibbonBaseElement(nullptr);
            if (m_pRibbonPanel->GetCount() <= 0) // Delete panel
            {
               if ( m_pRibbonPanel != nullptr )
               {
                 const int panel_idx = m_pRibbonPanel->GetParentCategory()->GetPanelIndex(m_pRibbonPanel);
                 if (panel_idx != -1)
                   m_pRibbonPanel->GetParentCategory()->RemovePanel(panel_idx);
                 m_pRibbonPanel = nullptr;
               }
            }
         }
      }
      else return S_OK;

      // Recycle the command ID 
      if ( iD != 0 )
      {
         std::pair< std::set< UINT >::iterator, BOOL > p =
            sUnusedElementIDs.insert( iD ); // Recycle the element IDs
         ASSERT( p.second && "Found recyclable command ID already in the reuse list" );
      }

      return S_OK;   
   }
   CATCH_BLOCK_RET
}


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonButtonsGroupImpl
//
////////////////////////////////////////////////////////////////////////////////
VOID CRibbonButtonsGroupImpl::FinalRelease()
{
   raw_RemoveAll();    // Doesn't matter - MFC buttons group code will do this anyway
   ElementFinalRelease();
}

STDMETHODIMP CRibbonButtonsGroupImpl::raw_AddButton(IRibbonElement* pRibbonElement)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonElementImpl* p = static_cast<CRibbonElementImpl *>(pRibbonElement);
      static_cast<CMFCRibbonButtonsGroup*>(GetRibbonBaseElement())->AddButton(p->GetRibbonBaseElement());

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonButtonsGroupImpl::raw_RemoveAll()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      static_cast<CMFCRibbonButtonsGroup*>(GetRibbonBaseElement())->RemoveAll();

      return S_OK;
   }
   CATCH_BLOCK_RET
}

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonRadioButtonsGroupImpl
//
////////////////////////////////////////////////////////////////////////////////

VOID CRibbonRadioButtonsGroupImpl::FinalRelease()
{
   raw_RemoveAll();                 // Doesn't matter - MFC buttons group code will do this anyway
   ElementFinalRelease();
}

STDMETHODIMP CRibbonRadioButtonsGroupImpl::raw_AddButton(FalconViewOverlayLib::IRibbonButton2* pRibbonButton)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonButtonImpl* p = static_cast<CRibbonButtonImpl *>(pRibbonButton);

      static_cast<CMFCRibbonButtonsGroup*>(GetRibbonBaseElement())->AddButton(p->GetRibbonBaseElement());
      IRibbonRadioButtonsGroup* pRadioGroup = static_cast<IRibbonRadioButtonsGroup*>(this);
      p->put_RadioGroup(pRadioGroup);
      pRibbonButton->put_Toggle(TRUE);
      m_aRadioButtons.push_back(pRibbonButton);
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonRadioButtonsGroupImpl::raw_AddButton2(FalconViewOverlayLib::IRibbonButton2* pRibbonButton)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      CRibbonButtonImpl* p = static_cast<CRibbonButtonImpl *>(pRibbonButton);

      static_cast<CMFCRibbonButtonsGroup*>(GetRibbonBaseElement())->AddButton(p->GetRibbonBaseElement());
      IRibbonRadioButtonsGroup* pRadioGroup = static_cast<IRibbonRadioButtonsGroup*>(this);
      p->put_RadioGroup(pRadioGroup);
      pRibbonButton->put_Toggle(TRUE);
      p->m_OneSiblingButtonEnabled = true;
      m_aRadioButtons.push_back(pRibbonButton);
   
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonRadioButtonsGroupImpl::raw_RemoveAll()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      static_cast<CMFCRibbonButtonsGroup*>(GetRibbonBaseElement())->RemoveAll();

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonRadioButtonsGroupImpl::raw_GetButtonCount( long* pCount )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      *pCount = m_aRadioButtons.size();
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonRadioButtonsGroupImpl::raw_GetButton( long Index, FalconViewOverlayLib::IRibbonButton2** ppRibbonButton )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      *ppRibbonButton = m_aRadioButtons[Index];
      (*ppRibbonButton)->AddRef();
      return S_OK;
   }
   CATCH_BLOCK_RET
}

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonButtonImpl
//
////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CRibbonButtonImpl::put_EventConnection( __in IUnknown* piu )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      IConnectionPointPtr spCP, spDCP;

      // Release any existing connection
      HRESULT hr = FindConnectionPoint( IID_IRibbonButtonEvents, &spCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryConnectionPointCookie != 0 )
      {
         hr = spCP->Unadvise( m_dwPrimaryConnectionPointCookie );
         m_dwPrimaryConnectionPointCookie = 0;
      }

      hr = FindConnectionPoint( DIID__IRibbonButtonEvents, &spDCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryDispConnectionPointCookie != 0 )
      {
         hr = spDCP->Unadvise( m_dwPrimaryDispConnectionPointCookie );
         m_dwPrimaryDispConnectionPointCookie = 0;
      }

      // Make the new connection (if any)
      if ( SUCCEEDED( hr ) && piu != nullptr )
      {
         // Try to connect as IRibbonButtonEvents
         hr = spCP->Advise( piu, &m_dwPrimaryConnectionPointCookie );

         // If couldn't connect as IRibbonButtonEvents,
         //   try _IRibbonButtonEvents (dispinterface)
         if ( FAILED( hr ) )
            hr = spDCP->Advise( piu, &m_dwPrimaryDispConnectionPointCookie );
      }
      return hr;
   }
   CATCH_BLOCK_RET
}


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonCheckBoxImpl
//
////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CRibbonCheckBoxImpl::put_EventConnection( __in IUnknown* piu )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      IConnectionPointPtr spCP, spDCP;

      // Release any existing connection
      HRESULT hr = FindConnectionPoint( IID_IRibbonCheckBoxEvents, &spCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryConnectionPointCookie != 0 )
      {
         hr = spCP->Unadvise( m_dwPrimaryConnectionPointCookie );
         m_dwPrimaryConnectionPointCookie = 0;
      }

      hr = FindConnectionPoint( DIID__IRibbonCheckBoxEvents, &spDCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryDispConnectionPointCookie != 0 )
      {
         hr = spDCP->Unadvise( m_dwPrimaryDispConnectionPointCookie );
         m_dwPrimaryDispConnectionPointCookie = 0;
      }

      // Make the new connection (if any)
      if ( SUCCEEDED( hr ) && piu != nullptr )
      {
         // Try to connect as IRibbonCheckBoxEvents
         hr = spCP->Advise( piu, &m_dwPrimaryConnectionPointCookie );

         // If couldn't connect as IRibbonCheckBoxEvents,
         //   try _IRibbonCheckBoxEvents (dispinterface)
         if ( FAILED( hr ) )
            hr = spDCP->Advise( piu, &m_dwPrimaryDispConnectionPointCookie );
      }
      return hr;
   }
   CATCH_BLOCK_RET
}


////////////////////////////////////////////////////////////////////////////////
//
// CRibbonComboBoxImpl
//
////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CRibbonComboBoxImpl::raw_AddItem( BSTR ItemText, DWORD ItemData, long* Index)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if (m_pRibbonBaseElement)    
      {
         int index = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->FindItem((LPCTSTR)_bstr_t(ItemText));

         if (index == -1)
            *Index = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->AddItem((LPCTSTR)_bstr_t(ItemText), ItemData);
         else
            return E_INVALIDARG;
      }
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonComboBoxImpl::raw_RemoveItem( BSTR ItemText )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if ( m_pRibbonBaseElement )
      {
         int index = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->FindItem((LPCTSTR)_bstr_t(ItemText));

         if (index >= 0)
            static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->DeleteItem( (LPCTSTR)_bstr_t(ItemText) );
         else
            return E_INVALIDARG;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonComboBoxImpl::raw_GetCurSel( BSTR* ItemText, unsigned long* ItemData, long* Index )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if ( m_pRibbonBaseElement )
      {
         *Index = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->GetCurSel();
         *ItemText = _bstr_t(static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->GetItem(*Index)).Detach();
         *ItemData = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->GetItemData(*Index);
      }
      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonComboBoxImpl::raw_SelectItem( BSTR ItemText )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      if ( m_pRibbonBaseElement )
      {
         int index = static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->FindItem((LPCTSTR)_bstr_t(ItemText));

         if (index >= 0)
            static_cast<CMFCRibbonComboBox*>(m_pRibbonBaseElement)->SelectItem( index );
         else
            return E_INVALIDARG;
      }

      return S_OK;
   }
   CATCH_BLOCK_RET
}

STDMETHODIMP CRibbonComboBoxImpl::put_EventConnection( __in IUnknown* piu )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   TRY_BLOCK
   {
      IConnectionPointPtr spCP, spDCP;

      // Release any existing connection
      HRESULT hr = FindConnectionPoint( IID_IRibbonComboBoxEvents, &spCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryConnectionPointCookie != 0 )
      {
         hr = spCP->Unadvise( m_dwPrimaryConnectionPointCookie );
         m_dwPrimaryConnectionPointCookie = 0;
      }

      hr = FindConnectionPoint( DIID__IRibbonComboBoxEvents, &spDCP );
      if ( SUCCEEDED( hr )
         && m_dwPrimaryDispConnectionPointCookie != 0 )
      {
         hr = spDCP->Unadvise( m_dwPrimaryDispConnectionPointCookie );
         m_dwPrimaryDispConnectionPointCookie = 0;
      }

      // Make the new connection (if any)
      if ( SUCCEEDED( hr ) && piu != nullptr )
      {
         // Try to connect as IRibbonCheckBoxEvents
         hr = spCP->Advise( piu, &m_dwPrimaryConnectionPointCookie );

         // If couldn't connect as IRibbonComboBoxEvents,
         //   try _IRibbonComboBoxEvents (dispinterface)
         if ( FAILED( hr ) )
            hr = spDCP->Advise( piu, &m_dwPrimaryDispConnectionPointCookie );
      }
      return hr;
   }
   CATCH_BLOCK_RET
}

////////////////////////////////////////////////////////////////////////////////
//
// CRibbonElementImpl
//
////////////////////////////////////////////////////////////////////////////////

VOID CRibbonElementImpl::ElementFinalRelease()
{
   if ( m_pRibbonPanel != nullptr )
   {
      m_pRibbonPanel->raw_RemoveRibbonBarElement( this );
      IRibbonPanelPtr( m_pRibbonPanel )->Release();   // Release parent delete protection
   }
   else if ( m_pRibbonBaseElement != nullptr )
   {
      CMFCRibbonBaseElement::RibbonElementLocation Loc = m_pRibbonBaseElement->GetLocationInGroup();
      if ( Loc == CMFCRibbonBaseElement::RibbonElementNotInGroup )
      {
      }
   }
}  // ElementFinalRelease()


////////////////////////////////////////////////////////////////////////////////
//
// CFvRibbonBar
//
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CFvRibbonBar, CCmdTarget )
IMPLEMENT_OLECREATE( CFvRibbonBar, "FalconView.FvRibbonBar", 
   0x6FCEAFE0, 0xAC8C, 0x4CD5, 0x8F, 0x27, 0x1E, 0x54, 0xC4, 0x76, 0x2A, 0xD8 );

CFvRibbonBar::CFvRibbonBar()
{
   EnableAutomation();
}

void CFvRibbonBar::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CFvRibbonBar, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFvRibbonBar, CCmdTarget)
   DISP_FUNCTION_ID(CFvRibbonBar, "GetMainRibbon", dispidGetMainRibbon, GetMainRibbon, VT_DISPATCH, VTS_NONE)
END_DISPATCH_MAP()

// Note: we add support for IID_IFvRibbonBar to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {7A5382BF-4A3F-462D-81D2-91296272494D}
static const IID IID_IFvRibbonBar =
{ 0x7A5382BF, 0x4A3F, 0x462D, { 0x81, 0xD2, 0x91, 0x29, 0x62, 0x72, 0x49, 0x4D } };

BEGIN_INTERFACE_MAP(CFvRibbonBar, CCmdTarget)
   INTERFACE_PART(CFvRibbonBar, IID_IFvRibbonBar, Dispatch)
END_INTERFACE_MAP()


// CFvRibbonBar message handlers

IDispatch* CFvRibbonBar::GetMainRibbon(void)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   IDispatchPtr spd = fvw_get_frame()->GetRibbon();
   spd->AddRef();
   return spd;
}

// CFvManagedDockablePaneButtonImpl message handler

void CFvManagedDockablePaneButtonImpl::OnClicked(long nID)
{
   // Show the dockable pane
   if (m_pane != nullptr)
   {
      if (m_pane->IsVisible() == TRUE)
         m_pane->ShowPane(FALSE, FALSE, TRUE);
      else
         m_pane->ShowPane(TRUE, FALSE, TRUE);
   }
}

long CFvManagedDockablePaneButtonImpl::GetChecked()
{
   return m_pane->IsVisible();
}

ULONG CFvManagedDockablePaneButtonImpl::AddRef(void)
{
   return ++m_cRef;
}

ULONG CFvManagedDockablePaneButtonImpl::Release(void)
{
   if (m_cRef == 0)
      return 0;

   if (--m_cRef == 0)
   {
      delete this;
      return 0;
   }
   return m_cRef;
}

HRESULT CFvManagedDockablePaneButtonImpl::QueryInterface( REFIID riid, void** ppv )
{
   *ppv = NULL;

   if ( ( riid == IID_IUnknown ) || ( riid == IID_IRibbonElement ) )
      *ppv = this;

   if ( *ppv )
   {
      ( (IUnknown*) *ppv )->AddRef();
      return S_OK;
   }
   return E_NOINTERFACE;
}

CFvManagedDockablePane* CFvManagedDockablePaneButtonImpl::GetDockablePane()
{
   return m_pane;
}

void CFvManagedDockablePaneButtonImpl::SetDockablePane(CFvManagedDockablePane* pane)
{
   m_pane = pane;
}

CMFCRibbonPanel* CFvManagedDockablePaneButtonImpl::GetParentPanel()
{
   return m_parentPanel;
}

void CFvManagedDockablePaneButtonImpl::SetParentPanel(CMFCRibbonPanel* parentPanel)
{
   m_parentPanel = parentPanel;
}

CMFCRibbonBar* CFvManagedDockablePaneButtonImpl::GetRibbonBar()
{
   return m_ribbonBar;
}

void CFvManagedDockablePaneButtonImpl::SetRibbonBar(CMFCRibbonBar* ribbonBar)
{
   m_ribbonBar = ribbonBar;
}

BOOL CFvManagedDockablePaneButtonImpl::OnClose()
{
   int parentPanelCount = m_parentPanel->GetCount();
   for (int i = 0; i < parentPanelCount; i++)
   {
      if (m_parentPanel->GetElement(i) == GetRibbonBaseElement())
      {
         m_parentPanel->Remove(i, FALSE);
         delete GetRibbonBaseElement();

         if (parentPanelCount - 1 == 0)
         {
            CMainFrame *pFrame = fvw_get_frame();
            pFrame->HideMinimizedPanesPanel();
         }

         break;
      }
   }

   return TRUE;
}

// End of RibbonImpl.cpp
