// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.


// fvwutil.cpp


#include "stdafx.h"
#include "fvwutil.h"
#include "mem.h"
#include "file.h"
#include "graphics.h"
#include "..\proj\Projectors.h"
#include "geo3\geotrans.h"
#include "utils.h"
#include "..\mainfrm.h"
#include "TextRendering.h"
#include "SnapTo.h"
#include "..\getobjpr.h"

#ifdef GOV_RELEASE
#import "XpSearchLib.tlb" no_namespace named_guids
#endif

// TEMP until CFvwUtil and COvlkitUtil are merged
//#include "ovlkit.h"

CFvwUtil *CFvwUtil::m_instance = NULL;

///////////////////////////////////////////////////////////////////////
// Statics

static inline
   int round(double val)
{
   return (val > 0.0 ? ((int) (val + 0.5)) : ((int) (val - 0.5)));
}


// compute the square of the magnitude of line <x1,y1> - <x2,y2>
static
   double _magnitude_sqr(double x1, double y1, double x2, double y2)
{
   double mag_sqr;

   double dx = x1 - x2;
   double dy = y1 - y2;

   mag_sqr = (dx * dx) + (dy * dy);

   return mag_sqr;
}

// compute the square of the perpendicular distance of
// point <xp,yp> to line <x1,y1> - <x2,y2>
static
   double _distance_to_line_sqr (  double x1, double y1,  /* pt on the line */
   double x2, double y2,  /* pt on the line */
   double xp, double yp ) /* pt to test */
{
   double bx, by, cx, cy;
   double len_sq;
   double dot;
   double dist_sqr;

   // --- translate all points to a = (0,0) ---
   bx = x2 - x1;
   cx = xp - x1;
   by = y2 - y1;
   cy = yp - y1;

   // ---  check for coincident line points ---
   if ((x1 == x2) && (y1 == y2))
   {
      len_sq = (cx * cx) + (cy * cy);
      dist_sqr = len_sq;
      return dist_sqr;
   }


   len_sq = (bx * bx) + (by * by);

   // --- first check to see if point c is "between" a and b ---
   // --- dot product of AC and AB is the length of AC projected onto AB ---
   dot = (bx * cx) + (by * cy);

   if ((dot < 0.0) || (dot > len_sq))
      return 999999999; //MAXWORLD;

   // --- dot product of AC and a perpendicular to AB divided by len AB
   // --- is the distance from point C to line AB ---
   dot = (by * cx) + ((0.0-bx) * cy);
   double dot_sq = dot * dot;
   dist_sqr = fabs(dot_sq / len_sq);
   return dist_sqr;
}

// End of Statics
///////////////////////////////////////////////////////////////////////

CFvwUtil::CFvwUtil()
{
}

// ********************************************************************
// ********************************************************************

util_country_t util_country[UTIL_COUNTRY_NUM] = {
   {"AA","Aruba"},
   {"AC","Antigua & Barbuda"},
   {"AF","Afghanistan"},
   {"AG","Algeria"},
   {"AJ","Azerbaijan"},
   {"AL","Albania"},
   {"AM","Armenia"},
   {"AN","Andorra"},
   {"AO","Angola"},
   {"AQ","American Somoa"},
   {"AR","Argentina"},
   {"AS","Australia"},
   {"AT","Ashmore & Cartier Island"},
   {"AU","Austria"},
   {"AV","Anguilla"},
   {"AY","Antarctica"},
   {"BA","Bahrain"},
   {"BB","Barbados"},
   {"BC","Botswana"},
   {"BD","Bermuda"},
   {"BE","Belgium"},
   {"BF","The Bahamas"},
   {"BG","Bangladesh"},
   {"BH","Belize"},
   {"BK","Bosnia & Herzegovia"},
   {"BL","Bolivia"},
   {"BM","Burma"},
   {"BN","Benin"},
   {"BO","Belarus"},
   {"BP","Solomon Island"},
   {"BQ","Navassa Island"},
   {"BR","Brazil"},
   {"BS","Bassas"},
   {"BT","Bhutan"},
   {"BU","Bulgaria"},
   {"BV","Bouvet Island"},
   {"BX","Brunei"},
   {"BY","Burundi"},
   {"CA","Canada"},
   {"CB","Cambodia"},
   {"CD","Chad"},
   {"CE","Sri Lanka"},
   {"CF","Congo"},
   {"CG","Zaire"},
   {"CH","China"},
   {"CI","Chile"},
   {"CJ","Cayman Islands"},
   {"CK","Cocos(Keeling)Island"},
   {"CM","Cameroon"},
   {"CN","Comoros"},
   {"CO","Colombia"},
   {"CQ","Northern Mariana Islands"},
   {"CR","Coral Sea Islands"},
   {"CS","Costa Rica"},
   {"CT","Central Africa"},
   {"CU","Cuba"},
   {"CV","Cape Verde"},
   {"CW","Cook Islands"},
   {"CY","Cyprus"},
   {"DA","Denmark"},
   {"DJ","Djibouti"},
   {"DO","Dominica"},
   {"DQ","Jarvis Island"},
   {"DR","Dominican Republic"},
   {"EC","Ecuador"},
   {"EG","Egypt"},
   {"EI","Ireland"},
   {"EK","Equatorial Guinea"},
   {"EN","Estonia"},
   {"ER","Eritrea"},
   {"ES","El Salvador"},
   {"ET","Ethiopia"},
   {"EU","Europa Island"},
   {"EZ","Czech Republic"},
   {"FI","Finland"},
   {"FK","Falkland Island"},
   {"FM","Micronesia"},
   {"FO","Faroe Islands"},
   {"FP","French Polynesia"},
   {"FQ","Baker Island"},
   {"FR","France"},
   {"FS","French Southern & Antarctic Islands"},
   {"GA","The Gambia"},
   {"GB","Gabon"},
   {"GG","Georgia"},
   {"GH","Ghana"},
   {"GI","Gibralter"},
   {"GJ","Grenada"},
   {"GK","Guernsey"},
   {"GL","Greenland"},
   {"GM","Germany"},
   {"GO","Glorioso Islands"},
   {"GP","Guadeloupe"},
   {"GQ","Guam"},
   {"GR","Greece"},
   {"GT","Guatemala"},
   {"GV","Guinea"},
   {"GY","Guyana"},
   {"GZ","Gaza Strip"},
   {"HA","Haiti"},
   {"HK","Hong Kong"},
   {"HM","Heard Island & McDonald Island"},
   {"HO","Honduras"},
   {"HQ","Howland Island"},
   {"HR","Croatia"},
   {"HU","Hungry"},
   {"IC","Iceland"},
   {"ID","Indonesia"},
   {"IM","Isle of Man"},
   {"IN","India"},
   {"IO","British Indian Ocean Terr."},
   {"IP","Clipperton Island"},
   {"IR","Iran"},
   {"IS","Israel"},
   {"IT","Italy"},
   {"IV","Ivory Coast"},
   {"IY","Iraq-Saudi Arabia Neu.Zone"},
   {"IZ","Iraq"},
   {"JA","Japan"},
   {"JE","Jersey"},
   {"JM","Jamaica"},
   {"JN","Jan Mayen"},
   {"JO","Jordan"},
   {"JQ","Johnson Atoll"},
   {"JU","Juan De Nova Island"},
   {"KE","Kenya"},
   {"KG","Kyrgyzstan"},
   {"KN","North Korea"},
   {"KQ","Kingman Reef"},
   {"KR","Kiribati"},
   {"KS","South Korea"},
   {"KT","Christmas Island"},
   {"KU","Kuwait"},
   {"KZ","Kazakhstan"},
   {"LA","Laos"},
   {"LE","Lebanon"},
   {"LG","Latvia"},
   {"LH","Lithuania"},
   {"LI","Liberia"},
   {"LO","Slovakia"},
   {"LQ","Palmyra Atoll"},
   {"LS","Liechtenstein"},
   {"LT","Lesotho"},
   {"LU","Luxembourg"},
   {"LY","Libya"},
   {"MA","Madagascar"},
   {"MB","Martinique"},
   {"MC","Macau"},
   {"MD","Moldova"},
   {"MF","Mayotte"},
   {"MG","Mongolia"},
   {"MH","Montserrat"},
   {"MI","Malawi"},
   {"MK","Macedonia"},
   {"ML","Mali"},
   {"MN","Monaco"},
   {"MO","Morocco"},
   {"MP","Mauritius"},
   {"MQ","Midway Islands"},
   {"MR","Mauritania"},
   {"MT","Malta"},
   {"MU","Oman"},
   {"MV","Maldives"},
   {"MW","Montenegro"},
   {"MX","Mexico"},
   {"MY","Malaysia"},
   {"MZ","Mozambique"},
   {"NC","New Caledonia"},
   {"NE","Niue"},
   {"NF","Norfolk Island"},
   {"NG","Niger"},
   {"NH","Vanuatu"},
   {"NI","Nigeria"},
   {"NL","Netherlands"},
   {"NO","Norway"},
   {"NP","Nepal"},
   {"NR","Nauru"},
   {"NS","Suriname"},
   {"NT","Netherlands Antilles"},
   {"NU","Nicaragua"},
   {"NZ","New Zealand"},
   {"PA","Paraguay"},
   {"PC","Pitcairn Island"},
   {"PE","Peru"},
   {"PF","Paracel Islands"},
   {"PG","Spratly Islands"},
   {"PK","Pakistan"},
   {"PL","Poland"},
   {"PM","Panama"},
   {"PO","Portugal"},
   {"PP","Papua New Guinea"},
   {"PS","Palau"},
   {"PU","Guinea-Bissau"},
   {"QA","Qatar"},
   {"RE","Reunion"},
   {"RM","Marshall Islands"},
   {"RO","Romania"},
   {"RP","Philippines"},
   {"RQ","Puerto Rico"},
   {"RS","Russia"},
   {"RW","Rwanda"},
   {"SA","Saudi Arabia"},
   {"SB","St. Pierre & Miqueion"},
   {"SC","St.Christopher & Nevis"},
   {"SE","Seychelles"},
   {"SF","South Africa"},
   {"SG","Senegal"},
   {"SH","St. Helena"},
   {"SI","Slovenia"},
   {"SL","Sierra Leone"},
   {"SM","San Marino"},
   {"SN","Singapore"},
   {"SO","Somalia"},
   {"SP","Spain"},
   {"SR","Serbia"},
   {"ST","St.Lucia"},
   {"SU","Sudan"},
   {"SV","Svalbard"},
   {"SW","Sweden"},
   {"SY","Syria"},
   {"SZ","Switzerland"},
   {"TC","United Arab Emirates"},
   {"TD","Trinidad & Tobago"},
   {"TE","Tromelin Island"},
   {"TH","Thailand"},
   {"TI","Tajikistan"},
   {"TK","Turks & Caicos Islands"},
   {"TL","Tokelau"},
   {"TN","Tonga"},
   {"TO","Togo"},
   {"TP","Sao Tome & Principe"},
   {"TS","Tunisia"},
   {"TU","Turkey"},
   {"TV","Tuvalu"},
   {"TW","Taiwan"},
   {"TX","Turkmenistan"},
   {"TZ","Tanzania"},
   {"UG","Uganda"},
   {"UK","United Kingdom"},
   {"UP","Ukraine"},
   {"US","United States"},
   {"UV","Burkina"},
   {"UY","Uruguay"},
   {"UZ","Uzbekistan"},
   {"VC","St.Vincent & Grenadines"},
   {"VE","Venezuela"},
   {"VI","British Virgin Islands"},
   {"VM","Vietnam"},
   {"VQ","Virgin Islands"},
   {"VT","Vatican City"},
   {"WA","Namibia"},
   {"WE","The West Bank"},
   {"WF","Wallis & Futuna"},
   {"WI","Western Sahara"},
   {"WQ","Wake Island"},
   {"WS","Western Samoa"},
   {"WZ","Swaziland"},
   {"YM","Yemen"},
   {"ZA","Zambia"},
   {"ZI","Zimbabwe"}};


   // ********************************************************************
   // ********************************************************************

   util_state_t util_state[UTIL_STATE_NUM] = {
      {"01","AL","Alabama"},
      {"02","AK","Alaska"},
      {"04","AZ","Arizona"},
      {"05","AR","Arkansas"},
      {"06","CA","California"},
      {"08","CO","Colorado"},
      {"09","CT","Connecticut"},
      {"10","DE","Delaware"},
      {"12","FL","Florida"},
      {"13","GA","Georgia"},
      {"15","HI","Hawaii"},
      {"16","ID","Idaho"},
      {"17","IL","Illinois"},
      {"18","IN","Indiana"},
      {"19","IA","Iowa"},
      {"20","KS","Kansas"},
      {"21","KY","Kentucky"},
      {"22","LA","Louisiana"},
      {"23","ME","Maine"},
      {"24","MD","Maryland"},
      {"25","MA","Massachusetts"},
      {"26","MI","Michigan"},
      {"27","MN","Minnesota"},
      {"28","MS","Mississippi"},
      {"29","MO","Missouri"},
      {"30","MT","Montana"},
      {"31","NE","Nebraska"},
      {"32","NV","Nevada"},
      {"33","NH","New Hampshire"},
      {"34","NJ","New Jersey"},
      {"35","NM","New Mexico"},
      {"36","NY","New York"},
      {"37","NC","North Carolina"},
      {"38","ND","North Dakota"},
      {"39","OH","Ohio"},
      {"40","OK","Oklahoma"},
      {"41","OR","Oregon"},
      {"42","PA","Pennsylvania"},
      {"44","RI","Rhode Island"},
      {"45","SC","South Carolina"},
      {"46","SD","South Dakota"},
      {"47","TN","Tennessee"},
      {"48","TX","Texas"},
      {"49","UT","Utah"},
      {"50","VT","Vermont"},
      {"51","VA","Virginia"},
      {"53","WA","Washington"},
      {"54","WV","West Virginia"},
      {"55","WI","Wisconsin"},
      {"56","WY","Wyoming"},
      {"60","AS","American Samoa"},
      {"64","FM","Federated States of Micronesia"},
      {"66","GM","Guam"},
      {"68","MR","Marshall Islands"},
      {"69","NI","Northern Mariana Islands"},
      {"70","PU","Palau"},
      {"72","PR","Puerto Rico"},
      {"74","MW","Midway Islands"},
      {"78","VI","Virgin Islands of the United States"}};


      // ********************************************************************
      // ********************************************************************

// Function to lookup a DAFIF value from a Fix/Point string.
long CFvwUtil::db_lookup(CString &string, SnapToInfo &db_info,
   boolean_t fix_point_search)
{
#ifdef GOV_RELEASE
         // remove a left bracket if one is present
         int k, len;
         len = string.GetLength();
         for (k=0; k<len; k++)
         {
            if (string.GetAt(k) == '[')
               string.SetAt(k, ' ');
         }

         // strip off leading\trailing white spaces
         string.TrimLeft();
         string.TrimRight();

         if(string.IsEmpty())
            return FALSE;

         // if the first character is a dot, its just a comment
         if (string[0] == '.')
            return FALSE;

         // valid fixed points have less than 11 characters
         // airports    - up to 6
         // navaids     - up to 7
         // waypoints   - up to 8
         // refuel      - up to 10
         // training    - up to 11
         if (fix_point_search && string.GetLength() > 11)
            return FALSE;

         HRESULT hr;
         IXpSearchPtr pXpSearch;
         long error_code;

         _bstr_t err_msg, key, fix, desc, datum;
         double lat, lon;

         MapView* map_view = fvw_get_view();
         if (map_view == nullptr)
            return FALSE;

         const MapProjectionParams proj = map_view->GetMapProjParams();
         lat = proj.center.lat;
         lon = proj.center.lon;

         bool is_xpSearch = true;

         hr = pXpSearch.CreateInstance(__uuidof(XpSearch));
         if (pXpSearch == NULL)
         {
            INFO_report("XPlan search library failed to load");
            is_xpSearch = false;
         }

         if (is_xpSearch == true)
         {
            try
            {
               key = (_bstr_t) string;

               long result;
               if (fix_point_search)
                  result = pXpSearch->db_search(key, 0, fix.GetAddress(), desc.GetAddress(), &lat, &lon, datum.GetAddress(), &error_code, err_msg.GetAddress());
               else
                  result = pXpSearch->db_search(key, 1, fix.GetAddress(), desc.GetAddress(), &lat, &lon, datum.GetAddress(), &error_code, err_msg.GetAddress());

         if (result <= 0)
            return result;

               strcpy_s(db_info.m_fix_rs, (char *)fix);
               strcpy_s(db_info.m_description_rs, (char *)desc);
               strcpy_s(db_info.m_src_datum, (char *)datum);

               db_info.m_lat = lat;
               db_info.m_lon = lon;

               // if a record was found copy its data into db_info and delete the
               // object created by the lookup function

               return TRUE;
            }
            catch(_com_error& e)
            {
               INFO_report(e.ErrorMessage());
               is_xpSearch = false;
            }
         }

         if (is_xpSearch == false)
         {
            try
            {
               FvObjectRegistryServerLib::IFvObjectRegistryPtr registry;
               registry.CreateInstance(__uuidof(FvObjectRegistryServerLib::FvObjectRegistry));

               FvDataSourcesLib::IFvDataSourcePtr dafif = registry->CreateDataSourceForConnectString("DAFIF", VARIANT_TRUE);
               dafif->Connect("DAFIF");
               if (dafif != NULL)
               {
                  FvDataSourcesLib::IFeaturePtr feature = NULL;

                  // ARPT table
                  if (feature == NULL)
                  {
                     FvDataSourcesLib::IFvDataSetPtr dataSet = dafif->GetDataSetByName("dafif.arpt");
                     FvDataSourcesLib::IAttributeFilterPtr attributeFilter;
                     attributeFilter.CreateInstance(GeodataDataSourcesLib::CLSID_AttributeFilter);
                     attributeFilter->WhereClause = _bstr_t("UPPER(ICAO) LIKE UPPER('%" + string + "%')");
                     FvDataSourcesLib::IFilterPtr filter = attributeFilter;
                     dataSet->Filter = filter;
                     FvDataSourcesLib::IFeatureDataSetPtr featureDataSet = dataSet;
                     if (featureDataSet->GetFeatureCount() > 0)
                     {
                        feature = featureDataSet->GetFeature(0);

                        _variant_t fix = feature->GetFieldByName("ICAO");
                        strcpy_s(db_info.m_fix_rs, (char *)_bstr_t(fix));

                        _variant_t name = feature->GetFieldByName("NAME");
                        strcpy_s(db_info.m_description_rs, (char *)_bstr_t(name));

                        _variant_t datum = feature->GetFieldByName("WGS_DATUM");
                        strcpy_s(db_info.m_src_datum, (char *)_bstr_t(datum));

                        _variant_t lat = feature->GetFieldByName("WGS_DLAT");
                        db_info.m_lat = lat.dblVal;

                        _variant_t lon = feature->GetFieldByName("WGS_DLONG");
                        db_info.m_lon = lon.dblVal;

                        return TRUE;
                     }
                  }

                  // HLPT table
                  if (feature == NULL)
                  {
                     FvDataSourcesLib::IFvDataSetPtr dataSet = dafif->GetDataSetByName("dafif.hlpt");
                     FvDataSourcesLib::IAttributeFilterPtr attributeFilter;
                     attributeFilter.CreateInstance(GeodataDataSourcesLib::CLSID_AttributeFilter);
                     attributeFilter->WhereClause = _bstr_t("UPPER(ICAO) LIKE UPPER('%" + string + "%')");
                     FvDataSourcesLib::IFilterPtr filter = attributeFilter;
                     dataSet->Filter = filter;
                     FvDataSourcesLib::IFeatureDataSetPtr featureDataSet = dataSet;
                     if (featureDataSet->GetFeatureCount() > 0)
                     {
                        feature = featureDataSet->GetFeature(0);

                        _variant_t fix = feature->GetFieldByName("ICAO");
                        strcpy_s(db_info.m_fix_rs, (char *)_bstr_t(fix));

                        _variant_t name = feature->GetFieldByName("NAME");
                        strcpy_s(db_info.m_description_rs, (char *)_bstr_t(name));

                        _variant_t datum = feature->GetFieldByName("WGS_DATUM");
                        strcpy_s(db_info.m_src_datum, (char *)_bstr_t(datum));

                        _variant_t lat = feature->GetFieldByName("WGS_DLAT");
                        db_info.m_lat = lat.dblVal;

                        _variant_t lon = feature->GetFieldByName("WGS_DLONG");
                        db_info.m_lon = lon.dblVal;

                        return TRUE;
                     }
                  }

                  // WPT table
                  if (feature == NULL)
                  {
                     FvDataSourcesLib::IFvDataSetPtr dataSet = dafif->GetDataSetByName("dafif.wpt");
                     FvDataSourcesLib::IAttributeFilterPtr attributeFilter;
                     attributeFilter.CreateInstance(GeodataDataSourcesLib::CLSID_AttributeFilter);
                     attributeFilter->WhereClause = _bstr_t("UPPER(WPT_IDENT) LIKE UPPER('%" + string + "%')");
                     FvDataSourcesLib::IFilterPtr filter = attributeFilter;
                     dataSet->Filter = filter;
                     FvDataSourcesLib::IFeatureDataSetPtr featureDataSet = dataSet;
                     if (featureDataSet->GetFeatureCount() > 0)
                     {
                        feature = featureDataSet->GetFeature(0);

                        _variant_t fix = feature->GetFieldByName("WPT_IDENT");
                        strcpy_s(db_info.m_fix_rs, (char *)_bstr_t(fix));

                        _variant_t name = feature->GetFieldByName("DESC");
                        strcpy_s(db_info.m_description_rs, (char *)_bstr_t(name));

                        _variant_t datum = feature->GetFieldByName("WGS_DATUM");
                        strcpy_s(db_info.m_src_datum, (char *)_bstr_t(datum));

                        _variant_t lat = feature->GetFieldByName("WGS_DLAT");
                        db_info.m_lat = lat.dblVal;

                        _variant_t lon = feature->GetFieldByName("WGS_DLONG");
                        db_info.m_lon = lon.dblVal;

                        return TRUE;
                     }
                  }

                  // NAV table
                  if (feature == NULL)
                  {
                     FvDataSourcesLib::IFvDataSetPtr dataSet = dafif->GetDataSetByName("dafif.nav");
                     FvDataSourcesLib::IAttributeFilterPtr attributeFilter;
                     attributeFilter.CreateInstance(GeodataDataSourcesLib::CLSID_AttributeFilter);
                     attributeFilter->WhereClause = _bstr_t("UPPER(NAV_IDENT) LIKE UPPER('%" + string + "%')");
                     FvDataSourcesLib::IFilterPtr filter = attributeFilter;
                     dataSet->Filter = filter;
                     FvDataSourcesLib::IFeatureDataSetPtr featureDataSet = dataSet;
                     if (featureDataSet->GetFeatureCount() > 0)
                     {
                        feature = featureDataSet->GetFeature(0);

                        _variant_t fix = feature->GetFieldByName("NAV_IDENT");
                        strcpy_s(db_info.m_fix_rs, (char *)_bstr_t(fix));

                        _variant_t name = feature->GetFieldByName("NAME");
                        strcpy_s(db_info.m_description_rs, (char *)_bstr_t(name));

                        _variant_t datum = feature->GetFieldByName("WGS_DATUM");
                        strcpy_s(db_info.m_src_datum, (char *)_bstr_t(datum));

                        _variant_t lat = feature->GetFieldByName("WGS_DLAT");
                        db_info.m_lat = lat.dblVal;

                        _variant_t lon = feature->GetFieldByName("WGS_DLONG");
                        db_info.m_lon = lon.dblVal;

                        return TRUE;
                     }
                  }
               }
            }
            catch (_com_error& e)
            {
               REPORT_COM_ERROR(e);
               return FALSE;
            }
         }
#endif
         return FALSE;
      }

      void CFvwUtil::pixels_to_km(MapProj* map, int x1, int y1, int x2, int y2,
         double *distx, double *disty)
      {
         double lat1, lon1, lat2, lon2;
         double tdist, tangle;

         map->surface_to_geo(x1, y1, &lat1, &lon1);
         map->surface_to_geo(x2, y2, &lat2, &lon2);
         GEO_geo_to_distance(lat1, lon1, lat1, lon2, &tdist, &tangle);
         *distx = tdist / 1000.0;
         GEO_geo_to_distance(lat1, lon1, lat2, lon1, &tdist, &tangle);
         *disty = tdist / 1000.0;
      }
      // end of pixels_to_km

      // add a tab characters to a null terminated string
      void CFvwUtil::add_tab(char * tstr)
      {
         int len;

         len = strlen(tstr);
         tstr[len] = 0x09;
         tstr[len+1] = '\0';
      }
      // end of add_tab

      // make a string a specified length, truncate or add spaces
      void CFvwUtil::set_text_length(char * tstr, int setlen)
      {
         int len, k;

         len = strlen(tstr);
         if (len > setlen)
            tstr[setlen] = '\0';
         return;

         TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         for (k=0; k<setlen-len; k++)
            strcat_s(tstr, setlen + 1, " ");
      }
      // end of set_text_len

      // ********************************************************************
      // ********************************************************************

      // construct an array of points that define an ellipse

      void CFvwUtil::construct_oval(int x, int y,   // center of oval
         int major,       // semi-major axis in pixels
         int minor,       // semi-minor axis in pixels
         double angle,   // CW rotation of oval
         int num_vertex, // number of vertices in polygon
         POINT* pt)      // array of points for polygon
      {
         int ptcnt, k;
         double ainc;
         double *px, *py;
         double ang, phi, rx, ry;
         double sinang, cosang;

         px = new double[num_vertex];
         py = new double[num_vertex];
         ptcnt = 0;
         ainc = TWO_PI / (double) (num_vertex-1);
         phi = 0.0;
         rx = cos(phi) * minor;
         ry = sin(phi) * major;
         while ((phi < TWO_PI) && (ptcnt < num_vertex))
         {
            px[ptcnt] = cos(phi) * minor;
            py[ptcnt] = sin(phi) * major;
            ptcnt++;
            phi += ainc;
         }

         // ensure closure
         px[num_vertex-1] = rx;
         py[num_vertex-1] = ry;

         // rotate the ovlal in 2-D
         ang = DEG_TO_RAD(angle);
         sinang = sin(ang);
         cosang = cos(ang);
         for (k=0; k<ptcnt; k++)
         {
            pt[k].x = (int) ((px[k] * cosang) - (py[k] * sinang)) + x;
            pt[k].y = (int) ((px[k] * sinang) + (py[k] * cosang)) + y;
         }
         delete [] py;
         delete [] px;
      }
      // end of construct_oval

      // ********************************************************************
      // ********************************************************************

      // forces the angle to be between 0 and 359 degrees

      int CFvwUtil::normalize_angle(int angle)
      {
         while (angle < 0)
            angle += 360;
         while (angle >= 360)
            angle -= 360;

         return angle;
      }

      double CFvwUtil::normalize_radians(double angle)
      {
         while (angle < 0)
            angle += TWO_PI;
         while (angle >= TWO_PI)
            angle -= TWO_PI;

         return angle;
      }

      // end of normalize_angle

      // ********************************************************************
      // ********************************************************************

      // forces the angle to be between 0 and 359.9 degrees

      double CFvwUtil::normalize_angle(double angle)
      {
         double k;

         if (_isnan(angle) || !_finite(angle))
            return 0;

         k = angle;
         while (k < 0.0)
            k += 360.0;
         while (k >= 360.0)
            k -= 360.0;
         return k;
      }
      // end of normalize_angle

      // convert a north up heading to a cartesian angle
      double CFvwUtil::heading_to_cartesian_angle(double heading)
      {
         // we want 0->90, 90->0, 180->-90, etc... return a positive
         // cartesian angle
         return normalize_angle(90.0 - heading);
      }

      // convert a cartesian angle to a north up heading
      double CFvwUtil::cartesian_angle_to_heading(double angle)
      {
         return normalize_angle(90.0 - angle);
      }

      // ********************************************************************
      // ********************************************************************

      int CFvwUtil::normalize_geo(double *lat, double *lon)
      {
         double tlat, tlon;

         tlat = *lat;
         tlon = *lon;
         if (tlat < -90.0)
         {
            tlat += tlat + 90.0;
            tlon += 180.0;
         }
         if (tlat > 90.0)
         {
            tlat -= tlat - 90.0;
            tlon += 180.0;
         }
         if (tlon < -180)
            tlon += 360.0;
         if (tlon > 180.0)
            tlon -= 360.0;

         if (!GEO_valid_degrees(tlat, tlon))
            return FAILURE;

         *lat = tlat;
         *lon = tlon;

         return SUCCESS;
      }

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::geo2text(double lat, double lon, char* slat, int slat_len, char* slon, int slon_len)
      {
         CGeoTrans geo;
         return geo.geo2text(lat,lon,slat,slat_len,slon,slon_len);
      }
      // end of geo2text

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::text2geo(char* slat, char* slon, double* lat, double* lon)
      {
         CGeoTrans geo;
         return geo.text2geo(slat,slon, lat,lon);
      }
      // end of text2geo

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::point_in_polygon(CPoint point, POINT *pgon, int numverts)
      {
         int c, i, j;
         int x, y;
         c = 0;

         x = point.x;
         y = point.y;
         for (i = 0, j = numverts - 1; i < numverts; j = i++)
         {
            if ((((pgon[i].y <= y) && (y < pgon[j].y)) || ((pgon[j].y <= y) && (y < pgon[i].y))) &&
               ((double)x < (double)(pgon[j].x - pgon[i].x) * (double)(y - pgon[i].y) /
               (double)(pgon[j].y - pgon[i].y) + (double)pgon[i].x))
               c = !c;
         }
         if (c == 0)
            return FALSE;
         else
            return TRUE;
      }
      // end of point_in_polygon

      // ********************************************************************
      // ********************************************************************

      // determines if a point is within a threshold value of one of the line segments of a polygon
      BOOL CFvwUtil::point_on_polygon(CPoint point, int threshold, POINT *pgon, int numverts)
      {
         int k, dist;
         BOOL hit;

         k = 0;
         hit = FALSE;
         while ((k<numverts-1) && !hit)
         {
            dist = distance_to_line (pgon[k].x, pgon[k].y, pgon[k+1].x, pgon[k+1].y, point.x, point.y);
            if (dist <= threshold)
               hit = TRUE;
            k++;
         }

         if (!hit)
         {
            // check the last leg
            dist = distance_to_line (pgon[numverts-1].x, pgon[numverts-1].y,
               pgon[0].x, pgon[0].y, point.x, point.y);
            if (dist <= threshold)
               hit = TRUE;
         }

         return hit;
      }
      // end of point_on_polygon

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::polygon_is_clockwise(POINT *pgon, int numverts)
      {
         int k, tk, tx, ty;
         int left, mid, right;
         double before_angle, after_angle;

         // find lowest, rightmost vertex
         tk = 0;
         tx = pgon[tk].x;
         ty = pgon[tk].y;
         for (k=1; k<numverts; k++)
         {
            if (ty >= pgon[k].y)
            {
               if (ty == pgon[k].y)
               {
                  if (tx >= pgon[k].x)
                     continue;
               }
               tk = k;
               tx = pgon[tk].x;
               ty = pgon[tk].y;
            }
         }

         // get the points before and after the found one
         mid = tk;
         left = tk-1;
         if (left < 0)
            left = numverts-1;
         while ((pgon[mid].x == pgon[left].x) && (pgon[mid].y == pgon[left].y) && (left != mid))
         {
            left--;
            if (left < 0)
               left = numverts-1;
         }
         right = tk+1;
         if (right >= numverts)
            right = 0;
         while ((pgon[mid].x == pgon[right].x) && (pgon[mid].y == pgon[right].y) && (right != mid))
         {
            right++;
            if (right >= numverts)
               right = 0;
         }

         // calculate the angle between them
         CPoint leftpt, midpt, rightpt;
         leftpt = pgon[left];
         midpt = pgon[mid];
         rightpt = pgon[right];
         before_angle = line_angle(midpt, leftpt);
         after_angle = line_angle(midpt, rightpt);
         if (before_angle > 180.0)
            before_angle -= 360.0;
         if (after_angle > 180.0)
            after_angle -= 360.0;

         if (before_angle < after_angle)
            return TRUE;
         else
            return FALSE;
      }
      //    end of polygon_is_clockwise (points)

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::polygon_is_clockwise(degrees_t *lat, degrees_t *lon, int numverts)
      {
         int k, tk;
         degrees_t tlat, tlon;
         int left, mid, right;
         double before_angle, after_angle;
         double dist;

         // find lowest, rightmost vertex
         tk = 0;
         tlat = lat[tk];
         tlon = lon[tk];
         for (k=1; k<numverts; k++)
         {
            if (tlat >= lat[k])
            {
               if (tlat == lat[k])
               {
                  if (GEO_east_of_degrees(tlon, lon[k]))
                     continue;
               }
               tk = k;
               tlat = lat[tk];
               tlon = lon[tk];
            }
         }

         // get the points before and after the found one
         mid = tk;
         left = tk-1;
         if (left < 0)
            left = numverts-1;
         while ((lat[mid] == lat[left]) && (lon[mid] == lon[left]) && (left != mid))
         {
            left--;
            if (left < 0)
               left = numverts-1;
         }
         right = tk+1;
         if (right >= numverts)
            right = 0;
         while ((lat[mid] == lat[right]) && (lon[mid] == lon[right]) && (right != mid))
         {
            right++;
            if (right >= numverts)
               right = 0;
         }

         // calculate the angle between them
         GEO_geo_to_distance(lat[mid], lon[mid], lat[left], lon[left], &dist, &before_angle);
         GEO_geo_to_distance(lat[mid], lon[mid], lat[right], lon[right], &dist, &after_angle);
         if (before_angle > 89.999999)
            before_angle -= 360.0;
         if (after_angle > 89.999999)
            after_angle -= 360.0;

         if (before_angle < after_angle)
            return TRUE;
         else
            return FALSE;
      }
      //    end of polygon_is_clockwise (lat/longs)

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::polygon_is_clockwise2(POINT *p, int numverts)
      {
         int i, j, k;
         int count = 0;
         double z;

         if (numverts < 3)
            return FALSE;  // don't know

         for (i=0; i<numverts; i++)
         {
            j = (i + 1) % numverts;
            k = (i + 2) % numverts;
            z  = (p[j].x - p[i].x) * (p[k].y - p[j].y);
            z -= (p[j].y - p[i].y) * (p[k].x - p[j].x);
            if (z < 0)
               count--;
            else if (z > 0)
               count++;
         }
         if (count > 0)
            return FALSE;
         else if (count < 0)
            return TRUE;
         else
            return FALSE; // don't know
      }
      //    end of polygon_is_clockwise2 (vertexes)

      // ********************************************************************
      // ********************************************************************

      // compute angle of a line segment, returns degrees from -> CCW

      double CFvwUtil::line_angle(int x1, int y1,      // start point of line
         int x2, int y2)      // end point of line
      {
         double ang = RAD_TO_DEG( atan2( (double) ( y2 - y1 ), (double) ( x2 - x1 ) ) );
         if ( ang < 0.0 )
            ang += 360.0;

         return ang;
      }
      // end of line_angle

      // ********************************************************************
      // ********************************************************************

      // compute angle of a line segment, returns degrees from -> CCW

      double CFvwUtil::line_angle(double x1, double y1,      // start point of line
         double x2, double y2)      // end point of line
      {
         double ang = RAD_TO_DEG( atan2( y2 - y1, x2 - x1 ) );
         if ( ang < 0.0 )
            ang += 360.0;

         return ang;
      }
      // end of line_angle (double)

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::line_segment_intersection(double ax1, double ay1, double ax2, double ay2,
         double bx1, double by1, double bx2, double by2,
         double *cx, double *cy)
      {
         double a1, b1, a2, b2;
         double xi, yi;

         if (ax1 != ax2)
         {
            b1 = (ay2-ay1) / (ax2-ax1);
            if (bx1 != bx2)
            {
               b2 = (by2-by1) / (bx2-bx1);
               a1 = ay1 - (b1 * ax1);
               a2 = by1 - (b2 * bx1);
               if (b1 == b2)
                  return FALSE;
               xi = -(a1-a2) / (b1-b2);
               yi = a1 + (b1 * xi);
            }
            else
            {
               xi = bx1;
               a1 = ay1 - (b1 * ax1);
               yi = a1 + (b1 * xi);
            }
         }
         else
         {
            xi = ax1;
            if (bx1 != bx2)
            {
               b2 = (by2-by1) / (bx2-bx1);
               a2 = by1 - (b2 * bx1);
               yi = a2 + (b2 * xi);
            }
            else
               return FALSE;
         }

         if (((ax1-xi) * (xi-ax2) >= 0.0) &&
            ((bx1-xi) * (xi-bx2) >= 0.0) &&
            ((ay1-yi) * (yi-ay2) >= 0.0) &&
            ((by1-yi) * (yi-by2) >= 0.0))
         {
            *cx = xi;
            *cy = yi;
            return TRUE;
         }

         return FALSE;
      }
      // end of line_segment_intersection

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::geo_line_segment_intersection(double lat11, double lon11, double lat12, double lon12,
         double lat21, double lon21, double lat22, double lon22,
         double *xlat, double *xlon)
      {
         double ax1, ay1, ax2, ay2, bx1, by1, bx2, by2;
         BOOL idl_cross = FALSE;
         BOOL rslt;

         ax1 = lon11;
         ay1 = lat11;
         ax2 = lon12;
         ay2 = lat12;
         bx1 = lon21;
         by1 = lat21;
         bx2 = lon22;
         by2 = lat22;

         // normalize to valid lat lons
         normalize_geo(&ay1, &ax1);
         normalize_geo(&ay2, &ax2);
         normalize_geo(&by1, &bx1);
         normalize_geo(&by2, &bx2);

         // test for IDL crossing
         if (((ax1 > 0.0) && (ax2 < 0.0)) || ((ax1 < 0.0) && (ax2 > 0.0)))
            if ((fabs(ax1) + fabs(ax2)) > 180.0)
               idl_cross = TRUE;

         if (((ax1 > 0.0) && (bx2 < 0.0)) || ((bx1 < 0.0) && (bx2 > 0.0)))
            if ((fabs(bx1) + fabs(bx2)) > 180.0)
               idl_cross = TRUE;

         if (idl_cross)
         {
            if (ax1 < 0.0)
               ax1 += 360.0;
            if (ax2 < 0.0)
               ax2 += 360.0;
            if (bx1 < 0.0)
               bx1 += 360.0;
            if (bx2 < 0.0)
               bx2 += 360.0;
         }

         rslt = line_segment_intersection(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2, xlat, xlon);
         if (!rslt)
            return rslt;

         if (*xlon > 180.0)
            *xlon -= 360.0;

         return TRUE;
      }
      // end of geo_line_segment_intersection

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::point_to_vector(int start_x, int start_y,
         int end_x, int end_y,
         int *magnitude, double *angle)
      {
         int tx, ty, k;

         tx = (start_x - end_x);
         ty = (start_y - end_y);
         k = (tx * tx) + (ty * ty);
         *magnitude = (int) sqrt((double) k);
         *angle = line_angle(start_x, start_y, end_x, end_y);
      }

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::vector_to_point(int start_x, int start_y,
         int magnitude, double angle,
         int *end_x, int *end_y)
      {
         double rang;

         rang = DEG_TO_RAD(angle);
         *end_x = (int) (magnitude * cos(rang)) + start_x;
         *end_y = (int) (magnitude * sin(rang)) + start_y;
      }

      // *************************************************************
      // ********************************************************************

      int CFvwUtil::point_distance(CPoint pt1, CPoint pt2)
      {
         double xdist, ydist;

         xdist = (double) abs(pt1.x - pt2.x);
         ydist = (double) abs(pt1.y - pt2.y);
         return (int) (sqrt((xdist*xdist) + (ydist * ydist)) + 0.5);
      }
      // end of point_distance

      // ********************************************************************
      // ********************************************************************

      double CFvwUtil::line_angle(CPoint &start_pt, CPoint &end_pt)
      {
         return line_angle(start_pt.x, start_pt.y, end_pt.x, end_pt.y);
      }

      // ********************************************************************
      // ********************************************************************

      int CFvwUtil::magnitude(int x1, int y1, int x2, int y2)
      {
         double mag_sqr;

         mag_sqr = ((double)(x1 - x2) * (double)(x1 - x2)) +
            ((double)(y1 - y2) * (double)(y1 - y2));

         return (int)(sqrt(mag_sqr) + 0.5);
      }
      // end of magnitude

      // ********************************************************************
      // ********************************************************************

      double CFvwUtil::magnitude(double x1, double y1, double x2, double y2)
      {
         double mag_sqr;

         mag_sqr = ((x1 - x2) * (x1 - x2)) +
            ((y1 - y2) * (y1 - y2));

         return sqrt(mag_sqr);
      }
      // end of magnitude

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::rotate_pt(int oldx, int oldy,     // original point
         int *newx, int *newy,   // new point
         double ang,             // angle in degrees
         int ctrx, int ctry)     // center point
      {
         double sinang, cosang;
         double x, y;
         double rang;

         rang = DEG_TO_RAD(ang);
         sinang = sin(rang);
         cosang = cos(rang);

         x = (double) (oldx - ctrx);
         y = (double) (oldy - ctry);

         *newx =  round((x * cosang) - (y * sinang) + 0.0) + ctrx;
         *newy = round((x * sinang) + (y * cosang) + 0.0) + ctry;
      }

      void CFvwUtil::rotate_pt(int oldx, int oldy,
         long *newx, long *newy,
         double ang,
         int ctrx, int ctry)
      {
         int tmpx, tmpy;
         rotate_pt(oldx, oldy, &tmpx, &tmpy, ang, ctrx, ctry);

         *newx = tmpx;
         *newy = tmpy;
      }

      // end of rotate_pt

      // ********************************************************************
      // ********************************************************************

      // get the point on a line that is closest to an exterior point
      int CFvwUtil::closest_point_on_line (int  ax, int  ay,  /* pt on the line */
         int  bx, int  by,  /* pt on the line */
         int  tx, int  ty,  /* pt to test */
         int *cx, int *cy )  // closest point on line
      {
         double ang, ang1, ang2, vang;
         double rang, cosang;
         int vdist, dist, hdist;

         // check to see if tx,ty is "between" the segment end points
         hdist = distance_to_line(ax, ay, bx, by, tx, ty);
         if (hdist > 99999999)
            return FAILURE;

         ang1 = line_angle(ax, ay, bx, by);
         ang2 = line_angle(ax, ay, tx, ty);
         ang = fabs(ang1 - ang2);
         if (ang > 180.0)
            ang = 360.0 - ang;

         if (ang > 90.0)
            return FAILURE;

         point_to_vector(ax, ay, bx, by, &vdist, &vang);
         hdist = magnitude(ax, ay, tx, ty);
         rang = DEG_TO_RAD(ang);
         cosang = cos(rang);
         dist = (int) ((hdist * cosang) + 0.5);

         vector_to_point(ax, ay, dist, vang, cx, cy);

         return SUCCESS;
      }

      // ********************************************************************
      // ********************************************************************

      // get the perpendicular distance from a point to a line
      int CFvwUtil::distance_to_line (   int x1, int y1,  /* pt on the line */
         int x2, int y2,  /* pt on the line */
         int xp, int yp ) /* pt to test */
      {
         double bx, by, cx, cy;
         double len_sq;
         double dot;
         int dist;

         // --- translate all points to a = (0,0) ---
         bx = (double) (x2 - x1);
         cx = (double) (xp - x1);
         by = (double) (y2 - y1);
         cy = (double) (yp - y1);

         // ---  check for coincident line points ---
         if ((x1 == x2) && (y1 == y2))
         {
            len_sq = (cx * cx) + (cy * cy);
            dist = (int) (sqrt(len_sq) + 0.5);
            return dist;
         }


         len_sq = (bx * bx) + (by * by);

         // --- first check to see if point c is "between" a and b ---
         // --- dot product of AC and AB is the length of AC projected onto AB ---
         dot = (bx * cx) + (by * cy);

         if ((dot < 0.0) || (dot > len_sq))
            return 999999999; //MAXWORLD;

         // --- dot product of AC and a perpendicular to AB divided by len AB
         // --- is the distance from point C to line AB ---
         dot = (by * cx) + ((0.0-bx) * cy);
         dist = (int) (fabs(dot / sqrt(len_sq)) + 0.5);
         return dist;
      }
      // end of distance_to_line

      // ********************************************************************
      // ********************************************************************

      // get the perpendicular distance from a point to a line using doubles
      double CFvwUtil::distance_to_line (   double x1, double y1,  /* pt on the line */
         double x2, double y2,  /* pt on the line */
         double xp, double yp ) /* pt to test */
      {
         double bx, by, cx, cy;
         double len_sq;
         double dot;
         double dist;

         // --- translate all points to a = (0,0) ---
         bx = x2 - x1;
         cx = xp - x1;
         by = y2 - y1;
         cy = yp - y1;

         // ---  check for coincident line points ---
         if ((x1 == x2) && (y1 == y2))
         {
            len_sq = (cx * cx) + (cy * cy);
            dist = sqrt(len_sq);
            return dist;
         }


         len_sq = (bx * bx) + (by * by);

         // --- first check to see if point c is "between" a and b ---
         // --- dot product of AC and AB is the length of AC projected onto AB ---
         dot = (bx * cx) + (by * cy);

         if ((dot < 0.0) || (dot > len_sq))
            return 999999999; //MAXWORLD;

         // --- dot product of AC and a perpendicular to AB divided by len AB
         // --- is the distance from point C to line AB ---
         dot = (by * cx) + ((0.0-bx) * cy);
         dist = fabs(dot / sqrt(len_sq));
         return dist;
      }
      // end of distance_to_line

      // ********************************************************************
      // *************************************************************

      int CFvwUtil::distance_to_geo_line(MapProj* map,
         degrees_t lat1, degrees_t lon1,  // coords of start point
         degrees_t lat2, degrees_t lon2,  // coords of end point
         int px, int py)  // point to test
      {
         d_geo_t ll, ur;
         int x1, y1;          /* device coordinates of western point */
         int x2, y2;          /* device coordinates of eastern point */
         int p1_flags;        /* NSEW bounds check flags for point 1 */
         int p2_flags;        /* NSEW bounds check flags for point 2 */
         boolean_t east_of;   /* point 2 is east of point 1 */
         int pixels_around_world;
         int screen_width, screen_height;
         int dist1, dist2;

         // check for screen intersection
         map->get_vmap_bounds(&ll, &ur);
         map->get_pixels_around_world(&pixels_around_world);
         map->get_vsurface_size(&screen_width, &screen_height);

         // treat world overview as a special case
         if (screen_width >= (pixels_around_world/ 2))
         {
            // get device coordinates
            if (GEO_east_of_degrees(lon2, lon1))
            {
               map->geo_to_surface(lat1, lon1, &x1, &y1);
               map->geo_to_surface(lat2, lon2, &x2, &y2);
            }
            else         /* p2 is west of p1 or lon1=lon2 */
            {
               map->geo_to_surface(lat2, lon2, &x1, &y1);
               map->geo_to_surface(lat1, lon1, &x2, &y2);
            }

            if (x1 > x2)
            {
               // wrap around world
               dist1 = distance_to_line(x1, y1, x2 + pixels_around_world, y2, px, py);
               dist2 = distance_to_line(x1 - pixels_around_world, y1, x2, y2, px, py);
               if (dist2 < dist1)
                  dist1 = dist2;
            }
            else
               dist1 = distance_to_line(x1, y1, x2, y2, px, py);

            return dist1;
         }

         // get NSEW bounds check flags for each point
         p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
            ur.lat, ur.lon, lat1, lon1);
         p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
            ur.lat, ur.lon, lat2, lon2);

         // quick accept - both points on map
         if ((p1_flags | p2_flags) == 0)
         {
            // get device coordinates
            map->geo_to_surface(lat1, lon1, &x1, &y1);
            map->geo_to_surface(lat2, lon2, &x2, &y2);

            dist1 = distance_to_line(x1, y1, x2, y2, px, py);

            if (dist1 < 0)
               dist1 = 999999999;
            return dist1;
         }

         // quick reject
         if (p1_flags & p2_flags)
            return 999999999;

         // reject lines that go from west of map to east of map but
         // not through the region defined by left_lon and right_lon
         if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
            (GEO_EAST_OF | GEO_WEST_OF))
         {
            if (p1_flags & GEO_EAST_OF)
            {
               if (GEO_east_of_degrees(lon2, lon1))
                  return 999999999;

               // needed to get device coordinates in right order
               east_of = FALSE;
            }
            else
            {
               if (GEO_east_of_degrees(lon1, lon2))
                  return 999999999;

               // needed to get device coordinates in right order
               east_of = TRUE;
            }
         }
         else
            east_of = GEO_east_of_degrees(lon2, lon1);

         // get device coordinates
         if (east_of)
         {
            map->geo_to_surface(lat1, lon1, &x1, &y1);
            map->geo_to_surface(lat2, lon2, &x2, &y2);
         }
         else
         {
            map->geo_to_surface(lat2, lon2, &x1, &y1);
            map->geo_to_surface(lat1, lon1, &x2, &y2);
         }

         dist1 = distance_to_line(x1, y1, x2, y2, px, py);

         if (dist1 < 0)
            dist1 = 999999999;
         return dist1;
      }
      // end of distance_to_geo_line

      // *************************************************************
      // ********************************************************************

      // restrict an integer to a range of values

      double CFvwUtil::limit(double num, double min, double max)
      {
         double k;

         k = num;
         if (k < min)
            k = min;
         if (k > max)
            k = max;
         return k;
      }
      // end of limit

      // ********************************************************************
      // ********************************************************************

      // restrict an integer to a range of values

      int CFvwUtil::limit(int num, int min, int max)
      {
         int k;

         k = num;
         if (k < min)
            k = min;
         if (k > max)
            k = max;
         return k;
      }
      // end of limit

      // restrict an integer to a range of values
      int CFvwUtil::limit_to_short(int num)
      {
         return limit(num, SHRT_MIN, SHRT_MAX);
      }

      int CFvwUtil::geo_to_view_short(MapProj* map, degrees_t lat, degrees_t lon, int *x, int *y)
      {
         return map->geo_to_surface(lat, lon, x, y);
      }

      void CFvwUtil::move_to(CDC* dc, int x, int y)
      {
         dc->MoveTo(x, y);
      }

      void CFvwUtil::line_to(CDC* dc, int x, int y)
      {
         dc->LineTo(x, y);
      }

      void CFvwUtil::rectangle(CDC* dc, int x1, int y1, int x2, int y2)
      {
         dc->Rectangle(x1, y1, x2, y2);
      }

      void CFvwUtil::rectangle(CDC* dc, LPCRECT rc)
      {
         dc->Rectangle(rc);
      }

      void CFvwUtil::arc(CDC* dc, CPoint ctr, CPoint p1, CPoint p2, int color, int width, BOOL xor)
      {
         double ang1, ang2, anginc, tang, rang;
         int length, x1, y1, x2, y2, clr;
         int oldrop;
         CPen pen;

         clr = color;
         if (xor)
         {
            clr = UTIL_COLOR_WHITE;
            oldrop = dc->SetROP2(R2_XORPEN);
         }

         pen.CreatePen(PS_SOLID, width, code2color(clr));
         CPen *oldpen = dc->SelectObject(&pen);

         point_to_vector(ctr.x, ctr.y, p1.x, p1.y, &length, &ang1);
         point_to_vector(ctr.x, ctr.y, p2.x, p2.y, &length, &ang2);

         if (ang2 < ang1)
            ang2 += 360.0;

         anginc = 6.0;
         tang = ang1;
         x1 = p1.x;
         y1 = p1.y;
         dc->MoveTo(p1.x, p1.y);
         while (tang < ang2)
         {
            rang = DEG_TO_RAD(tang);
            rang = normalize_radians(rang);
            x2 = ctr.x + (int) (cos(rang) * (double) length);
            y2 = ctr.y + (int) (sin(rang) * (double) length);
            dc->LineTo(x2, y2);
            tang += anginc;
         }

         // clean up dc and pen
         dc->SelectObject(oldpen);
         if (xor)
            dc->SetROP2(oldrop);
         pen.DeleteObject();
      }
      // end of arc

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::ellipse(CDC* dc, int x1, int y1, int x2, int y2)
      {
         dc->Ellipse(x1, y1, x2, y2);
      }

      void CFvwUtil::ellipse(CDC* dc, LPCRECT rc)
      {
         dc->Ellipse(rc);
      }

      void CFvwUtil::polygon(CDC* dc, POINT *pt, int numpt)
      {
         dc->Polygon(pt, numpt);
      }

      void CFvwUtil::fill_polygon(CDC* dc, POINT *pnt_list, int num_points,
         int fill_color, int fill_type, 
         int surface_width /* = 0 */,
         int surface_height /* = 0 */)
      {
         fill_polygon(dc, pnt_list, num_points, code2color(fill_color), fill_type,
            surface_width, surface_height);
      }
      // end of fill_polygon

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::fill_polygon(CDC* dc, POINT *fill_pnt_list, int num_points,
         COLORREF fill_color, int fill_type,
         int surface_width /* = 0 */,
         int surface_height /* = 0 */)
      {
         GraphicsUtilities utils(dc->GetSafeHdc());
         utils.fill_polygon(fill_pnt_list, num_points, fill_color, fill_type,
            dc->IsPrinting() == TRUE, surface_width, surface_height);
      }
      // end of fill_polygon

      void CFvwUtil::polyline(CDC* dc, POINT *pnt_list, int num_points, int color, int width)
      {
         CPen pen, *old_pen;

         pen.CreatePen(PS_SOLID, width, code2color(color));
         old_pen = (CPen *)dc->SelectObject(&pen);

         // fill the polygon
         dc->Polyline(pnt_list, num_points);

         // return dc to original state
         dc->SelectObject(old_pen);
         pen.DeleteObject();
      }

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::empty_polygon(CDC* dc, POINT *pt, int numpt)
      {
         dc->MoveTo(pt[0].x, pt[0].y);

         for (int k=1; k<numpt; k++)
            dc->LineTo(pt[k].x, pt[k].y);

         dc->LineTo(pt[0].x, pt[0].y);
      }

      void CFvwUtil::empty_polygon(CDC* dc, POINT *pt, int numpt, int color, int width)
      {
         CPen pen, *old_pen;
         int tcolor, olddrawmode;

         tcolor = color;
         if (color == -1)
         {
            olddrawmode = dc->SetROP2(R2_XORPEN);
            tcolor = UTIL_COLOR_WHITE;
         }

         pen.CreatePen(PS_SOLID, width, code2color(color));
         old_pen = (CPen *)dc->SelectObject(&pen);

         empty_polygon(dc, pt, numpt);

         // return dc to original state
         dc->SelectObject(old_pen);
         pen.DeleteObject();
         if (color == -1)
            dc->SetROP2(olddrawmode);
      }
      // end of empty_polygon

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::line_xor(CDC* dc, int width, CPoint pt1, CPoint pt2)
      {
         CPen WhitePen;

         // create the XOR pen
         WhitePen.CreatePen(PS_SOLID, width, RGB(255, 255, 255));
         int oldrop = dc->SetROP2(R2_XORPEN);
         CPen* oldpen = (CPen*) dc->SelectObject(&WhitePen);

         // erase the last line
         dc->MoveTo(pt1);
         dc->LineTo(pt2);

         // clean up dc and pen
         dc->SelectObject(oldpen);
         dc->SetROP2(oldrop);
         WhitePen.DeleteObject();
      }
      // end of line_xor

      // pt1 and pt2 specify opposite sides of a rectangle
      void CFvwUtil::rectangle_xor(CDC* dc, int width, CPoint pt1, CPoint pt2)
      {
         CFvwUtil *futil = CFvwUtil::get_instance();
         CPen WhitePen;

         // create the XOR pen
         WhitePen.CreatePen(PS_SOLID, width, RGB(255, 255, 255));
         int oldrop = dc->SetROP2(R2_XORPEN);
         CPen* oldpen = (CPen*) dc->SelectObject(&WhitePen);

         // erase the last line
         futil->move_to(dc, pt1.x, pt1.y);
         futil->line_to(dc, pt2.x, pt1.y);
         futil->line_to(dc, pt2.x, pt2.y);
         futil->line_to(dc, pt1.x, pt2.y);
         futil->line_to(dc, pt1.x, pt1.y);

         // clean up dc and pen
         dc->SelectObject(oldpen);
         dc->SetROP2(oldrop);
         WhitePen.DeleteObject();
      }
      // end of rectangle_xor

      // ********************************************************************
      // ********************************************************************

      boolean_t CFvwUtil::is_valid_color(int color)
      {
         boolean_t valid;

         valid = FALSE;

         if ((color == UTIL_COLOR_BLACK) ||
            (color == UTIL_COLOR_DARK_RED) ||
            (color == UTIL_COLOR_DARK_GREEN) ||
            (color == UTIL_COLOR_DARK_YELLOW) ||
            (color == UTIL_COLOR_DARK_BLUE) ||
            (color == UTIL_COLOR_DARK_MAGENTA) ||
            (color == UTIL_COLOR_DARK_CYAN) ||
            (color == UTIL_COLOR_LIGHT_GRAY) ||
            (color == UTIL_COLOR_MONEY_GREEN) ||
            (color == UTIL_COLOR_SKY_BLUE) ||
            (color == UTIL_COLOR_CREAM) ||
            (color == UTIL_COLOR_MEDIUM_GRAY) ||
            (color == UTIL_COLOR_DARK_GRAY) ||
            (color == UTIL_COLOR_RED) ||
            (color == UTIL_COLOR_GREEN) ||
            (color == UTIL_COLOR_YELLOW) ||
            (color == UTIL_COLOR_BLUE) ||
            (color == UTIL_COLOR_MAGENTA) ||
            (color == UTIL_COLOR_CYAN) ||
            (color == UTIL_COLOR_WHITE))
            valid = TRUE;

         return valid;
      }
      // end of is_valid_color

      // ********************************************************************
      // ********************************************************************

      COLORREF CFvwUtil::code2color(int code)
      {
         // defaults to bright white (a valid value) - never fail
         return GRA_get_color_ref(code);
      }
      // end of code2color

      // ********************************************************************
      // ********************************************************************

      int CFvwUtil::color2code(COLORREF color)
      {
         // defaults to bright white (a valid value) - never fail
         int code;

         code = GRA_get_color_constant(color);
         if (code == BRIGHT_WHITE)
         {
            if (color == RGB(255, 255, 255))
               return code;
         }

         // find closest code
         return GRA_get_color_constant_best_fit(color);
      }
      // end of color2code

      int CFvwUtil::code2palette(int color)
      {
         switch(color)
         {
         case UTIL_COLOR_BLACK: return 217;
         case UTIL_COLOR_DARK_RED: return 219;
         case UTIL_COLOR_DARK_GREEN: return 220;
         case UTIL_COLOR_DARK_YELLOW: return 221;
         case UTIL_COLOR_DARK_BLUE: return 222;
         case UTIL_COLOR_DARK_MAGENTA: return 223;
         case UTIL_COLOR_DARK_CYAN: return 224;
         case UTIL_COLOR_LIGHT_GRAY: return 225;
         case UTIL_COLOR_MONEY_GREEN: return 233;
         case UTIL_COLOR_SKY_BLUE: return 234;
         case UTIL_COLOR_CREAM: return 235;
         case UTIL_COLOR_MEDIUM_GRAY: return 236;
         case UTIL_COLOR_DARK_GRAY: return 226;
         case UTIL_COLOR_RED: return 227;
         case UTIL_COLOR_GREEN: return 228;
         case UTIL_COLOR_YELLOW: return 229;
         case UTIL_COLOR_BLUE: return 218;
         case UTIL_COLOR_MAGENTA: return 230;
         case UTIL_COLOR_CYAN: return 231;
         case UTIL_COLOR_WHITE: return 232;
         }

         return 0;
      }

      // ********************************************************************
      // *****************************************************************

      COLORREF CFvwUtil::string_to_color(CString color)
      {
         COLORREF rgb_color;
         CString tcolor, tstr;
         BYTE red, grn, blu;

         tcolor = color + "          ";
         tstr = tcolor.Left(3);
         red = (BYTE) atoi(tstr);
         tstr = tcolor.Mid(3, 3);
         grn = (BYTE) atoi(tstr);
         tstr = tcolor.Mid(6, 3);
         blu = (BYTE) atoi(tstr);
         rgb_color = RGB(red, grn, blu);
         return rgb_color;
      }

      // *****************************************************************
      // *****************************************************************

      CString CFvwUtil::color_to_string(COLORREF color)
      {
         CString tstr;
         int red, grn, blu;

         red = (int) GetRValue(color);
         grn = (int) GetGValue(color);
         blu = (int) GetBValue(color);
         tstr.Format("%3d%3d%3d", red, grn, blu);
         return tstr;
      }

      int CFvwUtil::linear_color_to_fvw_color(int lin_color)
      {
         int fvw_color;

         fvw_color = 0;

         switch(lin_color)
         {
         case  0: fvw_color = BLACK; break;
         case  1: fvw_color = STD_RED; break;
         case  2: fvw_color = STD_GREEN; break;
         case  3: fvw_color = YELLOW; break;
         case  4: fvw_color = BLUE; break;
         case  5: fvw_color = MAGENTA; break;
         case  6: fvw_color = CYAN; break;
         case  7: fvw_color = LIGHT_GREY; break;
         case  8: fvw_color = PALE_GREEN; break;
         case  9: fvw_color = LIGHT_BLUE; break;
         case 10: fvw_color = OFF_WHITE; break;
         case 11: fvw_color = MEDIUM_GRAY; break;
         case 12: fvw_color = WHITE; break;
         case 13: fvw_color = STD_BRIGHT_RED; break;
         case 14: fvw_color = STD_BRIGHT_GREEN; break;
         case 15: fvw_color = BRIGHT_YELLOW; break;
         case 16: fvw_color = BRIGHT_BLUE; break;
         case 17: fvw_color = BRIGHT_MAGENTA; break;
         case 18: fvw_color = BRIGHT_CYAN; break;
         case 19: fvw_color = BRIGHT_WHITE; break;
         }
         return fvw_color;
      }
      //  end of linear_color_to_fvw_color

      // *************************************************************
      // *************************************************************

      int CFvwUtil::fvw_color_to_linear_color(int fvw_color)
      {
         int lin_color;

         lin_color = 0;

         switch(fvw_color)
         {
         case  BLACK: lin_color = 0; break;
         case  STD_RED: lin_color = 1; break;
         case  STD_GREEN: lin_color = 2; break;
         case  YELLOW: lin_color = 3; break;
         case  BLUE: lin_color = 4; break;
         case  MAGENTA: lin_color = 5; break;
         case  CYAN: lin_color = 6; break;
         case  LIGHT_GREY: lin_color = 7; break;
         case  PALE_GREEN: lin_color = 8; break;
         case  LIGHT_BLUE: lin_color = 9; break;
         case OFF_WHITE: lin_color = 10; break;
         case MEDIUM_GRAY: lin_color = 11; break;
         case WHITE: lin_color = 12; break;
         case STD_BRIGHT_RED: lin_color = 13; break;
         case STD_BRIGHT_GREEN: lin_color = 14; break;
         case BRIGHT_YELLOW: lin_color = 15; break;
         case BRIGHT_BLUE: lin_color = 16; break;
         case BRIGHT_MAGENTA: lin_color = 17; break;
         case BRIGHT_CYAN: lin_color = 18; break;
         case BRIGHT_WHITE: lin_color = 19; break;
         }
         return lin_color;
      }
      //  end of fvw_color_to_linear_color

      // compute the corners of a polygon (rotated rectangle) that encloses a block of text

      void CFvwUtil::compute_text_poly(int tx, int ty,  // x/y of anchor point
         int anchor_pos,  // position of anchor point
         int width, int height,  // height and width of text
         double text_angle,  // angle of text
         POINT *cpt)  // OUT - corners of rectangle enclosing text
      {
         gfx::compute_text_poly(tx, ty, anchor_pos, width, height, text_angle, cpt);
      }
      // end of compute_text_poly

      void CFvwUtil::get_text_size
         (
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         CString font_name,// font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int *width,       // width of draw text
         int *height,      // height of draw text
         BOOL pad_spaces,   // default is TRUE
         double angle     // default is 0
         )
      {
         gfx::get_text_size(dc->GetSafeHdc(), std::string(text), std::string(font_name), font_size, font_attrib,
            width, height, pad_spaces, angle);
      }
      // end of get_text_size

      void CFvwUtil::draw_text(
         CDC *dc,            // pointer to DC to draw in
         CString text,       // text to draw
         int x, int y,       // screen x,y position
         int anchor_pos,     // reference position of text
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int background,     // background type
         int text_color,     // code for text color
         int back_color,     // code for background color
         double angle,       // angle of text
         POINT *cpt,         // 4 point array defining the text polygon corners
         BOOL pad_spaces     // default is TRUE
         )
      {
         gfx::draw_text(dc->GetSafeHdc(), text, x, y, anchor_pos, font, font_size, font_attrib,
            background, text_color, back_color, angle, cpt, pad_spaces);
      }

      void CFvwUtil::draw_text_RGB(
         CDC *dc,            // pointer to DC to draw in
         CString text,       // text to draw
         int x, int y,       // screen x,y position
         int anchor_pos,     // reference position of text
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int background,     // background type
         COLORREF color,     // RGB for text color
         COLORREF bkcolor,   // RGB for background color
         double angle,       // angle of text
         POINT *cpt,         // 4 point array defining the text polygon corners
         BOOL pad_spaces     // default is TRUE
         )
      {
         gfx::draw_text_RGB(dc->GetSafeHdc(), text, x, y, anchor_pos, font,
            font_size, font_attrib, background, color, bkcolor, angle, cpt, pad_spaces);
      }

      void CFvwUtil::parse_the_text(CString text, CList<CString *, CString *> &text_list)
      {
         // We want to separate 'text' into a list of CStrings each of
         // which is contained on a single line.  In other words, we
         // need to look for any '\n' or '\r' escape sequences and break
         // apart the string at these locations.
         //TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         char *context;
         char *token = strtok_s(text.GetBuffer(text.GetLength()), "\n\r", &context);
         while (token)
         {
            text_list.AddTail(new CString(token));
            token = strtok_s(NULL, "\n\r", &context);
         }
      }
      // end of parse_the_text

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::draw_multi_line_text_RGB(
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         int x, int y,     // screen x,y position
         int anchor_pos,   // reference position of text
         CString font,     // font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int background,   // background type
         COLORREF text_color,   // RGB for text color
         COLORREF back_color,   // RGB for background color
         double angle,     // angle of text
         POINT *cpt,       // 4 point array defining the text polygon corners
         BOOL pad_spaces   // default is TRUE
         )
      {
         gfx::draw_multi_line_text_RGB(dc->GetSafeHdc(), text, x, y, anchor_pos,
            font, font_size, font_attrib, background, text_color, back_color,
            angle, cpt, pad_spaces);
      }

      void CFvwUtil::draw_multi_line_text
         (
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         int x, int y,     // screen x,y position
         int anchor_pos,   // reference position of text
         CString font,     // font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int background,   // background type
         int text_color,   // code for text color
         int back_color,   // code for background color
         double angle,     // angle of text
         POINT *cpt,       // 4 point array defining the text polygon corners
         BOOL pad_spaces   // default is TRUE
         )
      {
         gfx::draw_multi_line_text(dc->GetSafeHdc(), text, x, y, anchor_pos,
            font, font_size, font_attrib, background, text_color, back_color,
            angle, cpt, pad_spaces);
      }
      // end of draw_multi_line_text

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::draw_multi_line_text_aligned_RGB(
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         int x, int y,     // screen x,y position
         int anchor_pos,   // reference position of text
         CString font,     // font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int background,   // background type
         COLORREF color,   // RGB value for foreground color
         COLORREF bkcolor,   // RGB value for background color
         double angle,     // angle of text
         int alignment,    // alignment of text within box
         POINT *cpt,       // 4 point array defining the text polygon corners
         BOOL pad_spaces   // default is TRUE
         )
      {
         gfx::draw_multi_line_text_aligned_RGB(dc->GetSafeHdc(), text, x, y, anchor_pos,
            font, font_size, font_attrib, background, color, bkcolor, angle, alignment,
            cpt, pad_spaces);
      }

      void CFvwUtil::draw_multi_line_text_aligned
         (
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         int x, int y,     // screen x,y position
         int anchor_pos,   // reference position of text
         CString font,     // font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int background,   // background type
         int text_color,   // code for text color
         int back_color,   // code for background color
         double angle,     // angle of text
         int alignment,    // alignment of text within box
         POINT *cpt,       // 4 point array defining the text polygon corners
         BOOL pad_spaces   // default is TRUE
         )
      {
         draw_multi_line_text_aligned_RGB(dc, text, x, y, anchor_pos,
            font, font_size, font_attrib, background, code2color(text_color),
            code2color(back_color), angle, alignment, cpt, pad_spaces);
      }
      // end of draw_multi_line_text_aligned

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::get_multi_line_bounds
         (
         CDC *dc,          // pointer to DC to draw in
         CString text,     // text to draw
         int x, int y,     // screen x,y position
         int anchor_pos,   // reference position of text
         CString font,     // font name
         int font_size,    // font size in points
         int font_attrib,  // font attributes (bold, italic, etc)
         int background,   // background type
         double angle,     // angle of text
         POINT *cpt,       // 4 point array defining the text polygon corners
         BOOL pad_spaces   // default is TRUE
         )
      {
         gfx::get_multi_line_bounds(dc->GetSafeHdc(), std::string(text), x, y, anchor_pos, std::string(font),
            font_size, font_attrib, background, angle, cpt, pad_spaces);
      }
      // end of get_multi_line_bounds

      // ********************************************************************
      // ********************************************************************

      // compute the bounding rectangle for a line from point 1 to point 2 with the
      // given style and width
      CRect CFvwUtil::get_bounding_rect(int x1, int y1, int x2, int y2, int style,
         int width)
      {
         CRect rect(0,0,0,0);
         double thickness;

         // compute effective thickness of a horizontal line from the width and the
         // line style
         switch (style)
         {
         case UTIL_LINE_SOLID:
         case UTIL_LINE_DASH:
         case UTIL_LINE_DOT:
         case UTIL_LINE_DASHDOT:
         case UTIL_LINE_DASHDOTDOT:
         case UTIL_LINE_ROUND_DOT:
         case UTIL_LINE_DIAMOND:
         case UTIL_LINE_TMARK:
            thickness = (double)width/2.0;
            break;

         case UTIL_LINE_RAILROAD:
            thickness =  4.0 + (double)width/2.0;
            break;

         case UTIL_LINE_POWERLINE:
            thickness =  6.0 + (double)width/2.0;
            break;

         case UTIL_LINE_ZIGZAG:
            thickness = 4.0 + (double)width/2.0;
            break;

         case UTIL_LINE_ARROW:
         case UTIL_LINE_LONG_DASH:
         case UTIL_LINE_DASH2:
         case UTIL_LINE_DASH_DOT:
         case UTIL_LINE_SQUARE_DOT:
         case UTIL_LINE_DASH_EXT:
         case UTIL_LINE_DASH_EXT2:

         case UTIL_LINE_BORDER_START:
         case UTIL_LINE_BORDER_END:
         case UTIL_LINE_NOTCHED:
         case UTIL_LINE_FLOT2_START:
         case UTIL_LINE_FLOT2_END:
         case UTIL_LINE_FLOT3_START:
         case UTIL_LINE_FLOT3_END:
         case UTIL_LINE_FEBA2_START:
         case UTIL_LINE_FEBA2_END:
         case UTIL_LINE_WIRE:
         case UTIL_LINE_ARROW2_START:
         case UTIL_LINE_ARROW2_END:

            thickness = 10.0 + 10.0 * (double)width;
            break;

         default:
            return rect;
         }

         // if both end points are the same, not a line
         if (x1 == x2 && y1 == y2)
         {
            return rect;
         }

         // This keeps the angle between 0.0 and 90.0, where the sine and
         // cosine are both positive, to simplify the computations.
         rect.SetRect(x1, y1, x2, y2);
         rect.NormalizeRect();

         // compute the amount the rectangle must be expanded in each direction to
         // account for line width and line style
         const int expand_x = static_cast<int>(thickness + 0.5);
         const int expand_y = static_cast<int>(thickness + 0.5);

         // expand rectangle from basic rectangle including endpoints
         rect.InflateRect(expand_x, expand_y);

         return rect;
      }
      // end of get_bounding_rect

      // ********************************************************************
      // ********************************************************************

      // translates the utility class code to WINAPI code

      int CFvwUtil::code2fill(int code)
      {
         GraphicsUtilities graphics;
         return graphics.GetGdiFillCode(code);
      }
      // end of code2fill

      // ********************************************************************
      // *************************************************************

void CFvwUtil::compute_center_geo(d_geo_t ll, d_geo_t ur, degrees_t *lat, degrees_t *lon)
{
   d_geo_t geo[2];
   geo[0] = ll;
   geo[1] = ur;

   d_geo_t center_geo;
   GEO_center_of_mass(geo, 2, &center_geo);

   *lat = center_geo.lat;
   *lon = center_geo.lon;
}
// end of compute_center_geo

int CFvwUtil::get_calculated_width(MapProj* map, int meters_wide, double lat, double lon)
{
   int width;
   double lat2, lon2;
   int x1, y1, x2, y2;

   // find geo earth_width to the north
   GEO_distance_to_geo(lat, lon, meters_wide, 0.0, &lat2, &lon2);
   geo_to_view_short(map, lat, lon, &x1, &y1);
   geo_to_view_short(map, lat2, lon2, &x2, &y2);
   //  geo_to_view(map, lat, lon, &x1, &y1);
   //  geo_to_view(map, lat2, lon2, &x2, &y2);
   width = magnitude(x1, y1, x2, y2);
   if (width < 1)
      width = 1;

   return width;
}
// end of get_calculated_width

      boolean_t CFvwUtil::clip_t(int denom, int num, float *tE, float *tL)
      {
         float t;

         if (denom > 0)                   // PE intersection
         {
            t = (float)num/(float)denom;  // t at intersection
            if (t > *tL)                  // PE point after leaving
               return FALSE;

            if (t > *tE)                  // new PE point found
               *tE = t;
         }
         else if (denom < 0)              // PL intersection
         {
            t = (float)num/(float)denom;  // t at intersection
            if (t < *tE)                  // PL point before entering
               return FALSE;

            if (t < *tL)                  // new PL point found
               *tL = t;
         }
         else if (num > 0)                // line is parallel to edge and outside
            return FALSE;

         return TRUE;
      }
      // end of clip_t

      // ********************************************************************
      // ********************************************************************

      int CFvwUtil::clip_point(int x, int y, CRect rc)
      {
         int flags = 0;

         if (x > rc.right)
            flags = UTIL_CLIP_RIGHT;
         else if (x < rc.left)
            flags = UTIL_CLIP_LEFT;

         if (y > rc.bottom)
            flags |= UTIL_CLIP_TOP;
         else if (y < rc.top)
            flags |= UTIL_CLIP_BOTTOM;

         return flags;
      }
      // end of clip_point

      // ********************************************************************
      // ********************************************************************

      // returns the intersection with a the rect bounds given the two
      // endpoint of a line
      CPoint CFvwUtil::get_intersection(CPoint p1, CPoint p2, CRect rect)
      {
         int cx1 = p1.x;
         int cy1 = p1.y;
         int cx2 = p2.x;
         int cy2 = p2.y;

         if (clip_line(&cx1, &cy1, &cx2, &cy2, rect))
         {
            if (cx1 != p1.x || cy1 != p1.y)
               return CPoint(cx1, cy1);
            else
               return CPoint(cx2, cy2);
         }

         return CPoint(-1, -1);
      }

      // returns TRUE for visible and FALSE otherwise

      boolean_t CFvwUtil::clip_line(int *x1, int *y1, // first point in line
         int *x2, int *y2, // last point in line
         CRect rc)    // rectangle to clip to
      {
         int flags1;    // bit flags TBRL for point 1
         int flags2;    // bit flags TBRL for point 2
         int xor;       // flags1 XOR flags2
         float tE, tL;  // t at entering and leaving point
         int dx, dy;    // run and rise

         // get clip flags for points
         flags1 = clip_point(*x1, *y1, rc);
         flags2 = clip_point(*x2, *y2, rc);

         // quick accept
         if ((flags1 | flags2) == 0)
            return TRUE;

         // quick reject
         if (flags1 & flags2)
            return FALSE;

         // parametric line P(t) = P1 + (P2 - P1)t
         dx = *x2 - *x1;      // run
         dy = *y2 - *y1;      // rise
         tE = (float)0.0;      // t at P1
         tL = (float)1.0;      // t at P2

         // bit(s) set in xor indicate which edges are crossed
         xor = flags1 ^ flags2;

         // clip to left edge if crossed
         if (xor & UTIL_CLIP_LEFT)
         {
            if (clip_t(dx, rc.left - *x1, &tE, &tL) == FALSE)
               return FALSE;
         }

         // clip to right edge if crossed
         if (xor & UTIL_CLIP_RIGHT)
         {
            if (clip_t(-dx, *x1 - rc.right, &tE, &tL) == FALSE)
               return FALSE;
         }

         // clip to bottom edge if crossed
         if (xor & UTIL_CLIP_BOTTOM)
         {
            if (clip_t(dy, rc.top - *y1, &tE, &tL) == FALSE)
               return FALSE;
         }

         // clip to top edge if crossed
         if (xor & UTIL_CLIP_TOP)
         {
            if (clip_t(-dy, *y1 - rc.bottom, &tE, &tL) == FALSE)
               return FALSE;
         }

         // compute leaving intersection, if tL has moved
         if (tL < 1.0)
         {
            if (dx < 0)
               *x2 = *x1 + (int)(tL * (float)dx - 0.5);
            else
               *x2 = *x1 + (int)(tL * (float)dx + 0.5);

            if (dy < 0)
               *y2 = *y1 + (int)(tL * (float)dy - 0.5);
            else
               *y2 = *y1 + (int)(tL * (float)dy + 0.5);
         }

         // compute leaving intersection, if tE has moved
         if (tE > 0.0)
         {
            if (dx < 0)
               *x1 = *x1 + (int)(tE * (float)dx - 0.5);
            else
               *x1 = *x1 + (int)(tE * (float)dx + 0.5);

            if (dy < 0)
               *y1 = *y1 + (int)(tE * (float)dy - 0.5);
            else
               *y1 = *y1 + (int)(tE * (float)dy + 0.5);
         }

         return TRUE;
      }
      // end of clip_line

      // ********************************************************************
      // ********************************************************************

      // draw ellipse with black background

      void CFvwUtil::draw_geo_ellipse(MapProj* map, CDC* dc, double lat, double lon,
         double vert, double horz, double angle,
         int line_color, int fill_color, int width,
         int line_style, int fill_style, BOOL xor,
         int bd_pt_cnt, POINT* bd_pt,
         CRect * bounds)
      {
         int k;
         CBrush brush;
         CPen pen, whitepen;
         COLORREF color, fcolor;
         int x, y, x2, y2;
         double rang,tang, radius, ang;
         double maxrad,minrad;
         double maxRadSq,minRadSq,RadSq;
         double sinang, cosang;
         double ainc, nlat, nlon;
         double dtmp;
         double *plat, *plon;
         int olddrawmode;
         BOOL fill;
         BOOL bigscr;
         int pixels, screen_width, screen_height;

         if (bd_pt_cnt < 4)
            return;
         if (bd_pt_cnt > 500)
            return;

         map->get_pixels_around_world(&pixels);
         map->get_vsurface_size(&screen_width, &screen_height);
         pixels = (pixels / 2) - 10;
         if (pixels < screen_width)
            bigscr = TRUE;
         else
            bigscr = FALSE;


         // find the center x,y
         geo_to_view_short(map, lat, lon, &x, &y);

         maxrad = vert;
         minrad = horz;
         maxRadSq = maxrad * maxrad;
         minRadSq = minrad * minrad;
         RadSq = maxRadSq * minRadSq;

         // prevent division by zero
         if ((maxrad == 0) && (minrad == 0))
            return;

         plat = (double*) MEM_malloc(bd_pt_cnt * sizeof(double));
         if (plat == NULL)
         {
            ERR_report("Unable to allocate memory for draw_geo_ellipse");
            return;
         }
         plon = (double*) MEM_malloc(bd_pt_cnt * sizeof(double));
         if (plon == NULL)
         {
            MEM_free(plat);
            ERR_report("Unable to allocate memory for draw_geo_ellipse");
            return;
         }

         ainc = 360.0/ (double) bd_pt_cnt;
         ang = 0.0;

         for (k=0; k<bd_pt_cnt; k++)
         {
            tang = ang;
            tang += angle;
            if (tang > 360.0)
               tang -= 360.0;
            rang = ang * PI/180;
            sinang = sin(rang);
            cosang = cos(rang);
            dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
            // prevent division by zero
            if (dtmp == 0.0)
               return;
            radius = sqrt(RadSq / dtmp);
            GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
            plat[k] = nlat;
            plon[k] = nlon;
            geo_to_view_short(map, nlat, nlon, &x2, &y2);

            bd_pt[k].x = x2;
            bd_pt[k].y = y2;
            ang = ang + ainc;
         }

         // get bounds
         bounds->SetRect(x, y, x, y);
         for (k=0; k<bd_pt_cnt; k++)
         {
            if (bounds->top > bd_pt[k].y)
               bounds->top = bd_pt[k].y;
            if (bounds->bottom < bd_pt[k].y)
               bounds->bottom = bd_pt[k].y;
            if (bounds->left > bd_pt[k].x)
               bounds->left = bd_pt[k].x;
            if (bounds->right < bd_pt[k].x)
               bounds->right = bd_pt[k].x;
         }

         if (bigscr || (fill_style == UTIL_FILL_NONE))
         {
            nlat = plat[0];
            nlon = plat[0];
            for (k=0; k<bd_pt_cnt-1; k++)
            {
               draw_geo_line(map, dc, plat[k], plon[k], plat[k+1], plon[k+1],
                  line_color, line_style, width,
                  UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
            }
            draw_geo_line(map, dc, plat[bd_pt_cnt-1], plon[bd_pt_cnt-1], plat[0], plon[0],
               line_color, line_style, width,
               UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
            MEM_free(plat);
            MEM_free(plon);
            return;
         }

         color = code2color(line_color);
         fcolor = code2color(fill_color);
         brush.CreateSolidBrush(fcolor);
         pen.CreatePen(PS_SOLID, width, color);
         CBrush* oldbrush = dc->SelectObject(&brush);
         CPen* oldpen = dc->SelectObject(&pen);
         CBrush nullbrush;
         nullbrush.CreateStockObject(NULL_BRUSH);
         dc->SelectObject(&nullbrush);
         brush.DeleteObject();
         fill = TRUE;
         switch (fill_style)
         {
         case UTIL_FILL_HORZ:
         case UTIL_FILL_VERT:
         case UTIL_FILL_BDIAG:
         case UTIL_FILL_FDIAG:
         case UTIL_FILL_CROSS:
         case UTIL_FILL_DIAGCROSS:
            brush.CreateHatchBrush(code2fill(fill_style), fcolor);
            break;
         case UTIL_FILL_SOLID:
            brush.CreateSolidBrush(fcolor);
            break;
         default:
            fill = FALSE;
         }
         if (fill)
            dc->SelectObject(&brush);

         int oldbkmode = dc->SetBkMode(TRANSPARENT);

         if (xor)
         {
            olddrawmode = dc->SetROP2(R2_XORPEN);
            whitepen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            dc->SelectObject(&whitepen);
            dc->SelectObject(&nullbrush);
         }

         dc->Polygon(bd_pt, bd_pt_cnt);

         if (!xor)
         {
            CPen blackpen;
            blackpen.CreatePen(PS_SOLID, width+2, RGB(0,0,0));
            dc->SelectObject(&blackpen);
            dc->SelectObject(&nullbrush);
            dc->Polygon(bd_pt, bd_pt_cnt);
            dc->SelectObject(&pen);
            dc->Polygon(bd_pt, bd_pt_cnt);
            blackpen.DeleteObject();
         }

         if (xor)
         {
            // draw the center cross
            dc->MoveTo(x - 9, y);
            dc->LineTo(x + 10, y);
            dc->MoveTo(x,     y - 9);
            dc->LineTo(x,     y + 10);
            dc->SetROP2(olddrawmode);
            dc->SelectObject(oldpen);
            whitepen.DeleteObject();
         }

         dc->SelectObject(oldpen);
         dc->SelectObject(oldbrush);
         dc->SetBkMode(oldbkmode);
         pen.DeleteObject();
         brush.DeleteObject();
         nullbrush.DeleteObject();
         MEM_free(plat);
         MEM_free(plon);
      }
      // end of draw_geo_ellipse

      // *************************************************************
      // ********************************************************************

      // draw ellipse with black background

      void CFvwUtil::draw_geo_ellipse_simple(MapProj* map, CDC* dc, double lat, double lon,
         double vert, double horz, double angle,
         int line_color, int fill_color, int width,
         int line_style, int fill_style, BOOL xor,
         int bd_pt_cnt, POINT* bd_pt,
         CRect * bounds)
      {
         int k;
         int x, y, x2, y2;
         double rang,tang, radius, ang;
         double maxrad,minrad;
         double maxRadSq,minRadSq,RadSq;
         double sinang, cosang;
         double ainc, nlat, nlon;
         double dtmp;

         if (bd_pt_cnt < 4 || bd_pt_cnt > 500)
            return;

         // find the center x,y
         map->geo_to_surface(lat, lon, &x, &y);

         maxrad = vert;
         minrad = horz;
         maxRadSq = maxrad * maxrad;
         minRadSq = minrad * minrad;
         RadSq = maxRadSq * minRadSq;

         // prevent division by zero
         if ((maxrad == 0) && (minrad == 0))
            return;

         ainc = 360.0/ (double) bd_pt_cnt;
         ang = 0.0;

         // compute the screen coordinates for the geo-ellipse
         CLineSegmentResizableArray lineSegments(TRUE);
         for (k=0; k<bd_pt_cnt; k++)
         {
            tang = ang;
            tang += angle;
            if (tang > 360.0)
               tang -= 360.0;
            rang = ang * PI/180;
            sinang = sin(rang);
            cosang = cos(rang);
            dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
            // prevent division by zero
            if (dtmp == 0.0)
               return;
            radius = sqrt(RadSq / dtmp);
            GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
            map->geo_to_surface(nlat, nlon, &x2, &y2);
            lineSegments.AddPoint(CPoint(x2, y2));

            bd_pt[k].x = x2;
            bd_pt[k].y = y2;
            ang = ang + ainc;
         }

         // render the geo-ellipse to the screen
         UtilDraw utilDraw(dc);
         utilDraw.set_pen(line_color, line_style, width);
         utilDraw.DrawPolyLine(lineSegments);

         // get bounds
         bounds->SetRect(x, y, x, y);
         for (k=0; k<bd_pt_cnt; k++)
         {
            if (bounds->top > bd_pt[k].y)
               bounds->top = bd_pt[k].y;
            if (bounds->bottom < bd_pt[k].y)
               bounds->bottom = bd_pt[k].y;
            if (bounds->left > bd_pt[k].x)
               bounds->left = bd_pt[k].x;
            if (bounds->right < bd_pt[k].x)
               bounds->right = bd_pt[k].x;
         }
      }
      // end of draw_geo_ellipse_simple

      // draw ellipse without black background

      void CFvwUtil::draw_geo_ellipse2(MapProj* map, CDC* dc, double lat, double lon,
         double vert, double horz, double angle,
         int line_color, int fill_color, int width,
         int line_style, int fill_style, BOOL xor,
         int bd_pt_cnt, POINT* bd_pt,
         CRect * bounds)
      {
         int k;
         CBrush brush;
         CPen pen, whitepen;
         COLORREF color, fcolor;
         int x, y, x2, y2;
         double rang,tang, radius, ang;
         double maxrad,minrad;
         double maxRadSq,minRadSq,RadSq;
         double sinang, cosang;
         double ainc, nlat, nlon;
         double dtmp;
         int olddrawmode;
         BOOL fill;

         if (bd_pt_cnt < 4)
            return;
         if (bd_pt_cnt > 500)
            return;

         // find the center x,y
         geo_to_view_short(map, lat, lon, &x, &y);

         maxrad = vert;
         minrad = horz;
         maxRadSq = maxrad * maxrad;
         minRadSq = minrad * minrad;
         RadSq = maxRadSq * minRadSq;

         // prevent division by zero
         if ((maxrad == 0) && (minrad == 0))
            return;

         ainc = 360.0/ (double) bd_pt_cnt;
         ang = 0.0;

         for (k=0; k<bd_pt_cnt; k++)
         {
            tang = ang;
            tang += angle;
            if (tang > 360.0)
               tang -= 360.0;
            rang = ang * PI/180;
            sinang = sin(rang);
            cosang = cos(rang);
            dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
            // prevent division by zero
            if (dtmp == 0.0)
               return;
            radius = sqrt(RadSq / dtmp);
            GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
            geo_to_view_short(map, nlat, nlon, &x2, &y2);

            bd_pt[k].x = x2;
            bd_pt[k].y = y2;
            ang = ang + ainc;
         }

         color = code2color(line_color);
         fcolor = code2color(fill_color);
         brush.CreateSolidBrush(fcolor);
         pen.CreatePen(PS_SOLID, width, color);
         CBrush* oldbrush = dc->SelectObject(&brush);
         CPen* oldpen = dc->SelectObject(&pen);
         CBrush nullbrush;
         nullbrush.CreateStockObject(NULL_BRUSH);
         dc->SelectObject(&nullbrush);
         brush.DeleteObject();
         fill = TRUE;
         switch (fill_style)
         {
         case UTIL_FILL_HORZ:
         case UTIL_FILL_VERT:
         case UTIL_FILL_BDIAG:
         case UTIL_FILL_FDIAG:
         case UTIL_FILL_CROSS:
         case UTIL_FILL_DIAGCROSS:
            brush.CreateHatchBrush(code2fill(fill_style), fcolor);
            break;
         case UTIL_FILL_SOLID:
            brush.CreateSolidBrush(fcolor);
            break;
         default:
            fill = FALSE;
         }
         if (fill)
            dc->SelectObject(&brush);

         int oldbkmode = dc->SetBkMode(TRANSPARENT);

         if (xor)
         {
            olddrawmode = dc->SetROP2(R2_XORPEN);
            whitepen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            dc->SelectObject(&whitepen);
            dc->SelectObject(&nullbrush);
         }

         dc->Polygon(bd_pt, bd_pt_cnt);

         if (!xor)
         {
            dc->SelectObject(&pen);
            dc->Polygon(bd_pt, bd_pt_cnt);
         }

         if (xor)
         {
            // draw the center cross
            dc->MoveTo(x - 9, y);
            dc->LineTo(x + 10, y);
            dc->MoveTo(x,     y - 9);
            dc->LineTo(x,     y + 10);
            dc->SetROP2(olddrawmode);
            dc->SelectObject(oldpen);
            whitepen.DeleteObject();
         }

         // get bounds
         bounds->SetRect(x, y, x, y);
         for (k=0; k<bd_pt_cnt; k++)
         {
            if (bounds->top > bd_pt[k].y)
               bounds->top = bd_pt[k].y;
            if (bounds->bottom < bd_pt[k].y)
               bounds->bottom = bd_pt[k].y;
            if (bounds->left > bd_pt[k].x)
               bounds->left = bd_pt[k].x;
            if (bounds->right < bd_pt[k].x)
               bounds->right = bd_pt[k].x;
         }

         dc->SelectObject(oldpen);
         dc->SelectObject(oldbrush);
         dc->SetBkMode(oldbkmode);
         pen.DeleteObject();
         brush.DeleteObject();
         nullbrush.DeleteObject();
      }
      // end of draw_geo_ellipse2

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::get_geo_ellipse_bounds(MapProj* map, double lat, double lon,
         double vert, double horz,
         double angle, CRect * bounds,
         d_geo_t * ll, d_geo_t * ur)
      {
         int k;
         int x, y, x2, y2;
         double rang,tang, radius, ang;
         double maxrad,minrad;
         double maxRadSq,minRadSq,RadSq;
         double sinang, cosang;
         double ainc, nlat, nlon;
         int bd_pt_cnt = 60;
         double ValOf_DEG_TO_RAD = 0.01745329251994;

         // find the center x,y
         geo_to_view_short(map, lat, lon, &x, &y);
         bounds->SetRect(x, y, x, y);
         ll->lat = lat;
         ll->lon = lon;
         ur->lat = lat;
         ur->lon = lon;

         maxrad = vert;
         minrad = horz;
         maxRadSq = maxrad * maxrad;
         minRadSq = minrad * minrad;
         RadSq = maxRadSq * minRadSq;

         // prevent division by zero
         if ((maxrad == 0) && (minrad == 0))
            return;

         ainc = 360.0/ (double) bd_pt_cnt;
         ang = 0.0;

         for (k=0; k<bd_pt_cnt; k++)
         {
            tang = ang;
            tang += angle;
            if (tang > 360.0)
               tang -= 360.0;
            rang = DEG_TO_RAD(ang);
            sinang = sin(rang);
            cosang = cos(rang);
            radius = sqrt(RadSq / ((minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang)));
            GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
            geo_to_view_short(map, nlat, nlon, &x2, &y2);
            ang = ang + ainc;
            if (ll->lat > nlat)
               ll->lat = nlat;
            if (ur->lat < nlat)
               ur->lat = nlat;
            if (GEO_east_of_degrees(ll->lon, nlon))
               ll->lon = nlon;
            if (GEO_east_of_degrees(nlon, ur->lon))
               ur->lon = nlon;
            if (bounds->top > y2)
               bounds->top = y2;
            if (bounds->bottom < y2)
               bounds->bottom = y2;
            if (bounds->left > x2)
               bounds->left = x2;
            if (bounds->right < x2)
               bounds->right = x2;
         }
      }
      // end of get_geo_ellipse_bounds

      // ********************************************************************
      // ********************************************************************

      // attempt to execute the file using its associated program
      BOOL CFvwUtil::shell_execute(char * filename, int* error_code)
      {
         return shell_execute(filename, NULL, error_code);
      }
      // end of shell_execute

      // attempt to execute the file using its associated program
      BOOL CFvwUtil::shell_execute(char * filename, char * parameters, int* error_code)
      {
         int i;
         if (error_code == NULL) error_code = &i;

         HINSTANCE hinst;

         hinst = ::ShellExecute(::GetDesktopWindow(), "open", filename, parameters, NULL, SW_SHOW);
         if (hinst <= (HANDLE)32)
         {
            hinst = ::ShellExecute(::GetDesktopWindow(), "edit", filename, parameters, NULL, SW_SHOW);
            if (hinst <= (HANDLE)32)
            {
               *error_code = (int) hinst;
               return FALSE;
            }
         }

         *error_code = 0;
         return TRUE;
      }
      // end of shell_execute

      // ********************************************************************
      // ********************************************************************

      // display shell_execute error message to user
      BOOL CFvwUtil::shell_execute_error_display(int error_code)
      {
         CString msg;

         if (error_code > 32)
            return FALSE;

         switch ((int) error_code)
         {
         case 0:   msg = "The operating system is out of memory or resources."; break;
         case ERROR_FILE_NOT_FOUND:   msg = "The specified file was not found."; break;
         case ERROR_PATH_NOT_FOUND:   msg = "The specified path was not found."; break;
         case ERROR_BAD_FORMAT:   msg = "The .EXE file is invalid (non-Win32 .EXE or \n"
                                     "error in .EXE image)."; break;
         case SE_ERR_ACCESSDENIED:   msg = "The operating system denied access to the \n"
                                        "specified file."; break;
         case SE_ERR_ASSOCINCOMPLETE:   msg = "The filename association is incomplete \n"
                                           "or invalid."; break;
         case SE_ERR_DDEBUSY:   msg = "The DDE transaction could not be completed because \n"
                                   "other DDE transactions were being processed."; break;
         case SE_ERR_DDEFAIL:   msg = "The DDE transaction failed."; break;
         case SE_ERR_DDETIMEOUT:   msg = "The DDE transaction could not be completed \n"
                                      "because the request timed out."; break;
         case SE_ERR_DLLNOTFOUND: msg = "The specified dynamic-link library was not found."; break;
         case SE_ERR_NOASSOC:   msg = "There is no application associated with the given \n"
                                   "filename extension."; break;
         case SE_ERR_OOM:   msg = "There was not enough memory to complete the operation."; break;
         case SE_ERR_SHARE:   msg = "A sharing violation occurred."; break;
         default: msg = "Unknown Error"; break;
         }
         AfxMessageBox(msg);
         return TRUE;
      }
      // end of shell_execute_error_display

      // end of copy_a_file

      int CFvwUtil::ensure_directory(const char* path)
      {
         if (FIL_access((char*) path, FIL_EXISTS) == FAILURE)
         {
            // create the directory
            if (FIL_create_directory((char*) path) == FAILURE)
            {
               ERR_report("FIL_create_directory() failed.");
               return FAILURE;
            }
         }

         if (FIL_access(path, FIL_READ_OK | FIL_WRITE_OK) == FAILURE)
         {
            // change the permissions
            if (FIL_set_permissions(path, FIL_READ_OK | FIL_WRITE_OK) == FAILURE)
            {
               ERR_report("FIL_set_permissions() failed.");
               return FAILURE;
            }
         }

         return SUCCESS;
      }

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::build_string(char* outstr, char* instr, int num)
      {
         strncpy_s(outstr, num + 1, instr, num);
         outstr[num] = '\0';
      }
      // end of build_string

      // ********************************************************************
      // ********************************************************************

      void CFvwUtil::remove_leading_zeros(char *txt)
      {
         int k, len;

         len = strlen(txt);
         k = 0;
         while ((k < len) && (txt[k] == '0'))
         {
            txt[k] = ' ';
            k++;
         }
      }
      // end of remove_leading_zeros

      // ********************************************************************
      // ********************************************************************

      // convert a CString to a string

      void CFvwUtil::cstring_to_string(CString in, char *out, int max_char_len)
      {
         int cnt;

         out[0] = '\0';
         cnt = in.GetLength();
         if (cnt < 1)
            return;

         if (cnt >= max_char_len)
            cnt = max_char_len - 1;

         TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         strncpy_s(out, cnt + 1, in.GetBuffer(cnt), cnt);
         out[cnt] = '\0';
      }
      // end of cstring_to_string

      // Find name associated with 2 character country code
      // load_country_codes() no longer needs to be called first
      // codes updated 3/2/98

      BOOL CFvwUtil::find_country(char *code, char *name)
      {
         BOOL found;
         int k;

         name[0] = '\0';
         found = FALSE;
         k = 0;
         while (!found && (k <UTIL_COUNTRY_NUM))
         {
            if ((util_country[k].code[0] == code[0]) && (util_country[k].code[1] == code[1]))
            {
               strncpy_s(name, 41, util_country[k].name, 40);
               found = TRUE;
            }
            else
               k++;
         }
         return found;
      }
      // end of find_country

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::get_first_country(char *name, const int NAME_LEN, char *code, const int CODE_LEN)
      {
         strcpy_s(name, NAME_LEN, util_country[0].name);
         strcpy_s(code, CODE_LEN, util_country[0].code);
         m_country_pos = 1;

         return TRUE;
      }
      // end of get_first_country

      // ********************************************************************
      // ********************************************************************

      BOOL CFvwUtil::get_next_country(char *name, const int NAME_LEN, char *code, const int CODE_LEN)
      {
         if (m_country_pos >= UTIL_COUNTRY_NUM)
            return FALSE;

         strcpy_s(name, NAME_LEN, util_country[m_country_pos].name);
         strcpy_s(code, CODE_LEN, util_country[m_country_pos].code);
         m_country_pos++;

         return TRUE;
      }
      // end of get_first_country

      // ********************************************************************
      // ********************************************************************

      // Find name associated with 2 character state code (FIPS Code)
      // load_country_codes() no longer needs to be called first

      BOOL CFvwUtil::find_state(char *code, char *name)
      {
         BOOL found;
         int k;

         name[0] = '\0';
         found = FALSE;
         k = 0;
         while (!found && (k <UTIL_STATE_NUM))
         {
            if ((util_state[k].code[0] == code[0]) && (util_state[k].code[1] == code[1]))
            {
               TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
               strncpy_s(name, 100, util_state[k].name, 40);
               found = TRUE;
            }
            else
               k++;
         }
         return found;
      }
      // end of find_state

      // ****************************************************************************
      // ****************************************************************************

      // Find name associated with 2 character state code (FIPS Code)

      BOOL CFvwUtil::find_state_abbrev(char *code, char *abbrev)
      {
         BOOL found;
         int k;

         abbrev[0] = '\0';

         found = FALSE;
         k = 0;
         while (!found && (k <UTIL_STATE_NUM))
         {
            if ((util_state[k].code[0] == code[0]) && (util_state[k].code[1] == code[1]))
            {
               TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
               strncpy_s(abbrev, 3, util_state[k].abbrev, 2);
               abbrev[2] = '\0';
               found = TRUE;
            }
            else
               k++;
         }
         return found;
      }
      // end of find_state

      // ***********************************************************************
      // ***********************************************************************

      boolean_t CFvwUtil::is_point_in_client_area(CWnd* view, CPoint pt)
      {
         RECT rc;

         view->GetClientRect(&rc);
         if ((pt.x >= rc.left) && (pt.x <= rc.right) &&
            (pt.y >= rc.top) && (pt.y <= rc.bottom))
            return TRUE;
         else
            return FALSE;
      }
      // end of is_point_in_client_area

      // ***********************************************************************
      // ***********************************************************************

      // returns a color that is the dark equivalent of the given color
      // used to show an object is not currently selected
      int CFvwUtil::get_dark_color(int color)
      {
         // define colors
         int colors[20] = { UTIL_COLOR_BLACK, UTIL_COLOR_DARK_RED, UTIL_COLOR_DARK_GREEN,
            UTIL_COLOR_DARK_YELLOW, UTIL_COLOR_DARK_BLUE, UTIL_COLOR_DARK_MAGENTA,
            UTIL_COLOR_DARK_CYAN, UTIL_COLOR_LIGHT_GRAY, UTIL_COLOR_MONEY_GREEN,
            UTIL_COLOR_SKY_BLUE, UTIL_COLOR_CREAM, UTIL_COLOR_MEDIUM_GRAY,
            UTIL_COLOR_DARK_GRAY, UTIL_COLOR_RED, UTIL_COLOR_GREEN, UTIL_COLOR_YELLOW,
            UTIL_COLOR_BLUE, UTIL_COLOR_MAGENTA, UTIL_COLOR_CYAN, UTIL_COLOR_WHITE };
         // define the mapping to be used
         int backcolor[20] = { UTIL_COLOR_BLACK, UTIL_COLOR_DARK_RED, UTIL_COLOR_DARK_GREEN,
            UTIL_COLOR_DARK_YELLOW, UTIL_COLOR_DARK_BLUE, UTIL_COLOR_DARK_MAGENTA,
            UTIL_COLOR_DARK_CYAN, UTIL_COLOR_LIGHT_GRAY, UTIL_COLOR_DARK_GREEN,
            UTIL_COLOR_DARK_BLUE, UTIL_COLOR_CREAM, UTIL_COLOR_LIGHT_GRAY, UTIL_COLOR_LIGHT_GRAY,
            UTIL_COLOR_DARK_RED, UTIL_COLOR_DARK_GREEN,  UTIL_COLOR_DARK_YELLOW,
            UTIL_COLOR_DARK_BLUE, UTIL_COLOR_DARK_MAGENTA, UTIL_COLOR_DARK_CYAN,
            UTIL_COLOR_LIGHT_GRAY };
         for (int i=0;i<20;i++)
            if (color == colors[i])
               return backcolor[i];

         // if the given color is not found in the color table then return black
         return (UTIL_COLOR_BLACK);
      }

      // converts a FalconView color to a route server color
      short CFvwUtil::color2rscolor(int color)
      {
         switch (color) {
         case UTIL_COLOR_BLACK:
            return RS_COLOR_BRIGHT_BLACK;
         case UTIL_COLOR_RED:
            return RS_COLOR_BRIGHT_RED;
         case UTIL_COLOR_GREEN:
            return RS_COLOR_BRIGHT_GREEN;
         case UTIL_COLOR_YELLOW:
            return RS_COLOR_BRIGHT_YELLOW;
         case UTIL_COLOR_BLUE:
            return RS_COLOR_BRIGHT_BLUE;
         case UTIL_COLOR_MAGENTA:
            return RS_COLOR_BRIGHT_MAGENTA;
         case UTIL_COLOR_CYAN:
            return RS_COLOR_BRIGHT_CYAN;
         case UTIL_COLOR_WHITE:
            return RS_COLOR_BRIGHT_WHITE;
         default:
            return 0;
         }
      }

      // converts a color from the route server to a FalconView color
      int CFvwUtil::rscolor2color(short color)
      {
         switch (color) {
         case RS_COLOR_BRIGHT_BLACK:
            return UTIL_COLOR_BLACK;
         case RS_COLOR_BRIGHT_RED:
            return UTIL_COLOR_RED;
         case RS_COLOR_BRIGHT_GREEN:
            return UTIL_COLOR_GREEN;
         case RS_COLOR_BRIGHT_YELLOW:
            return UTIL_COLOR_YELLOW;
         case RS_COLOR_BRIGHT_BLUE:
            return UTIL_COLOR_BLUE;
         case RS_COLOR_BRIGHT_MAGENTA:
            return UTIL_COLOR_MAGENTA;
         case RS_COLOR_BRIGHT_CYAN:
            return UTIL_COLOR_CYAN;
         case RS_COLOR_BRIGHT_WHITE:
            return UTIL_COLOR_WHITE;
         default:
            return 0;
         }
      }

      // *****************************************************************
      // *****************************************************************

      // open the cfps executable if it is not already opened
      void CFvwUtil::open_cfps(void)
      {
         HWND hWnd;
         CString key("Executables");

         CString window_name = PRM_get_registry_string(key + "\\CFPS","window_name",
            "Combat Flight Planning Software");

         // maximize window if the process is already running
         if (GetExternalApplicationIsOpen(window_name, &hWnd, FALSE))
         {
            WINDOWPLACEMENT wndpl;
            ::GetWindowPlacement(hWnd, &wndpl);
            wndpl.showCmd = SW_RESTORE;
            ::SetWindowPlacement(hWnd, &wndpl);
            return;
         }

         CString executable = PRM_get_registry_string(key + "\\CFPS","Path","");

         // Check for exiting executable name in the registry
         if (executable == "") {

            return;
         }

         //[EDR 2007-21592] Collaboration On Local Point File Drops points on initial subscribe.
         //The problem was caused by a UDP socket staying open in the case where FalconView was
         //exited but CFPS was spawned (using _spawnl) and still running.
         // With _spawnl, a child process inherits the file handles that the parent opened.
         // Since file handles and socket descriptors are the same thing in Win32,
         //the reference count of the socket handle in the Winsock dll was never being decremented
         // to zero so the socket was staying open.
         //The fix was to use CreateProcess instead of _spawnl, which does not inherit file handles by default.

         STARTUPINFO si;
         PROCESS_INFORMATION pi;
         ZeroMemory( &si, sizeof(si) );
         si.cb = sizeof(si);
         ZeroMemory( &pi, sizeof(pi) );
         ::CreateProcess( NULL, const_cast<char *>((LPCSTR)executable), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
      }

      // close the cfps executable
      void CFvwUtil::close_cfps(void)
      {
         HWND hWnd;

         // minimize the window if the process is running
         if (GetExternalApplicationIsOpen("Combat Flight Planning Software",
            &hWnd, FALSE)) {
               WINDOWPLACEMENT wndpl;
               ::GetWindowPlacement(hWnd, &wndpl);
               wndpl.showCmd = SW_MINIMIZE;
               ::SetWindowPlacement(hWnd, &wndpl);
               return;
         }
      }

      // Returns true if the application specified by its window
      // name is open and not minimized
      boolean_t CFvwUtil::GetExternalApplicationIsOpen(CString windowName,
         HWND *hWnd,
         boolean_t minimized_is_closed /*TRUE*/)
      {
         WINDOWPLACEMENT lpwndpl;
         int name_length = windowName.GetLength();
         boolean_t visible;

         // exclude FalconView as a external window
         CWnd *pMainWnd = AfxGetMainWnd();
         HWND hMainWnd = NULL;
         if (pMainWnd != NULL)
            hMainWnd = pMainWnd->m_hWnd;

         // Loop through list of windows starting from the desktop up and
         // determine if any windows with 'windowName' exist.  If so, check
         // to see if this window is visible and not minimized or maximized.
         CWnd *pWnd = (AfxGetMainWnd())->GetDesktopWindow()->GetTopWindow();
         CString window_text;
         while (pWnd)
         {
            // skip FalconView window
            if (pWnd->m_hWnd == hMainWnd)
            {
               pWnd = pWnd->GetWindow(GW_HWNDNEXT);
               continue;
            }

            // Get the window placement of the current window which is used to
            // determine if a window is maximized, minimized, or normal.  Also used
            // to determine if a window is a tooltip.
            if (::IsWindow(pWnd->m_hWnd))
               pWnd->GetWindowPlacement(&lpwndpl);
            else
               return FALSE;

            // Get the title of the current window
            if (::IsWindow(pWnd->m_hWnd))
               pWnd->GetWindowText(window_text);
            else
               return FALSE;

            // Determines if a window is considered visible or not
            if (::IsWindow(pWnd->m_hWnd))
               if (minimized_is_closed)
                  visible = ((lpwndpl.showCmd == SW_SHOWNORMAL || lpwndpl.showCmd == SW_SHOWMAXIMIZED)
                  && (pWnd->GetStyle() & WS_VISIBLE));
               else
                  visible = TRUE;
            else
               return FALSE;

            // If the window exists and the window is considered visible then return TRUE
            if (window_text.Find(windowName) != -1 && visible) {
               if (::IsWindow(pWnd->m_hWnd))
                  *hWnd = pWnd->m_hWnd;

               // This gets rid of regarding tool tips as windows
               if (::GetWindowLong(pWnd->m_hWnd, GWL_EXSTYLE) == WS_EX_TOOLWINDOW)
                  return FALSE;

               return TRUE;
            }

            // Get the next window
            if (::IsWindow(pWnd->m_hWnd))
               pWnd = pWnd->GetWindow(GW_HWNDNEXT);
            else
               return FALSE;
         }
         return FALSE;
      }


      // *****************************************************************
      // *****************************************************************

      // rounds num to the resolution of res (1, 0.1, 0.01, 0.001)
      double CFvwUtil::round_to_res(double num, double res)
      {
         double tf;
         int ti;

         tf = num;

         if (res <= 0.0)
            return tf;

         tf += 0.5 * res;
         tf /= res;
         ti = (int) tf;
         tf = (double) ti;
         tf *= res;
         return tf;
      }
      // end of round_to_res

      // round to nearest integer
      int CFvwUtil::round(double val)
      {
         return (val > 0.0 ? ((int) (val + 0.5)) : ((int) (val - 0.5)));
      }

      // *****************************************************************
      // *****************************************************************

      // This function returns one of five standard cursors (IDC_SIZEALL, IDC_SIZENS,
      // IDC_SIZENWSE, IDC_SIZEWE, IDC_SIZENESW) based on where an object is
      // selected, the map rotation, and the rotation of the object.  nesw indicates
      // what part of the object is selected.  See the UTL_IDC_SIZE #defines for a
      // list of valid nesw values.
      //    -UTL_IDC_SIZE_NSEW will always cause IDC_SIZEALL to be returned.
      //    -All other valid nesw inputs will return a cursor that depends on map
      //     rotation and object rotation.
      //    -Invalid nesw inputs will return IDC_ARROW
      HCURSOR UTL_get_IDC_SIZE_cursor(int nesw, double cw_rotation_map, double cw_rotation_object)
      {
         // the move cursor is always the same
         if (nesw == UTL_IDC_SIZE_NSEW)
            return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

         // set index as if it is an index into an array of HCURSOR values
         int index;
         switch (nesw)
         {
         case UTL_IDC_SIZE_N:
         case UTL_IDC_SIZE_S:
            index = 0;     // IDC_SIZENS - 0.0 or 180.0
            break;

         case UTL_IDC_SIZE_NW:
         case UTL_IDC_SIZE_SE:
            index = 1;     // IDC_SIZENWSE - 45.0 or 225.0
            break;

         case UTL_IDC_SIZE_E:
         case UTL_IDC_SIZE_W:
            index = 2;     // IDC_SIZEWE - 90.0 or 270.0
            break;

         case UTL_IDC_SIZE_NE:
         case UTL_IDC_SIZE_SW:
            index = 3;     // IDC_SIZENESW - 135.0 or 315.0
            break;

            // this should never happen, but just in case
         default:
            return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
         }

         // The above index gives the correct cursor on a North Up map.  When the map
         // and/or the object is rotated, the index needs to be adjusted so an
         // appropriate cursor is chosen.
         index += (int)((cw_rotation_map + cw_rotation_object + 22.5) / 45.0);

         // now (index%4) will get the correct cursor for the drag mode and map rotation
         LPCTSTR  cursors[4] = {IDC_SIZENS, IDC_SIZENWSE, IDC_SIZEWE, IDC_SIZENESW};

         return AfxGetApp()->LoadStandardCursor(cursors[index%4]);
      }

      // converts a float to a string with the given number of decimal places.
      // performs rounding
      CString CFvwUtil::ftoa(double value, int decimal_places)
      {
         const int BUF_LEN = 256;
         char buf[BUF_LEN];

         CString format_str;
         format_str.Format("%%0.%df", decimal_places);
         sprintf_s(buf, BUF_LEN, format_str, value);
         return CString(buf);
      }

      // *************************************************************
      // *************************************************************

      BOOL CFvwUtil::geo_line_to_xy(MapProj* map,
         degrees_t lat1,
         degrees_t lon1,
         degrees_t lat2,
         degrees_t lon2,
         int *num_line,  // number of lines (1 or 2)
         int *px1, int *py1,
         int *px2, int *py2,
         int *px3, int *py3,
         int *px4, int *py4)
      {
         d_geo_t ll, ur;
         int x1, y1;          /* device coordinates of western point */
         int x2, y2;          /* device coordinates of eastern point */
         int p1_flags;        /* NSEW bounds check flags for point 1 */
         int p2_flags;        /* NSEW bounds check flags for point 2 */
         boolean_t east_of;   /* point 2 is east of point 1 */
         int pixels_around_world;
         int screen_width, screen_height;
         BOOL reversed;
         BOOL bigscr;

         *px1 = 0;
         *py1 = 0;
         *px2 = 0;
         *py2 = 0;
         *px3 = 0;
         *py3 = 0;
         *px4 = 0;
         *py4 = 0;

         *num_line = 1;

         // check for screen intersection
         map->get_vmap_bounds(&ll, &ur);
         map->get_pixels_around_world(&pixels_around_world);
         map->get_vsurface_size(&screen_width, &screen_height);

         bigscr = screen_width >= (pixels_around_world/ 2);
         const ProjectionEnum proj_type = map->projection_type();
         if ((proj_type == LAMBERT_PROJECTION) || (proj_type == ORTHOGRAPHIC_PROJECTION))
         {
            geo_to_view_short(map, lat1, lon1, &x1, &y1);
            geo_to_view_short(map, lat2, lon2, &x2, &y2);
            *px1 = x1;
            *py1 = y1;
            *px2 = x2;
            *py2 = y2;
            return TRUE;
         }


         // treat world overview as a special case
         if (bigscr)
         {
            // get device coordinates
            if (GEO_east_of_degrees(lon2, lon1))
            {
               geo_to_view_short(map, lat1, lon1, &x1, &y1);
               geo_to_view_short(map, lat2, lon2, &x2, &y2);
            }
            else         /* p2 is west of p1 or lon1=lon2 */
            {
               geo_to_view_short(map, lat2, lon2, &x1, &y1);
               geo_to_view_short(map, lat1, lon1, &x2, &y2);
            }

            if (x1 > x2)
            {
               // wrap around world
               *num_line = 2;
               *px1 = x1;
               *py1 = y1;
               *px2 = x2 + pixels_around_world;
               *py2 = y2;
               *px3 = x1 - pixels_around_world;
               *py3 = y1;
               *px4 = x2;
               *py4 = y2;
            }
            else
            {
               *px1 = x1;
               *py1 = y1;
               *px2 = x2;
               *py2 = y2;
            }
            return TRUE;
         }

         // get NSEW bounds check flags for each point
         p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
            ur.lat, ur.lon, lat1, lon1);
         p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
            ur.lat, ur.lon, lat2, lon2);

         // quick accept - both points on map
         if ((p1_flags | p2_flags) == 0)
         {
            // get device coordinates
            geo_to_view_short(map, lat1, lon1, &x1, &y1);
            geo_to_view_short(map, lat2, lon2, &x2, &y2);

            *px1 = x1;
            *py1 = y1;
            *px2 = x2;
            *py2 = y2;

            return TRUE;
         }

         // quick reject
         if (p1_flags & p2_flags)
            return FALSE;

         // reject lines that go from west of map to east of map but
         // not through the region defined by left_lon and right_lon
         if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
            (GEO_EAST_OF | GEO_WEST_OF))
         {
            if (p1_flags & GEO_EAST_OF)
            {
               if (GEO_east_of_degrees(lon2, lon1))
                  return FALSE;

               // needed to get device coordinates in right order
               east_of = FALSE;
            }
            else
            {
               if (GEO_east_of_degrees(lon1, lon2))
                  return FALSE;

               // needed to get device coordinates in right order
               east_of = TRUE;
            }
         }
         else
            east_of = GEO_east_of_degrees(lon2, lon1);

         // get device coordinates
         if (east_of)
         {
            geo_to_view_short(map, lat1, lon1, &x1, &y1);
            geo_to_view_short(map, lat2, lon2, &x2, &y2);
            reversed = FALSE;
         }
         else
         {
            geo_to_view_short(map, lat2, lon2, &x1, &y1);
            geo_to_view_short(map, lat1, lon1, &x2, &y2);
            reversed = TRUE;
         }

         if (reversed)
         {
            *px1 = x2;
            *py1 = y2;
            *px2 = x1;
            *py2 = y1;
         }
         else
         {
            *px1 = x1;
            *py1 = y1;
            *px2 = x2;
            *py2 = y2;
         }
         return TRUE;
      }
      // end of geo_line_to_xy

      void CFvwUtil::draw_geo_line(MapProj* map, CDC* dc,
         degrees_t lat1,
         degrees_t lon1,
         degrees_t lat2,
         degrees_t lon2,
         int color,  // code for color
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL normal,  // draw normal line
         BOOL background, // draw background line
         int bgcolor /*= UTIL_COLOR_BLACK*/)
      {
         draw_geo_line(map, dc, lat1, lon1, lat2, lon2, code2color(color),
            style, width, line_type, normal, background, code2color(bgcolor));
      }

      // *************************************************************
      // *************************************************************

      void CFvwUtil::draw_geo_line(MapProj* map, CDC* dc,
         degrees_t lat1,
         degrees_t lon1,
         degrees_t lat2,
         degrees_t lon2,
         COLORREF color_rgb, // RGB color
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL normal,  // draw normal line
         BOOL background, // draw background line
         COLORREF bgcolor_rgb /*= RGB(0,0,0)*/)
      {
         if (background)
            draw_geo_line2(map, dc, lat1, lon1, lat2, lon2, color_rgb,
            style, width, line_type, FALSE, background, bgcolor_rgb);
         if (normal)
            draw_geo_line2(map, dc, lat1, lon1, lat2, lon2, color_rgb,
            style, width, line_type, normal, FALSE, bgcolor_rgb);
      }

      // *************************************************************
      // *************************************************************
      void CFvwUtil::draw_geo_line2(MapProj* map, CDC* dc,
         degrees_t lat1,
         degrees_t lon1,
         degrees_t lat2,
         degrees_t lon2,
         COLORREF color_rgb,  // RGB color
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL normal,  // draw normal line
         BOOL background, // draw background line
         COLORREF bgcolor_rgb)
      {
         const BOOL bClip = TRUE;

         if (line_type == UTIL_LINE_TYPE_SIMPLE)
         {
            draw_geo_line2(map, dc, CSimpleGeoLinePoints(map, lat1, lon1, lat2, lon2, bClip), color_rgb, style, width, normal, background, bgcolor_rgb);
         }
         else if (line_type == UTIL_LINE_TYPE_GREAT)
         {
            draw_geo_line2(map, dc, CGreatCirclePoints(map, lat1, lon1, lat2, lon2, bClip), color_rgb, style, width, normal, background, bgcolor_rgb);
         }
         else
         {
            draw_geo_line2(map, dc, CRhumbLinePoints(map, lat1, lon1, lat2, lon2, bClip), color_rgb, style, width, normal, background, bgcolor_rgb);
         }
      }
      // end of draw_geo_line

      void CFvwUtil::draw_geo_line2(MapProj* map, CDC* dc,
         IGeographicContourIterator &contour,
         COLORREF color_rgb,  // RGB color
         int style,  // code for style
         int width,  // line width in pixels
         BOOL normal,  // draw normal line
         BOOL background,  // draw background line
         COLORREF bgcolor_rgb)
      {
         UtilDraw utilDraw(dc);
         CGeographicContourLineSegments lineSegments(map, contour );
         if (background)
         {
            utilDraw.set_pen(bgcolor_rgb, style, width + 2, TRUE);
            utilDraw.DrawPolyLine(lineSegments);
         }
         if (normal)
         {
            utilDraw.set_pen(color_rgb, style, width, FALSE);
            utilDraw.DrawPolyLine(lineSegments);
         }
      }

      // *************************************************************
      // *************************************************************

      int CFvwUtil::draw_geo_text_line_vertex(MapProj* map, CDC* dc,
         degrees_t *lat,  // array of lats
         degrees_t *lon,  // arrat if longs
         int ptcnt,  // number of vertexes
         int color,  // code for color, -1 for XOR line
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL polygon,  // true if line is a polygon
         CString text,  // text for line vertexe
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int font_fg_color,  // font main color
         int font_bg_color,  // font background color
         int font_bg_type,  // font background type
         BOOL normal,  // draw normal line
         BOOL background,  // draw background line
         CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list)  // text polygons
      {
         int j, k, x1, y1, x2, y2, tx, ty;
         double lat1, lon1, lat2, lon2, distx, disty;
         double tdist, tangle, ang, last_ang, first_ang;
         int back_type, back_color, text_size;
         POINT pt[4];
         fv_core_mfc::C_poly_xy *polyxy;

         if (ptcnt < 2)
            return FAILURE;

         // calculate the end points for the line between the vertexes
         get_text_size(dc, text, font, font_size, font_attrib, &text_size, &k, TRUE);

         geo_to_view_short(map, lat[0], lon[0], &x1, &y1);
         x2 = x1 + (text_size/2) + 4;
         if (line_type == UTIL_LINE_TYPE_SIMPLE)
            x2 += 6;
         y2 = y1;
         back_type = UTIL_BG_SHADOW;
         if (!background)
            back_type = UTIL_BG_NONE;

         back_color = UTIL_COLOR_BLACK;
         if (color == UTIL_COLOR_BLACK)
            back_color = UTIL_COLOR_WHITE;

         pixels_to_km(map, x1, y1, x2, y2, &distx, &disty);
         distx *= 1000.0;

         for (k=0; k<ptcnt-1; k++)
         {
            if (line_type == UTIL_LINE_TYPE_SIMPLE)
            {
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[k+1], lon[k+1], &x2, &y2);
               tdist = (double) magnitude(x1, y1, x2, y2);
               tangle = line_angle(x1, y1, x2, y2);
               tdist /= 2.0;
               tdist = (text_size / 2) + 5;
               vector_to_point(x1, y1, (int) tdist, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat1, &lon1);
               vector_to_point(x2, y2, (int) tdist, tangle + 180.0, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat2, &lon2);
            }
            else
            {
               GEO_geo_to_distance(lat[k], lon[k], lat[k+1], lon[k+1], &tdist, &tangle);
               GEO_distance_to_geo(lat[k], lon[k], distx, tangle, &lat1, &lon1);
               GEO_distance_to_geo(lat[k+1], lon[k+1], distx, tangle + 180.0, &lat2, &lon2);
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[k+1], lon[k+1], &x2, &y2);
            }
            tangle = line_angle(x1, y1, x2, y2);
            if (k == 0)
            {
               ang = tangle;
               first_ang = tangle;
            }
            else
            {
               ang = (tangle + last_ang) / 2.0;
            }
            if ((ang > 90.0) && (ang < 270.0))
               ang += 180.0;
            last_ang = tangle;

            // draw the vertex text
            if (!polygon || (k>0))
            {
               draw_text(dc, text, x1, y1, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
                  font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
               polyxy = new fv_core_mfc::C_poly_xy;
               for (j=0; j<4; j++)
                  polyxy->add_point(pt[j].x, pt[j].y);
               poly_list.AddTail(polyxy);
            }


            // draw the line
            draw_geo_line(map, dc, lat1, lon1, lat2, lon2, color, style, width, line_type, TRUE, background);
         }
         // draw the last text
         ang = tangle;
         if (polygon)
         {
            if (line_type == UTIL_LINE_TYPE_SIMPLE)
            {
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[0], lon[0], &x2, &y2);
               tdist = (double) magnitude(x1, y1, x2, y2);
               tangle = line_angle(x1, y1, x2, y2);
               tdist /= 2.0;
               tdist = (text_size / 2) + 5;
               vector_to_point(x1, y1, (int) tdist, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat1, &lon1);
               vector_to_point(x2, y2, (int) tdist, tangle + 180.0, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat2, &lon2);
            }
            else
            {
               GEO_geo_to_distance(lat[k], lon[k], lat[0], lon[0], &tdist, &tangle);
               GEO_distance_to_geo(lat[k], lon[k], distx, tangle, &lat1, &lon1);
               GEO_distance_to_geo(lat[0], lon[0], distx, tangle + 180.0, &lat2, &lon2);
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[0], lon[0], &x2, &y2);
            }
            draw_geo_line(map, dc, lat1, lon1, lat2, lon2, color, style, width, line_type, TRUE, background);
            tangle = line_angle(x1, y1, x2, y2);
            ang = (tangle + last_ang) / 2.0;
            if ((ang > 90.0) && (ang < 270.0))
               ang += 180.0;

            draw_text(dc, text, x1, y1, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
               font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
            polyxy = new fv_core_mfc::C_poly_xy;
            for (j=0; j<4; j++)
               polyxy->add_point(pt[j].x, pt[j].y);
            poly_list.AddTail(polyxy);

            ang = (tangle + first_ang) / 2.0;
            if ((ang > 90.0) && (ang < 270.0))
               ang += 180.0;
            draw_text(dc, text, x2, y2, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
               font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
            polyxy = new fv_core_mfc::C_poly_xy;
            for (j=0; j<4; j++)
               polyxy->add_point(pt[j].x, pt[j].y);
            poly_list.AddTail(polyxy);
         }
         else
         {
            if ((ang > 90.0) && (ang < 270.0))
               ang += 180.0;
            geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
            draw_text(dc, text, x1, y1, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
               font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
            polyxy = new fv_core_mfc::C_poly_xy;
            for (j=0; j<4; j++)
               polyxy->add_point(pt[j].x, pt[j].y);
            poly_list.AddTail(polyxy);
         }

         return SUCCESS;
      }
      // end of draw_geo_text_line_vertex

      // *************************************************************
      // *************************************************************

      // draws a polyline with text outside the end vertexes of the end segments
      // returns FAILURE if line cannot be drawn, SUCCESS otherwise

      int CFvwUtil::draw_geo_text_line_vertex_offset(MapProj* map, CDC* dc,
         degrees_t *lat,  // array of lats
         degrees_t *lon,  // arrat if longs
         int ptcnt,  // number of vertexes
         int color,  // code for color, -1 for XOR line
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         CString text,  // text for line vertexe
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int font_fg_color,  // font main color
         int font_bg_color,  // font background color
         int font_bg_type,  // font background type
         BOOL normal,  // draw normal line
         BOOL background,  // draw background line
         CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list)  // text polygons
      {
         int j, k, x1, y1, x2, y2, tx, ty;
         double distx, disty;
         double tdist, tangle;
         double tlat, tlon;
         int back_type, back_color, text_size;
         int anchor;
         POINT pt[4];
         fv_core_mfc::C_poly_xy *polyxy;

         if (ptcnt < 2)
            return FAILURE;

         // calculate the end points for the line between the vertexes
         get_text_size(dc, text, font, font_size, font_attrib, &text_size, &k, TRUE);

         geo_to_view_short(map, lat[0], lon[0], &x1, &y1);
         x2 = x1 + (text_size/2) + 4;
         if (line_type == UTIL_LINE_TYPE_SIMPLE)
            x2 += 6;
         y2 = y1;
         back_type = UTIL_BG_SHADOW;
         if (!background)
            back_type = UTIL_BG_NONE;

         back_color = UTIL_COLOR_BLACK;
         if (color == UTIL_COLOR_BLACK)
            back_color = UTIL_COLOR_WHITE;

         pixels_to_km(map, x1, y1, x2, y2, &distx, &disty);
         distx *= 1000.0;

         // draw begining text
         tlat = lat[1];
         tlon = lon[1];
         if ((tlat == lat[0]) && (tlon == lon[0]))  // are the first two points the same?
         {
            if (ptcnt < 3)
               return FAILURE;  // can't draw line with text is line is zero length
            tlat = lat[2];
            tlon = lon[2];
         }
         geo_to_view_short(map, lat[0], lon[0], &x1, &y1);
         geo_to_view_short(map, tlat, tlon, &x2, &y2);
         tdist = (double) magnitude(x1, y1, x2, y2);
         tangle = line_angle(x2, y2, x1, y1);
         vector_to_point(x1, y1, 10, tangle, &tx, &ty);
         if (tx < x1)
         {
            anchor = UTIL_ANCHOR_CENTER_RIGHT;
            tangle += 180.0;
         }
         else
         {
            anchor = UTIL_ANCHOR_CENTER_LEFT;
         }

         draw_text(dc, text, tx, ty, anchor, font, font_size, font_attrib,
            font_bg_type, font_fg_color, font_bg_color, tangle, pt, FALSE);
         polyxy = new fv_core_mfc::C_poly_xy;
         for (j=0; j<4; j++)
            polyxy->add_point(pt[j].x, pt[j].y);
         poly_list.AddTail(polyxy);


         // draw the lilnes
         for (k=0; k<ptcnt-1; k++)
         {
            draw_geo_line(map, dc, lat[k], lon[k], lat[k+1], lon[k+1], color, style, width,
               line_type, normal, background);
         }

         // draw the last text
         geo_to_view_short(map, lat[ptcnt-1], lon[ptcnt-1], &x1, &y1);
         geo_to_view_short(map, lat[ptcnt-2], lon[ptcnt-2], &x2, &y2);
         tdist = (double) magnitude(x1, y1, x2, y2);
         tangle = line_angle(x2, y2, x1, y1);
         vector_to_point(x1, y1, 10, tangle, &tx, &ty);
         if (tx < x1)
         {
            anchor = UTIL_ANCHOR_CENTER_RIGHT;
            tangle += 180.0;
         }
         else
         {
            anchor = UTIL_ANCHOR_CENTER_LEFT;
         }

         draw_text(dc, text, tx, ty, anchor, font, font_size, font_attrib,
            font_bg_type, font_fg_color, font_bg_color, tangle, pt, FALSE);
         polyxy = new fv_core_mfc::C_poly_xy;
         for (j=0; j<4; j++)
            polyxy->add_point(pt[j].x, pt[j].y);
         poly_list.AddTail(polyxy);

         return SUCCESS;
      }
      // end of draw_geo_text_line_vertex_offset

      // *************************************************************
      // *************************************************************

      // draw line with text in the center

      // returns SUCCESS if all text was draw,
      //         UTIL_GEO_LINE_TEXT_NO_TEXT is no text was drawn,
      //         UTIL_GEO_LINE_TEXT_SOME_TEXT is some text was drawn
      //         FAILURE if nothing was drawn

      int CFvwUtil::draw_geo_text_line_center(MapProj* map, CDC* dc,
         degrees_t *lat,  // array of lats
         degrees_t *lon,  // arrat if longs
         int ptcnt,  // number of vertexes
         int color,  // code for color, -1 for XOR line
         int bg_color,     // code for background color
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,  // simple=1, rhumb=2, great_circle=3
         BOOL polygon,  // true if line is a polygon
         CString text,  // text for line vertexe
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int font_fg_color,  // font main color
         int font_bg_color,  // font background color
         int font_bg_type,  // font background type
         int text_type,  // auto = 0, top = 1, or bottom = 2
         BOOL normal,  // draw normal line
         BOOL background,  // draw background line
         CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list)  // text polygons
      {
         COLORREF fgcolor_rgb, bgcolor_rgb;
         int rslt;

         fgcolor_rgb = code2color(color);
         bgcolor_rgb = code2color(bg_color);
         rslt = draw_geo_text_line_center(map, dc, lat, lon, ptcnt, fgcolor_rgb, bgcolor_rgb, style, width, line_type,
            polygon, text, font, font_size, font_attrib, font_fg_color,
            font_bg_color, font_bg_type, text_type, normal, background, poly_list);
         return rslt;
      }
      // end of draw_geo_text_line_center

      // *************************************************************
      // *************************************************************

      // draw line with text in the center

      // returns SUCCESS if all text was draw,
      //         UTIL_GEO_LINE_TEXT_NO_TEXT is no text was drawn,
      //         UTIL_GEO_LINE_TEXT_SOME_TEXT is some text was drawn
      //         FAILURE if nothing was drawn

      int CFvwUtil::draw_geo_text_line_center(MapProj* map, CDC* dc,
         degrees_t *lat,  // array of lats
         degrees_t *lon,  // arrat if longs
         int ptcnt,  // number of vertexes
         COLORREF color,  // RGB color, -1 for XOR line
         COLORREF bgcolor, // RGB background color
         int style,  // code for style
         int width,  // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL polygon,  // true if line is a polygon
         CString text,  // text for line vertexe
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         int font_fg_color,  // RGB font main color
         int font_bg_color,  // RGB font background color
         int font_bg_type,  // font background type
         int text_type,  // auto = 0, top = 1, or bottom = 2
         BOOL normal,  // draw normal line
         BOOL background,  // draw background line
         CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list)  // text polygons
      {
         return draw_geo_text_line_center(map, dc, lat, lon, ptcnt, color, bgcolor, style, width,
            line_type, polygon, text, font, font_size, font_attrib, code2color(font_fg_color),
            code2color(font_bg_color), font_bg_type, text_type, normal, background, poly_list);
      }

      int CFvwUtil::draw_geo_text_line_center(MapProj* map, CDC* dc,
         degrees_t *lat, // array of lats
         degrees_t *lon,  // arrat if longs
         int ptcnt,  // number of vertexes
         COLORREF color,  // RGB color, -1 for XOR line
         COLORREF bgcolor, // RGB background color
         int style,   // code for style
         int width,   // line width in pixels
         int line_type,   // simple=1, rhumb=2, great_circle=3
         BOOL polygon,  // true if line is a polygon
         CString text,  // text for line vertexe
         CString font,       // font name
         int font_size,      // font size in points
         int font_attrib,    // font attributes (bold, italic, etc)
         COLORREF font_fg_color, // RGB font main color
         COLORREF font_bg_color, // RGB font background color
         int font_bg_type, // font background type
         int text_type,  // auto = 0, top = 1, or bottom = 2
         BOOL normal,  // draw normal line
         BOOL background, // draw background line
         CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list)  // text polygons
      {
         int j, k, x1, y1, x2, y2, x3, y3, tx, ty;
         double lat1, lon1, lat2, lon2, lat3, lon3, distx, disty;
         double tdist, tangle, ang, last_ang;
         double half_dist;
         int back_type, text_size, tsize;
         POINT pt[4];
         fv_core_mfc::C_poly_xy *polyxy;
         BOOL great_circle = (line_type == UTIL_LINE_TYPE_GREAT);
         BOOL text_drawn = FALSE;
         BOOL any_text_failed = FALSE;

         if (ptcnt < 2)
            return FAILURE;

         // calculate the end points for the line between the vertexes
         get_text_size(dc, text, font, font_size, font_attrib, &text_size, &k, TRUE);

         geo_to_view_short(map, lat[0], lon[0], &x1, &y1);
         x2 = x1 + (text_size/2);
         if (line_type == UTIL_LINE_TYPE_SIMPLE)
            x2 += 10;
         y2 = y1;
         back_type = UTIL_BG_SHADOW;
         if (!background)
            back_type = UTIL_BG_NONE;

         pixels_to_km(map, x1, y1, x2, y2, &distx, &disty);
         distx *= 1000.0;

         for (k=0; k<ptcnt-1; k++)
         {
            if (line_type == UTIL_LINE_TYPE_SIMPLE)
            {
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[k+1], lon[k+1], &x2, &y2);
               tdist = (double) magnitude(x1, y1, x2, y2);
               tangle = line_angle(x1, y1, x2, y2);
               tdist /= 2.0;
               if (tdist < (text_size / 2) + 10 )
               {
                  draw_geo_line(map, dc, lat[k], lon[k], lat[k+1], lon[k+1], color, style, width, line_type, TRUE, background, bgcolor);
                  any_text_failed = TRUE;
                  continue;
               }

               half_dist = tdist;
               tsize = text_size + 8;
               tdist -= (tsize / 2);
               x3 = (x1 + x2) / 2;
               y3 = (y1 + y2) / 2;
               vector_to_point(x1, y1, (int) tdist, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat1, &lon1);
               vector_to_point(x1, y1, (int) tdist+tsize, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat2, &lon2);
            }
            else
            {
               GEO_calc_range_and_bearing(lat[k], lon[k], lat[k+1], lon[k+1], &tdist, &tangle, great_circle);
               tdist /= 2.0;
               GEO_calc_end_point(lat[k], lon[k], tdist, tangle, &lat1, &lon1, great_circle);
               geo_to_view_short(map, lat1, lon1, &x1, &y1);
               tsize = text_size - 10;
               x1 -= tsize/2;
               y1 -= tsize/2;
               x2 = x1 + tsize;
               y2 = y1;
               map->surface_to_geo(x2, y2, &lat2, &lon2);
               GEO_calc_range_and_bearing(lat1, lon1, lat2, lon2, &distx, &ang, great_circle);
               if (tdist < distx)
               {
                  draw_geo_line(map, dc, lat[k], lon[k], lat[k+1], lon[k+1], color, style, width, line_type, TRUE, background, bgcolor);
                  any_text_failed = TRUE;
                  continue;
               }
               half_dist = tdist;
               tdist -= distx;
               GEO_calc_end_point(lat[k], lon[k], tdist, tangle, &lat1, &lon1, great_circle);
               GEO_calc_end_point(lat[k], lon[k], tdist+(distx*2), tangle, &lat2, &lon2, great_circle);
               GEO_calc_end_point(lat[k], lon[k], half_dist, tangle, &lat3, &lon3, great_circle);
               geo_to_view_short(map, lat1, lon1, &x1, &y1);
               geo_to_view_short(map, lat2, lon2, &x2, &y2);
               geo_to_view_short(map, lat3, lon3, &x3, &y3);
               tangle = line_angle(x1, y1, x2, y2);
            }
            ang = tangle;
            if (ang < 0.0)
               ang += 360.0;

            if (text_type == 0)
               if ((ang > 90.0) && (ang < 270.0))
                  ang += 180.0;
            if (text_type == 2)
               ang += 180.0;

            last_ang = tangle;

            // draw the center text
            draw_text_RGB(dc, text, x3, y3, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
               font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
            text_drawn = TRUE;
            polyxy = new fv_core_mfc::C_poly_xy;
            for (j=0; j<4; j++)
               polyxy->add_point(pt[j].x, pt[j].y);
            poly_list.AddTail(polyxy);

            // draw the line
            draw_geo_line(map, dc, lat[k], lon[k], lat1, lon1, color, style, width, line_type, TRUE, background, bgcolor);
            draw_geo_line(map, dc, lat2, lon2, lat[k+1], lon[k+1], color, style, width, line_type, TRUE, background, bgcolor);
         }
         ang = tangle;
         if (polygon)
         {
            if (line_type == UTIL_LINE_TYPE_SIMPLE)
            {
               geo_to_view_short(map, lat[k], lon[k], &x1, &y1);
               geo_to_view_short(map, lat[0], lon[0], &x2, &y2);
               tdist = (double) magnitude(x1, y1, x2, y2);
               tangle = line_angle(x1, y1, x2, y2);
               tdist /= 2.0;
               if (tdist < text_size / 2)
               {
                  draw_geo_line(map, dc, lat[k], lon[k], lat[0], lon[0], color, style, width, line_type, TRUE, background, bgcolor);
                  if (text_drawn)
                     return UTIL_GEO_TEXT_LINE_SOME_TEXT;
                  else
                     return UTIL_GEO_TEXT_LINE_NO_TEXT;
               }
               half_dist = tdist;
               tdist -= text_size / 2;
               x3 = (x1 + x2) / 2;
               y3 = (y1 + y2) / 2;
               vector_to_point(x1, y1, (int) tdist, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat1, &lon1);
               vector_to_point(x1, y1, (int) tdist+text_size, tangle, &tx, &ty);
               map->surface_to_geo(tx, ty, &lat2, &lon2);
            }
            else
            {
               GEO_calc_range_and_bearing(lat[k], lon[k], lat[0], lon[0], &tdist, &tangle, great_circle);
               tdist /= 2.0;
               GEO_calc_end_point(lat[k], lon[k], tdist, tangle, &lat1, &lon1, great_circle);
               geo_to_view_short(map, lat1, lon1, &x1, &y1);
               x1 -= text_size/2;
               y1 -= text_size/2;
               x2 = x1 + text_size;
               y2 = y1;
               map->surface_to_geo(x2, y2, &lat2, &lon2);
               GEO_calc_range_and_bearing(lat1, lon1, lat2, lon2, &distx, &ang, great_circle);
               if (tdist < distx)
               {
                  draw_geo_line(map, dc, lat[k], lon[k], lat[0], lon[0], color, style, width, line_type, TRUE, background, bgcolor);
                  any_text_failed = TRUE;
                  if (text_drawn)
                     return UTIL_GEO_TEXT_LINE_SOME_TEXT;
                  else
                     return UTIL_GEO_TEXT_LINE_NO_TEXT;
               }
               half_dist = tdist;
               tdist -= distx;
               GEO_calc_end_point(lat[k], lon[k], tdist, tangle, &lat1, &lon1, great_circle);
               GEO_calc_end_point(lat[k], lon[k], tdist+(distx*2), tangle, &lat2, &lon2, great_circle);
               GEO_calc_end_point(lat[k], lon[k], half_dist, tangle, &lat3, &lon3, great_circle);
               geo_to_view_short(map, lat1, lon1, &x1, &y1);
               geo_to_view_short(map, lat2, lon2, &x2, &y2);
               geo_to_view_short(map, lat3, lon3, &x3, &y3);
               tangle = line_angle(x1, y1, x2, y2);
            }
            ang = tangle;
            if (ang < 0.0)
               ang += 360.0;
            if (text_type == 0)
               if ((ang > 90.0) && (ang < 270.0))
                  ang += 180.0;
            if (text_type == 2)
               ang += 180.0;

            // draw the center text
            draw_text_RGB(dc, text, x3, y3, UTIL_ANCHOR_CENTER_CENTER, font, font_size, font_attrib,
               font_bg_type, font_fg_color, font_bg_color, ang, pt, FALSE);
            polyxy = new fv_core_mfc::C_poly_xy;
            for (j=0; j<4; j++)
               polyxy->add_point(pt[j].x, pt[j].y);
            poly_list.AddTail(polyxy);

            // draw the line
            draw_geo_line(map, dc, lat[k], lon[k], lat1, lon1, color, style, width, line_type, TRUE, background, bgcolor);
            draw_geo_line(map, dc, lat2, lon2, lat[0], lon[0], color, style, width, line_type, TRUE, background, bgcolor);
         }

         if (!text_drawn)
            return UTIL_GEO_TEXT_LINE_NO_TEXT;
         if (any_text_failed)
            return UTIL_GEO_TEXT_LINE_SOME_TEXT;

         return SUCCESS;
      }
      // end of draw_geo_text_line_center

      int CFvwUtil::wgs84_to_current_datum(double inlat, double inlon, double *outlat, double *outlon)
      {
         // check datum and convert if necessary
         const int TYPE_LEN = 21;
         char datum_type[TYPE_LEN];
         const int DATUM_LEN = 41;
         char datum[DATUM_LEN];
         double tlat, tlon;

         *outlat = inlat;
         *outlon = inlon;

         // check for valid geo
         if (!GEO_valid_degrees(inlat, inlon))
            return FAILURE;

         GEO_get_default_display(datum_type, TYPE_LEN);
         if (!strcmp(datum_type, "SECONDARY"))
            GEO_get_secondary_datum(datum, DATUM_LEN);
         else
            GEO_get_primary_datum(datum, DATUM_LEN);

         if (strcmp(datum, "WGS84"))
         {
            CGeoTrans trans;
            long rslt;

            rslt = trans.convert_datum( inlat, inlon, tlat, tlon, "WGS84", datum);
            if (rslt != SUCCESS)
            {
               return rslt;
            }
            else
            {
               // insure valid range
               if (tlat > 90.0)
                  tlat = 90.0;
               if (tlat < -90.0)
                  tlat = -90.0;
               if (tlon > 180.0)
                  tlon = 180.0;
               if (tlon < -180.0)
                  tlon = -180.0;

               *outlat = tlat;
               *outlon = tlon;
            }
         }

         return SUCCESS;
      }
      // wgs84_to_current_datum

      // *************************************************************
      // *************************************************************

      int CFvwUtil::current_datum_to_wgs84(double inlat, double inlon, double *outlat, double *outlon)
      {
         // check datum and convert if necessary
         const int DATUM_TYPE_LEN = 21;
         char datum_type[DATUM_TYPE_LEN];
         const int DATUM_LEN = 41;
         char datum[41];
         double tlat, tlon;

         *outlat = inlat;
         *outlon = inlon;

         // check for valid geo
         if (!GEO_valid_degrees(inlat, inlon))
            return FAILURE;

         GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
         if (!strcmp(datum_type, "SECONDARY"))
            GEO_get_secondary_datum(datum, DATUM_LEN);
         else
            GEO_get_primary_datum(datum, DATUM_LEN);

         if (strcmp(datum, "WGS84"))
         {
            CGeoTrans trans;
            long rslt;

            rslt = trans.convert_datum( inlat, inlon, tlat, tlon, datum, "WGS84");
            if (rslt != SUCCESS)
            {
               return rslt;
            }
            else
            {
               // insure valid range
               if (tlat > 90.0)
                  tlat = 90.0;
               if (tlat < -90.0)
                  tlat = -90.0;
               if (tlon > 180.0)
                  tlon = 180.0;
               if (tlon < -180.0)
                  tlon = -180.0;

               *outlat = tlat;
               *outlon = tlon;
            }
         }

         return SUCCESS;
      }
      // current_datum_to_wgs84

      // *************************************************************
      // *************************************************************

      // returns the width and height of the desktop window
      void CFvwUtil::get_screen_size(UINT *screen_width, UINT *screen_height)
      {
         RECT screen_rect;
         GetWindowRect(GetDesktopWindow(), &screen_rect);

         // note: don't add 1 to screen_width and screen_height
         *screen_width = screen_rect.right - screen_rect.left;
         *screen_height = screen_rect.bottom - screen_rect.top;
      }

      // shifts the given rect so that no part of the rect (if possible) is off
      // the screen
      void CFvwUtil::make_rect_visible_on_screen(CRect &rect)
      {
         UINT w, h;
         get_screen_size(&w, &h);
         int screen_width = w;
         int screen_height = h;

         int shift_x = 0;
         int shift_y = 0;

         if (rect.left < 0)
            shift_x = -rect.left;
         else if (rect.right > screen_width)
            shift_x = screen_width - rect.right;

         if (rect.top < 0)
            shift_y = -rect.top;
         else if (rect.bottom > screen_height)
            shift_y = screen_height - rect.bottom;

         rect.left += shift_x;
         rect.right += shift_x;
         rect.top += shift_y;
         rect.bottom += shift_y;
      }

      //  Callback procedure used to set the initial selection of the browser used in get_path_from_user
      int CALLBACK BrowseForFolderProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
      {
         if (uMsg==BFFM_INITIALIZED)
            ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData );

         return 0;
      }

      CString CFvwUtil::get_path_from_user(const CString &title, const CString &initialDirectory /*= ""*/)
      {
         CString result;  // folder user chose

         LPMALLOC pMalloc;

         /* Gets the Shell's default allocator */
         if (::SHGetMalloc(&pMalloc) == NOERROR)
         {
            BROWSEINFO bi;
            char pszBuffer[MAX_PATH];
            LPITEMIDLIST pidl;

            // Get help on BROWSEINFO struct - it's got all the bit settings.
            bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
            bi.pidlRoot = NULL;
            bi.pszDisplayName = pszBuffer;
            bi.lpszTitle = _T(title);
            bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS /*| BIF_STATUSTEXT*/;
            bi.lpfn = initialDirectory.GetLength() == 0 ? NULL : BrowseForFolderProc;
            bi.lParam = initialDirectory.GetLength() == 0 ? 0 : bi.lParam = (LPARAM)(LPCSTR) initialDirectory;

            // This next call issues the dialog box.
            if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
            {
               // At this point pszBuffer contains the selected path
               if (::SHGetPathFromIDList(pidl, pszBuffer))
               {
                  result = pszBuffer;
                  if ( result[result.GetLength()-1] != '\\' )      // if the path doesn't end in a '\'...
                     result += "\\";                               // add one
               }
               else
                  result = "";

               // Free the PIDL allocated by SHBrowseForFolder.
               pMalloc->Free(pidl);
            }
            // Release the shell's allocator.
            pMalloc->Release();
         }

         return result;
      }

