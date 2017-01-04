#include "stdafx.h"

#include "gtest/gtest.h"

#include "FalconView/localpnt/factory.h"
#include "FalconView/localpnt/GroupNamesSearchField.h"
#include "FalconView/localpnt/localpnt.h"

static const char* s_test_file_name =
   "c:\\data\\Setuptrees\\fvw 5.2.0\\government\\sample\\Points\\local.lps";
TEST(C_localpnt_ovl, DISABLED_NOT_A_UNIT_TEST_open_lps_succeeds)
{
   C_localpnt_ovl_factory factory;
   C_overlay* overlay;
   factory.CreateOverlayInstance(&overlay);
   C_localpnt_ovl* local_point_overlay =
      static_cast<C_localpnt_ovl*>(overlay);
   ASSERT_EQ(SUCCESS, local_point_overlay->open_lps(s_test_file_name));
   overlay->Release();
}

TEST(C_localpnt_ovl, DISABLED_NOT_A_UNIT_TEST_save_as_lps_succeeds)
{
   C_localpnt_ovl_factory factory;
   C_overlay* overlay;
   factory.CreateOverlayInstance(&overlay);
   C_localpnt_ovl* local_point_overlay =
      static_cast<C_localpnt_ovl*>(overlay);
   ASSERT_EQ(SUCCESS, local_point_overlay->save_as_lps(s_test_file_name));
   overlay->Release();
}

TEST(GroupNamesSearchField, DISABLED_NOT_A_UNIT_TEST_Initialize_succeeds)
{
   GroupNamesSearchField group;
   std::string filename(s_test_file_name);
   ASSERT_EQ(SUCCESS, group.Initialize(filename.c_str()));
}

TEST(GroupNamesSearchField, DISABLED_NOT_A_UNIT_TEST_UpdateValue_succeeds)
{
   GroupNamesSearchField group;
   std::string filename(s_test_file_name);
   group.Initialize(filename.c_str());

   std::string group_name("Default");
   ASSERT_EQ(SUCCESS, group.UpdateSearch(group_name, false));
}
