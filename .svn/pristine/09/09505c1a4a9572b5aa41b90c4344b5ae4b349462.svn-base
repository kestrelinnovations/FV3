#pragma once


#include "common.h"


const std::string fv_custom_drop_format_name = "FvCustomDropTarget";

//const long OVERLAY_TARGET = 1;
//const long CUSTOM_TARGET = 2;
typedef enum 
{ OVERLAY_DROP_TARGET = 1, 
  CUSTOM_DROP_TARGET
} EnumCustomDropTargetType;


typedef struct 
{
   // OVERLAY_DROP_TARGET, CUSTOM_DROP_TARGET
   EnumCustomDropTargetType target_type;

   // For overlay type, the OverlayType's uid,
   // for custom type, an application specific uid
   char uid[256];

   // For overlay type, the file overlay's file name to open,
   // for custom type, the IFvCustomDropTarget CLSID
   char cookie[256];

} FV_CUSTOM_DROP_TARGET;



class FvCustomDropTarget
{
public:
   FvCustomDropTarget(void);
   ~FvCustomDropTarget(void);

   boolean_t OnDrop(FalconViewOverlayLib::IFvMapView* pMapView, 
                           COleDataObject* data_object);
   boolean_t OnDragOver(COleDataObject* data_object);

private:
   UINT m_cfCustomDropTargetFormat;
};

