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

// CustomMapGroups.cpp : implementation file
//

#include "stdafx.h"
#include "..\MapDataServer\Common.h"
#include "CustomMapGroups.h"
#include "NewMapGroup.h"
#include "err.h"
#include "MapMenu.h"
#include "getobjpr.h"

using namespace std;

BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
//	ON_WM_CHAR()
//	ON_CONTROL_REFLECT(LBN_DBLCLK, &CListBoxEx::OnLbnDblclk)
//	ON_WM_CHARTOITEM_REFLECT()
//	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// int nMapCount = 0;

// CCustomMapGroups dialog

IMPLEMENT_DYNAMIC(CCustomMapGroups, CDialog)

CCustomMapGroups::CCustomMapGroups(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomMapGroups::IDD, pParent), m_bIsDirty( FALSE )
{

}

CCustomMapGroups::~CCustomMapGroups()
{
}

void CCustomMapGroups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAP_GROUP, m_cbStandardMapGroups);
	DDX_Control(pDX, IDC_CUSTOM_GROUP, m_cbCustomMapGroups);
	DDX_Control(pDX, IDC_MAP_TYPES, m_lbStandardMapNames);
	DDX_Control(pDX, IDC_MAP_TYPES2, m_lbCustomMapNames);
	DDX_Control(pDX, IDC_DELETE, m_btnCustomDelete);
	DDX_Control(pDX, IDC_ADD_MAP, m_btnAddMap);
	DDX_Control(pDX, IDC_REMOVE_MAP, m_btnRemoveMap);
}


BEGIN_MESSAGE_MAP(CCustomMapGroups, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_NEW, &CCustomMapGroups::OnBnClickedNew)
	ON_BN_CLICKED(IDC_DELETE, &CCustomMapGroups::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_ADD_MAP, &CCustomMapGroups::OnBnClickedAddMap)
	ON_BN_CLICKED(IDC_REMOVE_MAP, &CCustomMapGroups::OnBnClickedRemoveMap)
	ON_CBN_SELCHANGE(IDC_MAP_GROUP, &CCustomMapGroups::OnCbnSelchangeMapGroup)
	ON_CBN_SELCHANGE(IDC_CUSTOM_GROUP, &CCustomMapGroups::OnCbnSelchangeCustomGroup)
	ON_BN_CLICKED(IDOK, &CCustomMapGroups::OnBnClickedOk)
	ON_CBN_EDITUPDATE(IDC_CUSTOM_GROUP, &CCustomMapGroups::OnCbnEditupdateCustomGroup)
	ON_CBN_DROPDOWN(IDC_CUSTOM_GROUP, &CCustomMapGroups::OnCbnDropdownCustomGroup)
//	ON_LBN_DBLCLK(IDC_MAP_GROUP, &CCustomMapGroups::OnLButtonDblClk)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_LBN_DBLCLK(IDC_MAP_TYPES, &CCustomMapGroups::OnLbnDblclkMapTypes)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
BOOL CCustomMapGroups::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->m_lbStandardMapNames.ModifyStyle( 0, LBS_EXTENDEDSEL );


   try
   {
		//	Create the COM objects that wrap the map group database tables
      CO_CREATE(m_spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(m_spMapGroups, CLSID_MapGroups);

	   HRESULT hrGroupNames = m_spMapGroupNames->SelectAll();
      while (hrGroupNames == S_OK)
      {
			//	Iterate through the MapGroupNames table and place each
			//	in either the standard or custom (user created table)
			//	depending on MapGroupName Identity
			long iIndex;
			long lGroupIdentity = this->m_spMapGroupNames->m_Identity;
			_bstr_t bstrName = m_spMapGroupNames->m_GroupName;

         // Sanity check because table allows blanks
			if ( bstrName.length() && m_spMapGroupNames->m_Disabled == FALSE)
			{
				if ( this->m_spMapGroupNames->m_Identity < FIRST_USER_GROUP_IDENTITY )
				{
					//	Add to standard map group table
					iIndex = m_cbStandardMapGroups.AddString( bstrName );
					m_cbStandardMapGroups.SetItemDataPtr( iIndex, (void*)lGroupIdentity );
				}
	   		else //	Add to custom map group table
				{
					iIndex = this->m_cbCustomMapGroups.AddString( bstrName );
					m_cbCustomMapGroups.SetItemDataPtr( iIndex, (void*)lGroupIdentity );
				}
			}
		      
			hrGroupNames = m_spMapGroupNames->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error building map group combobox : [%s]", (char *)e.Description());
      // ERR_report(msg);
   }

	//	Set selection to first available name
	CString sName = (char*)this->m_spMapGroupNames->m_GroupName;
	if ( m_cbCustomMapGroups.SelectString( 0, sName ) == CB_ERR )
		m_cbCustomMapGroups.SetCurSel( 0 );
	if ( m_cbStandardMapGroups.SelectString( 0, sName ) == CB_ERR )
		m_cbStandardMapGroups.SetCurSel( 0 );
	
	//	Repopulate map name listboxes
	OnCbnSelchangeMapGroup();
	OnCbnSelchangeCustomGroup();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnDestroy()
{
	CDialog::OnDestroy();

	//	Delete all previous entries in the combo box
	for ( INT i = 0; i < this->m_lbCustomMapNames.GetCount(); i++ )
	{
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbCustomMapNames.GetItemDataPtr( i );
		if ( pEntry ) delete pEntry;
	}


	//	Delete all previous entries in the combo box
	for ( INT i = 0; i < this->m_lbStandardMapNames.GetCount(); i++ )
	{
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbStandardMapNames.GetItemDataPtr( i );
		if ( pEntry ) delete pEntry;
	}
}

// CCustomMapGroups message handlers
//-----------------------------------------------------------------------------
void CCustomMapGroups::OnBnClickedNew()
{
	if ( this->m_bIsDirty )
	{
		if ( AfxMessageBox( "Custom Map Group has changed. Do you want to save?", MB_YESNO ) == IDYES )
			OnApply();
	}

	//	User wants to add a new custom map group
	CNewMapGroup dlg;
	if ( dlg.DoModal() == IDOK )
	{
		//	Get group name from user input
		_bstr_t bstrName = dlg.m_sMapGroupName;
		
      try
      {
         if ( this->m_spMapGroupNames->SelectByName( bstrName ) != S_OK )
         {
            //	Name does not exist, so we add a new record in the MapGroupNames table
            this->m_spMapGroupNames->AddNew();
            this->m_spMapGroupNames->m_GroupName = bstrName;
            this->m_spMapGroupNames->m_IsSoftScale = 0L; // what should this be ??? TBD
            this->m_spMapGroupNames->Insert();

            //	Add it to the combo box and select it
            m_iGroupSel = this->m_cbCustomMapGroups.AddString( bstrName );
            this->m_cbCustomMapGroups.SetCurSel( m_iGroupSel );

            //	Store the Group Identity in the combox box selection data ptr.
            long lGroupIdentity = this->m_spMapGroupNames->m_Identity;
            this->m_cbCustomMapGroups.SetItemDataPtr( m_iGroupSel, (void*)lGroupIdentity );
         }
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }

		//	If we found this MapGroupName or created it, it will exist in the combobox
		//	so select it and populate the custom group map group names listbox
		this->m_cbCustomMapGroups.SelectString( 0, bstrName );
		OnCbnSelchangeCustomGroup();
	}
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnBnClickedDelete()
{
	this->m_bIsDirty = false;

	//	User has deleted a custom map group

	INT iSel = this->m_cbCustomMapGroups.GetCurSel();
	if ( iSel != CB_ERR )
	{
		//	Delete the map group from the combo box.
		CString sName;
		this->m_cbCustomMapGroups.GetWindowText( sName );

      // verify that the user wants to delete the group
      CString msg;
      msg.Format("Are you sure you want to delete the map group named '%s'?", sName);
      if (AfxMessageBox(msg, MB_YESNO) == IDNO)
         return;

		_bstr_t bstrName = sName;
		this->m_cbCustomMapGroups.DeleteString( iSel );
		
      try
      {
         //	Delete the map group from the database
         if ( this->m_spMapGroupNames->SelectByName( bstrName ) == S_OK )
         {
            //	Remove from MapGroupNames table
            long lIdentity = this->m_spMapGroupNames->m_Identity;
            this->m_spMapGroupNames->DeleteByIdentity( lIdentity );

            //	Get a list of all matching identities in the MapGroup table
            vector<long> vecIdentities;
            HRESULT hr = m_spMapGroups->SelectByGroupNameIdentity( lIdentity );
            while (hr == S_OK)
            {
               vecIdentities.push_back( m_spMapGroups->m_Identity );
               hr = m_spMapGroups->MoveNext();
            }

            //	Delete each matching record
            for ( size_t i = 0; i < vecIdentities.size(); i++ )
               this->m_spMapGroups->Delete( lIdentity );
         }
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }

		for ( INT i = 0; i < this->m_lbCustomMapNames.GetCount(); i++ ) 
		{
			MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbCustomMapNames.GetItemDataPtr( i );
			if ( pEntry ) delete pEntry;
		}

		//	Reset selection of map group combo box so we can see
		//	another entry
		this->m_iGroupSel = 0;
		this->m_cbCustomMapGroups.SetCurSel( m_iGroupSel );
		this->m_lbCustomMapNames.ResetContent();

		//	Repopulate the map names listbox
		OnCbnSelchangeCustomGroup();
	}
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnBnClickedAddMap()
{
	this->m_bIsDirty = true;

	//	User adds a map name from the standard map names table
	INT iMapSel = this->m_lbStandardMapNames.GetCurSel();
	INT iGroupSel = this->m_cbCustomMapGroups.GetCurSel();

	// Get the indexes of all the selected items.
	int nCount = this->m_lbStandardMapNames.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);
	this->m_lbStandardMapNames.GetSelItems(nCount, aryListBoxSel.GetData()); 

	//	Add each selected standard map name to the custom map name table
	for ( int i = nCount - 1; i >= 0; i-- )
	{
		iMapSel = aryListBoxSel.GetAt( i );
		long lGroupNameIdentity = (long)this->m_cbCustomMapGroups.GetItemDataPtr( iGroupSel );
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbStandardMapNames.GetItemDataPtr( iMapSel );
		pEntry->iStatus = 1;
	
		//	Delete the standard map name from the standard map name table
		this->m_lbStandardMapNames.DeleteString( iMapSel );
		CString sDescr = StripFKey( pEntry->m_source.get_string() + " " + pEntry->ToString() );
		
		//	Add the standard map name to the custom map name table, unless
		//	it already exists
		INT iMapSel2 = this->m_lbCustomMapNames.FindString( 0, sDescr );
		if ( iMapSel2 == LB_ERR )
			//	Add the standard map name to to custom map name table
			iMapSel2 = this->m_lbCustomMapNames.AddString( StripFKey( sDescr ) );
		this->m_lbCustomMapNames.SetItemDataPtr( iMapSel2, pEntry );

		//	If we had previously deleted this entry from the custom map
		//	group table, then we need to remove it from the cache of 
		//	custom map name records	that we must delete if the user
		//	presses the OK button
		for ( size_t j = 0; j < this->m_vecDeleted.size(); j++ )	{
			if ( this->m_vecDeleted[ j ] == pEntry->nIdentity ) {
				this->m_vecDeleted.erase( this->m_vecDeleted.begin() + j );
				break;
			}
		}
	}

	//	Only allow the user to remove custom map name entries if entries exist.
	this->m_btnRemoveMap.EnableWindow( this->m_lbCustomMapNames.GetCount() > 0 );
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnBnClickedRemoveMap()
{
	this->m_bIsDirty = true;

	INT iMapSel = this->m_lbCustomMapNames.GetCurSel();
	INT iGroupSel = this->m_cbStandardMapGroups.GetCurSel();
	long lGroupIdentity = (long)this->m_cbStandardMapGroups.GetItemDataPtr( iGroupSel );

	// Get the indexes of all the selected items.
	int nCount = this->m_lbCustomMapNames.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);
	this->m_lbCustomMapNames.GetSelItems(nCount, aryListBoxSel.GetData()); 

	for ( int i = nCount - 1; i >= 0; i-- )
	{
		iMapSel = aryListBoxSel.GetAt( i );
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbCustomMapNames.GetItemDataPtr( iMapSel );

		// Add to our delete list for this map group
		this->m_vecDeleted.push_back( pEntry->nIdentity );
		
		// If the deleted map group entry is in the currently selected standard map group,
		// then we need to add it back to that group and re-attach the data
		if ( pEntry->nGroupIdentity == lGroupIdentity )
		{
			pEntry->iStatus = -1;
			CString sDescr = StripFKey( pEntry->m_source.get_string() + " " + pEntry->ToString() );
			INT iMapSel2 = this->m_lbStandardMapNames.FindString( 0, sDescr );
			if ( iMapSel2 == LB_ERR )
				iMapSel2 = this->m_lbStandardMapNames.AddString( sDescr );
			this->m_lbStandardMapNames.SetItemDataPtr( iMapSel2, pEntry );
		}
		else	// Otherwise we can just delete the map group entry because it will get
		{		//	recreated when the combo box selection for the standard map names
				//	is changed.
			
			delete pEntry;
		}

		//	Delete each selected custom map name entry
		this->m_lbCustomMapNames.DeleteString( iMapSel );
		//	Only allow the user to remove custom map name entries if entries exist.
		this->m_btnRemoveMap.EnableWindow( this->m_lbCustomMapNames.GetCount() > 0 );
	}

	//	Reset map groups to resync listboxes
	OnCbnSelchangeMapGroup();
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnCbnSelchangeMapGroup()
{
	this->OnCbnSelchangeGroupEx( this->m_cbStandardMapGroups, this->m_lbStandardMapNames );

	// Go back to the standard map names listbox and remove any entries that we
	// have added to the custom map group listbox
	for ( INT i = 0; i < this->m_lbCustomMapNames.GetCount(); i++ )
	{
		CString sDescr;
		this->m_lbCustomMapNames.GetText( i, sDescr );
		INT iSel = this->m_lbStandardMapNames.FindStringExact( 0, sDescr );
		if ( iSel != LB_ERR )
		{
			MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbStandardMapNames.GetItemDataPtr( iSel );
			if ( pEntry ) delete pEntry;
			this->m_lbStandardMapNames.DeleteString( iSel );
		}
	}
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnCbnSelchangeCustomGroup()
{
	if ( this->m_bIsDirty )
	{
		if ( AfxMessageBox( "Custom Map Group has changed. Do you want to save?", MB_YESNO ) == IDYES )
			OnApply();
	}

	// Save the new index for the currently selected custom map group.
	m_iGroupSel = this->m_cbCustomMapGroups.GetCurSel();

	// new custom group means we need to throw away the old deleted records list
	// because it applies to the previous custom group
	this->m_vecDeleted.clear();

	//	Repopulate the custom map group listbox
	this->OnCbnSelchangeGroupEx( this->m_cbCustomMapGroups, this->m_lbCustomMapNames );

	// Resync the standard map group
	OnCbnSelchangeMapGroup();
		
	CString sName;
	this->m_cbCustomMapGroups.GetWindowText( sName );
	this->m_btnCustomDelete.EnableWindow( sName.IsEmpty() == FALSE );
	this->m_btnAddMap.EnableWindow( sName.IsEmpty() == FALSE );
	this->m_btnRemoveMap.EnableWindow( this->m_lbCustomMapNames.GetCount() > 0 );
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnCbnSelchangeGroupEx( CComboBox& cb, CListBox& lb )
{
	//	User changed either the standard or custom map group name combo box
	INT iSel = cb.GetCurSel();
	if ( iSel != CB_ERR )
	{
		//	Delete all previous entries in the combo box
		for ( INT i = 0; i < lb.GetCount(); i++ )
		{
			MapMenuEntry* pEntry = (MapMenuEntry*)lb.GetItemDataPtr( i );
			if ( pEntry ) delete pEntry;
		}

		multimap<double,MapMenuEntry*> results;

      try
      {
         //	Clear the listbox, and repopulate based on the group identity
         lb.ResetContent();
         long lGroupIdentity = (long)cb.GetItemDataPtr( iSel );
         HRESULT hr = m_spMapGroups->SelectByGroupNameIdentity( lGroupIdentity );
         while (hr == S_OK)
         {
            MapMenuEntry* pEntry = new MapMenuEntry;

            pEntry->iStatus = 0;
            pEntry->m_source = MapSource(m_spMapGroups->m_ProductName);
            pEntry->m_scale = MapScale(m_spMapGroups->m_Scale, m_spMapGroups->m_ScaleUnits);
            pEntry->m_series = MapSeries(m_spMapGroups->m_SeriesName);
            pEntry->m_is_soft_scale = 0;
            pEntry->nGroupIdentity = m_spMapGroups->m_GroupNameIdentity;
            pEntry->nIdentity = this->m_spMapGroups->m_Identity;

            results.insert(pair<double,MapMenuEntry*>( m_spMapGroups->m_ScaleDenominator, pEntry ));

            hr = m_spMapGroups->MoveNext();
         }
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }

		for (multimap<double,MapMenuEntry*>::reverse_iterator it = results.rbegin(); it != results.rend(); ++it)
		{
			double dScale = (*it).first;
			MapMenuEntry* pEntry2 = (*it).second;
			CString sDescr = StripFKey( pEntry2->m_source.get_string() + " " + pEntry2->ToString() );
			iSel = lb.AddString( sDescr );
			lb.SetItemDataPtr( iSel, pEntry2 );
		}
	}
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnBnClickedOk()
{
	// Make sure we are applying the currently selected custom group
	m_iGroupSel = this->m_cbCustomMapGroups.GetCurSel();
	
	OnApply();

	for ( INT i = 0; i < this->m_lbCustomMapNames.GetCount(); i++ ) 
	{
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbCustomMapNames.GetItemDataPtr( i );
		if ( pEntry ) delete pEntry;
	}

	for ( INT i = 0; i < this->m_lbStandardMapNames.GetCount(); i++ )	{
		MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbStandardMapNames.GetItemDataPtr( i );
		if ( pEntry ) delete pEntry;
	}
	
	OnOK();
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnApply()
{
	// If no custom groups, no work to do
	if ( m_iGroupSel == CB_ERR )
		return;

	INT iMapSel = this->m_lbStandardMapNames.GetCurSel();
	long lGroupNameIdentity = (long)this->m_cbCustomMapGroups.GetItemDataPtr( m_iGroupSel );
	ULONG hResult = 0;

	//	Save the map group name so we can change to this in the
	//	FalconView MapView
	this->m_cbCustomMapGroups.GetLBText( m_iGroupSel, this->m_sGroupName );
	if ( this->m_sGroupName.IsEmpty() )
		this->m_cbStandardMapGroups.GetLBText( iMapSel, this->m_sGroupName );

   try
   {
      //	Add a record to the map groups table only if it is a new entry we have made
      for ( INT i = 0; i < this->m_lbCustomMapNames.GetCount(); i++ )
      {
         MapMenuEntry* pEntry = (MapMenuEntry*)this->m_lbCustomMapNames.GetItemDataPtr( i );

         if ( pEntry->iStatus == 1 ) // Flagged as new entry
         {
            hResult = m_spMapGroups->CloneGroupMember( pEntry->nIdentity, lGroupNameIdentity );
         }
      }

      //	Remove any records we have flagged for deletion.
      for ( size_t j = 0; j < this->m_vecDeleted.size(); j++ )
         hResult = m_spMapGroups->Delete( this->m_vecDeleted[ j ] );

   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

	this->m_bIsDirty = false;
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnCbnEditupdateCustomGroup()
{
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnCbnDropdownCustomGroup()
{
	
}

//-----------------------------------------------------------------------------
CString CCustomMapGroups::StripFKey( CString sDescr )
{
	for ( INT j = 0; j < sDescr.GetLength(); j++ )
	{
		if ( ::iscntrl( sDescr.GetAt( j ) ) )
		{
			sDescr = sDescr.Left( j );
			break;
		}
	}

	return sDescr;
}

//-----------------------------------------------------------------------------
void CCustomMapGroups::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);

	if ( IsCTRLpressed() )
	{
		if ( nChar == 'A' || nChar == 'a' )
		{
			this->m_lbStandardMapNames.SetCurSel(0);
		}
	}
}

void CCustomMapGroups::OnLbnDblclkMapTypes()
{
	if ( this->m_cbCustomMapGroups.GetCount() > 0 )
		OnBnClickedAddMap();
}
LRESULT CCustomMapGroups::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

