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



#ifndef FACTLIST_H
#define FACTLIST_H

#include "common.h"
#include <vector>

//
// forward declarations
//
class CIconImage;
class C_overlay_factory;
class C_overlay;
class COvlToolConfiguration;
class CCustomInitializerCOM;

enum BackingStoreTypeEnum;

interface DisplayElementProviderFactory_Interface;
interface IFvOverlayEditor;
interface IFvOverlayFactory;

struct OverlayTypeDescriptor;

// external plug-in overlay GUIDs that are still referenced within FalconView
extern const GUID FVWID_Overlay_Aimpoints;
extern const GUID FVWID_Overlay_Dropzones;
extern const GUID FVWID_Overlay_Route;
extern const GUID FVWID_Overlay_Shadow;
extern const GUID FVWID_Overlay_Navaids;
extern const GUID FVWID_Overlay_Heliports;
extern const GUID FVWID_Overlay_Airports;
extern const GUID FVWID_Overlay_Airways;
extern const GUID FVWID_Overlay_RefuelingRoutes;
extern const GUID FVWID_Overlay_MilitaryTrainingRoutes;
extern const GUID FVWID_Overlay_Waypoints;
extern const GUID FVWID_Overlay_ParachuteJump;
extern const GUID FVWID_Overlay_SuasBoundaries;
extern const GUID FVWID_Overlay_AirspaceBoundaries;
extern const GUID FVWID_Overlay_Threat;
extern const GUID FVWID_Overlay_DigitalNauticalChart;
extern const GUID FVWID_Overlay_VectorSmartMap;
extern const GUID FVWID_Overlay_WorldVectorShoreline;
extern const GUID FVWID_Overlay_Points;
extern const GUID FVWID_Overlay_ManualChum;
extern const GUID FVWID_Overlay_ElectronicChum;
extern const GUID FVWID_Overlay_Drawing;
extern const GUID FVWID_Overlay_TacticalGraphics;
extern const GUID FVWID_Overlay_Illumination;

namespace overlay_type_util
{
   // This function compares the extension of the file being opened (ext) to
   // the default filter strings for each existing overlay factory.  When the
   // extension is found in a default filter, the corresponding factory is
   // passed back to be called in the opening of the file.
   OverlayTypeDescriptor *GetOverlayTypeFromFileExtension(const char *ext);

   // get the directory where files for this overlay were last opened/saved
   CString get_most_recent_directory(GUID overlayDescGuid);

   // set the directory where files for this overlay were last opened/saved
   void set_most_recent_directory(GUID overlayDescGuid, CString pathname);

   // Returns true if the given overlay should be handled as tiled, draped
   // overlay
   bool HandleAsDrapedOverlay(C_overlay* overlay);
};

// Provides information about the characteristics for an overlay, such as its
// guid, editor, and help components.
struct OverlayTypeDescriptor
{
   // GUID that uniquely defines this type of overlay
   GUID overlayDescriptorGuid;

   // The display order used to determine where a newly created overlay of this
   // type should be placed relative to other opened overlays.  The default
   // display order is based on the order that overlay type descriptors are
   // added.
   int displayOrder;

   // Whether this overlay is permitted to be rendered in 3d using legacy
   // handling.  If the overlay exposes 3d specific interfaces, this parameter
   // is ignored.
   bool is_3d_enabled;

   // This overlay is a top most overlay meaning that it will be rendered last, 
   // on top of all other overlays.
   bool is_top_most;

   // The opacity value for aplha blending the top most overlay with the map.
   // This is ignored for regular most overlays.
   int default_opacity;

   // TODO Move to IFvOverlayFactory *pOverlayFactory
   // Allows the user to close the overlay from the user interface.
   bool is_user_controllable;

   // TRUE if overlays of this type should be restored at startup.  This value
   // is only used if the restore at startup flag for this overlay type is not
   // specified in the registry key Restore2.
   BOOL bDefaultRestoreAtStartup;

   // User friendly name for this overlay type used throughout the user
   // interface.
   // Note: the overlay's display name can be the empty string.  In this case,
   // the overlay type will not show up in various overlay lists throughout
   // FalconView.  For example, in the overlay manager dialog, the startup
   // options dialog, the display order dialog, and the overlay menu.
   CString displayName;

   // User friendly name for the parent display name. Can be the empty string.
   // The overlay menu, for one, uses this to present a hierarchical menu to
   // the user for a collection of overlays with the same parentDisplayName.
   CString parentDisplayName;

   // The type of backing store that this type of overlay will use to render
   // itself to the map.
   BackingStoreTypeEnum backingStoreType;

   // Icon associated with this overlay type used throughout the user interface.
   CIconImage *pIconImage;

   // used to create a new overlay instance
   IFvOverlayFactory *pOverlayFactory;

   // used to create new display element provider instance
   DisplayElementProviderFactory_Interface* display_element_provider_factory;

   IFvOverlayEditor *pOverlayEditor;

   // The name of the editor (e.g. "Drawing Editor" or "Moving Map Tool") used:
   //    i) in the Tools menu.
   //    ii) used to construct the status bar help string when the cursor
   //        is over the entry in the Tools menu.
   //    iii) for the tooltip when the cursor is over the editor's button in the
   //         editor toogle toolbar.
   CString editorDisplayName;

   // The following substructure provides information about the file specific
   // characteristics of an overlay type
   struct FILE_TYPE_DESCRIPTOR
   {
      // TRUE if this overlay type is a file overlay.  FALSE, otherwise
      BOOL bIsFileOverlay;

      // The default directory where files for this overlay type are stored
      CString defaultDirectory;

      // The default file extension of files for this overlay type. Does not
      // contain a leading '.' character.
      CString defaultFileExtension;

      // Filters used in the common file dialog boxes (open/save).  The filters
      // are one or more pairs of '|' separated filter strings.  The first
      // string in each pair is a display string that describes the filter, and
      // the second string specifies the filter pattern (for example, "*.rte").
      CString openFileDialogFilter;
      CString saveFileDialogFilter;

      // used to retrieve the default directory whenever the file open dialog is
      // opened
      FalconViewOverlayLib::IFvFileOverlayTypeDescriptorPtr
         spFileOverlayTypeDesc;

   } fileTypeDescriptor;

   // The following substructure contains fields to enable context sensitive
   // help for an overlay type.
   struct CONTEXT_SENSITIVE_HELP
   {
      // help file name for context sensitive help.  Can be relative to HD_DATA.
      CString helpFileName;

      // help id for overlay specific help (e.g., overlay menu)
      long overlayHelpId;

      // help id for overlay editor specific help (e.g., Tools menu).  Can be
      // zero if not editor is implemented.
      long editorHelpId;

      // help file name for context sensitive help.  Can be relative to HD_DATA.
      CString overlayHelpURI;
      CString editorHelpURI;

   } contextSensitiveHelp;

   CCustomInitializerCOM *pCustomInitializer; 
};

// maintains the list of overlay type descriptors
class COverlayTypeDescriptorList
{
   COvlToolConfiguration* m_pOvlToolConfiguration;
   std::vector<OverlayTypeDescriptor *> m_overlayTypeDescriptors;
   std::vector<OverlayTypeDescriptor *>::iterator m_currentIt;

public:
   COverlayTypeDescriptorList();
   ~COverlayTypeDescriptorList();

   // initialize the list of overlay type descriptors
   int Initialize();

   // Return the overlay descriptor with the given guid
   OverlayTypeDescriptor* GetOverlayTypeDescriptor(GUID overlayDescGuid);

   // Disables the given overlay type (removes it from the list of overlay type
   // descriptors).
   void DisableOverlayType(OverlayTypeDescriptor *pOverlayTypeDesc);

   // methods to persist the default display order for an overlay type.
   void LoadDefaultDisplayOrder();
   void SaveDefaultDisplayOrder();

   // methods to set / get the default display order for an overlay type.
   void SetDisplayOrder(GUID overlayDescGuid, int displayOrder);
   int GetDisplayOrder(GUID overlayDescGuid);

   // Returns true if the given type of overlay is enabled.
   bool IsOverlayEnabled(GUID overlayDescGuid);

   // Returns true if the overlay type is a file overlay.
   bool IsFileOverlay(GUID overlayDescGuid);

   // Returns true if the overlay type is a static overlay.
   bool IsStaticOverlay(GUID overlayDescGuid);

   // returns the number of overlay types that have editors.
   int GetNumEditors();

   // clean up any editor toolbars that may have been created.
   void TerminateEditors();

   // cleanup list of overlay types.
   int Terminate();

   // given the clsid of an object implementing IFvOverlayTypeRuntimeEnabledPtr,
   // returns TRUE if the overlay type is enabled, FALSE otherwise.
   static BOOL IsRuntimeEnabled(GUID runtimeEnabledClsid);

   // Methods for enumerating overlay type descriptors
public:
   void ResetEnumerator();
   bool MoveNext();
   OverlayTypeDescriptor* m_pCurrent; 

protected:
   FalconViewOverlayLib::IFvFileOverlayTypeDescriptorPtr
      GetFileOverlayTypeDescFromObject(
      GUID customFileOverlayTypeDescriptorClsid,
      CString& defaultDirectory,
      CString& defaultFileExtension,
      CString& openFileDialogFilter,
      CString& saveFileDialogFilter);

   // initialize the plug-in overlay types
   void InitializePlugIns();

   void AddOverlayTypeDescriptor(BOOL bDefaultRestoreAtStartup,
      const GUID& overlayDescriptorGuid, const CString& displayName,
      BackingStoreTypeEnum backingStoreType, CIconImage *pIconImage,
      IFvOverlayFactory* pOverlayFactory, const CString& overlayHelpURI,
      bool is_3d_enabled = true,
      const CString& helpFileName = "", long overlayHelpId = 0,
      const CString& editorHelpURI = "", long editorHelpId = 0,
      BOOL bIsFileOverlay = FALSE, CString defaultDirectory = "",
      CString defaultFileExtension = "",
      const CString& openFileDialogFilter = "",
      const CString& saveFileDialogFilter = "",
      IFvOverlayEditor* pOverlayEditor = NULL,
      const CString& strEditorDisplayName = "",
      CCustomInitializerCOM *pCustomInitializer = NULL,
      FalconViewOverlayLib::IFvFileOverlayTypeDescriptor*
      pFileOverlayTypeDesc = NULL,
      bool is_top_most = false,
      int default_opacity = 100,
      bool is_user_controllable = true,
      DisplayElementProviderFactory_Interface* depf = nullptr);
};

#endif
