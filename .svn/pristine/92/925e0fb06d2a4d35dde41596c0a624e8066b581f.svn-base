// Copyright (c) 1994-2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

#ifndef FALCONVIEW_SHP_BOUNDARIESOVERLAY_H_
#define FALCONVIEW_SHP_BOUNDARIESOVERLAY_H_

#include "shp.h"
#include "shp/shapefil.h"
#include "overlay.h"
#include "maps_d.h"

class BoundariesTierComponent
{
public:
   SHPFileLoDMap  m_mpLoDs;
   degrees_t      m_dMinLat, m_dMinLon, m_dMaxLat, m_dMaxLon;
   BoundariesTierComponent() :
      m_dMinLat( +DBL_MAX ), m_dMinLon( +DBL_MAX ),
      m_dMaxLat( -DBL_MAX ), m_dMaxLon( -DBL_MAX )
   {}
};
typedef std::map< std::string, BoundariesTierComponent > BoundariesTierComponentMap;
typedef BoundariesTierComponentMap::iterator BoundariesTierComponentMapIter;

class BoundariesTier
{
public:
   bool m_on;
   BoundariesTierComponentMap m_mpTierComponents;
   SHPDisplayProperties m_display_properties;
   std::string m_strTierFolder;
   std::string m_color_key;
   std::string m_default_color;
};
typedef std::vector< BoundariesTier > BoundariesTierVec;

class BoundariesOverlay :
   public CBaseShapeFileOverlay,
   public FvOverlayImpl
{
   // Constructor
public:
   BoundariesOverlay();

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   // C_overlay overrides
public:
   virtual int draw(ActiveMap* map);

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   virtual void menu(ViewMapProj* map, CPoint point,
      CList<CFVMenuNode*,CFVMenuNode*> & list);

   virtual void release_focus();

   const char *get_class_name() { return "C_shp_ovl"; }
   const std::string & get_name() const;

   virtual void clear_ovl();

public:
   int open( INT iLoD );

   BOOL m_large_file;
   std::string m_data_path;
   std::set< int > m_data_loaded;
   d_geo_t m_scrn_ll;
   d_geo_t m_scrn_ur;

   void OnDisplayPropertiesChanged();
   void check_for_large_files();
   virtual BOOL IsAreaLabelEnabled( const SHPObject& ) const override;

   static void add_tier_file_names( __in const std::string& data_path,
      __in const std::string& tier_path, __out BoundariesTierComponentMap& components );
   static void FillTiers(const std::string& data_path,
      std::vector<BoundariesTier>& tiers);
   static std::string GetDataPath();
   static std::string ComputeColorString(int i);
   static void ReadInOpenState(std::vector<BoundariesTier>& tiers);

   // CBaseShapeFileOverlay overrides
protected:
   // we cannot use display order as index here as in environmental overlay
   // because each tier may have an arbirary number of files.  Perhaps each
   // shape should store a pointer to the filename string in the tier.
   virtual std::string GetFilenameFromShape(const SHPObject* pShapeObject) const override
   {
      return "";
   }

   // returns the hide above threshold for shape objects
   virtual MapScale  get_smallest_scale() override;
   virtual void      set_smallest_scale( MapScale ) override;

   // returns the hide above threshold for shape object labels
   virtual MapScale  get_smallest_labels_scale() override;
   virtual void      set_smallest_labels_scale( MapScale ) override;
   virtual MapScale  get_largest_area_labels_scale() override;
   void              set_largest_area_labels_scale( MapScale ) override;
   virtual BOOL      KeepAreaLabelsInView() const override { return m_bKeepAreaLabelsInView; }

   // Protected destructor. Call Release instead
   ~BoundariesOverlay();

private:
   std::vector<BoundariesTier> m_tiers;
   MapScale m_largest_area_labels_scale;
   BOOL m_bWaterAreaLabels;
   BOOL m_bKeepAreaLabelsInView;

#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES
   int check_localized_shapefiles();
   int make_localized_shapefiles( __inout CString &error_txt );
#endif

}; // class BoundariesOverlay

#endif  // #ifndef FALCONVIEW_SHP_BOUNDARIESOVERLAY_H_
