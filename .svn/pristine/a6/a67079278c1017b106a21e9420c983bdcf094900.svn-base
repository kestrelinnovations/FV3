// Copyright (c) 2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// ShpFile.cpp

#include "stdafx.h"
#include <assert.h>
#include "ShapeFil.h"


////////////////////////////////////////////////////////////////////////////////
// SHPObjectPtr
////////////////////////////////////////////////////////////////////////////////

SHPObjectPtr::SHPObjectPtr( SHPObject* pShapeObj )
{
   if ( nullptr != ( m_pShapeObj = pShapeObj ) )
      (m_pShapeObj->m_iRefCount)++;
}


SHPObjectPtr::SHPObjectPtr( const SHPObjectPtr& spShapeObj )
{
   m_pShapeObj = NULL;
   *this = spShapeObj;
}


SHPObjectPtr& SHPObjectPtr::operator=( const SHPObjectPtr& spShapeObj )
{
   if ( m_pShapeObj != spShapeObj.m_pShapeObj )
   {
      release();     // Any current reference

      ATLASSERT( spShapeObj.m_pShapeObj != nullptr && "No reference in SHPObjectPtr" );
      if ( nullptr != ( m_pShapeObj = spShapeObj.m_pShapeObj ) )
         (m_pShapeObj->m_iRefCount)++;
   }
   return *this;
}

SHPObjectPtr::~SHPObjectPtr()
{
   release();     // Any current reference
}

void SHPObjectPtr::release()
{
   if ( m_pShapeObj != nullptr
         && --( m_pShapeObj->m_iRefCount) == 0 )
   {
      HeapUtility::CHeapAllocator* pHeapAllocator
         = m_pShapeObj->m_pHeapAllocator;
      if ( pHeapAllocator != NULL )
      {
         m_pShapeObj->~SHPObject();
         pHeapAllocator->deallocate( m_pShapeObj );
      }
      else
         ::delete m_pShapeObj;
   }
}

SHPObject* SHPObjectPtr::detach()
{
   SHPObject* p = m_pShapeObj;
   if ( p != nullptr )
   {
      ( p->m_iRefCount )--;
      ATLASSERT( p->m_iRefCount >= 0 );
      m_pShapeObj = nullptr;
   }
   return p;
}


////////////////////////////////////////////////////////////////////////////////
// SHPFileLoD
////////////////////////////////////////////////////////////////////////////////

void SHPFileLoD::clear()
{
#ifdef FAST_SHAPES_CLEAR
   for ( SHPObjectMapIter it = m_mpCachedShapes.begin(); it != m_mpCachedShapes.end(); it++ )
      it->second.detach();
#endif
   m_mpCachedShapes.clear();
   m_gptCacheLL.lat = +90.0;
   m_gptCacheUR.lat = -90.0;
}

////////////////////////////////////////////////////////////////////////////////
// SHPObject
////////////////////////////////////////////////////////////////////////////////

SHPObject::~SHPObject()
{
   ClearStrings();
   if ( m_pHeapAllocator != nullptr )
      m_pHeapAllocator->deallocate( m_pbyShapeData );
   else
      ::free( m_pbyShapeData );
}

SHPVertex* SHPObject::GetVertices()
{
   return reinterpret_cast< SHPVertex* >( m_pbyShapeData );
}

SHPVertex* SHPObject::GetVertices( size_t iPart )
{
   return ( iPart >= m_cParts ) ? nullptr
      : &GetVertices()[ GetPart( iPart ).part_start ];
}

SHPPart& SHPObject::GetPart( size_t iPart )
{
   assert( iPart < m_cParts && "part number is out of bounds" );
   return reinterpret_cast< SHPPart* >( m_pbyShapeData + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) ) )[ iPart ];
}
      
size_t SHPObject::GetPartSize( size_t iPart )
{
   return ( iPart >= m_cParts ) ? 0
      : ( ( iPart + 1 < m_cParts )
         ? GetPart( iPart + 1 ).part_start : m_cVertices ) - GetPart( iPart ).part_start;
}

d_geo_rect_t& SHPObject::GetGeoBounds( size_t iPart )
{
   return m_cParts <= 1 ? m_grcBounds
      : reinterpret_cast< d_geo_rect_t* >( m_pbyShapeData
         + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) )
         + ( m_cParts * sizeof(SHPPart) / sizeof(BYTE) ) )[ iPart ];
}

int* SHPObject::GetPolygonScreenBounds()
{
   ASSERT( ( m_mFeatureMask & FEATURE_POLYGON ) != 0
      && "Shape object must be a polygon for GetPolygonScreenBounds()" );

   return reinterpret_cast< int* >(  m_pbyShapeData
         + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) )
         + ( m_cParts * sizeof(SHPPart) / sizeof(BYTE) )
         + ( m_cParts <= 1 ? 0 : m_cParts * sizeof(d_geo_rect_t) / sizeof(BYTE) ) );
}

PolygonLabelInfo& SHPObject::GetPolygonLabelInfo( size_t iPart )
{
   ASSERT( ( m_mFeatureMask & FEATURE_POLYGON ) != 0
      && "Shape object must be a polygon for GetPolygonLabelInfo()" );

   return reinterpret_cast< PolygonLabelInfo* >( m_pbyShapeData
      + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) )
      + ( m_cParts * sizeof(SHPPart) / sizeof(BYTE) )
      + ( m_cParts <= 1 ? 0 : sizeof(d_geo_rect_t) / sizeof(BYTE) )
      + ( ( m_mFeatureMask & SHPObject::FEATURE_POLYGON ) == 0 ? 0 : sizeof(int[4]) / sizeof(BYTE) ) )[ iPart ];
}

DOUBLE* SHPObject::GetMs()
{
   if ( ( m_mFeatureMask & FEATURE_M ) == 0 )
      return nullptr;

   return reinterpret_cast< DOUBLE* >( m_pbyShapeData
      + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) )  // Vertices
      + ( m_cParts * sizeof(SHPPart) / sizeof(BYTE) )       // Parts
      + ( m_cParts <= 1 ? 0 : sizeof(d_geo_rect_t) / sizeof(BYTE) )  // GeoBounds
      + ( ( m_mFeatureMask & SHPObject::FEATURE_POLYGON ) == 0 // Polygon screen bounds and label info
         ? 0 : ( sizeof(int[4]) + ( m_cParts * sizeof( PolygonLabelInfo ) ) / sizeof(BYTE) ) ) );
}  // GetMs()

DOUBLE* SHPObject::GetZs()
{
   if ( ( m_mFeatureMask & FEATURE_Z ) == 0 )
      return nullptr;

   return reinterpret_cast< DOUBLE* >( m_pbyShapeData
      + ( m_cVertices * sizeof(SHPVertex) / sizeof(BYTE) )  // Vertices
      + ( m_cParts * sizeof(SHPPart) / sizeof(BYTE) )       // Parts
      + ( m_cParts <= 1 ? 0 : sizeof(d_geo_rect_t) / sizeof(BYTE) )  // GeoBounds
      + ( ( m_mFeatureMask & FEATURE_POLYGON ) == 0 // Polygon screen bounds and label info
         ? 0 : ( sizeof(int[4]) + ( m_cParts * sizeof( PolygonLabelInfo ) ) / sizeof(BYTE) ) )
      + ( ( m_mFeatureMask & FEATURE_M ) == 0    // M values
         ? 0 : m_cVertices * sizeof(DOUBLE) / sizeof(BYTE) ) );
}  // GetZs()

VOID SHPObject::ClearStrings()
{
   if ( m_pHeapAllocator != nullptr )
      m_pHeapAllocator->deallocate( m_pchShapeStrings );
   else
      ::free( m_pchShapeStrings );
   m_pchShapeStrings = nullptr;
}

LPCSTR SHPObject::GetToolTip() const
{
   return ( m_pchShapeStrings == nullptr ) ? "" : m_pchShapeStrings;
}

LPCSTR SHPObject::GetHelpText() const
{
   return ( m_pchShapeStrings == nullptr ) ? "" : m_pchShapeStrings + m_wHelpTextOffset;
}

LPCSTR SHPObject::GetLabel() const
{
   return ( m_pchShapeStrings == nullptr ) ? "" : m_pchShapeStrings + m_wLabelOffset;
}

BOOL SHPObject::IsToolTipEmpty() const
{
   return m_pchShapeStrings == nullptr || m_pchShapeStrings[ 0 ] == '\0';
}

BOOL SHPObject::IsLabelEmpty() const
{
   return m_pchShapeStrings == nullptr || m_pchShapeStrings[ m_wLabelOffset ] == '\0';
}

// End of ShpFile.cpp
