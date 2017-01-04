// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"
#include "MDMInstallWebMapType.h"
#include "afxdialogex.h"
#include "FVNetFetcher/net_fetcher.h"
#include "WMTInstaller.h"
//#include "Commctrl.h"

CMDMInstallWebMapType::CMDMInstallWebMapType(CWnd* pParent /*=NULL*/)
   : CDialog(CMDMInstallWebMapType::IDD, pParent)
{
}

void CMDMInstallWebMapType::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_URL,m_editUrlTextField);
   DDX_Control(pDX, IDC_LAYER_TREE, m_layerTreeControl);
   DDX_Control(pDX, IDC_EDIT_WMT_SERVER_NAME,m_editServerNameField);
   DDX_Control(pDX, IDOK, m_OKButton);
}


BEGIN_MESSAGE_MAP(CMDMInstallWebMapType, CDialog)
   ON_BN_CLICKED(IDC_GET_LAYERS_BUTTON, OnGetLayersButtonClicked)
   //ON_MESSAGE(TREE_VIEW_CHECK_STATE_CHANGE, OnTreeViewCheckStateChange)
END_MESSAGE_MAP()


// CMDMInstallWebMapType message handlers

BOOL CMDMInstallWebMapType::OnInitDialog()
{
   //initialize reserved characters
   CMDMInstallWebMapType::reservedChars = ">\'<:|?*/\"\\";
   // TODO: Add all endpoints here, they will eventually be in a drop-down.
   CDialog::OnInitDialog();
   m_editUrlTextField.SetWindowTextA(
      "http://geoint.nrlssc.navy.mil/nrltileserver/wms?version=1.1.1&request=GetCapabilities&service=WMS");
      //"http://svs.gsfc.nasa.gov/cgi-bin/wms?version=1.3.0&request=GetCapabilities&service=WMS");

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMDMInstallWebMapType::OnGetLayersButtonClicked()
{
   try
   {
      /*
      TODO: Handle successive presses of the get button without pressing OK.
      Need to decide whether the tree should be wiped or the new data appended.
      If the new data is to be appended, we need a way to label the server for
      each.
      */

      /*
      TODO: extract this to a seperate method and call 
      when enter button is pressed.
      */
      HWND tree_hwnd = ::GetDlgItem(m_hWnd, IDC_LAYER_TREE);
      /*here are some test URLs:

      Navy's WMS service
      http://geoint.nrlssc.navy.mil/nrltileserver/wms?version=1.3.0&request=GetCapabilities&service=WMS
      http://geoint.nrlssc.navy.mil/nrltileserver/wms?version=1.1.1&request=GetCapabilities&service=WMS

      Joel's GeoServer Instance
      http://falcondata.ittl.gtri.org:8080/geoserver/ows?service=wms&version=1.1.1&request=GetCapabilities

      falconservices
      http://falconservices.icl.gtri.org:9090/wms?request=GetCapabilities&version=1.3.0&service=wms
      http://falconservices.icl.gtri.org:9090/wms?request=GetCapabilities&version=1.1.1&service=wms
      
      OSM
      http://129.206.228.72/cached/osm
      */

      // parse the url
      m_editUrlTextField.GetWindowText(m_url);
      net_fetcher::ParsedURI parsed(m_url);
      std::string scheme = parsed.GetScheme();
      std::string host = parsed.GetHost();

      std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);

      if (strlen(parsed.GetScheme()) > 0 && strlen(parsed.GetHost()) > 0)
      //bool isValid = scheme.size() > 0;
      //bool isHttp = isValid && (scheme.compare());
      //if(scheme.size() > 0 && scheme)// coming back to finish...
      {
         
         // add the layers to the tree
         if (m_wmsCapabilitiesReader.InitializeFromUrl(std::string(_bstr_t(m_url))))
         {
            //m_wmsCapabilities = m_wmsCapabilitiesReader.GetCapabilities();

            AddLayerToTree(m_wmsCapabilitiesReader.GetCapabilities().top_layer, tree_hwnd, nullptr);
            PopulateDefaultServerName();
            m_OKButton.EnableWindow(TRUE);
         }
         else
         {
            MessageBox(
               "Failed to parse WMS capabilities.", "Parse Failed", MB_OK);
         }

      }
      else
      {
         MessageBox("The url is not valid.", "Invalid URL", MB_OK);
      }
   }
   catch (_com_error)
   {
      ERR_report("Failed to fetch WMS capabilities.");
   }
}

void CMDMInstallWebMapType::FilterCapabilitiesToSelectedItems(HTREEITEM item)
{
   //start by checking the passed in item
   if(!m_layerTreeControl.GetCheck(item) &&
      !m_layerTreeControl.ItemHasSelectedChildren(item))
   {// neither the item or any of its children are selected
      m_wmsCapabilitiesReader.GetCapabilities().top_layer->
                  eraseChildWithNameAndTitle(
                                 *reinterpret_cast<std::string*>
                                 (m_layerTreeControl.GetItemData(item)),
                            std::string(m_layerTreeControl.GetItemText(item)));
      return;
   }

   // at this point the item is selected or it has selected children, we handle both the same way
   item = m_layerTreeControl.GetChildItem(item);
   while(item != nullptr)
   {// one of these children has a selection, so keep digging...
      FilterCapabilitiesToSelectedItems(item);
      item = m_layerTreeControl.GetNextSiblingItem(item);
   }
}

bool CMDMInstallWebMapType::EntireTreeIsUnselected()
{
   HTREEITEM root = m_layerTreeControl.GetRootItem();
   return (!m_layerTreeControl.GetCheck(root) 
        && !m_layerTreeControl.ItemHasSelectedChildren(root));
}

void CMDMInstallWebMapType::InstallSelectedLayers()
{
   const size_t BUF_LEN = 2048;
   char buf[BUF_LEN]; // used in two places

   // build the list of layers

   std::stringstream ss;
   bool first_layer = true;

   if(EntireTreeIsUnselected())
   {
      m_wmsCapabilitiesReader.GetCapabilities().top_layer->clearAllChildren();
      MessageBox(_T("No layers selected."), MB_OK);
      return;
   }
   else
   {
      FilterCapabilitiesToSelectedItems(m_layerTreeControl.GetRootItem());
   }

   HWND hwnd = ::GetDlgItem(m_hWnd, IDC_EDIT_WMT_SERVER_NAME);
   ::GetWindowText(hwnd, buf, BUF_LEN);

   if (strlen(buf) > 0)
   {
      // change the cursor to a wait cursor
      HCURSOR new_cursor = ::LoadCursor(nullptr, IDC_WAIT);
      HCURSOR old_cursor = ::SetCursor(new_cursor);

      // add the WMS

      HRESULT hr = E_FAIL;

         
         WMTInstaller installer;
         bool succeeded = installer.InstallMap(&m_wmsCapabilitiesReader.GetCapabilities(),
                                               std::string(_bstr_t(m_url)),
                                               std::string(_bstr_t(buf)));
      // restore the cursor
      ::SetCursor(old_cursor);

      // notify the user of completion
      if(!succeeded)
      {
         MessageBox(_T("Failed to add web service."), MB_OK);
      }


   }
   else
   {
      MessageBox(_T("Invalid server name."), _T("Invalid server name."), MB_OK);
   }
}

/**
 * @fn   bool CMDMInstallWebMapType::ServerNameIsOk()
 *
 * @brief   Determines if the server name in IDC_EDIT_WMT_SERVER_NAME
 *          text field already
 *          exists...
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @return  true if it is a valid server name, false otherwise.
 */

bool CMDMInstallWebMapType::ServerNameIsTaken()
{
   const size_t B_LEN = 2048;
   char b[B_LEN];

   HWND hwnd = ::GetDlgItem(m_hWnd, IDC_EDIT_WMT_SERVER_NAME);
   ::GetWindowText(hwnd, b, B_LEN);

   return !MDSWrapper::GetInstance()->WebSourceNameExists(std::string(b));

}

/**
 * @fn   void CMDMInstallWebMapType::PopulateDefaultServerName()
 *
 * @brief   Populate the server name in IDC_EDIT_WMT_SERVER_NAME text field.
 *          Defaults to the host name. Checks whether the default server name
 *          exists and adds numeric suffix until a an available entry is found.
 *
 * @author  MattWeber
 * @date 12/13/2013
 */

void CMDMInstallWebMapType::PopulateDefaultServerName()
{// TODO: come back and clean this up.

   //MDSWrapper::GetInstance()->GetDataSourceIds();
   net_fetcher::ParsedURI parsed(m_url);
   const std::string defaultName = parsed.GetHost();
   std::ostringstream tStream;
   tStream << defaultName;
   int a = 1;

   for(int a = 1; 
      (a < 999 && MDSWrapper::GetInstance()->WebSourceNameExists(std::string(tStream.str())));
      a++)
   {
      tStream.str(defaultName);
      tStream << defaultName << '_' << a;
   }
   m_editServerNameField.SetWindowText(std::string(tStream.str()).c_str());
}

void CMDMInstallWebMapType::AddLayerToTree(
   WMSCapabilitiesLayerInfo* layer, const HWND tree_hwnd, const HTREEITEM parent)
{
   TVINSERTSTRUCT tvinsert;

   tvinsert.hParent = parent; // null for root of tree
   tvinsert.hInsertAfter = parent == nullptr ? TVI_ROOT : TVI_LAST;
   tvinsert.item.mask = TVIF_TEXT|TVIF_STATE|TVIF_PARAM;
   tvinsert.item.stateMask = TVIS_EXPANDED|TVIS_STATEIMAGEMASK;
   tvinsert.item.state = TREE_NODE_UNCHECKED|TVIS_EXPANDED;

   //_bstr_t title(layer->title);
   //tvinsert.item.pszText = (char*)title;
   tvinsert.item.pszText = const_cast<char *>(layer->title.c_str());

   _bstr_t n(const_cast<char *>(layer->name.c_str()));
   std::string* name = new std::string((char*)n);

   m_strings_to_delete.push_back(name);


   tvinsert.item.lParam = reinterpret_cast<LPARAM>(name);

   HTREEITEM item = TreeView_InsertItem(tree_hwnd, &tvinsert);

   size_t num_children = layer->children.size();
   for (size_t i = 0; i < num_children; i++)
      AddLayerToTree(layer->children[i], tree_hwnd, item);
}

/* uncomment me to listen for checkbox state change messages
LRESULT CMDMInstallWebMapType::OnTreeViewCheckStateChange(WPARAM wParam, LPARAM lParam)
{
   HTREEITEM   item = (HTREEITEM)lParam;
   BOOL newState = m_layerTreeControl.GetCheck(item);
   return 0L;
}
*/
void CMDMInstallWebMapType::OnOK()
{
   //make sure the server name can be a windows directory name
   if(!ServerNameIsValidFileName())
   {
      std::string message = "That Server Name is not valid. \n"
                            "Please make sure the following characters "
                            "aren't being used: \n"+reservedChars;
      MessageBox(
         _T(message.c_str()),
         _T("Server Name is not valid."), MB_OK);
   }
   // make sure the chosen name won't throw a duplicate exception
   else if(ServerNameIsTaken())
   {
      InstallSelectedLayers();
      CDialog::OnOK();
   }
   else
   {
      MessageBox(
      _T("That Server Name already exists. Please enter another one."),
      _T("Server Name Exists"), MB_OK);
   }
}

/// @fn  bool CMDMInstallWebMapType::ServerNameIsValidFileName(void)
///
/// @brief  Determines whether the provided server name is a valid windows
///         directory name and doesn't have any characters that would throw 
///         off a query (ie: ' ). 
///
/// @author Matt Weber
/// @date   4/1/2014
///
/// @return false if an invalid character exists in the string, true otherwise

bool CMDMInstallWebMapType::ServerNameIsValidFileName(void)
{
   //grab the name
   const size_t B_LEN = 2048;
   char b[B_LEN];

      HWND hwnd = ::GetDlgItem(m_hWnd, IDC_EDIT_WMT_SERVER_NAME);
   ::GetWindowText(hwnd, b, B_LEN);
   std::string serverName(b);

   // most of these are the reserved characters listed here: 
   //http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#file_and_directory_names
   //><:|?*/\'\"\\
   // we also don't accept a ' because the database doesn't like it...
   // To add a reserved character, go to OnInitDialog() method and add 
   // it to the reservedChars string.

   for(size_t i = 0; i < reservedChars.size(); i++)
   {
      if(serverName.find(reservedChars[i],0) != std::string::npos)
      {
         //invalid character found
         return false;
      }
   }

   return(true);
}

