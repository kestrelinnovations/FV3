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
#include "gps.h"
#include "err.h"
#include "rb.h"
#include "MovingMapFeed.h"

#import "FVObjectRegistryServer.tlb" no_namespace named_guids


using namespace FvDataSourcesLib;
//using namespace GeodataDataSourcesLib;

int C_gps_trail::save_as_gpx(const CString &filespec)
{
   IUtilityMethodsPtr util;
   IFvObjectRegistryPtr registry;
   GPSPointIcon *point;
   IGeometryPtr geometry_pt;
   std::stringstream ss;
   std::string s;

   CO_CREATE(util, CLSID_UtilityMethods);
   CO_CREATE(registry, CLSID_FvObjectRegistry);

   // remove files that will be created
   if (remove(filespec) == -1 && errno == EACCES)
   {
      CString msg;
      msg.Format("Unable to remove %s: the path specifies a read-only "
         "file or the file is open.", filespec);
      ERR_report(msg);
      return FAILURE;
   }

   // don't save empty trails.
   if (get_icon_list()->is_empty())
   {
      ERR_report("No points in the Moving Map trail.");
      return FAILURE;
   }

   // export
   IFvDataSourceEditPtr outputDataSource = registry->CreateDataSourceForConnectString(_bstr_t(filespec), VARIANT_FALSE);
   outputDataSource->StartEditing();
   IFileDataSourcePtr outputFileDataSource = outputDataSource;
   if (S_OK != outputFileDataSource->CreateNewFile(_bstr_t(filespec)))
   {
      std::stringstream ss;
      ss << "Could not create output file: " << std::string(filespec) << std::endl;
      ERR_report(ss.str().c_str());
      return FAILURE;
   }


// create a prototype geometry for the first track point

   point = (GPSPointIcon *)get_icon_list()->get_first();


   ss.str("");
   ss<<"Point(" << point->get_longitude() << " " << point->get_latitude() << ")";
   s=ss.str();
   geometry_pt = util->CreateGeometryFromWKT(s.c_str());

   // create a prototype feature
   IFvOGRFeaturePtr ogrFeature;
   CO_CREATE(ogrFeature, CLSID_FvOGRFeature);
   ogrFeature->BeginPrototype(geometry_pt);
   ogrFeature->AddField("track_fid", FIELD_TYPE_LONG);  //0
   ogrFeature->AddField("track_seg_id", FIELD_TYPE_LONG);//1
   ogrFeature->AddField("track_seg_point_id", FIELD_TYPE_LONG); //2
   ogrFeature->AddField("track_name", FIELD_TYPE_STRING); //3  This becomes the name of the track
   ogrFeature->AddField("ele", FIELD_TYPE_DOUBLE);       //4
   ogrFeature->AddField("time", FIELD_TYPE_DATE);        //5
   ogrFeature->AddField("magvar", FIELD_TYPE_DOUBLE);    //6
   ogrFeature->AddField("geoidheight", FIELD_TYPE_DOUBLE);//7
   ogrFeature->AddField("name", FIELD_TYPE_STRING);      //8
   ogrFeature->AddField("cmt", FIELD_TYPE_STRING);       //9
   ogrFeature->AddField("desc", FIELD_TYPE_STRING);      //10
   ogrFeature->AddField("src", FIELD_TYPE_STRING);       //11
   ogrFeature->AddField("link1_href", FIELD_TYPE_STRING);//12
   ogrFeature->AddField("link1_text", FIELD_TYPE_STRING);//13
   ogrFeature->AddField("link1_type", FIELD_TYPE_STRING);//14   
   ogrFeature->AddField("link2_href", FIELD_TYPE_STRING);//15
   ogrFeature->AddField("link2_text", FIELD_TYPE_STRING);//16
   ogrFeature->AddField("link2_type", FIELD_TYPE_STRING);//17
   ogrFeature->AddField("sym", FIELD_TYPE_STRING);       //18
   ogrFeature->AddField("type", FIELD_TYPE_STRING);      //19
   ogrFeature->AddField("fix", FIELD_TYPE_STRING);       // 20 'none'|'2d'|'3d'|'dgps'|'pps'
   ogrFeature->AddField("sat", FIELD_TYPE_DOUBLE);       // 21 num satelites
   ogrFeature->AddField("hdop", FIELD_TYPE_DOUBLE);      // 22 horiz dilution of percision
   ogrFeature->AddField("vdop", FIELD_TYPE_DOUBLE);      // 23 vert   ''              ''
   ogrFeature->AddField("pdop", FIELD_TYPE_DOUBLE);      // 24 position dilution of percision
   ogrFeature->AddField("ageofdgpsdata", FIELD_TYPE_DOUBLE); //25 sec since last dgps signal
   ogrFeature->AddField("dgpsid", FIELD_TYPE_LONG);      // 26 num 1-1024
   ogrFeature->FinalizePrototype();
   IFeaturePtr feature = ogrFeature;


   IFeatureDataSetPtr dataSet = outputDataSource->CreateEmptyFeatureDataSet("track_points","GPS Trail Description");
   IFeatureDataSetEditPtr dataSetEdit = dataSet;
   dataSetEdit->StartEditing();


   int num_centers=1;
   if (point->has_multiple_centers())
   { // there is no way to get the number of centers other than walking all the centers until it returns 0 for the center_number
      double lat,lon;
      int test_center=1;
      num_centers=1;
      while (test_center)
      {
         point->get_center(test_center,lat,lon);    
         if (!test_center)
         {
            break;
         }
         test_center++;
         num_centers++; 
      }
   }

   for (int center=0;center<num_centers;center++)
   {
      int pt_num = 0;
      point = (GPSPointIcon *)get_icon_list()->get_first();
      while (point != NULL)
      {

         // For the very first point, we set the geometry in the prototype (above)
         if (pt_num!=0||center!=0)
         {
            ss.str("");
            double lat,lon;
            int temp_center=center;
            point->get_center(temp_center,lat,lon); // get center will modify the temp_center if it does not exist
            ss<<"Point(" << lon << " " << lat << ")";
            std::string s=ss.str();
            geometry_pt = util->CreateGeometryFromWKT(s.c_str());
            // change the geometry on the prototype feature
            ogrFeature->SetGeometry(geometry_pt);
         }
         // set all the feature attributes for the current point
         ogrFeature->SetFieldValue(0, FIELD_TYPE_LONG, center);   //track_fid         
         ogrFeature->SetFieldValue(1, FIELD_TYPE_LONG, 0);   //track_seg_id (we only handle one today but if we lost contact and reconnected, we would want to start a second segment) 
         ogrFeature->SetFieldValue(2, FIELD_TYPE_LONG, pt_num);   // track_seg_point_id
         ss.str("");
         ss<< "Track " << center;
         ogrFeature->SetFieldValue(3, FIELD_TYPE_STRING, _bstr_t(ss.str().c_str()));   // track_name 

         ogrFeature->SetFieldValue(4, FIELD_TYPE_DOUBLE, static_cast<double>(point->m_msl));   //ele 

         // time of this point
         COleDateTime coDt = point->m_dateTime;
         _variant_t time;
         time.vt = VT_DATE;
         time = coDt;
         ogrFeature->SetFieldValue(5, FIELD_TYPE_DATE, time);   //TIME 

         //ogrFeature->SetFieldValue(6, FIELD_TYPE_DOUBLE, ???);   //MagVar 
         ss.str("");
         ss<< "Point " << pt_num;
         ogrFeature->SetFieldValue(8, FIELD_TYPE_STRING, _bstr_t(ss.str().c_str()));   //name

         ogrFeature->SetFieldValue(9, FIELD_TYPE_STRING, _bstr_t(point->get_comment_txt()));   //Comment 
         //ogrFeature->SetFieldValue(10, FIELD_TYPE_STRING, "Description of Pt");   //description 
         ogrFeature->SetFieldValue(11, FIELD_TYPE_STRING, _bstr_t(GetMovingMapFeed()->get_feed_name()) );   // Source of point data (possibly name of GPS) 

         // OGR_GPX only supports 2 links per point
         CStringArray SA;
         point->GetLinks(SA);
         if (SA.GetCount() > 0)
         {
            ogrFeature->SetFieldValue(12, FIELD_TYPE_STRING, _bstr_t(SA.GetAt(0)));   //Link 1 HREF (URI)
            ogrFeature->SetFieldValue(13, FIELD_TYPE_STRING, _bstr_t(SA.GetAt(0)));   //Link1 text
            ogrFeature->SetFieldValue(14, FIELD_TYPE_STRING, "");   //Link1 mime type
         }
         else
         {
            ogrFeature->SetFieldValue(12, FIELD_TYPE_STRING, "");   //Link 1 HREF (URI)
            ogrFeature->SetFieldValue(13, FIELD_TYPE_STRING, "");   //Link1 text
            ogrFeature->SetFieldValue(14, FIELD_TYPE_STRING, "");   //Link1 mime type
         }


         if (SA.GetCount() > 1)
         {
            ogrFeature->SetFieldValue(15, FIELD_TYPE_STRING, _bstr_t(SA.GetAt(0)));   //Link 2 HREF (URI)
            ogrFeature->SetFieldValue(16, FIELD_TYPE_STRING, _bstr_t(SA.GetAt(0)));   //Link2 text
            ogrFeature->SetFieldValue(17, FIELD_TYPE_STRING, "");   //Link2 mime type
         }
         else
         {
            ogrFeature->SetFieldValue(15, FIELD_TYPE_STRING, "");   //Link 2 HREF (URI)
            ogrFeature->SetFieldValue(16, FIELD_TYPE_STRING, "");   //Link2 text
            ogrFeature->SetFieldValue(17, FIELD_TYPE_STRING, "");   //Link2 mime type
         }

         ogrFeature->SetFieldValue(18, FIELD_TYPE_STRING, _bstr_t(m_properties.get_symbol_type())); // SYMbole used on GPS
         // ogrFeature->SetFieldValue(20, FIELD_TYPE_STRING, ???); // Fix type
         // ogrFeature->SetFieldValue(21, FIELD_TYPE_LONG, ???); // satelights in view

         dataSetEdit->CopyFeature(feature);

         point = (GPSPointIcon *)get_icon_list()->get_next();
         pt_num++;
      }
   }

   if (S_OK != dataSetEdit->FinishEditing())
   {
       ERR_report("Error Finishing editing of GPS Data Source");
       return FAILURE;
   }
   ///// Done writing track points, note the GDAL GPX driver will generate the track multi-line-string for us.

   // Write Range and bearing points as waypoints in the GPX file
   //
   int rbcount = m_rb_mgr->get_count();
   if (rbcount > 0)
   {
      double lat, lon;      
      RangeBearing *rbpt = m_rb_mgr->get_rb_obj(0);

      // create a prototype feature
      IFvOGRFeaturePtr ogrRBFeature;
      CO_CREATE(ogrRBFeature, CLSID_FvOGRFeature);      
      
      ss.str("");
      lon = rbpt->get_data().m_from_longitude;
      lat = rbpt->get_data().m_from_latitude;
      ss<<"Point(" << lon << " " << lat << ")";
      std::string s=ss.str();
      geometry_pt = util->CreateGeometryFromWKT(s.c_str());
     
      ogrRBFeature->BeginPrototype(geometry_pt);
      
      ogrRBFeature->AddField("ele", FIELD_TYPE_DOUBLE);       //0
      ogrRBFeature->AddField("time", FIELD_TYPE_DATE);        //1
      ogrRBFeature->AddField("magvar", FIELD_TYPE_DOUBLE);    //2
      ogrRBFeature->AddField("geoidheight", FIELD_TYPE_DOUBLE);//3
      ogrRBFeature->AddField("name", FIELD_TYPE_STRING);      //4
      ogrRBFeature->AddField("cmt", FIELD_TYPE_STRING);       //5
      ogrRBFeature->AddField("desc", FIELD_TYPE_STRING);      //6
      ogrRBFeature->AddField("src", FIELD_TYPE_STRING);       //7
      ogrRBFeature->AddField("link1_href", FIELD_TYPE_STRING);//8
      ogrRBFeature->AddField("link1_text", FIELD_TYPE_STRING);//9
      ogrRBFeature->AddField("link1_type", FIELD_TYPE_STRING);//10
      ogrRBFeature->AddField("link2_href", FIELD_TYPE_STRING);//11   
      ogrRBFeature->AddField("link2_text", FIELD_TYPE_STRING);//12
      ogrRBFeature->AddField("link2_type", FIELD_TYPE_STRING);//13
      ogrRBFeature->AddField("sym", FIELD_TYPE_STRING);       //14
      ogrRBFeature->AddField("type", FIELD_TYPE_STRING);      //15
      ogrRBFeature->AddField("fix", FIELD_TYPE_STRING);       //16   'none'|'2d'|'3d'|'dgps'|'pps'
      ogrRBFeature->AddField("sat", FIELD_TYPE_DOUBLE);       // 17  num satelites
      ogrRBFeature->AddField("hdop", FIELD_TYPE_DOUBLE);      // 18  horiz dilution of percision
      ogrRBFeature->AddField("vdop", FIELD_TYPE_DOUBLE);      // 19  vert   ''              ''
      ogrRBFeature->AddField("pdop", FIELD_TYPE_DOUBLE);      // 20  position dilution of percision; //22 sec since last dgps signal
      ogrRBFeature->AddField("ageofdgpsdata", FIELD_TYPE_DOUBLE); //21 sec since last dgps update
      ogrRBFeature->AddField("dgpsid", FIELD_TYPE_LONG);      // 22 num 1-1024
      ogrRBFeature->FinalizePrototype();
      IFeaturePtr RBfeature = ogrRBFeature;


      IFeatureDataSetPtr RBdataSet = outputDataSource->CreateEmptyFeatureDataSet("waypoints","R&B points");
      IFeatureDataSetEditPtr RBdataSetEdit = RBdataSet;
      RBdataSetEdit->StartEditing();

      int pt_num = 0;
      for (int i=0;i<rbcount;i++)
      {

         // For the very first point, we set the geometry in the prototype (above)
         if (i!=0)
         {
            ss.str("");
            rbpt->get_lon(lon);
            rbpt->get_lat(lat);
            ss<<"Point(" << lon << " " << lat << ")";
            std::string s=ss.str();
            geometry_pt = util->CreateGeometryFromWKT(s.c_str());
            // change the geometry on the prototype feature
            ogrRBFeature->SetGeometry(geometry_pt);
         }
         // set all the feature attributes for the current point
         //ogrRBFeature->SetFieldValue(0, FIELD_TYPE_DOUBLE, ???);   // elevation

         // start time of RB
         COleDateTime coDt = rbpt->m_dtEventStart;
         _variant_t time;
         time.vt = VT_DATE;
         time = coDt;
         ogrRBFeature->SetFieldValue(1, FIELD_TYPE_DATE, time);   //TIME 

         ogrRBFeature->SetFieldValue(4, FIELD_TYPE_STRING, _bstr_t(rbpt->get_data().m_label));   //name 
         ogrRBFeature->SetFieldValue(5, FIELD_TYPE_STRING, "Range Bearing Start Point");   //comment 
         ogrRBFeature->SetFieldValue(6, FIELD_TYPE_STRING, _bstr_t(rbpt->get_data().m_from_description));   //name 
         RBdataSetEdit->CopyFeature(RBfeature);

         point = (GPSPointIcon *)get_icon_list()->get_next();
      }

      if (S_OK != RBdataSetEdit->FinishEditing())
      {
         ERR_report("Error Finishing editing of GPS Waypoints");
         return FAILURE;
      }

   }
   
   outputDataSource->FinishEditing();
   outputFileDataSource->CloseFile();
   return SUCCESS;
}


// open a gpx (XML standard for GPS data) using GDAL/OGR
int C_gps_trail::open_gpx(CString filespec)
{
   double prev_lat, prev_lon,heading=-1.0,new_heading,distance=0;
   // open the GPX and get the data set

   IFvDataSourcePtr dataSource;
   IFvDataSource2Ptr dataSource2;

   CO_CREATE(dataSource, CLSID_FvOGRDataSource);
   dataSource2 = dataSource;

   if (S_OK != dataSource->Connect(_bstr_t(filespec)))
      return FAILURE;
   int num_sets = dataSource->GetDataSetCount();
   IFeatureDataSetPtr featureDataSet = dataSource->GetDataSetByName("track_points");
   if (featureDataSet !=NULL)
   {

      // walk the data set with no filter

      long iFeatureCount = featureDataSet->FeatureCount;
      long iTrackID = 0;

      if (iFeatureCount >0)
      { // set up point for calculating heading
         IFeaturePtr feature = featureDataSet->GetFeature(0);
         IGeometryPtr spGeom = feature->GetGeometry();
         IPointPtr point = spGeom;
         ASSERT(point);
         if (point)
         {
            prev_lat = point->y;
            prev_lon = point->x;
         }
         // we can only read one track so we will ignore any point whos trac_fid != this one
         iTrackID = feature->GetFieldByName("track_fid");
      }

      for (long i = 0; i < iFeatureCount; i++)
      {

         IFeaturePtr feature = featureDataSet->GetFeature(i);
         if (iTrackID != (long)feature->GetFieldByName("track_fid"))
         {
            continue;
         }
         IGeometryPtr spGeom = feature->GetGeometry();
         IPointPtr point = spGeom;
         ASSERT(point);
         if (point)
         {
            GEO_calc_range_and_bearing(prev_lat,prev_lon,point->y,point->x,&distance,&new_heading,FALSE);
            if (distance>1.0)
            {
               heading=new_heading;
               prev_lat = point->y;
               prev_lon = point->x;
            }
            m_next_point.m_true_heading = (float)heading;
            m_next_point.set_latitude((float) point->y);
            m_next_point.set_longitude( (float)point->x);
            m_next_point.m_msl = (float)(feature->GetFieldByName("ele"));
            _variant_t time = feature->GetFieldByName("time");
            COleDateTime coDt;
            if (time.vt == VT_DATE)
            {
               coDt=COleDateTime(time);
               m_next_point.m_dateTime= coDt;
            }
            else if (time.vt == VT_BSTR)
            {
               CString sTime = time.bstrVal;

               coDt.ParseDateTime(CString(time.bstrVal));
               m_next_point.m_dateTime = coDt;
            }               

            _variant_t cmt = feature->GetFieldByName("cmt");
            ASSERT (cmt.vt == VT_BSTR);
            m_next_point.set_comment_txt( CString(cmt.bstrVal));
            if (m_next_point.get_comment_txt().GetLength() >0)
            {
               m_next_point.set_has_comment(TRUE);
            }
            CStringArray links;

            _variant_t link = feature->GetFieldByName("link1_href");
            ASSERT (link.vt == VT_BSTR);
            CString sLink = CString(link.bstrVal);
            if (sLink.GetLength() > 0)
               m_next_point.AddLink( sLink);

            link = feature->GetFieldByName("link2_href");
            ASSERT (link.vt == VT_BSTR);
            sLink = CString(link.bstrVal);
            if (sLink.GetLength() > 0)
               m_next_point.AddLink( sLink);

            add_point();
         }

      }

      /////////////////////////////////

      // include the last point in the file is it contains a fix
      if (m_next_point.is_valid())
      {
         add_point();
         set_current_point( &m_next_point );
      }

      dataSource2->Disconnect();

      if (get_icon_list()->is_empty())
      {
         ERR_report("Not a valid GPS file.");

         return FAILURE;
      }

      return SUCCESS;
   }
   else
   {
      dataSource2->Disconnect();
      return FAILURE;
   }
}



////////////////////////////////////////////////////////////////
//
// IFvDataSource Methods
//
///////////////////////////////////////////////////////////////
  STDMETHODIMP CFvGPSDataSource::get_DataSetCount(long * count)
   {
      *count = 1;
      return S_OK;
   }

  STDMETHODIMP CFvGPSDataSource::raw_GetDataSet(long index, FvDataSourcesLib::IFvDataSet ** ppDataSet)
  {
     if (index != 0)
     {
        *ppDataSet = NULL;
     }
     else
     {
        TRY_BLOCK
        {
           if (m_spDataSet != NULL)
           {
              *ppDataSet = m_spDataSet;
              (*ppDataSet)->AddRef();
              return S_OK;
           }
            CComObject<CFvGPSDataSet> *pDS;
            CComObject<CFvGPSDataSet>::CreateInstance(&pDS);
           m_spDataSet = pDS;
           this->AddRef();   
           // Release is called in the DS destructor
           pDS->Initialize(this,m_pGPSTrail->get_icon_list());
           *ppDataSet = m_spDataSet;
           (*ppDataSet)->AddRef();
           return S_OK;
        }
        CATCH_BLOCK_RET
     }
     return S_OK;
  }



   STDMETHODIMP CFvGPSDataSource::raw_GetDataSetByName(BSTR Name, FvDataSourcesLib::IFvDataSet **dataSet)
   {
      return raw_GetDataSet(0, dataSet);
   }

   STDMETHODIMP CFvGPSDataSource::get_DataSourceCount(long * count)
   {
      *count = 0;
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSource::raw_GetDataSource(long index, FvDataSourcesLib::IFvDataSource **dataSource)
   {
      *dataSource = NULL;
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSource::raw_GetDataSourceByName(BSTR Name, FvDataSourcesLib::IFvDataSource * * dataSource)
   {
      *dataSource = NULL;
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSource::raw_Extent2D(double * minX, double * minY, double * maxX, double * maxY)
   {
      if (m_spDataSet == NULL)
      {
         IFvDataSetPtr pds;
         raw_GetDataSet(0,&pds);
      }
      
      return m_spDataSet->raw_Extent2D(minX,minY,maxX,maxY);
      
   }

   STDMETHODIMP CFvGPSDataSource::get_Name(BSTR * Name)
   {
      return m_pGPSTrail->get_m_fileSpecification(Name);
   }

   STDMETHODIMP CFvGPSDataSource::get_Description(BSTR * Description)
   {

      *Description = _bstr_t(L"FV Moving Map Overlay Data Source").Detach();
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSource::raw_RegisterForCallbacks(FvDataSourcesLib::IFvDataSourceCallback * fvDataSourceCallback)
   {
      return E_NOTIMPL;
   }
   STDMETHODIMP CFvGPSDataSource::raw_UnregisterForCallbacks(FvDataSourcesLib::IFvDataSourceCallback * fvDataSourceCallback)
   {
      return E_NOTIMPL;
   }
   STDMETHODIMP CFvGPSDataSource::raw_CheckConnectString(BSTR connectString, VARIANT_BOOL * valid)
   {
      return E_NOTIMPL;
   }
   STDMETHODIMP CFvGPSDataSource::raw_Connect(BSTR connectString)
   {
      return E_NOTIMPL;
   }
   STDMETHODIMP CFvGPSDataSource::get_connectString(BSTR * connectString)
   {
      return m_pGPSTrail->get_m_fileSpecification(connectString);
   }


   //////////////////////////////////////////////////////////
   //
   // CFvGPSDataSet Members
   //
   ///////////////////////////////////////////////////////////

   void CFvGPSDataSet::Initialize(CFvGPSDataSource *pDSource, GPSPointIconList* pIconList)
   {
      m_pDataSource = pDSource;
      m_pIconList = pIconList;
   }

   OGRFeature *CFvGPSDataSet::GPSIcon2OGRFeature(GPSPointIcon* picon)
   {

      OGRFeature *pOGRF = OGRFeature::CreateFeature(GetFeatureDefn());

      std::stringstream ss;
      double lat,lon;  
      int center = 0;
      picon->get_center(center,lat,lon);
      ss << "MULTIPOINT Z ((" << lon << " " << lat << " " << picon->m_msl << ")";
      do
      {
         center++;
         picon->get_center(center,lat,lon);
         if (center) // center will be 0 if the center does not exist
         {
            ss << "(" << lon << " " << lat << " " << picon->m_msl << ")";
         }
      } while(center);
      ss << ")";
      OGRMultiPoint geom;
      geom.setCoordinateDimension(3);

      char * pszgeom = new char[ss.str().length() + 1];
      char *tstr = pszgeom;
      strncpy_s(pszgeom, ss.str().length() + 1, ss.str().c_str(), ss.str().length());
      geom.importFromWkt(&tstr);
      delete pszgeom;

      pOGRF->SetGeometry(&geom);
      pOGRF->SetField("timestamp",(int)picon->get_year(),(int)picon->get_month(),(int)picon->get_day(),(int)picon->get_hour(),(int)picon->get_minute(),(int)picon->get_second(),0);
      pOGRF->SetField("speed_km_h",picon->m_speed_km_hr);
      pOGRF->SetField("true_heading",picon->m_true_heading);
      if (picon->has_comment())
         pOGRF->SetField("comment",picon->get_comment_txt());
      else
         pOGRF->SetField("comment","");

      ss.clear();
      int linkcount = picon->GetLinksCount();
      char **stringlist = new char*[linkcount+1];
      CStringArray saLinks;
      picon->GetLinks(saLinks);
      for (int i=0;i<linkcount;i++)
      {
         int slen = saLinks.GetAt(i).GetLength();
         char* linkName = new char[slen + 1];
         strncpy_s(linkName, slen + 1, saLinks.GetAt(i), slen);
         stringlist[i]= linkName;
      }
      stringlist[linkcount]=NULL;
      
      pOGRF->SetField("links",stringlist);

      for (int i=0; i<linkcount;i++)
      {
         delete stringlist[i];
      }
      delete[] stringlist;

      return pOGRF;
   }

   ///////////////////////////////////////////////////////////////////
   //
   // IFvDataSet Methods
   //
   ///////////////////////////////////////////////////////////////////

   STDMETHODIMP CFvGPSDataSet::raw_Extent2D(double *minX, double *minY, double *maxX, double *maxY)
   {
      d_geo_t ll, ur;
      bool bExtentSet = false;
      ASSERT(m_pDataSource && m_pIconList);

      // if there are fiters applied, we need to fo this the hard way which
      // is implemented in the base class
      if (m_pFilterGeom || m_pAttribQuery)
      {
         return CFvBaseDataSet::raw_Extent2D(minX,minY,maxX,maxY);
      }

      // no filters set so we can use the trail extents which should be fast

      *minX = *maxX = *minY = *maxY = 0.0;


      /* -------------------------------------------------------------------- */
      /*      If this layer has a none geometry type, then we can             */
      /*      reasonably assume there are not extents available.              */
      /* -------------------------------------------------------------------- */
      if( GetFeatureDefn()->GetGeomType() == wkbNone )
         return S_OK;

      GPSPointIconList::dSubtrailList *subtrail_list = m_pIconList->GetSubtrailList();
      ASSERT(subtrail_list);

      if(subtrail_list->IsEmpty() || m_current_subtrail_pos == NULL || m_current_icon_pos == NULL)
      {

         return S_OK;
      }


      POSITION stlpos = m_current_subtrail_pos; //subtrail list position

      do
      {
         GPSPointSubList *subtrail = subtrail_list->GetNext(stlpos);
         if (!bExtentSet)
         {
            subtrail->get_bounds(&ll,&ur);
         }
         else
         {
            d_geo_t tll,tur;
            subtrail->get_bounds(&tll,&tur);
            ll.lat = min(ll.lat,tll.lat);
            ll.lon = min(ll.lon,tll.lon);
            ur.lat = max(ur.lat,tur.lat);
            ur.lon = max(ur.lon,tur.lon);
         }

      }
      while (stlpos);


      *maxX = ur.lon;
      *maxY = ur.lat;
      *minX = ll.lon;
      *minY = ll.lat;
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSet::get_Name(BSTR * Name)
   {
      *Name = _bstr_t(L"Track Points").Detach();
      return S_OK;
   }

   STDMETHODIMP CFvGPSDataSet::get_Description(BSTR * Description)
   {
      std::stringstream ss;
      *Description = _bstr_t(L"GPS Track Points").Detach();
      return S_OK;
   }


   //Returns a pointer to the Layers Feature Definition.  (Callers should not delete this)
   OGRFeatureDefn* CFvGPSDataSet::GetFeatureDefn()
   {
      if (m_pFeatureDefn == nullptr)
      {
         m_pFeatureDefn = new OGRFeatureDefn;  // I believe this will be released when new feature gets released
         m_pFeatureDefn->Reference();
      

         m_pFeatureDefn->SetGeomType(wkbMultiPoint25D);
         m_pFeatureDefn->AddFieldDefn(&OGRFieldDefn("timestamp",OFTDateTime));      
         m_pFeatureDefn->AddFieldDefn(&OGRFieldDefn("speed_km_h",OFTReal));
         m_pFeatureDefn->AddFieldDefn(&OGRFieldDefn("true_heading",OFTReal));
         m_pFeatureDefn->AddFieldDefn(&OGRFieldDefn("comment",OFTString));
         m_pFeatureDefn->AddFieldDefn(&OGRFieldDefn("links",OFTStringList));
      }
      return m_pFeatureDefn;
   }

   OGRFeature *CFvGPSDataSet::GetFirstFeature_prefilter()
   {
      ASSERT(m_pDataSource && m_pIconList);
      GPSPointIconList::dSubtrailList *subtrail_list = m_pIconList->GetSubtrailList();


      m_current_subtrail_pos = subtrail_list->GetHeadPosition();
      m_current_icon_pos = m_pIconList->get_first_position();
  
      
      return GetNextFeature_prefilter();
   }


   // A subtrail defines is a section of the main iconlist defined by a start position and end position
   // the subtrail also records the bounding box of all the points in the subtrail for quick filtering
   // this will seatch from the start_pos (or the start of the list if NULL) to the end and return the
   // first feature that matches the data sets filter envelope
   OGRFeature *CFvGPSDataSet::SearchSubtrail(GPSPointSubList *subtrail, POSITION start_pos)
   {

      d_geo_t ll,ur;

      subtrail->get_bounds(&ll,&ur);
      if (ll.lat > m_sFilterEnvelope.MaxY ||
         ll.lon > m_sFilterEnvelope.MaxX ||
         ur.lat < m_sFilterEnvelope.MinY ||
         ur.lon < m_sFilterEnvelope.MinX
         )
      {
         m_current_icon_pos = subtrail->get_last();
         return NULL;
      }


      if (start_pos == NULL)
      {
         start_pos = subtrail->get_first();
      }

      do 
      {
         m_current_icon_pos = start_pos;
         GPSPointIcon *picon = m_pIconList->get_next(start_pos);
         if (!picon)
            return NULL;
         if (picon->get_latitude() > m_sFilterEnvelope.MinY &&
            picon->get_latitude() < m_sFilterEnvelope.MaxY &&
            picon->get_longitude() > m_sFilterEnvelope.MinX &&
            picon->get_longitude() < m_sFilterEnvelope.MaxX)
         {
            m_current_icon_pos = start_pos;
            return GPSIcon2OGRFeature(picon);
         }

      }while(m_current_icon_pos != subtrail->get_last() && start_pos != NULL);
      
      return NULL;
   }

   OGRFeature *CFvGPSDataSet::GetNextFeature_prefilter()
   {
      ASSERT(m_pDataSource && m_pIconList);

      GPSPointIconList::dSubtrailList *subtrail_list = m_pIconList->GetSubtrailList();
      ASSERT(subtrail_list);

      if(subtrail_list->IsEmpty() || m_current_subtrail_pos == NULL || m_current_icon_pos == NULL)
         return NULL;

      POSITION stlpos = m_current_subtrail_pos; //subtrail list position

      do
      {
         GPSPointSubList *subtrail = subtrail_list->GetNext(stlpos);

         OGRFeature *pFeature = SearchSubtrail(subtrail,m_current_icon_pos);

         if (pFeature) 
         {
            return pFeature;
            // m_current_subtrail_pos remains pointing to the current subtrail
            // m_current_subtrail_pos soints to the next position (set in SearchSubtrail)
         }
         else
         {
            m_current_subtrail_pos = stlpos; // move to the next subtrail
            m_current_icon_pos = NULL;       // set_current_pos to null and it will be set to the start of the next subtrail
         }
      }
      while (stlpos);


   return NULL;   
   }

   int CFvGPSDataSet::GetUnfilteredFeatureCount()
   {
      return m_pIconList->get_count();
   }