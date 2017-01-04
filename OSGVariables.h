


#ifndef VARS
#define VARS

namespace osg
{
   class Vec3d;
}
#endif

class OSGUserVars
{
public:
   static void OSGUserVars::SetGroundState(bool b);
   static bool OSGUserVars::GetGroundState();
   static void OSGUserVars::SetFlightState(bool b);
   static bool OSGUserVars::GetFlightState();
   static void OSGUserVars::SetPanState(bool b);
   static bool OSGUserVars::GetPanState();
   static void OSGUserVars::SetAnimationState(bool b);
   static bool OSGUserVars::GetAnimationState();
   static std::pair<double, double> OSGUserVars::GetMousePosition();
   static void OSGUserVars::SetMousePosition(double latIn, double lonIn);
   static void OSGUserVars::SetViewshedState(bool b);
   static bool OSGUserVars::GetViewshedState();
   static void OSGUserVars::ViewshedUpdateRequired(bool b);
   static bool OSGUserVars::IsViewshedUpdateRequired();
   static int OSGUserVars::AddKey(std::string key);
   static int OSGUserVars::RemoveKey(std::string key);
   static int OSGUserVars::ContainsKey(std::string key);
   static int OSGUserVars::SetElevationUpscale(std::string key, int i);
   static double OSGUserVars::GetElevationUpscale(std::string key);
   static int OSGUserVars::SetElevationUpscaleNeed(std::string key, int i);
   static int OSGUserVars::GetElevationUpscaleNeed(std::string key);
   static void OSGUserVars::SetOffsetDistance(double d);
   static double OSGUserVars::GetOffsetDistance();
   static void OSGUserVars::SetCameraLatLon(double lat, double lon);
   static std::pair<double, double> OSGUserVars::GetCameraLatLon();
   static void OSGUserVars::SetCameraHeight(float height_in, float lod);
   static std::pair<double, double> OSGUserVars::GetCameraHeight();
   static void OSGUserVars::SetAltitude(double val);
   static double OSGUserVars::GetAltitude();
   static void OSGUserVars::SetActiveLOD(int lod);
   static int OSGUserVars::GetActiveLOD();
   static void OSGUserVars::WipeLOD(unsigned int lod);
   static void OSGUserVars::SetTimeToUpdate(double time_in);
   static double OSGUserVars::GetTimeToUpdate();
   static void OSGUserVars::SetDesiredForwardVec(osg::Vec3d* vec);
   static osg::Vec3d OSGUserVars::GetDesiredForwardVec();
};

