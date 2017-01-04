// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// TiledElevationDataset.h

_COM_SMARTPTR_TYPEDEF(IWICImagingFactory, __uuidof(IWICImagingFactory));
_COM_SMARTPTR_TYPEDEF(IWICBitmapDecoder, __uuidof(IWICBitmapDecoder));
_COM_SMARTPTR_TYPEDEF(IWICBitmapFrameDecode, __uuidof(IWICBitmapFrameDecode));
_COM_SMARTPTR_TYPEDEF(IWICBitmap, __uuidof(IWICBitmap));
_COM_SMARTPTR_TYPEDEF(IWICBitmapLock, __uuidof(IWICBitmapLock));

#include "geo_tool_d.h"

// Class to read directly from a Tiled Elevation Dataset
class TiledElevationDataset
{
public:
   TiledElevationDataset(const CString& filename, double geocell_width,
      double geocell_height, const d_geo_t& geocell_ul);

   inline float GetElevationMeters(double lat, double lon) const
   {
      if (m_lock != nullptr)
      {
         UINT i = static_cast<UINT>((lon - m_geocell_ul.lon) / m_xunit_lod);
         UINT j = static_cast<UINT>((m_geocell_ul.lat - lat) / m_yunit_lod);
         if (i < m_width && j < m_height)
         {
            return m_elevations[j * m_width + i];
         }
      }

      return m_default_value;
   }

private:
   UINT m_width, m_height;
   double m_xunit_lod, m_yunit_lod;
   d_geo_t m_geocell_ul;
   float m_default_value;
   IWICBitmapPtr m_bitmap;
   IWICBitmapLockPtr m_lock;
   float* m_elevations;
};