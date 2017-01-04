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



// OverlayBackingStore.h : header file
//
#pragma once

class C_overlay;
class ActiveMap_TMP;

#include "maps_d.h"     // for MapSpec
#include "overlay.h"    // for BackingStoreTypeEnum

class CSetGraphicsContext 
{
public:
   CSetGraphicsContext(IGraphicsContext4Ptr& gc, long hdc, long attribDC)
      : m_gc(gc)
   { 
      m_gc->SetDC(hdc, attribDC, &m_previousHdc, &m_previousAttribDC);
   }
       
   virtual ~CSetGraphicsContext() 
   { 
      m_gc->SetDC(m_previousHdc, m_previousAttribDC, &m_previousHdc, &m_previousAttribDC);
   }

protected:
   IGraphicsContext4Ptr& m_gc;
   long m_previousHdc;
   long m_previousAttribDC;
};

class COverlayBackingStore
{
   CDC m_memoryDC;

   BackingStoreTypeEnum m_backingStoreType;
   BYTE *m_pBits;
   HBITMAP m_bmpBackingStore;
   HBITMAP m_bmpOldBitmap;
   CSize m_bitmapDimension;

   BOOL m_bIsValid;
   C_overlay *m_pParentOverlay;

public:
   COverlayBackingStore(C_overlay* pParentOverlay, BackingStoreTypeEnum backingStoreType);
   ~COverlayBackingStore();

   int Reinit(ActiveMap_TMP* pMap);
   int OnDraw(ActiveMap_TMP* pMap, int opacity = 100); // default fully opaque

   int DrawToBackingStore(IMapRenderingEnginePtr& spMapRenderingEngine);

   void SetInvalid()
   {
      m_bIsValid = FALSE;
   }

   BOOL IsValid()
   {  
      return m_bIsValid;
   }

protected:
   // reinitialize the bitmap that represents the overlay's backing store
   int ReinitBackingStore(ActiveMap_TMP* pMap, int nSurfaceWidth, int nSurfaceHeight);
   int DrawToBackingStore(ActiveMap_TMP* pMap, int nSurfaceWidth, int nSurfaceHeight, int opacity);
};
