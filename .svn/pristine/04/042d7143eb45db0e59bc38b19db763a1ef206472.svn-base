// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// OverlayEventRouter_UnitTests.cpp
//

#include "stdafx.h"

#include "gtest/gtest.h"

#include "include/overlay.h"
#include "include/ovl_mgr.h"
#include "overlay/OverlayEventRouter.h"
// The anonymous namespace is to handle compiler issues with FakeOverlay being 
// defined in multiple Unit Tests.  
namespace
{
// OverlayMethodCalls is a class that contains a sequence representing the
// series of method calls made to one or more objects. Pass an instance of this
// class to an object to be observered.
class OverlayMethodCalls
{
public:
   OverlayMethodCalls()
   {
   }

   // OverlayMethodCallType - enumeration defining the type of method calls
   // that can be added to the sequence
   enum OverlayMethodCallType
   {
      kInvalidate = 0,
      kOnDraw,
      kOnDrawToBaseMap,
      kHaltOnDrawEventRouting,
      kAppendMenuItems,
      kCanSnapTo,
      kDrawToVerticalDisplay,
      kCanAddPixmapsToBaseMap
   };

   // Adds a method call to the sequence
   void AddMethodCall(std::string instance_name, OverlayMethodCallType type)
   {
      m_method_calls.push_back(std::make_pair<>(instance_name, type));
   }

   // Returns the numbers of method that were called
   size_t Count() const
   {
      return m_method_calls.size();
   }

   // Returns the name of the instance name corresponding to the n-th method
   // call
   std::string GetNthInstanceName(size_t n) const
   {
      return m_method_calls[n].first;
   }
   
   // Returns the type of the n-th method call made
   OverlayMethodCallType GetNthMethodCallType(size_t n) const
   {
      return m_method_calls[n].second;
   }

private:
   std::vector< std::pair<std::string, OverlayMethodCallType> > m_method_calls;

   DISABLE_COPY_AND_ASSIGN_CTORS(OverlayMethodCalls)
};

// FakeOverlay - used to test event routing to C_overlay instances
class FakeOverlay : public C_overlay
{
public:
   // constructor
   FakeOverlay(const std::string& name, bool is_visible, bool is_snappable,
      bool can_add_pixmaps, OverlayMethodCalls* overlay_method_calls) : 
      m_name(name),
      m_is_visible(is_visible),
      m_is_snappable(is_snappable),
      m_can_add_pixmaps(can_add_pixmaps),
      m_overlay_method_calls(overlay_method_calls)
   {
   }

   STDMETHOD_(ULONG, AddRef)() override
   {
      return 1;
   }
   STDMETHOD_(ULONG, Release)() override
   {
      delete this;
      return 0;
   }

   // Pure virtual that we are required to implement. It does not take part
   // in any of the tests.
   virtual FalconViewOverlayLib::IFvOverlayPtr GetFvOverlay() override
   {
      return nullptr;
   }

   // True if the overlay is visible, false otherwise.
   virtual long get_m_bIsOverlayVisible() override
   {
      return m_is_visible;
   }

   // Invalidate is called by an OverlayEventRouter to invalidate an overlay
   virtual void invalidate() override
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kInvalidate);
   }

   // Invalidate is called by an OverlayEventRouter to draw an overlay
   virtual HRESULT OnDraw(IActiveMapProj* ) override
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kOnDraw);
      return S_OK;
   }

   virtual int OnDrawToBaseMap(IActiveMapProj* ) override
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kOnDrawToBaseMap);
      return S_OK;
   }

   virtual HRESULT AppendMenuItems(
      FalconViewOverlayLib::IFvContextMenu* ,
      FalconViewOverlayLib::IFvMapView* , long , long ) override
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kAppendMenuItems);
      return S_OK;
   }

   virtual boolean_t test_snap_to(ViewMapProj* , CPoint )
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kCanSnapTo);
      return m_is_snappable ? TRUE : FALSE;
   }

   virtual int DrawToVerticalDisplay(CDC *dc,
      CVerticalViewProjector* vertical_view_proj) override
   {
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kDrawToVerticalDisplay);
      return SUCCESS;
   }

   virtual BOOL can_add_pixmaps_to_base_map() override
   { 
      m_overlay_method_calls->AddMethodCall(m_name,
         OverlayMethodCalls::kCanAddPixmapsToBaseMap);
      return m_can_add_pixmaps ? TRUE : FALSE;
   }

   virtual bool IsTopMostOverlay()
   {
      return false;
   }

   virtual int Opacity()
   {
      return 100;
   }

private:
   // Name to distinguish an instance of this class from other instances
   std::string m_name;

   // Flag which indicates whether the overlay is visible or not
   bool m_is_visible;

   // Flag which indicates whether test_snap_to will return true or not
   bool m_is_snappable;

   // Flag which indicates whether can_add_pixmaps_to_base_map will return
   // return or not
   bool m_can_add_pixmaps;

   // Used to sense which methods have been called on this object
   OverlayMethodCalls* m_overlay_method_calls;

   DISABLE_COPY_AND_ASSIGN_CTORS(FakeOverlay)
};

// TestEventRouterCallback - used to test behavior of event routing with
// a callback.
class TestEventRouterCallback : public OverlayEventRouterCallback_Interface
{
public:
   TestEventRouterCallback(OverlayMethodCalls* overlay_method_calls,
      int halt_on_ith_call) : 
      m_overlay_method_calls(overlay_method_calls),
      m_halt_on_ith_call(halt_on_ith_call),
      m_num_calls(0)
   {
   }
   virtual bool HaltOnDrawEventRouting() override
   {
      m_overlay_method_calls->AddMethodCall("",
         OverlayMethodCalls::kHaltOnDrawEventRouting);

      return m_halt_on_ith_call == m_num_calls++;
   }

private:
   // Used to sense method calls on this object
   OverlayMethodCalls* m_overlay_method_calls;

   // The total number of calls made to HaltOnDrawEventRouting
   int m_num_calls;

   // Zero-based index for which to return true in HaltOnDrawEventRouting
   int m_halt_on_ith_call;

   DISABLE_COPY_AND_ASSIGN_CTORS(TestEventRouterCallback)
};

// Test that a single, visible overlay is invalidated
TEST(OverlayEventRouterTest, TestInvalidateOverlaysSingleVisibleOverlay)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, visible overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay visible_overlay("", true, false, false, &method_calls);
   overlays.push_back(&visible_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.InvalidateOverlays();

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kInvalidate,
      method_calls.GetNthMethodCallType(0));
}

// Overlays that are not visible should not be invalidated. The following test
// validates this behavior
TEST(OverlayEventRouterTest, TestInvalidateOverlaysHiddenOverlaySkipped)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, hidden overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay hidden_overlay("", false, false, false, &method_calls);
   overlays.push_back(&hidden_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.InvalidateOverlays();

   EXPECT_EQ(0, method_calls.Count());
}

// When the hide-background overlays option is set, then the invalidate event
// should only be routed to the current overlay and no others
TEST(OverlayEventRouterTest, TestInvalidateOverlaysHideBackgroundOverlaysSet)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", true, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.InvalidateOverlays();

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_STREQ("Current", method_calls.GetNthInstanceName(0).c_str());
}

// When the hide-background overlays option is set and the current overlay is
// non-visible, then no invalidation event should occur
TEST(OverlayEventRouterTest, TestInvalidateOverlaysNonVisibleCurrent)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", false, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.InvalidateOverlays();

   EXPECT_EQ(0, method_calls.Count());
}

// The invalidate event should be sent to overlays in reverse order (from the
// bottom of the overlay stack up). This test will validate this.
TEST(OverlayEventRouterTest, TestInvalidateOverlaysReverseOrder)  
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create a sequence of overlays (A, B, C) where 'A' is the top-most overlay
   // in the list. We will expect the invalidation to be called first on 'C',
   // then 'B', and finally 'A'.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.InvalidateOverlays();

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(2).c_str());
}

// Even if the user specifies a callback it should not be used in the
// routing of the invalidation event
TEST(OverlayEventRouterTest, TestInvalidateOverlaysNoCallbackExpected)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). We expect all overlays to be
   // invalidated despite the callback.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.InvalidateOverlays();

   EXPECT_EQ(2, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kInvalidate,
      method_calls.GetNthMethodCallType(0));
   EXPECT_EQ(OverlayMethodCalls::kInvalidate,
      method_calls.GetNthMethodCallType(1));
}

// Test that a single, visible overlay is drawn
TEST(OverlayEventRouterTest, TestOnDrawOverlaysSingleVisibleOverlay)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, visible overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay visible_overlay("", true, false, false, &method_calls);
   overlays.push_back(&visible_overlay);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDraw,
      method_calls.GetNthMethodCallType(0));
}

// Overlays that are not visible should not be drawn. The following test
// validates this behavior
TEST(OverlayEventRouterTest, TestOnDrawOverlaysHiddenOverlaySkipped)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, hidden overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay hidden_overlay("", false, false, false, &method_calls);
   overlays.push_back(&hidden_overlay);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(0, method_calls.Count());
}

// When the hide-background overlays option is set, then the OnDraw event
// should only be routed to the current overlay and no others
TEST(OverlayEventRouterTest, TestOnDrawOverlaysHideBackgroundOverlaysSet)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", true, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDraw, method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("Current", method_calls.GetNthInstanceName(0).c_str());
}

// When the hide-background overlays option is set and the current overlay is
// non-visible, then no event should occur
TEST(OverlayEventRouterTest, TestOnDrawOverlaysNonVisibleCurrent)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", false, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(0, method_calls.Count());
}

// The OnDraw event should be sent to overlays in reverse order (from the
// bottom of the overlay stack up). This test will validate this.
TEST(OverlayEventRouterTest, TestOnDrawOverlaysReverseOrder)  
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create a sequence of overlays (A, B, C) where 'A' is the top-most overlay
   // in the list. We will expect OnDraw to be called first on 'C', then 'B',
   // and finally 'A'.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(2).c_str());
}

// Test that the OverlayEventRouter_Callback interface works as expected.
// Halt the first time the callback is triggered.
TEST(OverlayEventRouterTest, TestOnDrawOverlaysHaltAfterFirst) 
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). With the callback halting on the
   // first call we expect only the bottom overlay, 'B', to be drawn.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.OnDrawOverlays(nullptr);

   EXPECT_EQ(2, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDraw, method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(1));
}

// Ensure that if the callback never halts then all overlays are drawn as
// expected.
TEST(OverlayEventRouterTest, TestOnDrawOverlaysNeverHalt) 
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface.
   // Set up to never halt the event routing.
   TestEventRouterCallback never_halt_callback(&method_calls, INT_MAX);

   // Create a sequence of overlays (A, B, C).
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &never_halt_callback);
   router.OnDrawOverlays(nullptr);

   // Expect an OnDraw for each overlay (from the bottom of the stack to the
   // top) following by a call to the halt callback, which always returns false.
   // The halt callback should _not_ be called after the last overlay is drawn.
   EXPECT_EQ(5, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDraw, method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(1));
   EXPECT_EQ(OverlayMethodCalls::kOnDraw, method_calls.GetNthMethodCallType(2));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(2).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(3));
   EXPECT_EQ(OverlayMethodCalls::kOnDraw, method_calls.GetNthMethodCallType(4));
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(4).c_str());
}

// Test that a single, visible overlay is drawn to base map
TEST(OverlayEventRouterTest, TestOnDrawOverlaysToBaseMapSingleVisibleOverlay)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, visible overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay visible_overlay("", true, false, false, &method_calls);
   overlays.push_back(&visible_overlay);

   // Route the OnDrawBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(0));
}

// Overlays that are not visible should not be drawn. The following test
// validates this behavior
TEST(OverlayEventRouterTest, TestOnDrawOverlaysToBaseMapHiddenOverlaySkipped)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, hidden overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay hidden_overlay("", false, false, false, &method_calls);
   overlays.push_back(&hidden_overlay);

   // Route the OnDraw event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(0, method_calls.Count());
}

// When the hide-background overlays option is set, then the OnDraw event
// should only be routed to the current overlay and no others
TEST(OverlayEventRouterTest,
   TestOnDrawOverlaysToBaseMapHideBackgroundOverlaysSet)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", true, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the OnDrawToBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("Current", method_calls.GetNthInstanceName(0).c_str());
}

// When the hide-background overlays option is set and the current overlay is
// non-visible, then no event should occur
TEST(OverlayEventRouterTest, TestOnDrawToBaseMapOverlaysNonVisibleCurrent)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", false, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the OnDrawToBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(0, method_calls.Count());
}

// The OnDrawToBaseMap event should be sent to overlays in reverse order (from
// the bottom of the overlay stack up). This test will validate this.
TEST(OverlayEventRouterTest, TestOnDrawOverlaysToBaseMapReverseOrder)  
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create a sequence of overlays (A, B, C) where 'A' is the top-most overlay
   // in the list. We will expect OnDraw to be called first on 'C', then 'B',
   // and finally 'A'.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the OnDrawToBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(2).c_str());
}

// Test that the OverlayEventRouter_Callback interface works as expected.
// Halt the first time the callback is triggered.
TEST(OverlayEventRouterTest, TestOnDrawToBaseMapOverlaysHaltAfterFirst) 
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). With the callback halting on the
   // first call we expect only the bottom overlay, 'B', to be drawn.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);

   // Route the OnDrawToBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.OnDrawOverlaysToBaseMap(nullptr);

   EXPECT_EQ(2, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(1));
}

// Ensure that if the callback never halts then all overlays are drawn as
// expected.
TEST(OverlayEventRouterTest, TestOnDrawToBaseMapOverlaysNeverHalt) 
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface.
   // Set up to never halt the event routing.
   TestEventRouterCallback never_halt_callback(&method_calls, INT_MAX);

   // Create a sequence of overlays (A, B, C).
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the OnDrawOverlaysToBaseMap event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &never_halt_callback);
   router.OnDrawOverlaysToBaseMap(nullptr);

   // Expect an OnDraw for each overlay (from the bottom of the stack to the
   // top) following by a call to the halt callback, which always returns false.
   // The halt callback should _not_ be called after the last overlay is drawn.
   EXPECT_EQ(5, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(1));
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(2));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(2).c_str());
   EXPECT_EQ(OverlayMethodCalls::kHaltOnDrawEventRouting,
      method_calls.GetNthMethodCallType(3));
   EXPECT_EQ(OverlayMethodCalls::kOnDrawToBaseMap,
      method_calls.GetNthMethodCallType(4));
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(4).c_str());
}

// Test a single, visible overlay
TEST(OverlayEventRouterTest, TestAppendMenuItemsSingleVisibleOverlay)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, visible overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay visible_overlay("", true, false, false, &method_calls);
   overlays.push_back(&visible_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kAppendMenuItems,
      method_calls.GetNthMethodCallType(0));
}

// Overlays that are not visible should not have menu items appended. The
// following test validates this behavior
TEST(OverlayEventRouterTest, TestAppendMenuItemsHiddenOverlaySkipped)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, hidden overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay hidden_overlay("", false, false, false, &method_calls);
   overlays.push_back(&hidden_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(0, method_calls.Count());
}

// When the hide-background overlays option is set, then menu items should
// only be routed to the current overlay and no others
TEST(OverlayEventRouterTest, TestAppendMenuItemsHideBackgroundOverlaysSet)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", true, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_STREQ("Current", method_calls.GetNthInstanceName(0).c_str());
}

// When the hide-background overlays option is set and the current overlay is
// non-visible, then no append menu items event should occur
TEST(OverlayEventRouterTest, TestAppendMenuItemsNonVisibleCurrent)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create the current overlay, which we expect to be called when the
   // hide-background overlays option is set
   FakeOverlay current_overlay("Current", false, false, false, &method_calls);

   // Add a non-current overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay noncurrent_overlay("Non-current", true, false, false,
      &method_calls);
   overlays.push_back(&noncurrent_overlay);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, true, &current_overlay);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(0, method_calls.Count());
}

// Menu items should be added from top down (from the top of the overlay stack
// up). This test will validate this.
TEST(OverlayEventRouterTest, TestAppendMenuItemsInOrder)  
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create a sequence of overlays (A, B, C) where 'A' is the top-most overlay
   // in the list. We will expect append menu items to be called first on 'A',
   // then 'B', and finally 'C'.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(2).c_str());
}

// Even if the user specifies a callback it should not be used in the
// routing of the append menu items event
TEST(OverlayEventRouterTest, TestAppendMenuItemsNoCallbackExpected)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). We expect all overlays to be
   // invalidated despite the callback.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.AppendMenuItems(nullptr, nullptr, CPoint(0,0));

   EXPECT_EQ(2, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kAppendMenuItems,
      method_calls.GetNthMethodCallType(0));
   EXPECT_EQ(OverlayMethodCalls::kAppendMenuItems,
      method_calls.GetNthMethodCallType(1));
}

// Test that if an overlay returns that it has snappable points at the given
// location then the routing of events is properly halted
TEST(OverlayEventRouterTest, TestCanSnapToRoutingHalted)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). The overlay 'A' will signal that
   // it handled the event so 'B' should never receive the event.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, true, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.CanSnapTo(nullptr, CPoint(0, 0));

   EXPECT_EQ(1, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kCanSnapTo,
      method_calls.GetNthMethodCallType(0));
}

// If no overlay handles the event, then all overlays should have received
// the event. This test verifies that.
TEST(OverlayEventRouterTest, TestCanSnapToInOrder)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B). The overlay 'A' will signal that
   // it handled the event so 'B' should never receive the event.
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the invalidate event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.CanSnapTo(nullptr, CPoint(0, 0));

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kCanSnapTo,
      method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kCanSnapTo,
      method_calls.GetNthMethodCallType(1));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_EQ(OverlayMethodCalls::kCanSnapTo,
      method_calls.GetNthMethodCallType(2));
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(2).c_str());
}

// Verify that all overlays are routed the DrawToVerticalDisplay event
TEST(OverlayEventRouterTest, TestDrawToVerticalDisplayInOrder)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Used to observe method calls to the OverlayEventRouter_Callback interface
   TestEventRouterCallback callback(&method_calls, 0);

   // Create a sequence of overlays (A, B, C).
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Route the DrawToVerticalDisplay event to the list of overlays
   OverlayEventRouter router(overlays, false, nullptr, &callback);
   router.DrawToVerticalDisplay(nullptr, nullptr);

   EXPECT_EQ(3, method_calls.Count());
   EXPECT_EQ(OverlayMethodCalls::kDrawToVerticalDisplay,
      method_calls.GetNthMethodCallType(0));
   EXPECT_STREQ("C", method_calls.GetNthInstanceName(0).c_str());
   EXPECT_EQ(OverlayMethodCalls::kDrawToVerticalDisplay,
      method_calls.GetNthMethodCallType(1));
   EXPECT_STREQ("B", method_calls.GetNthInstanceName(1).c_str());
   EXPECT_EQ(OverlayMethodCalls::kDrawToVerticalDisplay,
      method_calls.GetNthMethodCallType(2));
   EXPECT_STREQ("A", method_calls.GetNthInstanceName(2).c_str());
}

TEST(OverlayEventRouterTest, TestIsOverlayValid)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create a sequence of overlays (A, B, C).
   std::vector<C_overlay *> overlays;
   FakeOverlay a("A", true, false, false, &method_calls);
   FakeOverlay b("B", true, false, false, &method_calls);
   FakeOverlay c("C", true, false, false, &method_calls);
   overlays.push_back(&a);
   overlays.push_back(&b);
   overlays.push_back(&c);

   // Use to the router to determine if a given overlay is invalid or not
   OverlayEventRouter router(overlays, false, nullptr);
   EXPECT_EQ(false,
      router.IsOverlayValid(reinterpret_cast<C_overlay *>(0xFFFF)));
   EXPECT_EQ(true, router.IsOverlayValid(&a));
   EXPECT_EQ(true, router.IsOverlayValid(&b));
   EXPECT_EQ(true, router.IsOverlayValid(&c));
   EXPECT_EQ(false, router.IsOverlayValid(nullptr));
}

TEST(OverlayEventRouterTest, TestCanAddPixmapsToBaseMap)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // A and B will not add pixmaps to the base map
   FakeOverlay a("", true, false, false, &method_calls);
   FakeOverlay b("", true, false, false, &method_calls);

   // C and D will add pixmaps to the base map
   FakeOverlay c("", true, false, true, &method_calls);
   FakeOverlay d("", true, false, true, &method_calls);

   // Create a list of overlays that do not add pixmaps and verify that
   // the method returns false.
   std::vector<C_overlay *> overlays_w_no_pixmaps;
   overlays_w_no_pixmaps.push_back(&a);
   overlays_w_no_pixmaps.push_back(&b);
   OverlayEventRouter router_w_no_pixmaps(overlays_w_no_pixmaps,
      false, nullptr);
   EXPECT_EQ(false, router_w_no_pixmaps.CanAddPixmapsToBaseMap());

   // Create a list of overlays all of which can add pixmaps to the base
   // map. Expect true in this case.
   std::vector<C_overlay *> overlays_w_pixmaps;
   overlays_w_pixmaps.push_back(&c);
   overlays_w_pixmaps.push_back(&d);
   OverlayEventRouter router_w_pixmaps(overlays_w_pixmaps, false, nullptr);
   EXPECT_EQ(true, router_w_pixmaps.CanAddPixmapsToBaseMap());

   // Create a mixed list of overlays. Since one or more of them can add
   // pixmaps to the base map, expect true.
   std::vector<C_overlay *> overlays;
   overlays.push_back(&a);
   overlays.push_back(&c);
   overlays.push_back(&b);
   overlays.push_back(&d);
   OverlayEventRouter router(overlays, false, nullptr);
   EXPECT_EQ(true, router.CanAddPixmapsToBaseMap());
}

TEST(OverlayEventRouterTest,
   HideBackgroundOverlaysEnabledWithNullCurrentOverlay)
{
   // Used to observe method calls on the TestOverlay instances
   OverlayMethodCalls method_calls;

   // Create and add a single, visible overlay to the list of overlays
   std::vector<C_overlay *> overlays;
   FakeOverlay overlay("", true, false, false, &method_calls);

   // Route the invalidate event to the list of overlays
   bool background_overlays_enabled = true;
   C_overlay* current_overlay = nullptr;
   OverlayEventRouter router(overlays, background_overlays_enabled,
      current_overlay);
   router.InvalidateOverlays();

   EXPECT_EQ(0, method_calls.Count());
}

}  // namespace

