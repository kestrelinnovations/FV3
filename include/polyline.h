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



//
// polyline.h
//

// PolyLine class for the overlay system



#ifndef CPOLYLN_H 
#define CPOLYLN_H 1


#include "common.h"
#include "ovl_d.h"
#include "overlay.h"
#include "fvwutil.h"
#include "IndexFil.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

// forward class declarations
class MapProj;
class CPolyLine;
class CPoly_vertex;

//*****************************************************************************
//*****************************************************************************

class CPoly_pt : public CPoint
{
public:
   degrees_t   m_lat;
   degrees_t   m_lon;

	// standard constructor
	CPoly_pt() {};
	CPoly_pt(MapProj* map, CPoint pt);
	CPoly_pt(MapProj* map, degrees_t lat, degrees_t lon);
   CPoly_pt& operator=(CPoly_vertex& rhs);

	// get the screen coordinates associated with the geo_pt - returns
	// the CPoint screen equivalent based upon the current MAP boundaries
	CPoint screen_pt();

};

//*****************************************************************************
//*****************************************************************************
// 2001/1/22 DVL :: removed C_icon base class, was unused (??), to save memory in VMap 
// overlay, re-insert if this causes problems, some umember variables size reduced
class CPoly_vertex // : C_icon
{
//private:
public:
	CPoly_pt	m_pos;						// location of vertex

	// member variables
public:
	bool     		m_selected;			// vertex selected flag
	bool     		m_hilighted;		// vertex has get info focus
	unsigned char	m_alt_color;		// alternate color for segment connected to...
	unsigned char  m_alt_bkgrnd_color;//background color for m_alt_color
	//short	   	m_vertex_id;		// id within related structure (ie., CPolyLine)
	CRect				m_rect;				// hit test bounds rectangle
	CPolyLine*		m_parent;			// parent poly_line for this vertex

   // If the line made by this vertex and the previous vertex in the CList
   // in CPolyLine "wraps around" the world and shows up on both the left
   // and the right side of the screen, then these screen coordinates and
   // the screen coordinates of the two vertices make up the two lines
   // that need to be drawn.  m_midpoint1 and the previous vertex make up
   // one line, and m_midpoint2 and this vertex make up the other line.
   CPoint*        m_midpoint1;      // screen coordinates of 
   CPoint*        m_midpoint2;      // screen coordinates of

	// member functions
public:
	// standard constructors and destructor
	CPoly_vertex(MapProj* map, CPolyLine* parent, double lat, double lon);
	CPoly_vertex(CPolyLine* parent, CPoly_pt point);

   // accessor (and assignment) functions
   CPoint    screen_pt();
   degrees_t lat();
   degrees_t lon();
   long      &x();
   long      &y();

   void set_position(CPoly_pt point);
   bool is_segmented();

	// overlay point information routines
	boolean_t is_kind_of(const char* class_name);
   const char* get_class_name();
   CString get_help_text();
	CString get_tool_tip();

	// information functions
	int hit_test(CPoint testpt);
	BOOL is_selected();
	BOOL is_hilighted();


	// state functions
	void select(BOOL selected);
	void hilight(BOOL hilighted);
	BOOL move(double lat, double lon, CPoint pt, CRect rc);
	
	// draw functions
	BOOL display_handle(CDC* dc, boolean_t filled);
};


//*****************************************************************************
//*****************************************************************************


class CPolyLine : public C_icon
{
	// member variables
public:
	BOOL					m_selected;			// is the polyline selected?
	BOOL					m_all_selected;	// are all vertices selected?
	CString				m_tool_tip_text;	// tool tip info for this polyline
	CString				m_help_text;		// status bar info for this polyline
	CString				m_name;				// text descriptor for this polyline

	// static variables needed to support drag/edit functions
	static CPoly_pt	m_start_pt;		// current drag starting screen coordinates
	static CPoly_pt	m_cur_pt;		// current drag end screen coordinates
	static CPoly_pt	m_old_pt;		// previous drag starting screen coordinates (for xor erase)

	static boolean_t	m_first_drag_point;	// indicates that a new polyline is starting
	static CPoint		m_drag_origin;		// beginning point from 
	static CPoint		m_last_origin;		// beginning point from 
	static int			m_drag_vertex;		// id of vertex in drag
	static CRect		m_orig_rect;		// bouding box at the beginning of any drag
	
	CList<CPoly_vertex*, CPoly_vertex*> m_vertices;

	// constructor and destructor
public:
   CPolyLine(C_overlay *overlay);
   CPolyLine(const CPolyLine& polyline);
   ~CPolyLine();
   CPolyLine &operator=(const CPolyLine& polyline);

   // overlay point information routines
   boolean_t is_kind_of(const char* class_name);
   const char* get_class_name();

	boolean_t hit_test(CPoint testpt);
	boolean_t hit_test(CPoint testpt, int* index);
	boolean_t in_view(MapProj* map);
   d_geo_rect_t get_bounds(){return m_bounds;}

   // Set Polyline simplification on or off,
   // returns the previous simplification state of the polyline
   boolean_t set_polyline_simplification( boolean_t on );

	// draw functions
	boolean_t draw(map_projection_utils::CMapProjWrapper* map, CDC* dc, boolean_t handles = FALSE);
	void draw_point_handles(CDC* dc);
	boolean_t draw_hilighted(map_projection_utils::CMapProjWrapper* map, CDC* dc);
	void invalidate_point_handles();
	void invalidate();
	void invalidate_vertex(int index);
	void invalidate_selected_vertices();
	void invalidate_line(CPoint p1, CPoint p2);
   void invalidate_line(int x1, int y1, int x2, int y2);
   void refresh_screen_coordinates(MapProj* map);

	// edit functions
	BOOL display_bounds_handles(CDC* dc);
	BOOL rubber_band_point(ViewMapProj* map, CDC* dc, CPoint oldpt, CPoint newpt, int index);
	BOOL rubber_band_line(MapProj* map, CDC* dc, CPoly_pt start_pt, CPoly_pt end_pt);

	// state functions -- color
	void set_color(int color);
	int get_color();
   void set_outline_color(int color);
   int get_outline_color();
	// state functions -- width
	void set_width(int width);
	int get_width();
	// state functions -- style
	void set_line_style(int style);
	int get_line_style();
	// state functions -- name
	void set_name(CString name);
	CString get_name();
	// state functions -- help text
	void set_help_text(CString text);
	CString get_help_text();
	// state functions -- tool tip
	void set_tool_tip(CString text);
	CString get_tool_tip();
	
	// vertex functions
	int add_vertex(CPoly_pt &geo_pt);
	int insert_vertex(MapProj* map, int index_before, int x, int y,	double lat, double lon);	// SUCCESS/FAILURE
	int move_vertex(int index, CPoly_pt point);
	int move_vertex(MapProj* map, int index, degrees_t lat, degrees_t lon);
	int get_vertex_pos(int index, degrees_t* lat, degrees_t* lon);
	
	int delete_vertex(int vertex);																// SUCCESS/FAILURE
	int delete_selected_vertices();																// SUCCESS/FAILURE
	void delete_all_vertices();

	BOOL select_all();
	BOOL unselect_all();
	BOOL select(int index);
	BOOL toggle_select(int index);
	boolean_t all_selected();

	int vertex_count();
	int select_count();

	// used for a drag operation of an existing point on the overlay
	//		begin_drag to be called by "select"
	//		move_drag to be called by "drag"
	//		end_drag to be called by "drop"
	static void begin_new_line();
	boolean_t is_first_segment();
   int compare_last_node(MapProj* map, CPoint point) const;

	// drag of new vertices
	int begin_drag(ViewMapProj* map, CPoint point);													// SUCCESS/FAILURE
	int move_drag(ViewMapProj* map, CPoint point);													// SUCCESS/FAILURE
	int end_drag(ViewMapProj* map, CPoint point);													// SUCCESS/FAILURE
	int cancel_drag(ViewMapProj* map);																	// SUCCESS/FAILURE

	// drag of single vertex
	int begin_vertex_drag(ViewMapProj* map, CPoint point, int vertex);						// SUCCESS/FAILURE
	int move_vertex_drag(ViewMapProj* map, CPoint point);											// SUCCESS/FAILURE
	int end_vertex_drag(ViewMapProj* map, CPoint point);											// SUCCESS/FAILURE
	int cancel_vertex_drag(ViewMapProj* mapw);														// SUCCESS/FAILURE


	// i/o functions
	void write_record(CIndexFile* out_file);
	void create_from_record(MapProj* map, CIndexFile* in_file);

private:
   // this will only draw standard (WINDOWS supported) style lines
   void draw_line(MapProj* map, CDC *dc, CPoly_pt p1, CPoly_pt p2);

   // this will invalidate any style line
   void invalidate_line_segment(int index);

   // Given two vertices and a list of points, this will assign the
   // appropriate values to vtx2->m_midpoint1 and vtx2->m_midpoint1.
   // It will allocate and de-allocate the pointers to m_midpoint1
   // and m_midpoint2 as necessary.
   int assign_midpoints(CPoly_vertex* vtx1, CPoly_vertex* vtx2, POINT points[], int point_count);

   // Helper functions for hit_test()
   boolean_t hit_test_vertices(CPoint testpt, int* index);
   boolean_t hit_test_lines(CPoint testpt, int* index);

   // Called during add_vertex(), this updates the bounding
   // box for the polyline, correctly accounting for crossing
   // the international date line.
   void update_bounding_box(CPoly_vertex* const vtx);

// Private Data
private:
   unsigned char m_width;          // line width
   unsigned char m_color;          // line color
   unsigned char m_outline_color;  // outline color
   // This will control when to simplify a polyline
   bool  m_bSimplify_Polyline_On;

   int m_style;          // line style

   d_geo_rect_t m_bounds;

};


#endif // CPOLYLN_H
