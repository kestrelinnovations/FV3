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



// ExifImage.cpp
#include "stdafx.h"
#include "ExifImage.h"

#include "geo_tool.h"

ExifImage::ExifImage(WCHAR *filename) :
   Gdiplus::Bitmap(filename)
{
   m_pPropertyItems = NULL;
   m_nNumProperties = 0;

   if (GetLastStatus() == Gdiplus::Ok)
   {
      // Find out how many property items are in the image, and find out the
      // required size of the buffer that will receive those property items.
      UINT totalBufferSize;
      GetPropertySize(&totalBufferSize, &m_nNumProperties);

      if (m_nNumProperties > 0)
      {
         // Allocate the buffer that will receive the property items.
         m_pPropertyItems = reinterpret_cast<Gdiplus::PropertyItem*>(new unsigned char[totalBufferSize]);

         // Fill the buffer.
         GetAllPropertyItems(totalBufferSize, m_nNumProperties, m_pPropertyItems);
      }
   }
}

ExifImage::~ExifImage()
{
   delete [] m_pPropertyItems;
}

// Date and time when the original image data was generated
BOOL ExifImage::GetDateTimeOrig(DATE& dt)
{
   if (m_pPropertyItems == NULL)
      return FALSE;

   for (UINT i=0; i<m_nNumProperties; i++)
   {
      if (m_pPropertyItems[i].id == PropertyTagExifDTOrig &&
         m_pPropertyItems[i].type == PropertyTagTypeASCII &&
         m_pPropertyItems[i].length == 20)
      {
         // format is "YYYY:MM:DD HH:MM:SS"
         //            0123456789012345678
         CString strDate(reinterpret_cast<char *>(m_pPropertyItems[i].value));

         const int nYear = atoi(strDate.Mid(0, 4));
         const int nMonth = atoi(strDate.Mid(5, 2));
         const int nDay = atoi(strDate.Mid(8, 2));
         const int nHour = atoi(strDate.Mid(11, 2));
         const int nMin = atoi(strDate.Mid(14, 2));
         const int nSec = atoi(strDate.Mid(17, 2));

         dt = COleDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);

         return TRUE;
      }
   }

   return FALSE;
}

BOOL ExifImage::GetGpsLocation(double& dLat, double& dLon)
{
   if (m_pPropertyItems == NULL)
      return FALSE;

   bool bLatitudeRefOk = false;
   bool bLatitudeOk = false;
   bool bLongitudeRefOk = false;
   bool bLongitudeOk = false;

   char latRef, lonRef;
   double lat, lon;

   for (UINT i=0; i<m_nNumProperties; i++)
   {
      if (m_pPropertyItems[i].id == PropertyTagGpsLatitudeRef &&
         m_pPropertyItems[i].type == PropertyTagTypeASCII &&
         m_pPropertyItems[i].length == 2)
      {
         bLatitudeRefOk = true;
         latRef = *((char *)m_pPropertyItems[i].value);
      }

      if (m_pPropertyItems[i].id == PropertyTagGpsLatitude &&
         m_pPropertyItems[i].type == PropertyTagTypeRational &&
         m_pPropertyItems[i].length == 24)
      {
         ULONG *pRationals = reinterpret_cast<ULONG *>(m_pPropertyItems[i].value);

         // check for divide by zero
         if (pRationals[1] != 0 && pRationals[3] != 0 && pRationals[5] != 0)
         {
            lat = 1.0*pRationals[0] / pRationals[1] + (1.0*pRationals[2] / pRationals[3])/60.0 + (1.0*pRationals[4] / pRationals[5])/3600.0;
            bLatitudeOk = true;
         }
      }

      if (m_pPropertyItems[i].id == PropertyTagGpsLongitudeRef &&
         m_pPropertyItems[i].type == PropertyTagTypeASCII &&
         m_pPropertyItems[i].length == 2)
      {
         bLongitudeRefOk = true;
         lonRef = *((char *)m_pPropertyItems[i].value);
      }

      if (m_pPropertyItems[i].id == PropertyTagGpsLongitude &&
         m_pPropertyItems[i].type == PropertyTagTypeRational &&
         m_pPropertyItems[i].length == 24)
      {
         ULONG *pRationals = reinterpret_cast<ULONG *>(m_pPropertyItems[i].value);

         // check for divide by zero
         if (pRationals[1] != 0 && pRationals[3] != 0 && pRationals[5] != 0)
         {
            lon = 1.0*pRationals[0] / pRationals[1] + (1.0*pRationals[2] / pRationals[3])/60.0 + (1.0*pRationals[4] / pRationals[5])/3600.0;
            bLongitudeOk = true;
         }
      }
   }

   if (bLatitudeRefOk && bLatitudeOk && bLongitudeRefOk && bLongitudeOk)
   {
      lat *= (latRef == 'N' || latRef == 'n') ? 1.0 : -1.0;
      lon *= (lonRef == 'E' || lonRef == 'e') ? 1.0 : -1.0;

      if (GEO_valid_degrees(lat, lon))
      {
         dLat = lat;
         dLon = lon;
         return TRUE;
      }
   }

   return FALSE;
}

BOOL ExifImage::GetImageDescription(CString& strDescription)
{
   if (m_pPropertyItems == NULL)
      return FALSE;

   for (UINT i=0; i<m_nNumProperties; i++)
   {
      if (m_pPropertyItems[i].id == PropertyTagImageDescription &&
         m_pPropertyItems[i].type == PropertyTagTypeASCII)
      {
         strDescription = reinterpret_cast<char *>(m_pPropertyItems[i].value);
         return TRUE;
      }
   }

   return FALSE;
}

BOOL ExifImage::SetGpsLocation(double dLat, double dLon)
{
   Gdiplus::PropertyItem propertyItem;

   CHAR north[] = "N";
   CHAR south[] = "S";
   CHAR east[] = "E";
   CHAR west[] = "W";
   ULONG latMinDegSec[6];
   ULONG lonMinDegSec[6];

   // write out the two properties for the latitude
   ConvertToRational(dLat, latMinDegSec);

   propertyItem.id = PropertyTagGpsLatitudeRef;
   propertyItem.type = PropertyTagTypeASCII;
   propertyItem.length = 2;   //  includes null terminator
   propertyItem.value = (dLat >= 0.0) ? north : south;
   SetPropertyItem(&propertyItem);

   propertyItem.id = PropertyTagGpsLatitude;
   propertyItem.type = PropertyTagTypeRational;
   propertyItem.length = 24;
   propertyItem.value = latMinDegSec;
   SetPropertyItem(&propertyItem);

   // write out the two properties for the longitude
   ConvertToRational(dLon, lonMinDegSec);

   propertyItem.id = PropertyTagGpsLongitudeRef;
   propertyItem.type = PropertyTagTypeASCII;
   propertyItem.length = 2;   //  includes null terminator
   propertyItem.value = (dLon >= 0.0) ? east : west;
   SetPropertyItem(&propertyItem);

   propertyItem.id = PropertyTagGpsLongitude;
   propertyItem.type = PropertyTagTypeRational;
   propertyItem.length = 24;
   propertyItem.value = lonMinDegSec;
   SetPropertyItem(&propertyItem);

   return TRUE;
}

BOOL ExifImage::SetImageDescription(const CString& strImageDescription)
{
   Gdiplus::PropertyItem propertyItem;
   propertyItem.id = PropertyTagImageDescription;
   propertyItem.type = 	PropertyTagTypeASCII;
   propertyItem.length = strImageDescription.GetLength() + 1; // length includes null terminator
   propertyItem.value = reinterpret_cast<void *>(const_cast<char *>((LPCTSTR)strImageDescription));
   Gdiplus::Status status = SetPropertyItem(&propertyItem);

   return status == Gdiplus::Ok ? TRUE : FALSE;
}

void ExifImage::ConvertToRational(double value, ULONG rational[6])
{
   // Note: PropertyTagTypeRational consists of a pair of unsigned longs.  The first number in the pair
   // being the numerator, the second the denominator.  The format for latitude and longitude is dd/1, 
   // mmmm/100, 0/1 (a total of 6 unsigned longs).  Note that this format does not allow for an exact location.

   value = fabs(value);
   rational[0] = (ULONG)value;
   rational[1] = 1;
   rational[2] = (ULONG)((value - rational[0])*60.0*100.0);
   rational[3] = 100;
   rational[4] = 0;
   rational[5] = 1;
}
