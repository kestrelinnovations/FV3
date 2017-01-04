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

// MdsConfigFile.h
//

#pragma once

#define MDS_CONFIG_FILENAME "\\config\\MapDataServerConfig.xml"   // name relative to USER_DATA
#define MDS_CONFIG_ELEMENT_NAME "MapDataServerConfig"
#define TARGET_DATA_SOURCE_ID_NODE_NAME "TargetDataSourceId"
#define DATA_SOURCE_ID_ATTRIBUTE "dataSourceId"

// CMdsConfigFile - The CMdsConfigFile class loads and stores Map Data Server configuration
//    information to an xml file format.
//
class CMdsConfigFile
{
   long m_nTargetDataSourceId;

public:
   CMdsConfigFile() : m_nTargetDataSourceId(-1)
   {
      LoadConfigFile();
   }
   ~CMdsConfigFile()
   {
      SaveConfigFile();
   }

   long GetTargetDataSourceId() { return m_nTargetDataSourceId; }
   void SetTargetDataSourceId(long nDataSourceId) { m_nTargetDataSourceId = nDataSourceId; }

private:
   void LoadConfigFile();
   void SaveConfigFile();
};