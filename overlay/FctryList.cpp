// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

   

#include "stdafx.h"
#include "stdlib.h"
#include "param.h"
#include "err.h"
#include "file.h"
#include "ovl_mgr.h"

// NOTE:  The following were brought in from CHUM.h.   Need to sort out which ones are really necessary.
#include "common.h"
#include "overlay.h"
#include "chstruct.h"
#include "polyline.h"
#include "IndexFil.h"
#include "maps_d.h"
#include "param.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "ogrsf_frmts\ogrsf_frmts.h"
#import "fvw.tlb" named_guids
#import "SampledMotionPlayback.tlb" no_namespace, named_guids, exclude("IMotionPlayback")
#include "..\..\..\fvw_core\FvBaseDataSet\FvBaseDataSet.h"

#include "..\getobjpr.h"
#include "..\fvw.h"
#include "..\mainfrm.h"
#include "FvCore\Include\GuidStringConverter.h"
#include "..\OvlToolConfiguration.h"

#include "..\factory.h"
#include "OverlayFactoryCOM.h"
#include "OverlayEditorCOM.h"
#include "OverlayTypeCustomInitializerCOM.h"

#include "..\ar_edit\factory.h"
#include "..\include\ar_edit.h"
#include "..\PrintToolOverlay\factory.h"
#include "..\include\PrntTool.h"
#include "..\catalog\factory.h"
#include "..\catalog\cov_ovl.h"
#include "..\scene_mgr\sm_factory.h"
#include "..\scene_mgr\scene_cov_ovl.h"

#include "..\SkyViewOverlay\factory.h"
#include "mov_sym.h"

#include "..\pointex\factory.h"
#include "..\include\PntExprt.h"
#include "..\scalebar\factory.h"
#include "..\include\Scalebar.h"
#include "..\grid_map\factory.h"
#include "..\include\grid_map.h"
#include "..\contour\factory.h"
#include "..\include\contour.h"
#include "..\TAMask\factory.h"
#include "..\include\TAMask.h"
#include "..\nitf\factory.h"
#include "..\include\nitf.h"
#include "..\shp\factory.h"
#include "..\include\shp.h"
#include "..\include\env.h"
#include "..\include\TacModel.h"
#include "..\include\BoundariesOverlay.h"
#include "..\MovingMapOverlay\factory.h"
#include "FalconView/localpnt/factory.h"
#include "FalconView/TacticalModel/factory.h"
#include "..\include\gps.h"

#include "HelpId.h"

#include "FalconView/DisplayElementProviderFactoryCOM.h"
#include "FalconView/LayerOvl.h"

// external GUIDs that are still referenced by FalconView

// {365FADD0-D0F3-42ab-89A0-92E8094A13E1}
const GUID FVWID_Overlay_Aimpoints = 
   { 0x365fadd0, 0xd0f3, 0x42ab, { 0x89, 0xa0, 0x92, 0xe8, 0x9, 0x4a, 0x13, 0xe1 } };

// {CF3FB2D5-DBB6-45F0-A1F4-658E51B2D7F7}
const GUID FVWID_Overlay_Drawing = 
{ 0xcf3fb2d5, 0xdbb6, 0x45f0, { 0xa1, 0xf4, 0x65, 0x8e, 0x51, 0xb2, 0xd7, 0xF7 } };

// {4A754FE2-4402-4571-AE7D-DBBC846C361D}
const GUID FVWID_Overlay_TacticalGraphics = 
   { 0x4a754fe2, 0x4402, 0x4571, { 0xae, 0x7d, 0xdb, 0xbc, 0x84, 0x6c, 0x36, 0x1d } };

// {BAB4CC5E-408D-416f-BDAD-FC7A56973A76}
const GUID FVWID_Overlay_Dropzones = 
   { 0xbab4cc5e, 0x408d, 0x416f, { 0xbd, 0xad, 0xfc, 0x7a, 0x56, 0x97, 0x3a, 0x76 } };

// {F604F4EE-B95D-4d1b-8BE8-63032B10380E}
const GUID FVWID_Overlay_Route = 
{ 0xf604f4ee, 0xb95d, 0x4d1b, { 0x8b, 0xe8, 0x63, 0x3, 0x2b, 0x10, 0x38, 0xe } };

// {F00402EA-1ACD-411b-A5A0-476E1F25342E}
const GUID FVWID_Overlay_Shadow = 
   { 0xf00402ea, 0x1acd, 0x411b, { 0xa5, 0xa0, 0x47, 0x6e, 0x1f, 0x25, 0x34, 0x2e } };

// {5C238B2F-B455-4596-AF5D-8B08652E4BBF}
const GUID FVWID_Overlay_Navaids = 
   { 0x5c238b2f, 0xb455, 0x4596, { 0xaf, 0x5d, 0x8b, 0x8, 0x65, 0x2e, 0x4b, 0xbf } };

// {FCDBE8C4-5F8E-4fd0-B44B-E4A44610D463}
const GUID FVWID_Overlay_Heliports = 
   { 0xfcdbe8c4, 0x5f8e, 0x4fd0, { 0xb4, 0x4b, 0xe4, 0xa4, 0x46, 0x10, 0xd4, 0x63 } };

// {4EBB61B2-8F43-41f9-A385-DC6F975B5C63}
const GUID FVWID_Overlay_Airports = 
   { 0x4ebb61b2, 0x8f43, 0x41f9, { 0xa3, 0x85, 0xdc, 0x6f, 0x97, 0x5b, 0x5c, 0x63 } };

// {FFAA0E8C-0217-4acd-B477-9C1EE63DB0D8}
const GUID FVWID_Overlay_Airways = 
   { 0xffaa0e8c, 0x217, 0x4acd, { 0xb4, 0x77, 0x9c, 0x1e, 0xe6, 0x3d, 0xb0, 0xd8 } };

// {B85A71DA-6C85-45d4-9947-183D3C45011A}
const GUID FVWID_Overlay_RefuelingRoutes = 
   { 0xb85a71da, 0x6c85, 0x45d4, { 0x99, 0x47, 0x18, 0x3d, 0x3c, 0x45, 0x1, 0x1a } };

// {70FAAA96-7472-48a7-8850-4740323E0533}
const GUID FVWID_Overlay_MilitaryTrainingRoutes = 
   { 0x70faaa96, 0x7472, 0x48a7, { 0x88, 0x50, 0x47, 0x40, 0x32, 0x3e, 0x5, 0x33 } };

// {AF6E6D1C-30AA-4899-9E04-E0648E2F21A4}
const GUID FVWID_Overlay_Waypoints = 
   { 0xaf6e6d1c, 0x30aa, 0x4899, { 0x9e, 0x4, 0xe0, 0x64, 0x8e, 0x2f, 0x21, 0xa4 } };

// {9F64FB8A-4746-432a-AB81-2CCCD4A0C66E}
const GUID FVWID_Overlay_ParachuteJump = 
   { 0x9f64fb8a, 0x4746, 0x432a, { 0xab, 0x81, 0x2c, 0xcc, 0xd4, 0xa0, 0xc6, 0x6e } };

// {1D1A3098-4C95-472c-894C-3A830A3F8BDD}
const GUID FVWID_Overlay_SuasBoundaries = 
   { 0x1d1a3098, 0x4c95, 0x472c, { 0x89, 0x4c, 0x3a, 0x83, 0xa, 0x3f, 0x8b, 0xdd } };

// {9A177537-E4B1-403b-9B84-33CDEAEB1612}
const GUID FVWID_Overlay_AirspaceBoundaries = 
   { 0x9a177537, 0xe4b1, 0x403b, { 0x9b, 0x84, 0x33, 0xcd, 0xea, 0xeb, 0x16, 0x12 } };

// {20421179-879A-4b6d-ADEF-06506AEF38D5}
const GUID FVWID_Overlay_Threat = 
   { 0x20421179, 0x879a, 0x4b6d, { 0xad, 0xef, 0x6, 0x50, 0x6a, 0xef, 0x38, 0xd5 } };

// {8BD7EE98-3A86-4705-9A64-69E0F4BBBDEF}
const GUID FVWID_Overlay_DigitalNauticalChart = 
   { 0x8bd7ee98, 0x3a86, 0x4705, { 0x9a, 0x64, 0x69, 0xe0, 0xf4, 0xbb, 0xbd, 0xef } };

// {40CF4A3F-7FC0-49bf-B896-F74899DD313F}
const GUID FVWID_Overlay_VectorSmartMap = 
   { 0x40cf4a3f, 0x7fc0, 0x49bf, { 0xb8, 0x96, 0xf7, 0x48, 0x99, 0xdd, 0x31, 0x3f } };

// {A77A9DBA-F3A7-4cbd-808C-7B51D35157EA}
const GUID FVWID_Overlay_WorldVectorShoreline = 
   { 0xa77a9dba, 0xf3a7, 0x4cbd, { 0x80, 0x8c, 0x7b, 0x51, 0xd3, 0x51, 0x57, 0xea } };

// {FB60F51E-3672-4030-BE48-BDA287207257}
const GUID FVWID_Overlay_Points = 
   { 0xfb60f51e, 0x3672, 0x4030, { 0xbe, 0x48, 0xbd, 0xa2, 0x87, 0x20, 0x72, 0x57 } };

// {7BA274D8-E003-4b11-8896-70D7A4716182}
const GUID FVWID_Overlay_ManualChum = 
   { 0x7ba274d8, 0xe003, 0x4b11, { 0x88, 0x96, 0x70, 0xd7, 0xa4, 0x71, 0x61, 0x82 } };

// {4AF6EBE8-DEC2-4669-A1CB-05F99A6462F4}
const GUID FVWID_Overlay_ElectronicChum = 
   { 0x4af6ebe8, 0xdec2, 0x4669, { 0xa1, 0xcb, 0x5, 0xf9, 0x9a, 0x64, 0x62, 0xf4 } };

// {BBF47577-F30D-4AA4-A33E-9726B3B0F93D}
const GUID FVWID_Overlay_Illumination =
   { 0xbbf47577, 0xf30d, 0x4aa4, { 0xa3, 0x3e, 0x97, 0x26, 0xb3, 0xb0, 0xf9, 0x3d } };


bool SortByDecreasingDisplayOrder(OverlayTypeDescriptor* a, OverlayTypeDescriptor* b)
{
   return a->displayOrder > b->displayOrder;
}

namespace 
{
   const char* SAVE_FILTER_DISABLED = "disabled";
}

namespace overlay_type_util {

// This function compares the extension of the file being opened (ext) to the
// default filter strings for each existing overlay factory.  When the extension
// is found in a default filter, the corresponding factory is passed back to be
// called in the opening of the file
OverlayTypeDescriptor *GetOverlayTypeFromFileExtension(const char *ext)
{
   // For later comparison purposes, create a new CString of the current file
   // extension and then make sure all of it's characters are lower case...
   CString file_extension = ext;
   file_extension.MakeLower();

   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext() )
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         CString current_default_filter = pOverlayTypeDesc->fileTypeDescriptor.openFileDialogFilter;

         // In order to eliminate error during the search for the current file extension
         // in the current default filter, make all characters lower case...
         current_default_filter.MakeLower();

         // if the current file's extension is found in the current factory's default
         // filter string, return the current factory because it is the correct one to
         // open the current file overlay...
         if (current_default_filter.Find(file_extension) != -1)
            return OVL_get_type_descriptor_list()->m_pCurrent;
      }
   }

   return NULL;
}

// get the directory where files for this overlay were last opened/saved
CString get_most_recent_directory(GUID overlayDescGuid)
{
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL)
   {
      // get the most recent directory (path) from the registry. Pass in the factory default
      // directory as the third parameter for use in case the _MOST_RECENT_DIR registry key
      // does not yet exist for this overlay type...
      string_utils::CGuidStringConverter guidConv(overlayDescGuid);

      CString most_recent_directory;
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         CString defaultDirectory = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;

         // If a custom file type descriptor is defined for the overlay, then
         // ask it for the default directory. Note that if the overlay type
         // wants to override the value stored in the registry then it should delete
         // the registry key.
         if (pOverlayTypeDesc->fileTypeDescriptor.spFileOverlayTypeDesc != NULL)
         {
            try
            {
               defaultDirectory = (char *)pOverlayTypeDesc->fileTypeDescriptor.spFileOverlayTypeDesc->GetDefaultDirectory();
            }
            catch(_com_error& e)
            {
               REPORT_COM_ERROR(e);
            }
         }

         most_recent_directory = PRM_get_registry_string("Overlay File Paths", guidConv.GetGuidString().c_str(), 
            defaultDirectory);
      }

      return most_recent_directory;
   }

   return "";
}

// set the directory where files for this overlay were last opened/saved
void set_most_recent_directory(GUID overlayDescGuid, CString pathname)
{
   // extract the directory from the given pathname
   int index = pathname.ReverseFind('\\');
   CString current_path = pathname.Left(index);

   string_utils::CGuidStringConverter guidConv(overlayDescGuid);
   PRM_set_registry_string("Overlay File Paths", guidConv.GetGuidString().c_str(), current_path);
}

// Returns true if the given overlay should be handled as tiled, draped
// overlay
bool HandleAsDrapedOverlay(C_overlay* overlay)
{
   const GUID& overlay_desc_guid = overlay->get_m_overlayDescGuid();
   if (overlay_desc_guid == FVWID_Overlay_TerrainAvoidanceMask)
      return false;

   // Overlays which are LayerOvls but do not implement ILayerEditor3 will never
   // draw directly to a GDI device context.  No need to handle in 3d.
   CBaseLayerOvl* layer_ovl = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (layer_ovl != nullptr)
   {
      LayerEditorImpl* layer_editor_impl = layer_ovl->get_ILayerEditor();
      if (layer_editor_impl != nullptr &&
         layer_editor_impl->GetInterfaceVersion() < 3)
      {
         return false;
      }
   }

   OverlayTypeDescriptor* desc = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(overlay_desc_guid);

   // Overlays that implement an display element provider factory are
   // already handled in 3d.
   if (desc->display_element_provider_factory != nullptr)
      return false;

   // Other overlays may opt out of 3d
   return desc->is_3d_enabled;
}

} // namespace overlay_type_util

// OverlayTypeDescriptorList
//
COverlayTypeDescriptorList::COverlayTypeDescriptorList() :
   m_pCurrent(NULL)
{
   m_pOvlToolConfiguration = new COvlToolConfiguration();
}

COverlayTypeDescriptorList::~COverlayTypeDescriptorList()
{
   delete m_pOvlToolConfiguration;
}

// initialize the list of overlay type descriptors
int COverlayTypeDescriptorList::Initialize()
{
   // Note: the default display order is based on the order that overlay types
   // are added to the overlay type descriptor list.
   //
   // Note: Until all overlays are moved out into the config file (or we come up
   // with another scheme for the default display order) the default display
   // order of overlays will not match previous versions of FalconView.
   //
   try
   {
      IFvOverlayTypesConfig3Ptr spOverlayTypesConfig;
      CO_CREATE(spOverlayTypesConfig, CLSID_FvOverlayTypesConfig);

      if (spOverlayTypesConfig->Initialize() == S_OK)
      {
         long lRet = spOverlayTypesConfig->SelectAll(TRUE);
         while (lRet)
         {
            // if an overlay type specifies an object implementing
            // IFvOverlayTypeRuntimeEnabled check to see if the overlay type is
            // enabled
            if (IsRuntimeEnabled(spOverlayTypesConfig->RuntimeEnabledClsid) ==
               FALSE)
            {
               lRet = spOverlayTypesConfig->MoveNext();
               continue;
            }

            // create the CIconImage from the icon's filename
            CIconImage* pIconImage = CIconImage::load_images(
               (char *)spOverlayTypesConfig->IconFilename);

            // create the custom initializer if necessary
            CCustomInitializerCOM* pCustomInit = NULL;
            CLSID customInitClsid = spOverlayTypesConfig->CustomInitializerClsid;
            if (customInitClsid != GUID_NULL)
               pCustomInit = new CCustomInitializerCOM(customInitClsid);

            // create the overlay's factory based on whether it is a file
            // overlay or a static overlay
            IFvOverlayFactory* pOverlayFactory = NULL;
            CLSID overlayClsid = spOverlayTypesConfig->OverlayClsid;
            CString pDisplayName = (char *)spOverlayTypesConfig->DisplayName;
            if (spOverlayTypesConfig->IsFileOverlay)
            {
               pOverlayFactory = new CFileOverlayFactoryCOM(
                  pDisplayName, overlayClsid);
            }
            else
            {
               pOverlayFactory = new COverlayFactoryCOM(
                  pDisplayName, overlayClsid);
            }

            // create the display element provider factory if necessary
            DisplayElementProviderFactory_Interface* depf = nullptr;
            CLSID display_element_provider_factory_clsid =
               spOverlayTypesConfig->DisplayElementProviderFactoryClsid;
            if (display_element_provider_factory_clsid != GUID_NULL)
            {
               depf = new DisplayElementProviderFactoryCOM(
                  display_element_provider_factory_clsid);
            }

            // create the overlay editor if necessary
            IFvOverlayEditor*  pOverlayEditor = NULL;
            CLSID editorClsid = spOverlayTypesConfig->EditorClsid;
            if (editorClsid != GUID_NULL)
            {
               pOverlayEditor = new COverlayEditorCOM(editorClsid,
                  spOverlayTypesConfig->DisplayName);
            }

            CString defaultDirectory, defaultFileExtension,
               openFileDialogFilter, saveFileDialogFilter;
            FalconViewOverlayLib::IFvFileOverlayTypeDescriptorPtr
               spFileOverlayTypeDesc;
            if (spOverlayTypesConfig->CustomFileDescClsid != GUID_NULL)
            {
               spFileOverlayTypeDesc = GetFileOverlayTypeDescFromObject(
                  spOverlayTypesConfig->CustomFileDescClsid, defaultDirectory,
                  defaultFileExtension, openFileDialogFilter,
                  saveFileDialogFilter);
            }
            else
            {
               defaultDirectory =
                  (char *)spOverlayTypesConfig->DefaultDirectory;
               defaultFileExtension =
                  (char *)spOverlayTypesConfig->DefaultFileExtension;
               openFileDialogFilter =
                  (char *)spOverlayTypesConfig->OpenFileDialogFilter;
               saveFileDialogFilter =
                  (char *)spOverlayTypesConfig->SaveFileDialogFilter;
            }

            bool is_top_most = spOverlayTypesConfig->IsTopMost == TRUE;
            int default_opacity = spOverlayTypesConfig->DefaultOpacity;
            bool is_user_controlled =
               spOverlayTypesConfig->IsUserControlled == TRUE;

            AddOverlayTypeDescriptor(
               spOverlayTypesConfig->DefaultRestoreAtStartup,
               spOverlayTypesConfig->OverlayDescGuid,
               (char *)spOverlayTypesConfig->DisplayName,
               spOverlayTypesConfig->BackingStoreType,
               pIconImage,
               pOverlayFactory,
               (char *)spOverlayTypesConfig->OverlayHelpURI,
               spOverlayTypesConfig->GetIs3dEnabled() == TRUE,
               (char *)spOverlayTypesConfig->HelpFilename,
               spOverlayTypesConfig->OverlayHelpId,
               (char *)spOverlayTypesConfig->EditorHelpURI,
               spOverlayTypesConfig->EditorHelpId,
               spOverlayTypesConfig->IsFileOverlay,
               defaultDirectory,
               defaultFileExtension,
               openFileDialogFilter,
               saveFileDialogFilter,
               pOverlayEditor,
               (char *)spOverlayTypesConfig->EditorDisplayName,
               pCustomInit,
               spFileOverlayTypeDesc,
               is_top_most,
               default_opacity,
               is_user_controlled, depf);
            lRet = spOverlayTypesConfig->MoveNext();
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg = (char *)e.Description();
      AfxMessageBox(msg);
   }

   IFvOverlayFactory* pOverlayFactory;
   IFvOverlayEditor* pOverlayEditor;
   CIconImage *pIconImage;

   pOverlayFactory = new OverlayFactory<C_contour_ovl>();
   pIconImage = CIconImage::load_images("ovlmgr\\contour.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_ContourLines,
      "Contour Lines", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Contour_Lines_Overlay.htm");

   pOverlayFactory = new OverlayFactory<C_TAMask_ovl>();
   pIconImage = CIconImage::load_images("ovlmgr\\TAMask.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_TerrainAvoidanceMask,
      "Terrain Avoidance Mask", BACKINGSTORE_TYPE_NONE,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\"
      "Terrain_Avoidance_Mask_Overlay.htm");

   pOverlayFactory = new OverlayFactory<GridOverlay>();
   pIconImage = CIconImage::load_images("ovlmgr\\grid.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_CoordinateGrid,
      "Coordinate Grid", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory, 
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\"
      "Coordinate_Grid_Overlay_start.htm");

   pOverlayFactory = new OverlayFactory<C_shp_ovl>();
   pIconImage = CIconImage::load_images("ovlmgr\\shp.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_ShapeFile,
      "Shape File", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Shape_Files_Overview.htm",
      true, "", 0, "", 0,
      TRUE, "FOLDERID_ReadWriteUserData\\Shape", ".shp",
      "Shape Files (*.shp)|*.shp|Shape Collection Files (*.shc)|*.shc|",
      SAVE_FILTER_DISABLED);

   pOverlayFactory = new C_map_server_ovl_factory();
   pIconImage = CIconImage::load_images("ovlmgr\\map_server.ico");
   AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_MapServer,
      "", BACKINGSTORE_TYPE_NONE,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\mdm\\Map_Data_Manager_Overview.htm");

   pOverlayFactory = new OverlayFactory<C_env_ovl>();
   pIconImage = CIconImage::load_images("ovlmgr\\env.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_Environmental,
      "Environmental", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Environmental_Overlay.htm");

   pOverlayFactory = new OverlayFactory<BoundariesOverlay>();
   pIconImage = CIconImage::load_images("ovlmgr\\boundaries_overlay.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_Boundaries,
      "Boundaries Overlay", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Boundaries_Overlay.htm");

   pOverlayFactory = new OverlayFactory<CScaleBarOverlay>();
   pIconImage = CIconImage::load_images("ovlmgr\\scalebar.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_ScaleBar,
      "Scale Bar", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Scale_Bar_Overlay.htm");

   // Note: the default order of the editor toolbar buttons is based on the
   // order that the overlay descriptors containing editors are added to the
   // factory list
   //

   // Note: the moving map overlay's draw currently makes call to the TAMask
   // overlay which end up trying to send a message to a dialog control (tamask
   // status dialog) in the main thread.  The main thread is waiting for all
   // overlay rendering threads to complete so FalconView becomes deadlocked.
   // We need to rethink how the TAMask overlay is updated.  It should not be
   // updated from the Moving Map overlay's draw.  This is why the moving map's
   // backing store is BACKINGSTORE_TYPE_24BPP.
   pOverlayFactory = new OverlayFactory<C_gps_trail>();
   pOverlayEditor = new C_gps_trail_editor();
   pIconImage = CIconImage::load_images("ovlmgr\\MovingMap.ico");
   AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_MovingMapTrail,
      "Moving Map Trail", BACKINGSTORE_TYPE_NONE,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_Display.htm",
      true, "", 0,
      "Help\\fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_Mode_Toolbar.htm", 0,
      TRUE, "FOLDERID_ReadWriteUserData\\Gps", ".gpb",
      "GPS Files (*.gpb, *.gps, *.gpx)|*.gpb;*.gps;*.gpx|",
      "GPS Files (*.gpb)|*.gpb|GPS Files NMEA Data (*.gps)|*.gps|"
      "GPS XML Files (*.gpx)|*.gpx|", pOverlayEditor, "&Moving Map Tool",
      nullptr, nullptr, false, 100, true, new GPSTrailDisplayElementProviderFactory);


#ifdef GOV_RELEASE
   // the ARTrack tool is for drawing refueling tracks for aircraft and has no
   // utility outside of DoD users.

   pOverlayFactory = new OverlayFactory<C_ar_edit>();
   pOverlayEditor = new C_ar_edit_ovl_editor();
   pIconImage = CIconImage::load_images("ovlmgr\\TrackOrbitEditor.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_TrackOrbit,
      "Track/Orbit", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\DrawingEditor.chm::\\trackorb\\Track_Orbit_Editor_Overview.htm"
      "Track_Orbit_Editor_Overview.htm", true, "", 0,
      "Help\\fvw.chm::\\DrawingEditor.chm::\\trackorb\\Track_Orbit_Editor_Overview.htm"
      "Track_Orbit_Editor_Overview.htm", 0,
      TRUE, "FOLDERID_ReadWriteUserData\\TrackOrbit", ".uar",
      "Track/Orbit Files (*.uar)|*.uar|User AR Tool (*.mdb)|*.mdb|",
      "Track/Orbit Files (*.uar)|*.uar|User AR Tool (*.mdb)|*.mdb|"
      "Track/Orbit 3.3.1 Files (*.uar)|*.uar|",
      pOverlayEditor, "Track/Orbit Editor");

#endif

   pOverlayFactory = new C_localpnt_ovl_factory();
   pOverlayEditor = new C_localpnt_ovl_editor();
   pIconImage = CIconImage::load_images("ovlmgr\\PointsEditor.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_Points, "Points",
      BACKINGSTORE_TYPE_NONE, pIconImage, pOverlayFactory,
      "pointseditor.chm::\\Points_Editor_topics\\Introduction.htm",
      true, "", 0,
      "pointseditor.chm::\\Points_Editor_topics\\Introduction.htm",
      0, TRUE, "FOLDERID_ReadWriteUserData\\Points", ".lps",
      "Point Files (*.lps)|*.lps|Point 4.2.1 Files (*.lpt)|*.lpt|"
      "Point 3.3.1 Files (*.lpt)|*.lpt|", "", pOverlayEditor, "&Points Editor",
      nullptr, nullptr, false, 100, true, new PointsDisplayElementProviderFactory);

   pOverlayFactory = new OverlayFactory<C_model_ovl>();
   pIconImage = CIconImage::load_images("ovlmgr\\TacticalModel.ico");

   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_TacticalModel, "Tactical Model File",
      BACKINGSTORE_TYPE_NONE, pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Model_Overview.htm",
      true, "", 0,
      "fvw.chm::\\PointsEditor.chm::\\Points_Editor_topics\\Introduction.htm",
      0, TRUE, "FOLDERID_ReadWriteUserData\\TacticalModel", ".xml",
      "CDB Files (*.xml)|*.xml|OpenFlight Files (*.flt)|*.flt|", 
      SAVE_FILTER_DISABLED, nullptr, "&Model Editor",
      nullptr, nullptr, false, 100, true, new ModelDisplayElementProviderFactory);

   pOverlayFactory = new OverlayFactory<CPrintToolOverlay>();
   pOverlayEditor = new CPrintToolOverlayEditor();
   pIconImage = CIconImage::load_images("ovlmgr\\ChartPrint.ico");
   AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_PageLayout,
      "Page Layout", BACKINGSTORE_TYPE_24BPP,
      pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Introduction.htm",
      true, "", 0,
      "Help\\fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Introduction.htm",
      0, TRUE,
      "FOLDERID_ReadWriteUserData\\Chart", ".cht", "Chart Print Files (*.cht)|"
      "*.cht|", "", pOverlayEditor, "&Page Layout Tool");

   pOverlayFactory = new OverlayFactory<CoverageOverlay>();
   pOverlayEditor = new CCoverageOverlayEditor();
   pIconImage = CIconImage::load_images("ovlmgr\\cover.ico");
   AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_MapDataManager,
      "Map Data Coverage", BACKINGSTORE_TYPE_NONE, pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\mdm\\Map_Data_Manager_Overview.htm",
      false, "", 0,
      "Help\\fvw.chm::\\fvw_core.chm::\\mdm\\Map_Data_Manager_Overview.htm", 0,
      FALSE, "", "", "", "", pOverlayEditor, "&Map Data Manager\tCtrl+M");

   // Scene manager overlay
   pOverlayFactory = new OverlayFactory<scene_mgr::SceneCoverageOverlay>();
   pOverlayEditor = new scene_mgr::CSceneCovOverlayEditor();
   pIconImage = CIconImage::load_images("ovlmgr\\SceneDataMgr.ico");
   AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_SceneManager,
      "Scene Data Coverage", BACKINGSTORE_TYPE_NONE, pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\sm\\Scene_Data_Manager_Overview.htm",
      true, "", 0,
      "Help\\fvw.chm::\\fvw_core.chm::\\sm\\Scene_Data_Manager_Overview.htm", 0,
      FALSE, "", "", "", "", pOverlayEditor, "&Scene Data Manager\tCtrl+M");

   if (Cmov_sym_overlay::is_viewer_registered())
   {
      pOverlayFactory = new OverlayFactory<Cmov_sym_overlay>();
      pOverlayEditor = new Cmov_sym_overlay_editor();
      pIconImage = CIconImage::load_images("ovlmgr\\SkyViewMode.ico");
      AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_SkyView,
         "SkyView", BACKINGSTORE_TYPE_24BPP,
         pIconImage, pOverlayFactory,
         "Help\\fvw.chm::\\fvw_core.chm::\\3dview\\SkyView_Overlay.htm",
         true, "", 0,
         "Help\\fvw.chm::\\fvw_core.chm::\\3dview\\SkyView_Overlay.htm", 0,
         TRUE,
         "FOLDERID_ReadWriteUserData\\SkyView", ".svw", "SkyView Files (*.svw)|"
         "*.svw|", "", pOverlayEditor, "SkyView Mode");
   }

#ifdef GOV_RELEASE
   if (PRM_get_registry_int("Main", "EnablePointExportTool", 0))
   {
      pOverlayFactory = new OverlayFactory<CPointExportOverlay>();
      pOverlayEditor = new CPointExportOverlayEditor();
      pIconImage = CIconImage::load_images("ovlmgr\\pntexprt.ico");
      AddOverlayTypeDescriptor(FALSE, FVWID_Overlay_PointExport,
         "Point Export", BACKINGSTORE_TYPE_24BPP,
         pIconImage, pOverlayFactory,
         "Help\\fvw.chm::\\fvw_core.chm::\\pntxpt\\Point_Export_Overview.htm",
         true, "", 0,
         "Help\\fvw.chm::\\fvw_core.chm::\\pntxpt\\Point_Export_Overview.htm",
         0, TRUE, "FOLDERID_ReadWriteUserData\\PointExport", ".pex",
         "Point Export Files (*.pex)|*.pex|", "", pOverlayEditor,
         "&Point Export Tool");
   }
#endif

   pOverlayFactory = new OverlayFactory<C_nitf_ovl>();
   pOverlayEditor = new C_nitf_ovl_editor();
   pIconImage = CIconImage::load_images("ovlmgr\\TacticalImageManager.ico");
   AddOverlayTypeDescriptor(TRUE, FVWID_Overlay_NitfFiles,
      "Tactical Imagery", BACKINGSTORE_TYPE_NONE, pIconImage, pOverlayFactory,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Imagery_Overlay.htm",
      true, "help\\fvw.chm", NITF_OVERLAY_HELP,
      "Help\\fvw.chm::\\fvw_core.chm::\\overlays\\"
      "Tactical_Imagery_Data_Manager_Toolbar.htm", IDR_NITF_TOOLBAR + 0x50000,
      FALSE, "", "", "", "", pOverlayEditor, "Tactical Imagery Data Ma&nager");

   InitializePlugIns();

   return SUCCESS;
}

// given the clsid of an object implementing IFvOverlayTypeRuntimeEnabledPtr, returns TRUE if the overlay type is
// enabled, FALSE otherwise
BOOL COverlayTypeDescriptorList::IsRuntimeEnabled(GUID runtimeEnabledClsid)
{
   if (runtimeEnabledClsid == GUID_NULL)
      return TRUE;

   try
   {
      // create instance of IFvOverlayRuntimeEnabled object from the given class id and call IsOverlayEnabled
      FalconViewOverlayLib::IFvOverlayTypeRuntimeEnabledPtr spOverlayTypeRuntimeEnabled; 
      CO_CREATE(spOverlayTypeRuntimeEnabled, runtimeEnabledClsid);

      return spOverlayTypeRuntimeEnabled->IsOverlayTypeEnabled();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("RuntimeIsOverlayEnabled failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return FALSE;
}

FalconViewOverlayLib::IFvFileOverlayTypeDescriptorPtr 
   COverlayTypeDescriptorList::GetFileOverlayTypeDescFromObject(
      GUID customFileOverlayTypeDescriptorClsid,
      CString& defaultDirectory, CString& defaultFileExtension, 
      CString& openFileDialogFilter, CString& saveFileDialogFilter)
{
   try
   {
      FalconViewOverlayLib::IFvFileOverlayTypeDescriptorPtr spFileOverlayTypeDesc;
      CO_CREATE(spFileOverlayTypeDesc, customFileOverlayTypeDescriptorClsid);

      defaultDirectory = (char *)spFileOverlayTypeDesc->GetDefaultDirectory();
      defaultFileExtension = (char *)spFileOverlayTypeDesc->GetDefaultFileExtension();
      openFileDialogFilter = (char *)spFileOverlayTypeDesc->GetFileDialogFilter(TRUE);
      saveFileDialogFilter = (char *)spFileOverlayTypeDesc->GetFileDialogFilter(FALSE);

      return spFileOverlayTypeDesc;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("GetFileOverlayTypeDescFromObject failed: %s", (char *)e.Description());
      WriteToLogFile(_bstr_t(msg));
   }

   return NULL;
}

void COverlayTypeDescriptorList::InitializePlugIns()
{
   if (!m_pOvlToolConfiguration->include_overlay_tool("PLUGIN_OVERLAY"))
      return;

   // for each of the client overlays listed in the registry
   CString class_ID_string;
   int interface_version;
   int isStaticOverlay;
   CString overlayHelpURI;
   CString helpFileName;
   long overlayHelpId;
   CString editorHelpURI;
   long editorHelpId;

   CString key = "Client Editors";

   // loop until we reach the maximum number of editors 
   int i = 0;
   do
   {
      CString num_str;
      num_str.Format("\\%d", i);

      // try to get the class id string from the registry for the current index
      class_ID_string =
         PRM_get_registry_string(key + num_str, "classIDString", "DNE");
      interface_version =
         PRM_get_registry_int(key + num_str, "InterfaceVersion", 1);
      isStaticOverlay =
         PRM_get_registry_int(key + num_str, "isStaticOverlay", 0);
      overlayHelpURI =
         PRM_get_registry_string(key + num_str, "overlayHelpURI", "");
      helpFileName =
         PRM_get_registry_string(key + num_str, "helpFileName", "help\\fvw.chm");
      overlayHelpId =
         PRM_get_registry_int(key + num_str, "overlayHelpId", PLUG_IN_OVERLAY_HELP);
      editorHelpURI =
         PRM_get_registry_string(key + num_str, "editorHelpURI", "");
      editorHelpId =
         PRM_get_registry_int(key + num_str, "editorHelpId", PLUG_IN_OVERLAY_HELP);

      // For example, "326ABA2A-B0C1-45D9-A9CD-FFDE6C386614"
      CString depf_guid_str = PRM_get_registry_string(key + num_str,
         "DisplayElementProviderFactoryGuid", "");

      // if the class id string exists...
      if (class_ID_string != "DNE")
      {
         // create a new CLayerOvlFactory
         CLayerOvlFactory *layer_factory = NULL;
         IFvOverlayEditor *pOverlayEditor = NULL;
         CString strEditorDisplayName;
         _bstr_t bstrDefaultDir, bstrDefaultExt,
            bstrOpenFileDialogFilter, bstrSaveFileDialogFilter;

         LayerEditorImpl *pLayerEditorImpl = NULL;
         int is_3d_enabled =
            PRM_get_registry_int(key + num_str, "is3dEnabled", TRUE);

         if (isStaticOverlay)
         {
            layer_factory =
               new CLayerOvlFactory(i, class_ID_string, interface_version);
            pLayerEditorImpl = layer_factory->get_ILayerEditor();
         }
         else
         {
            CLayerFileOvlFactory *layerFileOvlFactory =
               new CLayerFileOvlFactory(i, class_ID_string, interface_version);
            pLayerEditorImpl = layerFileOvlFactory->get_ILayerEditor();
            layer_factory = layerFileOvlFactory;

            if (pLayerEditorImpl != NULL && pLayerEditorImpl->interface_OK())
            {
               layerFileOvlFactory->GetDefaultDirectory(
                  bstrDefaultDir.GetAddress() );
               layerFileOvlFactory->GetDefaultFileExtension(
                  bstrDefaultExt.GetAddress() );
               layerFileOvlFactory->GetFileDialogFilter(
                  TRUE, bstrOpenFileDialogFilter.GetAddress() );
               layerFileOvlFactory->GetFileDialogFilter(
                  FALSE, bstrSaveFileDialogFilter.GetAddress() );

               // all plug-in file overlays will implement an editor
               CLayerOvlEditor *layerOvlEditor =
                  new CLayerOvlEditor(layer_factory->get_ILayerEditor() );
               strEditorDisplayName = layerOvlEditor->GetEditorDisplayName();
               pOverlayEditor = layerOvlEditor;
            }
         }

         // check to be sure the interface pointer was successfully created,
         // if not then we don't want to register this overlay factory
         if (pLayerEditorImpl == NULL || !pLayerEditorImpl->interface_OK())
         {
            delete pOverlayEditor;
            delete layer_factory;
         }
         else
         {
            CIconImage *pIconImage = CIconImage::load_images(
               layer_factory->GetIconFile());

            DisplayElementProviderFactory_Interface* depf = nullptr;
            if (!depf_guid_str.IsEmpty())
            {
               depf = new DisplayElementProviderFactoryCOM(
                  string_utils::CGuidStringConverter((LPCTSTR)depf_guid_str).
                     GetGuid());
            }

            AddOverlayTypeDescriptor(TRUE,
               layer_factory->GetOverlayDescriptorGuid(),
               layer_factory->GetDisplayName(),
               BACKINGSTORE_TYPE_NONE, pIconImage,
               layer_factory,
               overlayHelpURI, is_3d_enabled == TRUE, helpFileName, overlayHelpId,
               editorHelpURI, editorHelpId,
               !isStaticOverlay, (char *)bstrDefaultDir, (char *)bstrDefaultExt,
               (char *)bstrOpenFileDialogFilter,
               (char *)bstrSaveFileDialogFilter, pOverlayEditor,
               strEditorDisplayName, nullptr, nullptr, false, 100, true,
               depf);
         }
      }

      // increment counter
      i++;

   } while (i < MAX_EXTERNAL_EDITORS);
}

// Return the overlay descriptor with the given guid
OverlayTypeDescriptor* COverlayTypeDescriptorList::GetOverlayTypeDescriptor(GUID overlayDescGuid)
{
   for(size_t i = 0; i < m_overlayTypeDescriptors.size(); i++)
   {
      if (m_overlayTypeDescriptors[i]->overlayDescriptorGuid == overlayDescGuid)
         return m_overlayTypeDescriptors[i];
   }

   return NULL;
}

// Disables the given overlay type (removes it from the list of overlay type descriptors)
void COverlayTypeDescriptorList::DisableOverlayType(OverlayTypeDescriptor *pOverlayTypeDesc)
{
   std::vector<OverlayTypeDescriptor *>::iterator it = std::find(m_overlayTypeDescriptors.begin(),
      m_overlayTypeDescriptors.end(), pOverlayTypeDesc);

   if (it != m_overlayTypeDescriptors.end())
   {
      delete pOverlayTypeDesc->pOverlayEditor;
      delete pOverlayTypeDesc->pOverlayFactory;
      delete pOverlayTypeDesc->pCustomInitializer;
      delete pOverlayTypeDesc;
      m_overlayTypeDescriptors.erase(it);
   }
}

// methods to persist the default display order for an overlay type
void COverlayTypeDescriptorList::LoadDefaultDisplayOrder()
{
   for(size_t i = 0; i < m_overlayTypeDescriptors.size(); i++)
   {
      OverlayTypeDescriptor* pOverlayTypeDesc = m_overlayTypeDescriptors[i];
   
      string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);
      const int display_order = PRM_get_registry_int("Overlay Manager\\Display Order2", guidConv.GetGuidString().c_str(), -1);

      // If entry is missing, then state is invalid
      if (display_order == -1)
      {
         // clear out the Display Order2 registry key
         if (PRM_delete_registry_section("Overlay Manager\\Display Order2") != SUCCESS)
            ERR_report("PRM_delete_registry_section() failed.");

         // the default display order will be based on the order that overlay types are added to the type descriptor list
         for(size_t j = 0; j < m_overlayTypeDescriptors.size(); j++)
            m_overlayTypeDescriptors[j]->displayOrder = j;

         break;
      }

      // Assign display order from registry value.
      pOverlayTypeDesc->displayOrder = display_order;
   }

   // sort the factories in the list by their display order
   std::sort(m_overlayTypeDescriptors.begin(), m_overlayTypeDescriptors.end(), SortByDecreasingDisplayOrder);
}

void COverlayTypeDescriptorList::SaveDefaultDisplayOrder()
{
   for(size_t i = 0; i < m_overlayTypeDescriptors.size(); i++)
   {
      OverlayTypeDescriptor* pOverlayTypeDesc = m_overlayTypeDescriptors[i];
      string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);

      // write the display state of overlay i
      PRM_set_registry_int("Overlay Manager\\Display Order2", guidConv.GetGuidString().c_str(), pOverlayTypeDesc->displayOrder);
   }
}

void COverlayTypeDescriptorList::SetDisplayOrder(GUID overlayDescGuid, int displayOrder)
{
   OverlayTypeDescriptor* pOverlayTypeDesc = GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL)
   {
      pOverlayTypeDesc->displayOrder = displayOrder;

      // since the display order changed, resort the list of overlay types
      std::sort(m_overlayTypeDescriptors.begin(), m_overlayTypeDescriptors.end(), SortByDecreasingDisplayOrder);
   }
   else
      ERR_report("Unable to locate a overlay descriptor by its GUID.");
}

int COverlayTypeDescriptorList::GetDisplayOrder(GUID overlayDescGuid)
{
   OverlayTypeDescriptor* pOverlayTypeDesc = GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL)
      return pOverlayTypeDesc->displayOrder;

   ERR_report("Unable to locate a factory by its overlay descriptor GUID.");
   return -1;
}

// Returns true if the given type of overlay is enabled
bool COverlayTypeDescriptorList::IsOverlayEnabled(GUID overlayDescGuid)
{
   return GetOverlayTypeDescriptor(overlayDescGuid) != NULL;
}

// Returns true if the overlay type is a file overlay (a IFvFileOverlayTypeDescriptor was given for the overlay type)
bool COverlayTypeDescriptorList::IsFileOverlay(GUID overlayDescGuid)
{
   OverlayTypeDescriptor *pOverlayTypeDesc = GetOverlayTypeDescriptor(overlayDescGuid);
   return pOverlayTypeDesc != NULL && pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay;
}

// Returns true if the overlay type is a static overlay
bool COverlayTypeDescriptorList::IsStaticOverlay(GUID overlayDescGuid)
{
   OverlayTypeDescriptor *pOverlayTypeDesc = GetOverlayTypeDescriptor(overlayDescGuid);
   return pOverlayTypeDesc != NULL && !pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay;
}

// returns the number of overlay types that have editors
int COverlayTypeDescriptorList::GetNumEditors()
{
   int num_editors = 0;
   for (size_t i=0; i<m_overlayTypeDescriptors.size(); ++i)
   {
      if (m_overlayTypeDescriptors[i]->pOverlayEditor != NULL)
         num_editors++;
   }
   return num_editors;
}

// clean up any COverlayEditorCOMs that may have been created
void COverlayTypeDescriptorList::TerminateEditors()
{
   for (size_t i=0; i<m_overlayTypeDescriptors.size(); ++i)
   {
      COverlayEditorCOM *pEditor = dynamic_cast<COverlayEditorCOM *>(m_overlayTypeDescriptors[i]->pOverlayEditor);
      if (pEditor != NULL)
      {
         delete pEditor;
         m_overlayTypeDescriptors[i]->pOverlayEditor = NULL;
      }
   }
}

// cleanup list of overlay types
int COverlayTypeDescriptorList::Terminate()
{
   for(size_t i=0; i<m_overlayTypeDescriptors.size(); ++i)
   {
      delete m_overlayTypeDescriptors[i]->pOverlayEditor;
      delete m_overlayTypeDescriptors[i]->pOverlayFactory;
      delete m_overlayTypeDescriptors[i]->pCustomInitializer;
      delete m_overlayTypeDescriptors[i]->display_element_provider_factory;
      delete m_overlayTypeDescriptors[i];
   }

   return SUCCESS;
}

void COverlayTypeDescriptorList::AddOverlayTypeDescriptor(
   BOOL bDefaultRestoreAtStartup, const GUID& overlayDescriptorGuid,
   const CString& displayName, BackingStoreTypeEnum backingStoreType,
   CIconImage *pIconImage, IFvOverlayFactory* pOverlayFactory,
   const CString& overlayHelpURI, bool is_3d_enabled /* = true */,
   const CString& helpFileName,
   long overlayHelpId, const CString& editorHelpURI, long editorHelpId,
   BOOL bIsFileOverlay, CString defaultDirectory,
   CString defaultFileExtension, const CString& openFileDialogFilter,
   const CString& saveFileDialogFilter, IFvOverlayEditor* pOverlayEditor,
   const CString& strEditorDisplayName,
   CCustomInitializerCOM *pCustomInitializer,
   FalconViewOverlayLib::IFvFileOverlayTypeDescriptor* pFileOverlayTypeDesc,
   bool is_top_most, 
   int default_opacity, 
   bool is_user_controllable, DisplayElementProviderFactory_Interface* depf)
{
   // determine if the overlay type is disabled in the ovltoolconfig.txt file
   // (this is to support legacy configurations that use ovltoolconfig.txt to
   // disable an overlay rather than the enabled attribute)
   if (displayName.GetLength() > 0 && !m_pOvlToolConfiguration->include_overlay_tool(displayName))
   {
      delete pOverlayEditor;
      delete pOverlayFactory;
      delete depf;
      return;
   }

   if (pIconImage == NULL)
   {
      CString msg;
      msg.Format("Unable to load the icon for the %s overlay type", displayName);
      ERR_report(msg);
      delete pOverlayEditor;
      delete pOverlayFactory;
      delete depf;
      return;
   }

   OverlayTypeDescriptor *pOverlayType =  new OverlayTypeDescriptor();

   pOverlayType->overlayDescriptorGuid = overlayDescriptorGuid;
   pOverlayType->displayOrder = m_overlayTypeDescriptors.size();
   pOverlayType->bDefaultRestoreAtStartup = bDefaultRestoreAtStartup;
   pOverlayType->is_3d_enabled = is_3d_enabled;

   // top-most overlay
   pOverlayType->is_top_most = is_top_most;
   pOverlayType->default_opacity = default_opacity;
   pOverlayType->is_user_controllable = is_user_controllable;

   // if the display name contains a backslash then separate the 
   // given display name into the parent display name and display name
   int pos = displayName.Find('\\');
   if (pos != -1)
   {
      pOverlayType->parentDisplayName = displayName.Left(pos);
      pOverlayType->displayName = displayName.Mid(pos + 1);
   }
   else
      pOverlayType->displayName = displayName;
   
   pOverlayType->backingStoreType = backingStoreType;
   pOverlayType->pIconImage = pIconImage;
   pOverlayType->pOverlayFactory = pOverlayFactory;
   pOverlayType->display_element_provider_factory = depf;
   pOverlayType->pOverlayEditor = pOverlayEditor;
   pOverlayType->editorDisplayName = strEditorDisplayName;

   pOverlayType->fileTypeDescriptor.bIsFileOverlay = bIsFileOverlay;
   if (bIsFileOverlay)
   {
      // replace FOLDERID_PublicDocuments with the appropriate directory
      TCHAR commonDocumentsPath[MAX_PATH];
      ::SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, 0, commonDocumentsPath);
      defaultDirectory.Replace("FOLDERID_PublicDocuments", commonDocumentsPath);

      // replace FOLDERID_ReadWriteUserData with the appropriate directory
      CString rw_user_data = PRM_get_registry_string("Main", 
         "ReadWriteUserData", "");
      defaultDirectory.Replace("FOLDERID_ReadWriteUserData", rw_user_data);

      pOverlayType->fileTypeDescriptor.spFileOverlayTypeDesc = pFileOverlayTypeDesc;
      pOverlayType->fileTypeDescriptor.defaultDirectory = defaultDirectory;

      defaultFileExtension.TrimLeft('.');  // Remove any leading periods from the file extension. See Bug #1281.
      pOverlayType->fileTypeDescriptor.defaultFileExtension = defaultFileExtension;
      pOverlayType->fileTypeDescriptor.openFileDialogFilter = openFileDialogFilter;

      // The save file dialog filter is optional. If it is not explicitly set,
      // then the open file dialog will be used. As a work-around for this bad
      // design choice, we use disabled to indicate that the save filter should
      // remain empty. In this case, the user will not be able to save overlays
      // of this type.
      if (saveFileDialogFilter != SAVE_FILTER_DISABLED)
      {
         pOverlayType->fileTypeDescriptor.saveFileDialogFilter =
            saveFileDialogFilter.IsEmpty() ?
               openFileDialogFilter : saveFileDialogFilter;
      }
   }

   pOverlayType->contextSensitiveHelp.helpFileName = helpFileName;
   pOverlayType->contextSensitiveHelp.overlayHelpId = overlayHelpId;
   pOverlayType->contextSensitiveHelp.editorHelpId = editorHelpId;

   if (overlayHelpURI && overlayHelpURI.GetLength() > 0)
   {
      pOverlayType->contextSensitiveHelp.overlayHelpURI = overlayHelpURI;
   }

   if (overlayHelpURI && editorHelpURI.GetLength() > 0)
   {
      pOverlayType->contextSensitiveHelp.editorHelpURI = editorHelpURI;
   }

   pOverlayType->pCustomInitializer = pCustomInitializer;

   m_overlayTypeDescriptors.push_back(pOverlayType);
}

// Methods for enumerating overlay type descriptors
//

void COverlayTypeDescriptorList::ResetEnumerator()
{
   m_pCurrent = NULL;
   m_currentIt = m_overlayTypeDescriptors.begin();
}

bool COverlayTypeDescriptorList::MoveNext()
{
   if (m_currentIt == m_overlayTypeDescriptors.end())
      return false;

   m_pCurrent = *m_currentIt++;
   return true;
}
