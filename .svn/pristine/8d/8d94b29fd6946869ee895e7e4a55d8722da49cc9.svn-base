// util.cpp

#include "stdafx.h"
#include "util.h"
#include <math.h>
#include <io.h>

void CUtil::compute_center_geo(d_geo_t ll, d_geo_t ur, double *lat, double *lon)
{
   *lat =- (ll.lat + ur.lat) / 2.0;

   d_geo_t geo[2];
   geo[0] = ll;
   geo[1] = ur;

   d_geo_t center_geo;
   center_of_mass(geo, 2, &center_geo);

   *lat = center_geo.lat;
   *lon = center_geo.lon;
}


BOOL CUtil::geo_east_of(double a, double b)
{
   double diff;
   double around_world = 360.0;
   double half_world = 180.0;

   diff = a - b;

   if (diff < 0.0) /* convert diff to eastward distance from b to a */
      diff += around_world;

   if (0.0 < diff && diff <= half_world)
      return TRUE;

   return FALSE;
} 

// *****************************************************************
// ******************************************************************

// function to convert geo in decimal degrees to Cartesian xyz in meters
int CUtil::geo_to_xyz(double lat, double lon, double height, 
				   double *x, double *y, double *z)
{
	double rlat, rlon;
//	double n, f, e, a, b;
	double n, a, b;
	double sinlat, coslat, sinlon, coslon;

	rlat = DEG_TO_RAD(lat);
	rlon = DEG_TO_RAD(lon);

	sinlat = sin(rlat);
	coslat = cos(rlat);
	sinlon = sin(rlon);
	coslon = cos(rlon);

	a = 6378137.0; // WGS84 meters at equator
//	f = 1.0 / 298.257223563;  // flattening
//	e = (2.0 * f) - (f * f);  // eccentricity squared
//	n = a / sqrt(1.0 - (e * sinlat * sinlat));

	b = 6356752.3142;
	n = (a*a) / sqrt((a*a*coslat*coslat) + (b*b*sinlat*sinlat));

	*x = (n + height) * coslat * coslon;
	*y = (n + height) * coslat * sinlon;
//	*z = ((n * (1.0 - e)) + height) * sinlat;
	*z = ((n * (b*b)/(a*a)) + height) * sinlat;

	return SUCCESS;
}
// end of GEO_geo_to_xyz

// ******************************************************************
// ******************************************************************

// function to convert Cartesian xyz in meters to decimal degrees
int CUtil::xyz_to_geo(double x, double y, double z, 
				   double *lat, double *lon, double *height)
{
	double tlat, tlon, tf, tf2, tf3;
	double f, e, a, u, p, r;
	double sinu, cosu;

	double sinlat, coslat;

	a = 6378137.0; // WGS84 meters at equator
	f = 1.0 / 298.257223563;  // flattening
	e = (2.0 * f) - (f * f);  // eccentricity squared
	p = sqrt((x * x) + (y * y));
	r = sqrt((p * p) + (z * z));
	u = (z / p) * ((1.0 - f) + (e * (a / r)));
	u = atan(u);
	sinu = sin(u);
	cosu = cos(u);

	tf = (z * (1.0 -f)) + (e * a * sinu * sinu * sinu);
	tf2 = (1.0 - f) * (p - (e * a * cosu * cosu * cosu));
	tf3 = tf / tf2;
	tf3 = atan(tf3);

	tlat = tf3;
	tlon = atan(y / x);

	coslat = cos(tlat);
	sinlat = sin(tlat);
	tf = (p * coslat) + (z * sinlat) - (a * sqrt(1.0 - (e * sinlat * sinlat)));
	*height = tf;

	*lat = RAD_TO_DEG(tlat);
	*lon = RAD_TO_DEG(tlon);

	// correct lon if needed
	if (x < 0)
		*lon += 180.0;
	if (*lon > 180.0)
		*lon -= 360.0;

	return SUCCESS;
}
// end of GEO_xyz_to_geo

// ******************************************************************
// *****************************************************************

// this function takes an array of geo coords and computes the "center of mass"
int CUtil::center_of_mass(d_geo_t *geo, int numpts, d_geo_t *center_geo)
{
	double sx, sy, sz;
	double tx, ty, tz;
	double tlat, tlon, th;
	int k, rslt;

	sx = 0;
	sy = 0;
	sz = 0;

	for (k=0; k<numpts; k++)
	{
		rslt = geo_to_xyz(geo[k].lat, geo[k].lon, 0.0, &tx, &ty, &tz);
		if (rslt != SUCCESS)
			return FAILURE;
		sx += tx;
		sy += ty;
		sz += tz;
	}

	tx = sx / (double) numpts;
	ty = sy / (double) numpts;
	tz = sz / (double) numpts;

	rslt = xyz_to_geo(tx, ty, tz, &tlat, &tlon, &th);
	if (rslt != SUCCESS)
		return FAILURE;
	
	center_geo->lat = tlat;
	center_geo->lon = tlon;

	return SUCCESS;
}

// ******************************************************************
// *****************************************************************
// ********************************************************************
// ********************************************************************

BOOL CUtil::geo_lon_in_range(double left_lon, double right_lon, double point_lon)
{
   if (left_lon <= right_lon)
   {
      if (point_lon < left_lon || right_lon < point_lon)
         return FALSE;
   }
   else // if (left_lon > right_lon)   dvl :: redundant check
   {
      if (point_lon < left_lon && point_lon > right_lon)
         return FALSE;
   }

   return TRUE;    /* point in bounds */
} 

// ********************************************************************
// ********************************************************************



//      Determine if region A and region B intersect.

BOOL CUtil::geo_intersect(double ll_A_lat, double ll_A_lon,
				   double ur_A_lat, double ur_A_lon,
				   double ll_B_lat, double ll_B_lon,
				   double ur_B_lat, double ur_B_lon)
{
   if (ur_A_lat <= ll_B_lat)   /* region A completely below B */
      return FALSE;

   if (ll_A_lat >= ur_B_lat)   /* region A completely above B */
      return FALSE;

   /* if either region goes around the world, intersection exists */
   if (ll_A_lon == ur_A_lon || ll_B_lon == ur_B_lon)
      return TRUE;

   /* if right edge of region B is in region A */
   if (geo_lon_in_range(ll_A_lon, ur_A_lon, ur_B_lon) == TRUE) 
      return TRUE;

   /* if left edge of region B is in region A */
   if (geo_lon_in_range(ll_A_lon, ur_A_lon, ll_B_lon) == TRUE) 
      return TRUE;

   /* if right edge of region A is in region B */ 
   if (geo_lon_in_range(ll_B_lon, ur_B_lon, ur_A_lon) == TRUE) 
      return TRUE;

   /* if left edge of region A is in region B */
   if (geo_lon_in_range(ll_B_lon, ur_B_lon, ll_A_lon) == TRUE)
      return TRUE;

   return FALSE;
}
// end of geo_intersect

// ********************************************************************
// ********************************************************************

BOOL CUtil::geo_intersect(d_geo_t ll_A, d_geo_t ur_A, d_geo_t ll_B, d_geo_t ur_B)
{
   return geo_intersect(ll_A.lat, ll_A.lon, ur_A.lat, ur_A.lon, ll_B.lat, ll_B.lon, ur_B.lat, ur_B.lon);
}

// ********************************************************************
// ********************************************************************

// compute angle of a line segment, returns degrees from -> CCW

double CUtil::line_angle(POINT p1,      // start point of line
                         POINT p2)      // end point of line
{
	double rslt;

	rslt = line_angle(p1.x, p1.y, p2.x, p2.y);

	return rslt;
}

// ********************************************************************
// ********************************************************************

// compute angle of a line segment, returns degrees from -> CCW

double CUtil::line_angle(int x1, int y1,      // start point of line
                         int x2, int y2)      // end point of line
{
   double ang;
   double n;

   if ((x1 - x2) == 0)
   {
      if (y1 < y2)
         ang = 90.0;
      else
         ang = 270.0;
		return ang;
   }
   if ((y1 - y2) == 0)
   {
      if (x1 < x2)
         ang = 0.0;
      else
         ang = 180.0;
		return ang;
   }

   n = (double) abs(y1-y2);
   n /= (double) abs(x1-x2);
   ang = atan(n);
	ang = RAD_TO_DEG(ang);

   if (((x1-x2) < 0) && ((y1-y2) < 0))   // quad 1
      ang = ang;
   if (((x1-x2) > 0) && ((y1-y2) < 0))   // quad 2
      ang = 180.0 - ang;
   if (((x1-x2) > 0) && ((y1-y2) > 0))   // quad 3
      ang += 180.0;
   if (((x1-x2) < 0) && ((y1-y2) > 0))   // quad 4
      ang = -ang;

   // make angle between 0 and 360 degrees
   if (ang > 360.0)
      ang = ang - 360.0;
   if (ang < 0)
      ang = ang + 360.0;

   return ang;
}
// end of line_angle

// ********************************************************************
// ********************************************************************

// compute angle of a line segment, returns degrees from -> CCW

double CUtil::line_angle(double x1, double y1,      // start point of line
                         double x2, double y2)      // end point of line
{
   double ang;
   double n;
	double epsilon = 0.0000001;

   if (fabs(x1 - x2) < epsilon)
   {
      if (y1 < y2)
         ang = 90.0;
      else
         ang = 270.0;
		return ang;
   }
   if (fabs(y1 - y2) < epsilon)
   {
      if (x1 < x2)
         ang = 0.0;
      else
         ang = 180.0;
		return ang;
   }

   n = fabs(y1-y2);
   n /= fabs(x1-x2);
   ang = atan(n);
	ang = RAD_TO_DEG(ang);

   if (((x1-x2) < 0) && ((y1-y2) < 0))   // quad 1
      ang = ang;
   if (((x1-x2) > 0) && ((y1-y2) < 0))   // quad 2
      ang = 180.0 - ang;
   if (((x1-x2) > 0) && ((y1-y2) > 0))   // quad 3
      ang += 180.0;
   if (((x1-x2) < 0) && ((y1-y2) > 0))   // quad 4
      ang = -ang;

   // make angle between 0 and 360 degrees
   if (ang > 360.0)
      ang = ang - 360.0;
   if (ang < 0)
      ang = ang + 360.0;

   return ang;
}
// end of line_angle (double)

// ********************************************************************
// ********************************************************************

BOOL CUtil::line_segment_intersection(	double ax1, double ay1, double ax2, double ay2,
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

CString CUtil::extract_path( const CString& csFilespec )
{
	INT iPos;
	iPos = csFilespec.ReverseFind( '\\' );    // Last backslash
   return ( iPos >= 0 ) ? csFilespec.Left( iPos + 1 ) : _T("");
}

// ********************************************************************
// ********************************************************************

CString CUtil::extract_filename( const CString& csFilespec )
{
	INT iPos;
	iPos = csFilespec.ReverseFind('\\');
   return ( iPos >= 0 ) ? csFilespec.Mid( iPos + 1 ) : _T("");
}

// ********************************************************************
// ********************************************************************

CString CUtil::extract_extension(CString fullname)
{
	int len, pos;
	CString ext;

	ext = "";
	len = fullname.GetLength();
	pos = fullname.ReverseFind('.');
	if (pos >= 0)
		ext = fullname.Right(len-pos-1);
	return ext;
}

// ********************************************************************
// ********************************************************************

BOOL CUtil::create_directory(const CString& dirname)
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

// ********************************************************************
// ********************************************************************

// round to nearest integer
int CUtil::round(double val)
{
   return (val > 0.0 ? ((int) (val + 0.5)) : ((int) (val - 0.5)));
}

// ********************************************************************
// ********************************************************************

int CUtil::magnitude(int x1, int y1, int x2, int y2)
{
   double mag_sqr;

   mag_sqr = ((double)(x1 - x2) * (double)(x1 - x2)) +
      ((double)(y1 - y2) * (double)(y1 - y2));

   return (int)(sqrt(mag_sqr) + 0.5);
}
// end of magnitude

// ********************************************************************
// ********************************************************************

void CUtil::draw_text(
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
	int oldmode, oldalign;
	COLORREF color, bkcolor, oldcolor, oldbkcolor;
	UINT align_type;
	int tx, ty;
	int width, height;
	CString drawtext;
	CFont cfont;

   // Allocate memory for a LOGFONT structure.
	PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));
//   PLOGFONT plf = (PLOGFONT) malloc(sizeof(LOGFONT));

   // Specify a font typeface name and weight.
	lstrcpy(plf->lfFaceName, font.GetBuffer(sizeof(plf->lfFaceName)));
	plf->lfWeight = FW_NORMAL;

	oldmode = dc->SetBkMode(TRANSPARENT);

	plf->lfHeight = font_size;
	plf->lfWidth = 0;

   // set the font attributes
	if (font_attrib & UTIL_FONT_BOLD)
		plf->lfWeight = 800;
	else
		plf->lfWeight = 400;

	if (font_attrib & UTIL_FONT_ITALIC)
		plf->lfItalic = TRUE;
	else
		plf->lfItalic = FALSE;

   if (font_attrib & UTIL_FONT_UNDERLINE)
      plf->lfUnderline = TRUE;
   else
      plf->lfUnderline = FALSE;

   if (font_attrib & UTIL_FONT_STRIKEOUT)
      plf->lfStrikeOut = TRUE;
   else
      plf->lfStrikeOut = FALSE;

//	plf->lfQuality = PROOF_QUALITY;

	plf->lfEscapement = (long) (- (double) angle * 10.0);
	plf->lfOrientation = plf->lfEscapement;

	lstrcpy(plf->lfFaceName, font.GetBuffer(sizeof(plf->lfFaceName)));
	if (!cfont.CreateFontIndirect(plf))
	{
		lstrcpy(plf->lfFaceName, "Arial");
		if (!cfont.CreateFontIndirect(plf))
		{
			LocalFree((LOCALHANDLE) plf);
			return;
		}
	}

   CFont* cfntPrev = (CFont*) dc->SelectObject(&cfont);
   color = code2color(text_color);
   bkcolor = code2color(back_color);
   oldcolor = dc->SetTextColor(color);
   oldbkcolor = dc->SetBkColor(bkcolor);
   align_type = anchor2textalign(anchor_pos);
   CRect trc(x, y, x + 200, y + 75);

   oldalign = dc->SetTextAlign(align_type);
   dc->SetTextCharacterExtra(1);

   tx = x;
   ty = y;

   if ((background == UTIL_BG_RECT) || (background == UTIL_BG_RECT))
   {
      tx += 1;
   }

   if (pad_spaces)
   {
      // put spaces before and after the text
      drawtext = " ";
      drawtext += text;
      drawtext += " ";
   }
   else
      drawtext = text;

   CSize size = dc->GetTextExtent(drawtext);

   width = size.cx;
   height = size.cy;

 	// compute the new anchor point if not a standard one
	if ((anchor_pos == UTIL_ANCHOR_CENTER_CENTER) ||
		 (anchor_pos == UTIL_ANCHOR_CENTER_LEFT) ||
		 (anchor_pos == UTIL_ANCHOR_CENTER_RIGHT))
	{
		double tang, newangle;
		int dx, dy;

		ty += height / 2;
		tang = (double) angle + 90.0;

		if (fabs(tang - 90.0) > 0.0001)
		{
			newangle = DEG_TO_RAD(tang - 90.0);
			// rotate the starting point about the origin
			dx = tx - x;
			dy = ty - y;
			tx = round((dx * cos(newangle)) - (dy * sin(newangle)) + x);
			ty = round((dy * cos(newangle)) + (dx * sin(newangle)) + y);
		}
	}

	compute_text_poly(x, y, anchor_pos,   width, height, angle, cpt);

	switch(background)
	{
		case UTIL_BG_RECT:
		case UTIL_BG_3D:
		if ((background == UTIL_BG_3D) && (angle == 0.0))
		{
			CBrush brush;
			CPen pen;
			brush.CreateSolidBrush(bkcolor);
			pen.CreatePen(PS_SOLID, 1, color);
			CPen* oldpen = (CPen*) dc->SelectObject(&pen);
			CBrush* oldbrush = (CBrush*) dc->SelectObject(&brush);
			dc->Polygon(cpt, 4);
			CPen lightpen, darkpen, blackpen;
			lightpen.CreatePen(PS_SOLID, 1, code2color(UTIL_COLOR_WHITE));
			darkpen.CreatePen(PS_SOLID, 1, code2color(UTIL_COLOR_MEDIUM_GRAY));
			blackpen.CreatePen(PS_SOLID, 1, code2color(UTIL_COLOR_BLACK));
			dc->SelectObject(&darkpen);
			dc->MoveTo(cpt[0].x, cpt[0].y);
			dc->LineTo(cpt[1].x, cpt[1].y);
			dc->LineTo(cpt[2].x, cpt[2].y);
			dc->LineTo(cpt[3].x, cpt[3].y);
			dc->LineTo(cpt[0].x, cpt[0].y);
			dc->SelectObject(&lightpen);
			dc->MoveTo(cpt[3].x - 1, cpt[3].y);
			dc->LineTo(cpt[0].x - 1, cpt[0].y - 1);
			dc->LineTo(cpt[1].x + 1, cpt[1].y  - 1);
			dc->SelectObject(&blackpen);
			dc->MoveTo(cpt[1].x+1, cpt[1].y  - 1);
			dc->LineTo(cpt[2].x+1, cpt[2].y+1);
			dc->LineTo(cpt[3].x - 2, cpt[3].y+1);
			dc->SelectObject(oldpen);
			dc->SelectObject(oldbrush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
		else
		{
			CBrush brush;
			CPen pen;
			brush.CreateSolidBrush(bkcolor);
			pen.CreatePen(PS_SOLID, 1, color);
			CPen* oldpen = (CPen*) dc->SelectObject(&pen);
			CBrush* oldbrush = (CBrush*) dc->SelectObject(&brush);
			dc->Polygon(cpt, 4);
			dc->SelectObject(oldpen);
			dc->SelectObject(oldbrush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
		break;
   case UTIL_BG_SHADOW:
		{
		 int j, k;
		 dc->SetTextColor(bkcolor);
		 for (k=-1; k<=1; k++)
			for (j=-1; j<=1; j++)
			   dc->TextOut(tx+j, ty+k, drawtext);
		 dc->SetTextColor(color);
		}
	}

	dc->TextOut(tx, ty, drawtext);
	dc->SetTextAlign(oldalign);
	dc->SetTextColor(oldcolor);
	dc->SetBkColor(oldbkcolor);
	dc->SelectObject(cfntPrev);
	cfont.DeleteObject();

	// Reset the background mode to its default.
	dc->SetBkMode(oldmode);

	// Free the memory allocated for the LOGFONT structure.
	LocalFree((LOCALHANDLE) plf);
}
// end of draw_text

// ********************************************************************
// ********************************************************************

void CUtil::get_text_size
(
   CDC *dc,          // pointer to DC to draw in
   CString text,     // text to draw
   CString font_name,// font name
   int font_size,    // font size in points
   int font_attrib,  // font attributes (bold, italic, etc)
   int *width,       // width of draw text
   int *height,      // height of draw text
   BOOL pad_spaces   // default is TRUE
)
{
   //generic pass/fail result
   BOOL bResult;

   //make the desired font reference
   CFont myFont;
   bResult = myFont.CreatePointFont( font_size*10, font_name, dc );

   //put desired font into context
   CFont *prevFont = dc->SelectObject( &myFont );

   //get the widths for all possible ascii characters
   int charWidth[256];
   bResult = dc->GetCharWidth( 0, 255, charWidth );

   //add up the char widths in our string
   *width=0;
   int n = text.GetLength();
   for (int i=0;i<n;i++)
   {
      unsigned c = text[i];
      c &= 0xff;
      *width += charWidth[ c ];
   }

   //windows seems to know the correct height in all cases
   CSize size = dc->GetTextExtent(text);
   *height = size.cy;

   //font width structure
   ABC abc;

   //what is the size of the last character in the string?
   bResult = dc->GetCharABCWidths( text[n-1], text[n-1], &abc );
   //trim the white space on the end
   width -= abc.abcC;

   //what is the size of the first character in the string?
   bResult = dc->GetCharABCWidths( text[0], text[0], &abc );
   //trim the white space on the front
   width -= abc.abcA;

   //restore the previous font context
   dc->SelectObject( prevFont );

   //wax the temporary object we made
   myFont.DeleteObject();
}

// ********************************************************************
// ********************************************************************

void CUtil::get_text_size
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
	int oldmode;
	CString drawtext;
	CFont cfont;
	BOOL multi = FALSE;
	int k, len;
	char ch;

	len = text.GetLength();
	for (k=0; k<len; k++)
	{
		ch = text.GetAt(k);
		if (ch == 0x0a)
			multi = TRUE;
	}

	// check for multi-line text
	if (multi)
	{
		int tx, ty;
		int anchor_pos = UTIL_ANCHOR_LOWER_LEFT;
		int background = UTIL_BG_NONE;
		POINT cpt[4];

		tx = 100;
		ty = 100;
		get_multi_line_bounds(dc, text, tx, ty, anchor_pos, font_name, font_size, font_attrib, background,
							angle, cpt, pad_spaces);
		*width = magnitude(cpt[0].x, cpt[0].y, cpt[1].x, cpt[1].y);
		*height = magnitude(cpt[1].x, cpt[1].y, cpt[2].x, cpt[2].y);
		return;
	}

   /* Allocate memory for a LOGFONT structure. */
   PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));
//   PLOGFONT plf = (PLOGFONT) malloc(sizeof(LOGFONT));

   /* Specify a font typeface name and weight. */
   lstrcpy(plf->lfFaceName, font_name.GetBuffer(sizeof(plf->lfFaceName)));
   plf->lfWeight = FW_NORMAL;

   oldmode = dc->SetBkMode(TRANSPARENT);

   plf->lfHeight = font_size;
   plf->lfWidth = 0;

   plf->lfEscapement = (long) (- (double) angle * 10.0);
   plf->lfOrientation = plf->lfEscapement;


   // set the font attributes
   if (font_attrib & UTIL_FONT_BOLD)
      plf->lfWeight = 800;
   else
      plf->lfWeight = 400;

   if (font_attrib & UTIL_FONT_ITALIC)
      plf->lfItalic = TRUE;
   else
      plf->lfItalic = FALSE;

   if (font_attrib & UTIL_FONT_UNDERLINE)
      plf->lfUnderline = TRUE;
   else
      plf->lfUnderline = FALSE;

   if (font_attrib & UTIL_FONT_STRIKEOUT)
      plf->lfStrikeOut = TRUE;
   else
      plf->lfStrikeOut = FALSE;

   lstrcpy(plf->lfFaceName, font_name.GetBuffer(sizeof(plf->lfFaceName)));
   if (!cfont.CreateFontIndirect(plf))
   {
         LocalFree((LOCALHANDLE) plf);
         return;
   }

   CFont* cfntPrev = (CFont*) dc->SelectObject(&cfont);
   dc->SetTextCharacterExtra(1);

   if (pad_spaces)
   {
      // put spaces before and after the text
      drawtext = " ";
      drawtext += text;
      drawtext += " ";
   }
   else
      drawtext = text;

   CSize size = dc->GetTextExtent(drawtext);

   *width = size.cx;
   *height = size.cy;

   dc->SelectObject(cfntPrev);
   cfont.DeleteObject();

   /* Free the memory allocated for the LOGFONT structure. */
   LocalFree((LOCALHANDLE) plf);
}
// end of get_text_size

// ********************************************************************
// ********************************************************************

void CUtil::get_multi_line_bounds
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
   int oldalign;
   UINT align_type;
   int tx, ty;
   int width, height;
   CString drawtext;
   CList<CString*, CString*> text_list;
   // set up the font
   CFont cfont;

   // Allocate memory for a LOGFONT structure.
   PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));

   // Specify a font typeface name and weight.
   lstrcpy(plf->lfFaceName, font.GetBuffer(sizeof(plf->lfFaceName)));
   plf->lfWeight = FW_NORMAL;
    plf->lfHeight = font_size;
   plf->lfWidth = 0;
   plf->lfEscapement = (long) (- (double) angle * 10.0);
   plf->lfOrientation = plf->lfEscapement;

   // set the font attributes
   if (font_attrib & UTIL_FONT_BOLD)
      plf->lfWeight = 800;
   else
      plf->lfWeight = 400;

   if (font_attrib & UTIL_FONT_ITALIC)
      plf->lfItalic = TRUE;
   else
      plf->lfItalic = FALSE;

   if (font_attrib & UTIL_FONT_UNDERLINE)
      plf->lfUnderline = TRUE;
   else
      plf->lfUnderline = FALSE;

   if (font_attrib & UTIL_FONT_STRIKEOUT)
      plf->lfStrikeOut = TRUE;
   else
      plf->lfStrikeOut = FALSE;

   lstrcpy(plf->lfFaceName, font.GetBuffer(sizeof(plf->lfFaceName)));
   if (!cfont.CreateFontIndirect(plf))
   {
         LocalFree((LOCALHANDLE) plf);
         return;
   }

   CFont* cfntPrev = (CFont*) dc->SelectObject(&cfont);

   parse_the_text(text, text_list);
   if (text_list.IsEmpty())
   {
         LocalFree((LOCALHANDLE) plf);
         return;
   }

   align_type = anchor2textalign(anchor_pos);
   CRect trc(x, y, x + 200, y + 75);

//   oldalign = dc->SetTextAlign(align_type);
   oldalign = dc->SetTextAlign(TA_TOP | TA_LEFT);
   dc->SetTextCharacterExtra(1);

   tx = x;
   ty = y;

   if (background == UTIL_BG_RECT)
   {
      tx += 1;
   }

   // build the text block
   width = 0;
   height = 0;
   POSITION next;
   CString *tstr;
   next = text_list.GetHeadPosition();
   do
   {
      tstr = text_list.GetNext(next);
      if (pad_spaces)
      {
         // put spaces before and after the text
         drawtext = " ";
         drawtext += *tstr;
         drawtext += " ";
      }
      else
         drawtext = *tstr;
      CSize size = dc->GetTextExtent(drawtext);
      if (size.cx > width)
         width = size.cx;
      height += size.cy;
   } while (next != NULL);

   compute_text_poly(x, y, anchor_pos,   width, height, angle, cpt);

   dc->SetTextAlign(oldalign);
   dc->SelectObject(cfntPrev);
   cfont.DeleteObject();

   /* Free the memory allocated for the LOGFONT structure. */
   LocalFree((LOCALHANDLE) plf);

   // clean up the text_list
   while (!text_list.IsEmpty())
   {
      tstr = text_list.RemoveHead();
      delete tstr;
   }
}
// end of get_multi_line_bounds

// ********************************************************************
// ********************************************************************

void CUtil::parse_the_text(CString text, CList<CString *, CString *> &text_list)
{
   // We want to separate 'text' into a list of CStrings each of
   // which is contained on a single line.  In other words, we
   // need to look for any '\n' or '\r' escape sequences and break
   // apart the string at these locations.
	char *token = strtok(text.GetBuffer(text.GetLength()), "\n\r");
	while (token)
	{
		text_list.AddTail(new CString(token));
		token = strtok(NULL, "\n\r");
	}
}
// end of parse_the_text

// ********************************************************************
// ********************************************************************

UINT CUtil::anchor2textalign(int anchor)
{
   switch(anchor)
   {
     case UTIL_ANCHOR_LOWER_LEFT     :
        return TA_BOTTOM | TA_LEFT;
        break;
      case UTIL_ANCHOR_UPPER_LEFT     :
        return TA_TOP | TA_LEFT;
        break;
      case UTIL_ANCHOR_LOWER_CENTER   :
         return TA_BOTTOM | TA_CENTER;
       break;
      case UTIL_ANCHOR_UPPER_CENTER   :
        return TA_TOP | TA_CENTER;
        break;
      case UTIL_ANCHOR_LOWER_RIGHT    :
        return TA_BOTTOM | TA_RIGHT;
        break;
      case UTIL_ANCHOR_UPPER_RIGHT    :
        return TA_TOP | TA_RIGHT;
        break;
      case UTIL_ANCHOR_CENTER_LEFT    :
        return TA_BOTTOM | TA_LEFT;
        break;
      case UTIL_ANCHOR_CENTER_RIGHT    :
        return TA_BOTTOM | TA_RIGHT;
        break;
      case UTIL_ANCHOR_CENTER_CENTER   :
        return TA_BOTTOM | TA_CENTER;
        break;
   }
   return 0;
}
// end of anchor2textalign

// ********************************************************************
// ********************************************************************

// translates the utility class code to WINAPI code

int CUtil::code2fill(int code)
{
   int fill = 0;

   switch (code)
   {
      case UTIL_FILL_HORZ:
         fill = HS_HORIZONTAL;
         break;
      case UTIL_FILL_VERT:
         fill = HS_VERTICAL;
         break;
      case UTIL_FILL_BDIAG:
         fill = HS_BDIAGONAL;
         break;
      case UTIL_FILL_FDIAG:
         fill = HS_FDIAGONAL;
         break;
      case UTIL_FILL_CROSS:
         fill = HS_CROSS;
         break;
      case UTIL_FILL_DIAGCROSS:
         fill = HS_DIAGCROSS;
         break;
   }
   return fill;
}
// end of code2fill

// ********************************************************************
// ********************************************************************

COLORREF CUtil::code2color(int code)
{
	switch(code)
	{
		case UTIL_COLOR_BLACK: return RGB(0,0,0); break;
		case UTIL_COLOR_DARK_RED: return RGB(128,   0,   0); break;
		case UTIL_COLOR_DARK_GREEN: return RGB(0, 128,   0); break;
		case UTIL_COLOR_DARK_YELLOW: return RGB(128, 128,   0); break;
		case UTIL_COLOR_DARK_BLUE: return RGB(0,   0, 128); break;
		case UTIL_COLOR_DARK_MAGENTA: return RGB(128,   0, 128); break;
		case UTIL_COLOR_DARK_CYAN: return RGB(0, 128, 128); break;
		case UTIL_COLOR_LIGHT_GRAY: return RGB(192, 192, 192); break;
		case UTIL_COLOR_MONEY_GREEN: return RGB(192, 220, 192); break;
		case UTIL_COLOR_SKY_BLUE: return RGB(166, 202, 240); break;
		case UTIL_COLOR_CREAM: return RGB(255, 251, 240); break;
		case UTIL_COLOR_MEDIUM_GRAY: return RGB(160, 160, 164); break;
		case UTIL_COLOR_DARK_GRAY: return RGB(128, 128, 128); break;
		case UTIL_COLOR_RED: return RGB(255,   0,   0); break;
		case UTIL_COLOR_GREEN: return RGB(0, 255,   0); break;
		case UTIL_COLOR_YELLOW: return RGB(255, 255,   0); break;
		case UTIL_COLOR_BLUE: return RGB(0,   0, 255); break;
		case UTIL_COLOR_MAGENTA: return RGB(255,   0, 255); break;
		case UTIL_COLOR_CYAN: return RGB(  0, 255, 255); break;
		case UTIL_COLOR_WHITE: return RGB(255, 255, 255); break;
		default: return RGB(255, 255, 255); break;
	}
}
// end of code2color

// ********************************************************************
// ********************************************************************

// compute the corners of a polygon (rotated rectangle) that encloses a block of text

void CUtil::compute_text_poly(	int tx, int ty,			// x/y of anchor point
								int anchor_pos,			// position of anchor point
                                int width, int height,	// height and width of text
								double text_angle,		// angle of text
                                POINT *cpt)					// OUT - corners of rectangle enclosing text
{
	// define the bounding rectangle
	double angle, newangle, cosang, sinang;
	int k, dx, dy;
	int x, y, cx, cy;

	x = tx;
	y = ty;
	cx = width;
	cy = height;

	dx = cx / 2;
	dy = cy / 2;

	switch(anchor_pos)
	{
		case UTIL_ANCHOR_LOWER_LEFT     :
			cpt[0].x = tx;
			cpt[0].y = ty - cy;
			cpt[1].x = tx + cx;
			cpt[1].y = ty - cy;
			cpt[2].x = tx + cx;
			cpt[2].y = ty;
			cpt[3].x = tx;
			cpt[3].y = ty;
			break;
		case UTIL_ANCHOR_UPPER_LEFT     :
			cpt[0].x = tx;
			cpt[0].y = ty;
			cpt[1].x = tx + cx;
			cpt[1].y = ty;
			cpt[2].x = tx + cx;
			cpt[2].y = ty + cy;
			cpt[3].x = tx;
			cpt[3].y = ty + cy;
			break;
		case UTIL_ANCHOR_LOWER_CENTER   :
			cpt[0].x = tx - dx;
			cpt[0].y = ty - cy;
			cpt[1].x = tx + dx;
			cpt[1].y = ty - cy;
			cpt[2].x = tx + dx;
			cpt[2].y = ty;
			cpt[3].x = tx - dx;
			cpt[3].y = ty;
			break;
		case UTIL_ANCHOR_UPPER_CENTER   :
			cpt[0].x = tx - dx;
			cpt[0].y = ty;
			cpt[1].x = tx + dx;
			cpt[1].y = ty;
			cpt[2].x = tx + dx;
			cpt[2].y = ty + cy;
			cpt[3].x = tx - dx;
			cpt[3].y = ty + cy;
			break;
		case UTIL_ANCHOR_LOWER_RIGHT    :
			cpt[0].x = tx - cx;
			cpt[0].y = ty - cy;
			cpt[1].x = tx;
			cpt[1].y = ty - cy;
			cpt[2].x = tx;
			cpt[2].y = ty;
			cpt[3].x = tx - cx;
			cpt[3].y = ty;
			break;
		case UTIL_ANCHOR_UPPER_RIGHT    :
			cpt[0].x = tx - cx;
			cpt[0].y = ty;
			cpt[1].x = tx;
			cpt[1].y = ty;
			cpt[2].x = tx;
			cpt[2].y = ty + cy;
			cpt[3].x = tx - cx;
			cpt[3].y = ty + cy;
			break;
		case UTIL_ANCHOR_CENTER_LEFT    :
			cpt[0].x = tx;
			cpt[0].y = ty - dy;
			cpt[1].x = tx + cx;
			cpt[1].y = ty - dy;
			cpt[2].x = tx + cx;
			cpt[2].y = ty + dy;
			cpt[3].x = tx;
			cpt[3].y = ty + dy;
			break;
		case UTIL_ANCHOR_CENTER_RIGHT    :
			cpt[0].x = tx - cx;
			cpt[0].y = ty - dy;
			cpt[1].x = tx;
			cpt[1].y = ty - dy;
			cpt[2].x = tx;
			cpt[2].y = ty + dy;
			cpt[3].x = tx - cx;
			cpt[3].y = ty + dy;
			break;
		case UTIL_ANCHOR_CENTER_CENTER   :
			cpt[0].x = tx - dx;
			cpt[0].y = ty - dy;
			cpt[1].x = tx + dx;
			cpt[1].y = ty - dy;
			cpt[2].x = tx + dx;
			cpt[2].y = ty + dy;
			cpt[3].x = tx - dx;
			cpt[3].y = ty + dy;
			break;
		default:
			cpt[0].x = tx;
			cpt[0].y = ty;
			cpt[1].x = tx;
			cpt[1].y = ty;
			cpt[2].x = tx;
			cpt[2].y = ty;
			cpt[3].x = tx;
			cpt[3].y = ty;
			break;
	}


	angle = (double) text_angle + 90.0;

	//  rotate the string vector list
	if (fabs(angle - 90.0) > 0.0001)
	{
		newangle = DEG_TO_RAD(angle - 90.0);
		cosang = cos(newangle);
		sinang = sin(newangle);
		// rotate the bounding rectangle about the origin
		for (k=0; k< 4; k++)
		{
			dx = cpt[k].x - tx;
			dy = cpt[k].y - ty;
			cpt[k].x = round((dx * cosang) - (dy * sinang) + tx);
			cpt[k].y = round((dy * cosang) + (dx * sinang) + ty);
		}
	}
}
// end of compute_text_poly


// ********************************************************************
// ********************************************************************
