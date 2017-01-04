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

// nitf_util.cpp

#include "stdafx.h"
#include "nitf_util.h"
#include <direct.h>
#include "common.h"
#include "..\resource.h"
#include "..\getobjpr.h"


// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::file_is_georeferenced(CString filename) const
{
	int err_code, width, height;
	_bstr_t err_msg;
	double ul_lat, ul_lon, ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon;

	IImageLibPtr m_imagelib(__uuidof(ImageLib));

	int len = filename.GetLength();
	m_imagelib->load(_bstr_t(filename.GetBuffer(len)), &width, &height, &err_code, err_msg.GetAddress());
	if (err_code != 0)
	{
		return FALSE;
	}
	m_imagelib->get_corner_coords(&ul_lat, &ul_lon, &ur_lat, &ur_lon, 
									&lr_lat, &lr_lon, &ll_lat, &ll_lon, &err_code);
	if (err_code != 0)
	{
		return FALSE;
	}
	return TRUE;

}

// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::geo_tiff_file_exists(CString pathname) const
{
	long handle;
	char saved_directory[_MAX_DIR];
	_finddata_t file_info;
	int rslt;

	// save current directory
	_getcwd( saved_directory, _MAX_DIR );

	// change to specified directory
	if( _chdir( pathname ) == -1 )
		return FALSE;
   
	// see if there are any .tif files in the path
	handle = _findfirst( "*.tif", &file_info );
	if ( handle != -1 )
	{
		// now see if any of the .tif files are geotiff's
		rslt = 0;
		while( rslt != -1 )
		{
			if (file_is_georeferenced(file_info.name))
			{
				// return to saved directory
				_findclose( handle );
				_chdir( saved_directory );
				return TRUE;
			}
			rslt = _findnext( handle, &file_info );
		}
		_findclose( handle );
	}
	return FALSE;
}
// end of geo_tiff_file_exists

// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::geo_nitf_file_exists(CString pathname) const
{
	long handle;
	char saved_directory[_MAX_DIR];
	_finddata_t file_info;
	int rslt;

	// save current directory
	_getcwd( saved_directory, _MAX_DIR );

	// change to specified directory
	if( _chdir( pathname ) == -1 )
		return FALSE;
   
	// see if there are any .tif files in the path
	handle = _findfirst( "*.ntf", &file_info );
	if ( handle != -1 )
	{
		// now see if any of the .tif files are geotiff's
		rslt = 0;
		while( rslt != -1 )
		{
			if (file_is_georeferenced(file_info.name))
			{
				// return to saved directory
				_findclose( handle );
				_chdir( saved_directory );
				return TRUE;
			}
			rslt = _findnext( handle, &file_info );
		}
		_findclose( handle );
	}
	return FALSE;
}
// end of geo_nitf_file_exists

// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::geo_mrsid_file_exists(CString pathname) const
{
	long handle;
	char saved_directory[_MAX_DIR];
	_finddata_t file_info;
	int rslt;

	// save current directory
	_getcwd( saved_directory, _MAX_DIR );

	// change to specified directory
	if( _chdir( pathname ) == -1 )
		return FALSE;
   
	// see if there are any .sid files in the path
	handle = _findfirst( "*.sid", &file_info );
	if ( handle != -1 )
	{
		// now see if any of the .sid files are geotiff's
		rslt = 0;
		while( rslt != -1 )
		{
			if (file_is_georeferenced(file_info.name))
			{
				// return to saved directory
				_findclose( handle );
				_chdir( saved_directory );
				return TRUE;
			}
			rslt = _findnext( handle, &file_info );
		}
		_findclose( handle );
	}
	return FALSE;
}
// end of geo_mrsid_file_exists

// ******************************************************************************
// ******************************************************************************

int CNitfUtil::get_file_set_info(CString filename, C_nitf_file_set *set)
{
	int err_code, width, height, pos;
	_bstr_t err_msg, info;
	double ul_lat, ul_lon, ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon;
	CString cinfo, sdata, date;

	IImageLibPtr m_imagelib(__uuidof(ImageLib));

	int len = filename.GetLength();
	m_imagelib->load(_bstr_t(filename.GetBuffer(len)), &width, &height, &err_code, err_msg.GetAddress());
	if (err_code != 0)
	{
		return FAILURE;
	}
	set->m_ImageWidth  = width;
	set->m_ImageHeight = height;
	m_imagelib->get_corner_coords(&ul_lat, &ul_lon, &ur_lat, &ur_lon, 
									&lr_lat, &lr_lon, &ll_lat, &ll_lon, &err_code);
	if (err_code != 0)
	{
		return FAILURE;
	}

	sdata = " ";
	pos = filename.ReverseFind('\\');
	if (pos > 0)
		sdata = filename.Left(pos);
	set->m_SearchPath = sdata;
	set->m_Filespec = filename;
	set->m_ULLat = ul_lat;
	set->m_ULLong = ul_lon;
	set->m_URLat = ur_lat;
	set->m_URLong = ur_lon;
	set->m_LRLat = lr_lat;
	set->m_LRLong = lr_lon;
	set->m_LLLat = ll_lat;
	set->m_LLLong = ll_lon;

	set->m_MinLat = ll_lat;
	if (lr_lat < set->m_MinLat)
		set->m_MinLat = lr_lat;
	if (ul_lat < set->m_MinLat)
		set->m_MinLat = ul_lat;
	if (ur_lat < set->m_MinLat)
		set->m_MinLat = ur_lat;

	set->m_MinLong = ll_lon;
	if (lr_lon < set->m_MinLong)
		set->m_MinLong = lr_lon;
	if (ul_lon < set->m_MinLong)
		set->m_MinLong = ul_lon;
	if (ur_lon < set->m_MinLong)
		set->m_MinLong = ur_lon;

	set->m_MaxLat = ul_lat;
	if (lr_lat > set->m_MaxLat)
		set->m_MaxLat = lr_lat;
	if (ll_lat > set->m_MaxLat)
		set->m_MaxLat = ll_lat;
	if (ur_lat > set->m_MaxLat)
		set->m_MaxLat = ur_lat;

	set->m_MaxLong = ur_lon;
	if (lr_lon > set->m_MaxLong)
		set->m_MaxLong = lr_lon;
	if (ul_lon > set->m_MaxLong)
		set->m_MaxLong = ul_lon;
	if (ll_lon > set->m_MaxLong)
		set->m_MaxLong = ll_lon;

	m_imagelib->get_info(info.GetAddress(), &err_code, err_msg.GetAddress());
	if (err_code != 0)
	{
		return SUCCESS;
	}
	AfxBSTR2CString(&cinfo, info);
	extract_info_data(cinfo, "Date", sdata);
//	convert_date(sdata, date);
	set->m_ImageDate = sdata;
	extract_info_data(cinfo, "File Title", sdata);
	set->m_FileTitle = sdata;
	extract_info_data(cinfo, "Image Title", sdata);
	set->m_ImageTitle = sdata;
	extract_info_data(cinfo, "Native Pixel Size", sdata);
	if (sdata.Find("meters"))
		sdata = sdata.Left(sdata.GetLength() - 7);
	set->m_Resolution = atof(sdata);
	extract_info_data(cinfo, "Color Image", sdata);
	if (!sdata.Compare("YES"))
		set->m_BoolColor = 1;
	else
		set->m_BoolColor = 0;
	return SUCCESS;
}
// end of get_file_set_info

// ******************************************************************************
// ******************************************************************************

int CNitfUtil::extract_info_data(CString info, CString key, CString &data) 
{
	int pos, end, keylen;
	CString tkey;

// debug only -- remove
//char buf[5001];
//int len;
//len = info.GetLength();
//if (len > 5000)
//	len = 5000;
//strncpy(buf, info.GetBuffer(len), len);
//buf[len] = '\0';

	tkey = key;
	tkey += ": ";
	keylen = tkey.GetLength();

	data = "";
	pos = info.Find(tkey, 0);
	if (pos < 0)
		return FAILURE;

	end = info.Find((char) 13, pos);
	if (end < 0)
		end = info.Find((char) 92, pos);
	data = info.Mid(pos+keylen, end - pos - keylen);

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int CNitfUtil::convert_date(CString raw_date, CString &date) 
{
	CString year, mon, day, hour, min, sec, monstr;
	int imon;

	if (raw_date.GetLength() < 14)
		return FAILURE;

	year = raw_date.Left(4);
	mon = raw_date.Mid(4, 2);
	day = raw_date.Mid(6,2);
	hour = raw_date.Mid(8,2);
	min = raw_date.Mid(10,2);
	sec = raw_date.Mid(12,2);

	imon = atoi(mon);
	switch(imon)
	{
		case 1: monstr = "January"; break;
		case 2: monstr = "February"; break;
		case 3: monstr = "March"; break;
		case 4: monstr = "April"; break;
		case 5: monstr = "May"; break;
		case 6: monstr = "June"; break;
		case 7: monstr = "July"; break;
		case 8: monstr = "August"; break;
		case 9: monstr = "September"; break;
		case 10: monstr = "October"; break;
		case 11: monstr = "November"; break;
		case 12: monstr = "December"; break;
		default:
			date = raw_date;
			return FAILURE;
			break;
	}

	date = day + " " + monstr + " " + year + "   " + hour + ":" + min + ":" + sec + "Z";

	return SUCCESS;
}
// end of convert_date
	
// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::add_geo_files_in_path(CString pathname, C_nitf_db *db)
{
	long handle;
	char saved_directory[_MAX_DIR];
	_finddata_t file_info;
	int rslt;
	C_nitf_file_set set;
	CString curpath;

	// save current directory
	_getcwd( saved_directory, _MAX_DIR );

	// change to specified directory
	if( _chdir( pathname ) == -1 )
		return FALSE;
   
	// see if there are any .tif files in the path
	handle = _findfirst( "*.*", &file_info );
	if ( handle != -1 )
	{
		rslt = 0;
		while( rslt != -1 )
		{ 
			if (file_info.name[0] != '.')
			{
				curpath = pathname;
				curpath += "\\";
				curpath += file_info.name;
				if (file_info.attrib & _A_SUBDIR)
				{
					add_geo_files_in_path(curpath, db);
				}
				else
				{
					rslt = get_file_set_info(curpath, &set);
					set.m_BoolChecked = 1;
					if (rslt == SUCCESS)
						db->add_file(&set);
				}
			}
			rslt = _findnext( handle, &file_info );
		}
		_findclose( handle );
	}
	_chdir( saved_directory );
	return FALSE;
}
// end of add_geo_files_in_path

// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::add_file(CString filename, C_nitf_db *db)
{
	int rslt;
	C_nitf_file_set set;

	rslt = get_file_set_info(filename, &set);
	set.m_SearchPath = "<single>";
	if (rslt == SUCCESS)
	{
		db->add_file(&set);
		return TRUE;
	}
	return FALSE;
}

// ******************************************************************************
// ******************************************************************************

int CNitfUtil::remove_files_by_source_path(CList<CString*, CString*> deleted_path_list)
{


	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

CString CNitfUtil::extract_filename(CString fullname)
{
	int len, pos;
	CString filename;

	filename = "";
	len = fullname.GetLength();
	pos = fullname.ReverseFind('\\');
	if (pos >= 0)
		filename = fullname.Right(len-pos-1);
	return filename;
}

// ******************************************************************************
// ******************************************************************************

BOOL CNitfUtil::create_directory(const CString& dirname)
{
   // check for existence of the directory, create is necessary
   if (_access(dirname, 0) == -1)
   {
      SECURITY_ATTRIBUTES security;

      security.nLength = sizeof(SECURITY_ATTRIBUTES);
      security.lpSecurityDescriptor = NULL;
      security.bInheritHandle = TRUE;

      if (!CreateDirectory(dirname, &security))
      {
         const CString msg = "Could not create directory -- " + dirname;
         AfxMessageBox(msg);
         return FALSE;
      }
   }
   return TRUE;
}

// ******************************************************************************
// ********************************************************************

int maximum(int a, int b, int c)
{
	int max;

	max = a;
	if (b > max)
		max = b;
	if (c > max)
		max = c;
	return max;
}

// ********************************************************************
// ********************************************************************

int minimum(int a, int b, int c)
{
	int min;

	min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}

// ********************************************************************
// ********************************************************************

// Updates the RGB color from the HSV
void CNitfUtil::hsv2rgb( BYTE h, BYTE s, BYTE v, BYTE & r, BYTE & g, BYTE & b) 
{
	int conv;
	double hue, sat, val;
	int base;

	hue = (float)h / 360.0f;
	sat = (float)s / 255.0f;
	val = (float)v / 255.0f;

	if ((float)s == 0) // Acromatic color (gray). Hue doesn't mind.
	{
		conv = (unsigned short) (255.0f * val);
		r = b = g = conv;
		return;
	}
	
	base = (unsigned short)(255.0f * (1.0 - sat) * val);

	switch ((unsigned short)((float)h/60.0f))
	{
		case 0:
			r = (BYTE)(255.0f * val);
			g = (BYTE)((255.0f * val - base) * (h/60.0f) + base);
			b = (BYTE)base;
		break;

		case 1:
			r = (BYTE)((255.0f * val - base) * (1.0f - ((h%60)/ 60.0f)) + base);
			g = (BYTE)(255.0f * val);
			b = (BYTE)base;
		break;

		case 2:
			r = (BYTE)base;
			g = (BYTE)(255.0f * val);
			b = (BYTE)((255.0f * val - base) * ((h%60)/60.0f) + base);
		break;
		
		case 3:
			r = (BYTE)base;
			g = (BYTE)((255.0f * val - base) * (1.0f - ((h%60) / 60.0f)) + base);
			b = (BYTE)(255.0f * val);
		break;
		
		case 4:
			r = (BYTE)((255.0f * val - base) * ((h%60) / 60.0f) + base);
			g = (BYTE)base;
			b = (BYTE)(255.0f * val);
		break;
		
		case 5:
			r = (BYTE)(255.0f * val);
			g = (BYTE)base;
			b = (BYTE)((255.0f * val - base) * (1.0f - ((h%60) / 60.0f)) + base);
		break;
	}
}

// ********************************************************************
// ********************************************************************

void CNitfUtil::rgb2hsv( BYTE r, BYTE g, BYTE b, BYTE & h, BYTE & s, BYTE & v ) 
{
	unsigned short max, min, delta;
	short temp;
    
	max = maximum(r, g, b);
	min = minimum(r, g, b);
	delta = max-min;

    if (max == 0)
	{
		s = h = v = 0;
		return;
	}
    
	v = (BYTE) max;
	s = (BYTE) (((double) delta / max)*255.0);

	if (r == max)
		temp = (short)(60 * ((g-b) / (double) delta));
	else if (g == max)
		temp = (short)(60 * (2.0 + (b-r) / (double) delta));
	else
		temp = (short)(60 * (4.0 + (r-g) / (double) delta));
	
	if (temp<0)
		h = (BYTE) (temp + 360);
	else
		h = (BYTE) temp;
}

// ********************************************************************
// ******************************************************************************
/////////////////////////////////////////////////////////////////////////////


//
// CYesNoAllDlg
//

BEGIN_MESSAGE_MAP( CYesNoAllDlg, CDialog )
	//{{AFX_MSG_MAP( CYesNoAllDlg )
   ON_BN_CLICKED( IDC_OVL_NITF_YES, OnClickedYes )
   ON_BN_CLICKED( IDC_OVL_NITF_YES_TO_ALL, OnClickedYesToAll )
   ON_BN_CLICKED( IDC_OVL_NITF_NO, OnClickedNo )
   ON_BN_CLICKED( IDC_OVL_NITF_NO_TO_ALL, OnClickedNoToAll )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CYesNoAllDlg::CYesNoAllDlg( BOOL bMultiple, CWnd* pParent /*=NULL*/) :
   m_bMultiple( bMultiple ),
   CDialog( CYesNoAllDlg::IDD, pParent )
{
}
   
BOOL CYesNoAllDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   if ( m_bMultiple )
   {
      GetDlgItem( IDC_OVL_NITF_YES_TO_ALL )->EnableWindow( TRUE );
      GetDlgItem( IDC_OVL_NITF_NO_TO_ALL )->EnableWindow( TRUE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// Message handlers

afx_msg VOID CYesNoAllDlg::OnClickedYes()
{
   EndDialog( IDC_OVL_NITF_YES );
}

afx_msg VOID CYesNoAllDlg::OnClickedYesToAll()
{
   EndDialog( IDC_OVL_NITF_YES_TO_ALL );
}

afx_msg VOID CYesNoAllDlg::OnClickedNo()
{
   EndDialog( IDC_OVL_NITF_NO );
}

afx_msg VOID CYesNoAllDlg::OnClickedNoToAll()
{
   EndDialog( IDC_OVL_NITF_NO_TO_ALL );
}

// End of CYesNoAllDlg methods

// End of nitf_util.cpp
LRESULT CYesNoAllDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

