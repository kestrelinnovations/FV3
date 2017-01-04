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

// TiledElevationDataset.cpp
#include "stdafx.h"
#include "TiledElevationDataset.h"

TiledElevationDataset::TiledElevationDataset(const CString& filename,
   double geocell_width, double geocell_height, const d_geo_t& geocell_ul) :
m_geocell_ul(geocell_ul), m_default_value(0.0f)
{
   try
   {
      // Create an IWICImagingFactory object to create WIC objects
      IWICImagingFactoryPtr wic_factory;
      wic_factory.CreateInstance(CLSID_WICImagingFactory);

      // Create an IWICBitmapDecoder from an image file
      IWICBitmapDecoderPtr decoder;
      wic_factory->CreateDecoderFromFilename(_bstr_t(filename),
         NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);

      // Get the first frame of the image
      IWICBitmapFrameDecodePtr decoder_frame;
      decoder->GetFrame(0, &decoder_frame);
      wic_factory->CreateBitmapFromSource(decoder_frame,
         WICBitmapCacheOnDemand, &m_bitmap);

      // Obtain a lock, for reading, of the entire frame
      decoder_frame->GetSize(&m_width, &m_height);
      WICRect rc_lock = { 0, 0, m_width, m_height };
      m_bitmap->Lock(&rc_lock, WICBitmapLockRead, &m_lock);

      // Get the elevation data pointer
      UINT buffer_size = 0;
      BYTE* pv = nullptr;
      m_lock->GetDataPointer(&buffer_size, &pv);
      m_elevations = reinterpret_cast<float *>(pv);

      // Compute the XUnitLOD and YUnitLOD for use in GetElevation
      m_xunit_lod = geocell_width / m_width;
      m_yunit_lod = geocell_height / m_height;
   }
   catch (_com_error& )
   {
      // In the case of error, the default elevation value will be
      // used (read from Metadata/Defaults.xml). If that value does not
      // exist, we will use 0.0f instead
      m_default_value = 0.0f;
   }
}
