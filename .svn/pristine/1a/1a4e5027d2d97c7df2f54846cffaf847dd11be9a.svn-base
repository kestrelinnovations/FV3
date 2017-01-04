#include "StdAfx.h"
#include "FvCustomDropTarget.h"
#include "..\FvCore\Include\GuidStringConverter.h"
#include "..\Common\ComErrorObject.h"
#include "ovl_mgr.h"
#include "err.h"

FvCustomDropTarget::FvCustomDropTarget(void)
{
   // register custom drop target format
   m_cfCustomDropTargetFormat = ::RegisterClipboardFormat(fv_custom_drop_format_name.c_str());
}


FvCustomDropTarget::~FvCustomDropTarget(void)
{
}

boolean_t FvCustomDropTarget::OnDragOver(COleDataObject* data_object)
{
   if ( data_object->IsDataAvailable(m_cfCustomDropTargetFormat) )
      return TRUE;

   return FALSE;
}
boolean_t FvCustomDropTarget::OnDrop(FalconViewOverlayLib::IFvMapView* pMapView, 
                                     COleDataObject* data_object)
{
   boolean_t drop_success = FALSE;

   try
   {
      // get the custom target data
      if ( data_object->IsDataAvailable(m_cfCustomDropTargetFormat) )
	   {
		   HGLOBAL hData = data_object->GetGlobalData( m_cfCustomDropTargetFormat );

         if ( hData == NULL )
         {
            return FALSE;
         }

		   LPSTR pData = (LPSTR)GlobalLock(hData);

		   FV_CUSTOM_DROP_TARGET custom_drop_target;
		   memcpy( &custom_drop_target, pData, sizeof(FV_CUSTOM_DROP_TARGET) );
         GlobalUnlock(hData);

         FalconViewOverlayLib::IFvMapViewPtr spMapView(pMapView);
         FalconViewOverlayLib::IFvOverlayManagerPtr pOverlayManager = spMapView->OverlayManager;
            
         std::string guid_str = custom_drop_target.uid;
         CString cookie_str = custom_drop_target.cookie;

         string_utils::CGuidStringConverter guidConv(guid_str);
         GUID guid = guidConv.GetGuid();

         // Move this to ocp function
         if (custom_drop_target.target_type == OVERLAY_DROP_TARGET)
         {               
            if (OVL_get_type_descriptor_list()->IsFileOverlay(guid))
            {
               // open the file overlay
               if ( cookie_str.GetLength() > 0 )
               {
                  pOverlayManager->OpenFileOverlay(guid, _bstr_t(cookie_str));

                  drop_success = TRUE;
               }
            }
            else if (OVL_get_type_descriptor_list()->IsStaticOverlay(guid))
            {
               if ( !pOverlayManager->SelectByOverlayDescGuid(guid) )
               {
                  pOverlayManager->ToggleStaticOverlay(guid);

                  drop_success = TRUE;
               }
            }
            else
            {
               CString msg;
               msg.Format("%s is not a recognized overlay type uid", guid);
               ERR_report(msg);            
            }
         }
         else if (custom_drop_target.target_type == CUSTOM_DROP_TARGET)
         {         
	         // obtain the classID from the class ID string
            CLSID clsid;
	         USES_CONVERSION;
	         if (CLSIDFromString((LPOLESTR) T2COLE(cookie_str),
		         &clsid) != S_OK)
	         {
		         CString msg;
		         msg.Format("Unable to get class id string = %s",
			         cookie_str);
		         ERR_report(msg);

               return FALSE;
	         }

            // create component and pass cookie
            FalconViewOverlayLib::IFvCustomDropTargetPtr drop_target;
	         CO_CREATE(drop_target, clsid);

            if (SUCCEEDED(drop_target->OnDrop(spMapView, guid)))
            {
               drop_success = TRUE;
            }
         }
         else
         {
            ERR_report("Invalid ");
            ASSERT(FALSE);
         }
      }
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("Error in  OnDrop: %s", (char *)e.Description());
      ERR_report(msg);
   }
   
   return drop_success;
}
