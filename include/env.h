// Copyright (c) 1994-2009,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// env.h -- shape file overlay header

#ifndef ENV_H
#define ENV_H 1

#include "shp.h"
#include "overlay.h"
#include "maps_d.h"

static const INT NUM_ENVIRONMENTAL_TYPES = 6;

class C_env_ovl : 
   public CBaseShapeFileOverlay,
   public FvOverlayImpl
{
   // Constructor
public:                         
   C_env_ovl();

   virtual void Finalize();

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   // C_overlay overrides
   virtual int draw(ActiveMap* map);

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   virtual void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);

   virtual void release_focus();

   const char *get_class_name() { return "C_shp_ovl"; }
   const CString & get_name() const;

   int open( int iLoD );
   void OnDisplayPropertiesChanged();

   virtual void clear_ovl();

private:
   typedef CList< CString*, CString* > StringList;

   // Methods
   //void clear_string_list( StringList& string_list);
   //void clear_all_string_lists();
   int check_group_files();
#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES
   void load_default_groups(); 
   int make_group_shape_files(CString &error_txt);
#endif
   void check_for_large_files();

   // Data
   SHPFileLoDMap m_ampCachedShapesInfo[ NUM_ENVIRONMENTAL_TYPES ];

   // display properties associated with each type
   SHPDisplayProperties *m_pDisplayProperties[ NUM_ENVIRONMENTAL_TYPES ];
   
#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES
   struct LPCSTRLess
   {
      bool operator()( LPCSTR const & x, LPCSTR const & y ) const
      {
         return strcmp( x, y) < 0;
      }
   };
   typedef std::map< LPCSTR, int, LPCSTRLess > EnvGroupMap;
   typedef EnvGroupMap::iterator EnvGroupMapIter;
   typedef EnvGroupMap::value_type EnvGroupMapValue;
   EnvGroupMap m_mpGroupEnvTypes; // Map feature type to environment group
#endif

   CString m_data_path;
   BOOL m_large_file;
   std::set< int > m_data_loaded;
   d_geo_t m_scrn_ll;
   d_geo_t m_scrn_ur;


   // CBaseShapeFileOverlay overrides
protected:

   // returns the filename associated with the given shape object
   virtual std::string GetFilenameFromShape(const SHPObject* pShapeObject) const override;

   // returns the hide above threshold for shape objects
   virtual MapScale get_smallest_scale() override; 
   virtual void set_smallest_scale( MapScale ) override;

   // returns the hide above threshold for shape object labels
   virtual MapScale get_smallest_labels_scale();
   void set_smallest_labels_scale(MapScale scale);

   // Protected destructor. Call Release instead
   ~C_env_ovl();
};

#endif  // #ifndef ENV_H
