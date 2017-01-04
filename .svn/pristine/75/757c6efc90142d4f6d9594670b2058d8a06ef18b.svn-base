// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

#include "StdAfx.h"
#include "gtest/gtest.h"
#include "OverlayElements.h"
#include "osg/ref_ptr"
#include "osgEarthUtil/Controls"

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
#define SURFACE_WIDTH 1000
#define SURFACE_HEIGHT 1000

osg::ref_ptr<osgEarth::Util::Controls::ImageControl> PositionScreenOverlay(
   double image_anchor_x, double image_anchor_y,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
   double rotation_center_x, double rotation_center_y,
   double rotation_degrees, double x_scale, double y_scale)
{
   // initialize a dummy image control
   osg::ref_ptr<osg::Image> image;
   osg::ref_ptr<osgEarth::Util::Controls::ImageControl> image_control =
      new osgEarth::Util::Controls::ImageControl(image);
   OverlayElements::InitScreenOverlayImageControl(image_control,
      image_anchor_x, image_anchor_y, screen_anchor_x, screen_anchor_x_units,
      screen_anchor_y, screen_anchor_y_units, rotation_center_x,
      rotation_center_y, rotation_degrees, x_scale, y_scale);

   // do some tricks to make it think there is an image
   image_control->setSize(IMAGE_WIDTH, IMAGE_HEIGHT);
   osgEarth::Util::Controls::ControlContext cx;
   osg::Vec2f out_size;
   image_control->calcSize(cx, out_size);

   // calculate the position
   osg::Vec2f cursor(0, 0), parent_size(SURFACE_WIDTH, SURFACE_HEIGHT);
   image_control->calcPos(cx, cursor, parent_size);

   // return the smart pointer
   return image_control;
}

TEST(overlay_elements_tests, test_screen_overlay_centered)
{
   auto image_control = PositionScreenOverlay(
      0.5, 0.5, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(450, image_control->renderPos().x());
   EXPECT_EQ(452, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_above_center)
{
   auto image_control = PositionScreenOverlay(
      0.5, -0.1, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(450, image_control->renderPos().x());
   EXPECT_EQ(392, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_below_center)
{
   auto image_control = PositionScreenOverlay(
      0.5, 1.1, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(450, image_control->renderPos().x());
   EXPECT_EQ(512, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_left_of_center)
{
   auto image_control = PositionScreenOverlay(
      1.1, 0.5, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(390, image_control->renderPos().x());
   EXPECT_EQ(452, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_right_of_center)
{
   auto image_control = PositionScreenOverlay(
      -0.1, 0.5, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(510, image_control->renderPos().x());
   EXPECT_EQ(452, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_quarter_lower_left)
{
   auto image_control = PositionScreenOverlay(
      0.5, 0.5, // image anchor (fraction)
      0.25, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.25, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(200, image_control->renderPos().x());
   EXPECT_EQ(703, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_quarter_upper_left)
{
   auto image_control = PositionScreenOverlay(
      0.5, 0.5, // image anchor (fraction)
      0.25, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.75, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(200, image_control->renderPos().x());
   EXPECT_EQ(201, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_quarter_upper_right)
{
   auto image_control = PositionScreenOverlay(
      0.5, 0.5, // image anchor (fraction)
      0.75, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.75, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(700, image_control->renderPos().x());
   EXPECT_EQ(201, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_quarter_lower_right)
{
   auto image_control = PositionScreenOverlay(
      0.5, 0.5, // image anchor (fraction)
      0.75, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.25, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(700, image_control->renderPos().x());
   EXPECT_EQ(703, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_lower_left)
{
   auto image_control = PositionScreenOverlay(
      0.0, 0.0, // image anchor (fraction)
      50, SCREEN_ANCHOR_UNITS_PIXELS, // screen X anchor
      50, SCREEN_ANCHOR_UNITS_PIXELS, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(52, image_control->renderPos().x());
   EXPECT_EQ(852, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_upper_left)
{
   auto image_control = PositionScreenOverlay(
      0.0, 1.0, // image anchor (fraction)
      50, SCREEN_ANCHOR_UNITS_PIXELS, // screen X anchor
      50, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(52, image_control->renderPos().x());
   EXPECT_EQ(52, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_upper_right)
{
   auto image_control = PositionScreenOverlay(
      1.0, 1.0, // image anchor (fraction)
      50, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen X anchor
      50, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(852, image_control->renderPos().x());
   EXPECT_EQ(52, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_lower_right)
{
   auto image_control = PositionScreenOverlay(
      1.0, 0.0, // image anchor (fraction)
      50, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen X anchor
      50, SCREEN_ANCHOR_UNITS_PIXELS, // screen Y anchor
      0.0, 0.0, 0.0, 1.0, 1.0);

   EXPECT_EQ(852, image_control->renderPos().x());
   EXPECT_EQ(852, image_control->renderPos().y());
}

TEST(overlay_elements_tests, test_screen_overlay_above_center_rotated)
{
   auto image_control = PositionScreenOverlay(
      0.5, -0.1, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.5, 0.5, // rotation center X & Y
      45.0, // rotation degrees
      1.0, 1.0);

   // Screen overlay rotation about a point is not working quite right, so these
   // test values may change a bit once that is fixed.

   EXPECT_NEAR(429, image_control->renderPos().x(), 1.0);
   EXPECT_NEAR(346, image_control->renderPos().y(), 1.0);

   osgEarth::Util::Controls::ControlContext cx;
   osg::Vec3Array* verts = new osg::Vec3Array(4); // deletes itself
   image_control->calcRotation(
      SURFACE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, verts);

   EXPECT_NEAR(429, (*verts)[0][0], 1.0);
   EXPECT_NEAR(583, (*verts)[0][1], 1.0);

   EXPECT_NEAR(500, (*verts)[1][0], 1.0);
   EXPECT_NEAR(513, (*verts)[1][1], 1.0);

   EXPECT_NEAR(570, (*verts)[2][0], 1.0);
   EXPECT_NEAR(583, (*verts)[2][1], 1.0);

   EXPECT_NEAR(500, (*verts)[3][0], 1.0);
   EXPECT_NEAR(654, (*verts)[3][1], 1.0);
}

TEST(overlay_elements_tests, test_screen_overlay_above_center_rotated_by_corner)
{
   auto image_control = PositionScreenOverlay(
      0.5, -0.1, // image anchor (fraction)
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen X anchor
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, // screen Y anchor
      0.0, 1.0, // rotation center X & Y
      -180.0, // rotation degrees
      1.0, 1.0);

   // Screen overlay rotation about a point is not working quite right, so these
   // test values may change a bit once that is fixed.

   EXPECT_EQ(450, image_control->renderPos().x());
   EXPECT_EQ(392, image_control->renderPos().y());

   osgEarth::Util::Controls::ControlContext cx;
   osg::Vec3Array* verts = new osg::Vec3Array(4); // deletes itself
   image_control->calcRotation(
      SURFACE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, verts);

   EXPECT_EQ(450, (*verts)[0][0]);
   EXPECT_EQ(608, (*verts)[0][1]);

   EXPECT_EQ(450, (*verts)[1][0]);
   EXPECT_EQ(708, (*verts)[1][1]);

   EXPECT_EQ(350, (*verts)[2][0]);
   EXPECT_EQ(708, (*verts)[2][1]);

   EXPECT_EQ(350, (*verts)[3][0]);
   EXPECT_EQ(608, (*verts)[3][1]);
}

TEST(overlay_elements_tests, test_screen_overlay_corner_rotation)
{
   auto image_control = PositionScreenOverlay(
      1.0, 1.0, // image anchor (fraction)
      0, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen X anchor
      0, SCREEN_ANCHOR_UNITS_INSET_PIXELS, // screen Y anchor
      1.0, 1.0, // rotation center X & Y
      -25.0, // rotation degrees
      1.0, 1.0);

   // Screen overlay rotation about a point is not working quite right, so these
   // test values may change a bit once that is fixed.

   EXPECT_NEAR(869, image_control->renderPos().x(), 1.0);
   EXPECT_NEAR(2, image_control->renderPos().y(), 1.0);

   osgEarth::Util::Controls::ControlContext cx;
   osg::Vec3Array* verts = new osg::Vec3Array(4); // deletes itself
   image_control->calcRotation(
      SURFACE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, verts);

   EXPECT_NEAR(889, (*verts)[0][0], 1.0);
   EXPECT_NEAR(1032, (*verts)[0][1], 1.0);

   EXPECT_NEAR(847, (*verts)[1][0], 1.0);
   EXPECT_NEAR(942, (*verts)[1][1], 1.0);

   EXPECT_NEAR(938, (*verts)[2][0], 1.0);
   EXPECT_NEAR(899, (*verts)[2][1], 1.0);

   EXPECT_NEAR(980, (*verts)[3][0], 1.0);
   EXPECT_NEAR(990, (*verts)[3][1], 1.0);
}
