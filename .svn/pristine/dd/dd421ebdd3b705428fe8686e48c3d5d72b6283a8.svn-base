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



#include "stdafx.h"
#include "MdsConfigFile.h"

#include "ComErrorObject.h"
#include "..\MdsUtilities\MdsUtilities.h"
#include "param.h"
#include "err.h"

void CMdsConfigFile::LoadConfigFile()
{
   try
   {
      _bstr_t bstrMdsConfigFilename = PRM_get_registry_string("Main", "USER_DATA", "");
      bstrMdsConfigFilename += MDS_CONFIG_FILENAME;

      // If the file doesn't exists, there is nothing to do
      if (::GetFileAttributesW(bstrMdsConfigFilename) ==
         INVALID_FILE_ATTRIBUTES)
      {
         return;
      }

      MSXML2::IXMLDOMDocumentPtr spXMLDOMDocument;
      CO_CREATE(spXMLDOMDocument, __uuidof(MSXML2::DOMDocument60));

      spXMLDOMDocument->async = VARIANT_FALSE;

      if (spXMLDOMDocument->load(bstrMdsConfigFilename) != VARIANT_TRUE)
      {
         ERR_report("Unable to load Map Data Server Config file");
         return;  
      }

      // populate our internal structure with values from the config file - currently there is 
      // only a single child node
      //
      MSXML2::IXMLDOMNodeListPtr smpNodeList = spXMLDOMDocument->documentElement->childNodes;
      MSXML2::IXMLDOMNodePtr smpNode = smpNodeList->nextNode();
      while (smpNode != NULL)
      {
         if (smpNode->nodeName == _bstr_t(TARGET_DATA_SOURCE_ID_NODE_NAME))
         {
            MSXML2::IXMLDOMNamedNodeMapPtr spNamedNodeMap = smpNode->attributes;
            MSXML2::IXMLDOMNodePtr smpAttribute = spNamedNodeMap->getNamedItem(DATA_SOURCE_ID_ATTRIBUTE);
            if (smpAttribute != NULL)
               m_nTargetDataSourceId = atoi((char *)_bstr_t(smpAttribute->nodeValue.bstrVal));

            return;
         }

         smpNode = smpNodeList->nextNode();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to load config file: %s", (char *)e.Description());
      INFO_report(msg);
   }
}

void CMdsConfigFile::SaveConfigFile()
{
   try
   {
      _bstr_t bstrMdsConfigFilename = PRM_get_registry_string("Main", "USER_DATA", "");
      bstrMdsConfigFilename += MDS_CONFIG_FILENAME;

      const DWORD dwStatus = CreateAllDirectories(bstrMdsConfigFilename);
      if (dwStatus != NO_ERROR)
      {
         CString msg;
         msg.Format("CreateAllDirectories failed : %d", dwStatus);
         ERR_report(msg);
         return;
      }

      MSXML2::IXMLDOMDocumentPtr spXMLDOMDocument;
      CO_CREATE(spXMLDOMDocument, __uuidof(MSXML2::DOMDocument60));

      MSXML2::IXMLDOMProcessingInstructionPtr spXMLDOMProcessingInstruction =
         spXMLDOMDocument->createProcessingInstruction("xml", "version=\"1.0\"");

      spXMLDOMDocument->appendChild(spXMLDOMProcessingInstruction);

      MSXML2::IXMLDOMElementPtr spRootElement = spXMLDOMDocument->createNode(MSXML2::NODE_ELEMENT, MDS_CONFIG_ELEMENT_NAME, "");
      spXMLDOMDocument->documentElement = spRootElement;

      MSXML2::IXMLDOMElementPtr spTargetIdNode = spXMLDOMDocument->createNode(MSXML2::NODE_ELEMENT, TARGET_DATA_SOURCE_ID_NODE_NAME, "");
      std::stringstream ss;
      ss << m_nTargetDataSourceId;
      spTargetIdNode->setAttribute(DATA_SOURCE_ID_ATTRIBUTE, ss.str().c_str());
      spRootElement->appendChild(spTargetIdNode);

      spXMLDOMDocument->save(bstrMdsConfigFilename);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to save config file: %s", (char *)e.Description());
      INFO_report(msg);
   }
}