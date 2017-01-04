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



// MovingMapFeed.h
//

// for IGPSFeed
#import "fvw.tlb" named_guids

class C_gps_trail;

class MovingMapFeed 
{
   C_gps_trail *m_parent;
   BOOL m_is_connected;

   // pointer to the external GPS Feed component
   fvw::IGPSFeed *m_pGPSFeed;
   

   int m_created_feed_type;

public:
   // constructor, destructor
   MovingMapFeed(C_gps_trail *parent);
   ~MovingMapFeed();
fvw::IMovingMapFeed *m_pMovingMapFeed;
   BOOL is_connected() { return m_is_connected; }

   // called when the feed type has changed
   void on_feed_changed();

   // toggles between connect/disconnect state
   int ToggleConnection();

   // display the options dialog for this feed
   int Options();

   CString get_feed_name();

   BOOL SupportsProperties() { return m_pMovingMapFeed != NULL; }

   int SaveProperties(CString strFilename);
   int LoadProperties(CString strFilename, BOOL bAutoConnect);

private:

   // disconnect from the moving map feed
   int disconnect();

   // connect to the moving map feed
   int connect();

   // creates the gps feed component object
   int CreateFeedComponent();

   void communication_dlg();

   int get_feed_type();
};