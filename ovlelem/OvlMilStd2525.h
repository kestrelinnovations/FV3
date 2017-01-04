#pragma once

#include "ovlelem.h"

// for OvlMilStd2525Symbol
#ifdef GOV_RELEASE
#import "FvTacticalGraphicsOverlayServer.tlb" no_namespace named_guids
#endif

class OvlMilStd2525Symbol : public OvlElement
{
   double m_lat;
   double m_lon;
   CPoint m_view_coordinates;
   
   CRect m_rect;
   boolean_t m_in_view;

	BSTR m_milstd_id;
   
#ifdef GOV_RELEASE
   ITacticalGraphicsGsdRendererPtr m_gsd_renderer;
#endif

public:
   // Constructor 
   OvlMilStd2525Symbol( BSTR milstd_id );
   
   // Destructor
   ~OvlMilStd2525Symbol();

   // Draw the element from scratch.  This function will get called if there
   // is any change to the underlying map, i.e., scale, center, rotation, etc.
   // This function will get called if there is any change to the underlying
   // data this overlay element represents.  This function must be able to
   // determine if the overlay element is in the current view and draw it, if
   // it is in the view.  Part of this process is making the necessary
   // preparations for the redraw(), hit_test(), invalidate(), and get_rect()
   // members, as all of these functions depend on the action taken by the
   // last call to the draw() function.
   int draw(MapProj* map, CDC* dc);
   
   // Redraw the element exactly as it was drawn by the last call to draw().
   int redraw(MapProj *map, CDC *dc);
   
   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);
   
   // set the location of the icon
   void set_location(double lat, double lon);

	// set the symbol id
	void set_symbol_id(BSTR sym_id) { m_milstd_id = sym_id; }
   
   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen
   CRect get_rect() { return m_rect; }
   
   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point);
   
   // Invalidate the part of the screen covered by this frame
   void invalidate(boolean_t erase_background = FALSE);
   
   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();
   
   // Uses the same information used by the hit_test(), invalidate(), and 
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);

   // returns TRUE if the class name is OvlIcon
   virtual boolean_t is_kind_of(const char *class_name);
};
