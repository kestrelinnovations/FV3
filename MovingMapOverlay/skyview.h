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



// skyview.h

#pragma once

#import "SkyView.tlb" no_namespace named_guids

// Wrapper class for calling SkyView COM methods.
class SkyViewInterface
{
public:

   int init();
   void uninit();

public:

   // ISkyView method wrappers
   long Add3DObject(CString FileName);
   long Add3DShape(short Shape);
   long SetPosition(long Handle, double Latitude, double Longitude, float Alt_MSL);
   long SetOrientation(long Handle, float Heading, float Pitch, float Roll);
   long DeleteObject(long Handle);
   long Scale3DObject(long Handle, float Scale);
   long GetCameraHandle();
   long SetPositionMeters(long Handle, float X, float Y, float Z);
   long AttachObject(long Handle, long ParentHandle);
   long AddStringObject(CString Text);
   long SetStringText(long Handle, CString Text);
   float SetFOVAngle(short Select, float AngleInDeg);
   short SelectFOV(short NewSelect);
   long LoadMap(double Latitude, double Longitude, long MapScale, short LoadNextAvailable);
   // Stretch the specified 3D object to the specified size in meters.
   long Size3DObject(long Handle, float len, float wid, float height);
   long SetColor(long Handle, float Red, float Green, float Blue);
   long SetDisplay(long Handle, short is_solid_not_wireframe);
   long GetObjectSize(long Handle, float* Length, float* Width, float* Height);

   // ISkyView2 method wrappers
   void AddCOM3DObject(CString progid, ISkyView3DObjectPtr &object, long &object_handle);
   void CreateAndAssociateCOMMotionPlayback(CString progid, long object_handle, IMotionPlaybackPtr &playback);
   void UnassociateAndReleaseCOMMotionPlayback(long object_handle);
   void EnablePlayback(bool enable);
   void SetPlaybackTime(DATE time);
   void SetPlaybackTimeAdvancementMultiplier(double multiplier);
   void EnablePlaybackTimeAdvancement(bool enable);

   IDispatch *GetDispatch() { return IDispatchPtr(m_skyview); }

private:

   ISkyViewPtr m_skyview;
   ISkyView2Ptr m_skyview2;
};
