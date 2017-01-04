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



#include "stdafx.h"
#include "Common\ComErrorObject.h"
#include "OverlayBackingStore.h"
#include "overlay.h"
#include "mapx.h"
#include "..\getobjpr.h"
#include <gdiplus.h>

const UINT TRANSPARENT_COLOR_KEY = RGB(1, 23, 4);

COverlayBackingStore::COverlayBackingStore(C_overlay* pParentOverlay, BackingStoreTypeEnum backingStoreType) :
   m_pParentOverlay(pParentOverlay),
   m_bIsValid(FALSE),
   m_bmpBackingStore(NULL),
   m_bmpOldBitmap(NULL),
   m_backingStoreType(backingStoreType)
{
}

COverlayBackingStore::~COverlayBackingStore()
{
   // if the memory DC is initialized, then deselect the backing store bitmap from it before it gets deleted
   if (m_memoryDC.m_hDC)
   {
      m_memoryDC.SelectObject(m_bmpOldBitmap);

      if (m_bmpBackingStore != NULL)
         ::DeleteObject(m_bmpBackingStore);
   }
}

int COverlayBackingStore::OnDraw(ActiveMap_TMP* pMap, int opacity)
{
   Reinit(pMap);

   // get the surface width and height from the map
   int nSurfaceWidth, nSurfaceHeight;
   pMap->get_surface_size(&nSurfaceWidth, &nSurfaceHeight);

   // if the map spec is different or the overlay has been invalidated we need to redraw from scratch
   if (!m_bIsValid)
      DrawToBackingStore(pMap, nSurfaceWidth, nSurfaceHeight, opacity);

   // transparent Blt memory DC to screen
   if (m_backingStoreType == BACKINGSTORE_TYPE_24BPP)
      pMap->get_CDC()->TransparentBlt(0, 0, nSurfaceWidth, nSurfaceHeight, &m_memoryDC, 0, 0, nSurfaceWidth, nSurfaceHeight, TRANSPARENT_COLOR_KEY);

   else if (m_backingStoreType == BACKINGSTORE_TYPE_32BPP)
   {
      BLENDFUNCTION blendFunc;
      blendFunc.BlendOp = AC_SRC_OVER;
      blendFunc.BlendFlags = 0;
      blendFunc.SourceConstantAlpha = 255; // use per-pixel alpha values
      blendFunc.AlphaFormat = AC_SRC_ALPHA;
      pMap->get_CDC()->AlphaBlend(0, 0, nSurfaceWidth, nSurfaceHeight, &m_memoryDC, 0, 0, nSurfaceWidth, nSurfaceHeight, blendFunc);
   }

   return SUCCESS;
}

int COverlayBackingStore::Reinit(ActiveMap_TMP* pMap)
{
   // is the memory DC initialized?  If not, then initialize it
   if (!m_memoryDC.m_hDC)
      m_memoryDC.CreateCompatibleDC(pMap->get_CDC());

   // get the surface width and height from the map
   int nSurfaceWidth, nSurfaceHeight;
   pMap->get_surface_size(&nSurfaceWidth, &nSurfaceHeight);
   const CSize surfaceSize = CSize(nSurfaceWidth, nSurfaceHeight);

   // if backing store hasn't been created yet or the size of the map has changed then we need to re-create bitmap
   if (m_bmpBackingStore == NULL || surfaceSize != m_bitmapDimension)
      ReinitBackingStore(pMap, nSurfaceWidth, nSurfaceHeight);

   return SUCCESS;
}

// reinitialize the bitmap that represents the overlay's backing store
int COverlayBackingStore::ReinitBackingStore(ActiveMap_TMP* pMap, int nSurfaceWidth, int nSurfaceHeight)
{
   // if the backing store has already been created
   if (m_bmpBackingStore != NULL)
   {
      // unselect the backing store from the memory DC
      m_memoryDC.SelectObject(m_bmpOldBitmap);

      // delete the backing store
      ::DeleteObject(m_bmpBackingStore);
   }

   // create a new backing store with the map's surface width and height
   m_bitmapDimension = CSize(nSurfaceWidth, nSurfaceHeight);
   if (nSurfaceWidth % 4 != 0)
      nSurfaceWidth = 4*(nSurfaceWidth/4 + 1);

   m_pBits = NULL;
   BITMAPINFOHEADER bih = {0};
   bih.biSize = sizeof(BITMAPINFOHEADER);
   bih.biBitCount = m_backingStoreType == BACKINGSTORE_TYPE_24BPP ? 24 : 32;
   bih.biCompression = BI_RGB;
   bih.biPlanes = 1;
   bih.biWidth = nSurfaceWidth;
   bih.biHeight = nSurfaceHeight;
   bih.biSizeImage = nSurfaceWidth * nSurfaceHeight * (bih.biBitCount / 8);
   bih.biXPelsPerMeter = 0;
   bih.biYPelsPerMeter = 0;
   bih.biClrUsed = 0;
   bih.biClrImportant = 0;

   m_bmpBackingStore = ::CreateDIBSection(pMap->get_CDC()->GetSafeHdc(), (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0x0);

   // select the backing store into the device context
   m_bmpOldBitmap = (HBITMAP)m_memoryDC.SelectObject(m_bmpBackingStore);

   // the backing store is now invalid and should be redrawn from scratch
   m_bIsValid = FALSE;

   return SUCCESS;
}

int COverlayBackingStore::DrawToBackingStore(ActiveMap_TMP* pMap, int nSurfaceWidth, int nSurfaceHeight, int opacity)
{
   // fill the backing store with the transparent color
   if (m_backingStoreType == BACKINGSTORE_TYPE_24BPP)
      m_memoryDC.FillSolidRect(0, 0, nSurfaceWidth, nSurfaceHeight, TRANSPARENT_COLOR_KEY);
   else if (m_backingStoreType == BACKINGSTORE_TYPE_32BPP && m_pBits != NULL)
   {
      if (nSurfaceWidth % 4 != 0)
         nSurfaceWidth = 4*(nSurfaceWidth/4 + 1);
      memset(m_pBits, 255, nSurfaceWidth * nSurfaceHeight * 4);
   }

   // manipulate the ActiveMap so that the overlay draws to the backing store rather then to the map
 IGraphicsContextPtr GC;
   HRESULT hr = pMap->m_interface_ptr->GetGraphicsContext(&GC);
   if (FAILED(hr))
   {
      // Unable to get graphics context
      return FAILURE;
   }

   IGraphicsContext4Ptr GC4 = GC;
   if (GC4 == nullptr)
   {
      // Unable to get graphics context
      return FAILURE;
   }

   // destructor will reset the GC
   CSetGraphicsContext setGC(GC4, 
      reinterpret_cast<long>(m_memoryDC.GetSafeHdc()), 
      reinterpret_cast<long>(m_memoryDC.GetSafeHdc()));

   // draw the overlay to the backing store
   m_pParentOverlay->draw(pMap);

   // once the overlay renders, premultiply the alpha value (opacity)
   if (m_backingStoreType == BACKINGSTORE_TYPE_32BPP && m_pBits != NULL)
   {
      for (int y=0; y<nSurfaceHeight; y++)
      {
         BYTE * pixel = (BYTE *) m_pBits + nSurfaceWidth * 4 * y;

         for (int x=0; x<nSurfaceWidth; x++)
         {
            if (pixel[3] == 255 )
               pixel[3] = 0;
            else if (pixel[3] == 0 )
               pixel[3] = opacity * 255 / 100;
            
            // premultiply by alpha;
            pixel[0] = pixel[0] * pixel[3] / 255;
            pixel[1] = pixel[1] * pixel[3] / 255;
            pixel[2] = pixel[2] * pixel[3] / 255;

            pixel += 4;
         }
      }
   }


   // mark the backing store as valid
   m_bIsValid = TRUE;
   
   return SUCCESS;
}