// Copyright (c) 1994-2011,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// search.cpp

#include "stdafx.h"
#include "geo_tool.h"
#include "showrmk.h"
#include "fvwutil.h"
#include "shpread.h"
#include "factory.h"
#include "ovl_mgr.h"
#include "StringUtils.h"

static int listnum;
CString C_shp_ovl::m_found_item;

// **************************************************************************
// **************************************************************************

// returns the search suffix from the given string
/* static */
CString C_shp_ovl::GetSearchSuffix(const CString& str0)
{
   CString str( str0 );
   str.TrimRight();
   str.MakeUpper();
   int pos = str.ReverseFind('/');
   if (pos != -1)
      return str.Mid(pos + 1);

   return "";
}

// is the search string a shape file search
/* static */
BOOL C_shp_ovl::is_shape_search_key(const CString& str)
{
   CString suffix = GetSearchSuffix(str);
   if (suffix.GetLength() == 0 || suffix.GetLength() > 4)
      return FALSE;

   // validate each character in the suffix
   CString validChars[] = { "S", "FGNX", "AT", "AL" };
   for (int i=0; i<suffix.GetLength(); ++i)
   {
      if (validChars[i].Find(suffix[i]) == -1)
         return FALSE;
   }

   return TRUE;
}

// static function to search displayed shaped databases
/* static */
int C_shp_ovl::search(const CString& keystr, d_geo_t *center_geo, CString& filename)
{
   CString tstr, tstr2, key, *txt;
   C_overlay* ovl;
   C_shp_ovl *shpovl;
   C_shp_found_item *item;
   CList<C_shp_found_item*, C_shp_found_item*> found_list;
   BOOL found, search_all, item_selected, search_top_only;
   int rslt, pos, len, sufnum, cnt, num;
   const int BUF_SIZE = 100;
   char suf1, suf2, suf3, buf[BUF_SIZE];
   CList<CString*, CString*> strlist;
   POSITION next;
   int mode;

   m_found_item = "";

   key = keystr;
   key.MakeUpper();
   key.TrimLeft();
   key.TrimRight();

   // strip off the suffix
   // if there is a suffix then /S means search shape files using literal key
   // and /SF means search using full soundex
   // and /SG means search using first soundex
   // and /SX means search using exact match
   // third letter determines whether to search top layer only or all layers
   // T = top only, A = all layers
   // fourth letter determines whether to search label fields only or all fields
   // L = labels only, A = all fields
   len = key.GetLength();
   pos = key.ReverseFind('/');
   sufnum = len - pos - 1;
   mode = 1;
   search_all = FALSE;
   search_top_only = TRUE;

   if (pos > 0)
   {
      suf1 = key[len-3];
      suf2 = key[len-2];
      suf3 = key[len-1];
      switch(suf1)
      {
         case 'F': mode = 3; break;
         case 'G': mode = 2; break;
         case 'N': mode = 1; break;
         case 'X': mode = 0; break;
      }
      if (suf2 == 'A')
         search_all = TRUE;
      if (suf3 == 'A')
         search_top_only = FALSE;
      if (sufnum > 0)
         key = keystr.Left(pos);
   }

   found = FALSE;
   listnum = 0;

   // loop through the open shape overlays and do a search
   ovl = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ShapeFile);
   if (ovl == NULL)
      return FAILURE;

   while (ovl != NULL)
   {
      shpovl = (C_shp_ovl*) ovl;
      rslt = shpovl->search(key, mode, search_all, search_top_only, found_list);
      if (rslt == SUCCESS)
         found = TRUE;

      if (search_top_only)
         ovl = NULL;
      else
         ovl = OVL_get_overlay_manager()->get_next_of_type(ovl, FVWID_Overlay_ShapeFile);
   }

   if (!found)
      return FAILURE;

   cnt = found_list.GetCount();
   if (cnt < 1)  // sanity check
      return FAILURE;

   // a single match was found, so select the item...
   if (cnt < 2)
   {
      item = found_list.GetHead();
      center_geo->lat = item->m_lat;
      center_geo->lon = item->m_lon;
      filename = item->m_filename;
      m_found_item = item->m_found_str1;
      return SUCCESS;
   }

   // multiple matches were found, so build the list from which the user will choose an item...
   next = found_list.GetHeadPosition();
   while (next != NULL)
   {
      item = found_list.GetNext(next);
      txt = new CString;
      txt->Format("%5d  ", item->m_idnum);
      tstr = item->m_found_str1;
      if (tstr.GetLength() > 20)
         tstr = tstr.Left(20);
      *txt += tstr;
      *txt += "  ";
      tstr = item->m_found_str2;
      if (tstr.GetLength() > 20)
         tstr = tstr.Left(20);
      *txt += tstr;
      GEO_lat_lon_to_string(item->m_lat, item->m_lon, buf, BUF_SIZE);
      *txt += "  ";
      *txt += buf;
      strlist.AddTail(txt);
   }

   // select an item from the list of matches found
   item_selected = select_item(strlist, &num);

   // clear the string list
   while (!strlist.IsEmpty())
      delete strlist.RemoveHead();

   int iResult = FAILURE;
   if ( item_selected && num > 0 )
   {
      // find the item the user selected from the list of matches...
      next = found_list.GetHeadPosition();
      while (next != NULL)
      {
         item = found_list.GetNext(next);
         if (item->m_idnum == num)
         {
            center_geo->lat = item->m_lat;
            center_geo->lon = item->m_lon;
            filename = item->m_filename;
            m_found_item = item->m_found_str1;
            iResult = SUCCESS;
            break;
         }
      }

      ASSERT( iResult == SUCCESS );  // should never get here
   }
   // clear the list
   while (!found_list.IsEmpty())
      delete found_list.RemoveHead();

   return iResult;
}
// end of search (static)

// **************************************************************************
// ***********************************************************
// Allow user to select string from list using dialog box 

BOOL C_shp_ovl::select_item(const CList<CString*, CString*> & strlist, int *num)
{
   int cnt, rslt;
   CRemarkListDlg dlg;
   CString tstr, key;

   key = "";
   cnt = strlist.GetCount();
   if (cnt < 1)
      return FALSE;

   dlg.init_list(strlist);
   rslt = dlg.DoModal();
   if (rslt != IDOK)
      return FALSE;

   dlg.get_selection(key);
   tstr = key.Left(5);
   *num = atoi(tstr);
   return TRUE;
}
// end of select_point

// ***********************************************************
// **************************************************************************

int C_shp_ovl::search(const CString& keystr, int mode, BOOL search_all, BOOL top_only,
                              CList<C_shp_found_item*, C_shp_found_item*> & found_list)
{
   CString tstr, tstr2;
   BOOL rslt;

   // if the fieldname is blank, make it empty
   m_tooltip_fieldname1 = trim(&m_tooltip_fieldname1);
   m_tooltip_fieldname2 = trim(&m_tooltip_fieldname2);

   rslt = search_db(m_filename.c_str(),
      m_tooltip_fieldname1.c_str(), m_tooltip_fieldname2.c_str(),
      keystr, mode, search_all, top_only, found_list);

   if (!rslt)
      return FAILURE;

   return SUCCESS;
}
// end of search

// **************************************************************************
// **************************************************************************

// mode: 0=exact, 1=normal, 2=first_soundex, 3=full_soundex

// search the database
BOOL C_shp_ovl::search_db(const CString& filename, const CString& fieldname10, const CString& fieldname20, const CString& keystr, 
                    int mode, BOOL search_all, BOOL top_only,  CList<C_shp_found_item*, C_shp_found_item*> & found_list)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CDbase dbf;
   const int DBFNAME_LEN = 201;
   char dbfname[DBFNAME_LEN];
   const int FNAME_LEN = 21;
   char fname[FNAME_LEN];
   const int FDATA_LEN = 300;
   char fdata1[FDATA_LEN];
   char fdata2[FDATA_LEN];
   int rslt, cnt, k, pos1, pos2;
   BOOL use_default = TRUE;
   BOOL found;
   CString key, key0, test1, test2;
   C_shp_found_item *item;
   double tlat, tlon;
   CString tstr, rawdata1, rawdata2, shortname, test1first, test2first, keyfirst, fieldname1, fieldname2;
   CList<CString*, CString*> list;
   CString *txt;
   SHPHandle hSHP;
   int nShapeType, nEntities;
   CShapeRead shp( this );
   d_geo_t bnd_ll, bnd_ur;
   int len, pos;
   int fieldcnt, getrslt;
   BOOL foundkey, match;
   double adfMinBound[4], adfMaxBound[4];
   SHPObject shape( &m_clsHeapAllocator );

   if (filename.GetLength() < 1)
      return FALSE;

   CWaitCursor wait;

   shortname = "";
   len = filename.GetLength();
   pos = filename.ReverseFind('\\');
   if (pos >= 0)
      shortname = filename.Right(len-pos-1);

   key = keystr;
   key.MakeUpper();
   key0 = key;
   keyfirst = key.Left(1);
   if ((mode == 2) || (mode == 3))
      key = soundex(keystr);


   tstr = fieldname1 = fieldname10;
   tstr.TrimRight();
   if (tstr.GetLength() < 1)
      fieldname1 = "";
   tstr = fieldname2 = fieldname20;
   tstr.TrimRight();
   if (tstr.GetLength() < 1)
      fieldname2 = "";

   if (mode != 0)
      remove_spaces(key);

   // open the shape file
   hSHP = shp.SHPOpen( filename, "rb" );

   if( hSHP == NULL )
   {
      return FALSE;
   }

    shp.SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

   // open the shape's database file
   strncpy_s(dbfname, DBFNAME_LEN, filename, 200);
   dbfname[200] = '\0';
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      shp.SHPClose( hSHP );
      return FALSE;
   }

   found = FALSE;

   cnt = dbf.GetNumberOfRecords();

   ASSERT(cnt == nEntities);

   fieldcnt = dbf.GetNumFields();
   if (fieldcnt < 1)
   {
      shp.SHPClose( hSHP );
      return FALSE;
   }

   const int NAME_LEN = 20;
   char name[NAME_LEN];
   int dec;

   if (search_all)
   {
      for (k=0; k<cnt; k++)
      {
         dbf.GotoRec(k+1);
         getrslt = dbf.GetFirstFieldName(name, NAME_LEN, &len, &dec);
         foundkey = FALSE;
         while (getrslt == DB_NO_ERROR)
         {
            rslt = dbf.GetFieldData(name, fdata1);
            if (rslt != DB_NO_ERROR)
               goto FAIL;
            test1 = fdata1;
            test1.TrimRight();
            rawdata1 = test1;
            test1.MakeUpper();
            test1first = test1.Left(1);
            if (mode != 0)
               remove_spaces(test1);

            pos1 = key0.CompareNoCase(test1);
            match = (pos1 == 0);

            if (!match)
            {
               if ((mode == 2) || (mode == 3))
                  test1 = soundex(test1);
               if (mode != 0)
               {
                  pos1 = test1.Find(key);
                  match = (pos1 >= 0);
                  if ((mode == 2) && match)
                  {
                     pos1 = keyfirst.CompareNoCase(test1first);
                     match = (pos1 == 0);
                  }
               }
            }
            if (match)
            {
               item = new C_shp_found_item;
               item->m_recnum = k;
               item->m_filename = shortname;
               // get the geo from the shp file

               shp.SHPReadObject( hSHP, k, shape );

               bnd_ll.lat = shape.m_grcBounds.ll.lat;
               bnd_ll.lon = shape.m_grcBounds.ll.lon;
               bnd_ur.lat = shape.m_grcBounds.ur.lat;
               bnd_ur.lon = shape.m_grcBounds.ur.lon;
               futil->compute_center_geo(bnd_ll, bnd_ur, &tlat, &tlon);
               item->m_lat = tlat;
               item->m_lon = tlon;
               listnum++;
               item->m_idnum = listnum;
               item->m_found_str1 = rawdata1;
               item->m_found_str2 = "";
               found_list.AddTail(item);
               found = TRUE;
               foundkey = TRUE;
            } // if
            getrslt = dbf.GetNextFieldName(name, NAME_LEN, &len, &dec);
         } // while
      } // for
   } // if
   else
   {
      // use only field1 and field2
      for (k=0; k<cnt; k++)
      {
         fdata1[0] = '\0';
         fdata2[0] = '\0';
         dbf.GotoRec(k+1);
         if ((fieldname1.GetLength() < 1) && (fieldname2.GetLength() < 1))
         {
            rslt = dbf.FindFieldData("NAME", list);
            if (list.IsEmpty())
               continue;
            txt = list.GetHead();
            strcpy_s(fdata1, FDATA_LEN, txt->GetBuffer(13));

            // clear the list
            while (!list.IsEmpty())
            {
               txt = list.RemoveHead();
               delete txt;
            }
         }
         else
         {
            use_default = FALSE;
            if (fieldname1.GetLength() > 0)
            {
               strncpy_s(fname, FNAME_LEN, fieldname1, 12);
               fname[12] = '\0';
               rslt = dbf.GetFieldData(fname, fdata1);
            }
            if (rslt != DB_NO_ERROR)
               goto FAIL;
            if (fieldname2.GetLength() > 0)
            {
               strncpy_s(fname, FNAME_LEN, fieldname2, 12);
               fname[12] = '\0';
               rslt = dbf.GetFieldData(fname, fdata2);
            }
            if (rslt != DB_NO_ERROR)
               goto FAIL;
         }
         test1 = fdata1;
         test1.TrimRight();
         rawdata1 = test1;
         test1.MakeUpper();
         test1first = test1.Left(1);
         test2 = fdata2;
         test2.TrimRight();
         rawdata2 = test2;
         test2.MakeUpper();
         test2first = test2.Left(1);

         if (mode != 0)
         {
            remove_spaces(test1);
            remove_spaces(test2);
         }

         if ((mode == 2) || (mode == 3))
         {
            test1 = soundex(test1);
            test2 = soundex(test2);
         }

         // make upper again because of soundex
         test1.MakeUpper();
         test2.MakeUpper();

         if (mode == 0)
         {
            pos1 = key.CompareNoCase(test1);
            pos2 = key.CompareNoCase(test2);
            match = !pos1;
            if (!match)
               match = !pos2;
         }
         else
         {
            pos1 = test1.Find(key);
            pos2 = test2.Find(key);
            match = (pos1 >= 0);
            if (!match)
               match = (pos2 >= 0);
            if (mode == 2)
            {
               if (pos1 >= 0)
               {
                  pos1 = keyfirst.CompareNoCase(test1first);
                  match = !pos1;
               }
               if (pos2 >= 0)
               {
                  pos2 = keyfirst.CompareNoCase(test2first);
                  match = !pos2;
               }
            }

         }
         if (match)
         {
            item = new C_shp_found_item;
            item->m_recnum = k;
            item->m_filename = shortname;
            shp.SHPReadObject( hSHP, k, shape );

            bnd_ll.lat = shape.m_grcBounds.ll.lat;
            bnd_ll.lon = shape.m_grcBounds.ll.lon;
            bnd_ur.lat = shape.m_grcBounds.ur.lat;
            bnd_ur.lon = shape.m_grcBounds.ur.lon;
            futil->compute_center_geo(bnd_ll, bnd_ur, &tlat, &tlon);
            item->m_lat = tlat;
            item->m_lon = tlon;
            listnum++;
            item->m_idnum = listnum;
            item->m_found_str1 = rawdata1;
            item->m_found_str2 = rawdata2;
            found_list.AddTail(item);
            found = TRUE;
         }
      }
   }

   dbf.CloseDbf();
   shp.SHPClose( hSHP );
   return found;

FAIL:
   dbf.CloseDbf();
   shp.SHPClose( hSHP );
   return found;
}
// end of search_db

// **************************************************************************
// **************************************************************************

CString C_shp_ovl::soundex(const CString& input) 
{
   CString output, in, chs;
   int pos, len;
   char ch;
   BOOL found;

   in = input;
   in.MakeLower();
   in.TrimLeft();
   in.TrimRight();

   len = in.GetLength();

   // check for a no-letter situation
   found = FALSE;
   pos = 0;
   while (pos < len)
   {
      ch = in[pos];
      found = isalpha(ch);
      if (found)
         pos = len;
      pos++;
   }

   if (!found)  // no alpha characters
   {
      // remove the zeros
      pos = 0;
      while (pos < len)
      {
         ch = in[pos];
         chs = ch;
         if (output.GetLength() > 0)  // is it a repeated character?
         {
            if (!output.Right(1).Compare(chs))
            {
               pos++;
               continue;
            }
         }
         if ((ch >= '1') && (ch <= '9'))
            output += chs;
         pos++;
      }
      return output;
   }

   pos = 0;
   while (pos < len)
   {
      ch = in[pos];
      if (pos < len-1)
      {
         while ((pos < len-1) && (in[pos+1] == in[pos]))
         {
            pos++;
         }
      }
      if (pos > len-1)
      {
         pos++;
         continue;
      }
      switch (ch)
      {
         case 'b' : output += "1"; break;
         case 'p' : output += "1"; break;
         case 'f' : output += "1"; break;
         case 'v' : output += "1"; break;
         case 'c' : 
            output += "2"; 
            if (pos < len-1)
               if (in[pos+1] == 'k')
                  pos++;
            break;
         case 's' : output += "2"; break;
         case 'k' : output += "2"; break;
         case 'g' : output += "2"; break;
         case 'j' : output += "2"; break;
         case 'q' : output += "2"; break;
         case 'x' : output += "2"; break;
         case 'z' : output += "2"; break;
         case 'd' : output += "3"; break;
         case 't' : output += "3"; break;
         case 'l' : output += "4"; break;
         case 'm' : output += "5"; break;
         case 'n' : output += "5"; break;
         case 'r' : output += "6"; break;
      }
      pos++;
   }

   // handle the all vowel case
   if (output.GetLength() < 1)
      output = "0";

   return output;
}
// end soundex

// **************************************************************************
// ***********************************************************

void C_shp_ovl::remove_spaces(CString & text)
{
   text.TrimLeft();
   text.TrimRight();
   text.Remove(' ');
}
// end of select_point

// ***********************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
