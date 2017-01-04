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

// PointOverlayFonts.cpp
//
#include "stdafx.h"
#include "PointOverlayFonts.h"
#include "..\fvw.h"              // for MyDaoStringAllocCallback

#define OVERLAY_FONT 0
#define GROUP_NAME_FONT 1
#define POINT_FONT 2

const char *FONT_REGISTRY_KEY = "Database Settings\\Local Point Font";

const char *DEFAULT_FONT_NAME = UTIL_FONT_NAME_ARIAL;
const char *DEFAULT_BACKGROUND_COLOR = "#FF000000";  // black
const char *DEFAULT_FOREGROUND_COLOR = "#FFFFFFFF";  // white
const int DEFAULT_POINT_SIZE = 12;
const short DEFAULT_BACKGROUND_TYPE = UTIL_BG_SHADOW;
const bool DEFAULT_STRIKEOUT = false;
const bool DEFAULT_UNDERLINE = false;
const bool DEFAULT_BOLD = false;
const bool DEFAULT_ITALICIZE = false;

CPointOverlayFonts::CPointOverlayFonts()
{
   OvlFont font;
   font.ReadFromRegistry(FONT_REGISTRY_KEY, DEFAULT_FONT_NAME, 
      DEFAULT_POINT_SIZE, DEFAULT_BACKGROUND_COLOR, DEFAULT_FOREGROUND_COLOR, 
      DEFAULT_BACKGROUND_TYPE, DEFAULT_STRIKEOUT, DEFAULT_UNDERLINE, DEFAULT_BOLD, DEFAULT_ITALICIZE);
   SetOverlayFont(font);
}

void CPointOverlayFonts::SetOverlayFont(OvlFont& font, bool bClearGroupAndPointFonts /*= false*/)
{
   const int nIndex = getFontIndex(font);
   m_nOverlayFontLink = nIndex;

   if (bClearGroupAndPointFonts)
   {
      m_mapGroupFontLinks.erase(m_mapGroupFontLinks.begin(), m_mapGroupFontLinks.end());
      m_mapPointFontLinks.erase(m_mapPointFontLinks.begin(), m_mapPointFontLinks.end());
      removeUnusedFonts();
   }
}

void CPointOverlayFonts::SetGroupFont(OvlFont& font, std::string strGroupName)
{
   const int nIndex = getFontIndex(font);
   m_mapGroupFontLinks[strGroupName] = nIndex;
}

void CPointOverlayFonts::SetPointFont(OvlFont& font, FVPOINT p)
{
   const int nIndex = getFontIndex(font);
   m_mapPointFontLinks[p] = nIndex;
}

void CPointOverlayFonts::RemovePointFont(FVPOINT p)
{
   std::map<FVPOINT, int>::iterator it = m_mapPointFontLinks.find(p);
   if (it != m_mapPointFontLinks.end())
      m_mapPointFontLinks.erase(it);
}

bool CPointOverlayFonts::DoesPointFontExist(std::string strPointId, std::string strGroupName)
{
   FVPOINT p(strPointId, strGroupName);
   if (m_mapPointFontLinks.find(p) != m_mapPointFontLinks.end())
      return true;

   return false;
}

bool CPointOverlayFonts::DoesGroupFontExist(std::string strGroupName)
{
   return (m_mapGroupFontLinks.find(strGroupName) != m_mapGroupFontLinks.end());
}

OvlFont& CPointOverlayFonts::GetFont(std::string strPointId, std::string strGroupName)
{
   // first check to see if there is a font associated with the given point id
   FVPOINT p(strPointId, strGroupName);
   if (m_mapPointFontLinks.find(p) != m_mapPointFontLinks.end())
      return m_mapFontData[m_mapPointFontLinks[p]];

   // next, check to see if there is a font associated with the given group
   if (m_mapGroupFontLinks.find(strGroupName) != m_mapGroupFontLinks.end())
      return m_mapFontData[m_mapGroupFontLinks[strGroupName]];
   
   // otherwise, use the font associated with the overlay
   return m_mapFontData[m_nOverlayFontLink];
}

void CPointOverlayFonts::ReadFontsFromFile(CFontDataSet *pFontData, CFontLinksSet *pFontLinks)
{
   while (!pFontData->IsEOF() && !pFontData->IsBOF())
   {
      OvlFont font;
      font.SetFont(pFontData->m_Name, pFontData->m_Size, pFontData->m_BackgroundColor,
         pFontData->m_ForegroundColor, pFontData->m_BackgroundEffect, pFontData->m_Strikeout == VARIANT_TRUE,
         pFontData->m_Underline == VARIANT_TRUE, pFontData->m_Bold == VARIANT_TRUE, pFontData->m_Italic == VARIANT_TRUE);

      m_mapFontData[pFontData->m_FontID] = font;

      pFontData->MoveNext();
   }

   while (!pFontLinks->IsEOF() && !pFontLinks->IsBOF())
   {
      switch (pFontLinks->m_Type)
      {
      case OVERLAY_FONT:  // overlay font
         m_nOverlayFontLink = pFontLinks->m_FontID;
         break;

      case GROUP_NAME_FONT:  // group name font
         m_mapGroupFontLinks[std::string(pFontLinks->m_Group_Name)] = pFontLinks->m_FontID;
         break;

      case POINT_FONT:  // point font
         FVPOINT p(std::string(pFontLinks->m_PointID), std::string(pFontLinks->m_Group_Name));
         m_mapPointFontLinks[p] = pFontLinks->m_FontID;
         break;
      }

      pFontLinks->MoveNext();
   }
}

void CPointOverlayFonts::SaveFontsToFile(CFontDataSet *pFontData, CFontLinksSet *pFontLinks)
{
   removeUnusedFonts();

   std::map<int, OvlFont>::iterator itData = m_mapFontData.begin();
   while (itData != m_mapFontData.end())
   {
      OvlFont& font = itData->second;

      pFontData->AddNew();
      pFontData->m_FontID = (itData++)->first;
      font.GetFont(pFontData->m_Name, pFontData->m_Size, pFontData->m_BackgroundColor,
         pFontData->m_ForegroundColor, pFontData->m_BackgroundEffect, pFontData->m_Strikeout,
         pFontData->m_Underline, pFontData->m_Bold, pFontData->m_Italic);
      
      // Convert back to "AARRGGBB" for LPT format
      if (pFontData->m_ForegroundColor.GetLength() == 9)
      {
         pFontData->m_ForegroundColor = pFontData->m_ForegroundColor.Mid(1, 9);
      }

      if (pFontData->m_BackgroundColor.GetLength() == 9)
      {
         pFontData->m_BackgroundColor = pFontData->m_BackgroundColor.Mid(1, 9);
      }

      pFontData->Update();
   }

   pFontLinks->AddNew();
   pFontLinks->m_FontID = m_nOverlayFontLink;
   pFontLinks->m_Type = OVERLAY_FONT;
   pFontLinks->Update();

   std::map<std::string, int>::iterator itLink = m_mapGroupFontLinks.begin();
   while (itLink != m_mapGroupFontLinks.end())
   {
      pFontLinks->AddNew();
      pFontLinks->m_FontID = itLink->second;
      pFontLinks->m_Type = GROUP_NAME_FONT;
      pFontLinks->m_Group_Name = itLink->first.c_str();
      pFontLinks->Update();
      itLink++;
   }

   std::map<FVPOINT, int>::iterator itPointLink = m_mapPointFontLinks.begin();
   while (itPointLink != m_mapPointFontLinks.end())
   {
      pFontLinks->AddNew();
      pFontLinks->m_FontID = itPointLink->second;
      pFontLinks->m_Type = POINT_FONT;
      pFontLinks->m_PointID = itPointLink->first.strPointId.c_str();
      pFontLinks->m_Group_Name = itPointLink->first.strGroupName.c_str();
      pFontLinks->Update();
      itPointLink++;
   }
}

void CPointOverlayFonts::SetFont(int fontId, CString name, int size, CString bgColor,
   CString fgColor, short bgEffect, bool strikeout, bool underline, bool bold, bool italic)
{
      OvlFont font;
      font.SetFont(name, size, bgColor, fgColor, bgEffect, strikeout, underline, bold, italic);
      m_mapFontData[fontId] = font;
}

void CPointOverlayFonts::SetOverlayFontId(int fontId)
{
   m_nOverlayFontLink = fontId;
}

void CPointOverlayFonts::SetGroupFontId(std::string strGroupName, int fontId)
{
   m_mapGroupFontLinks[strGroupName] = fontId;
}

void CPointOverlayFonts::SetPointFontId(std::string strPointId,
   std::string strGroupName, int fontId)
{
   // Check to see if there is a font associated with the given point id
   FVPOINT p(strPointId, strGroupName);
   m_mapPointFontLinks[p] = fontId;
}

// Helper methods
//

int CPointOverlayFonts::getFontIndex(OvlFont& font)
{
   std::map<int, OvlFont>::iterator it = m_mapFontData.begin();
   while (it != m_mapFontData.end())
   {
      if (it->second == font)
         return it->first;
      it++;
   }

   const int nIndex = getGetNextIndex();
   m_mapFontData[nIndex] = font;

   return nIndex;
}

int CPointOverlayFonts::getGetNextIndex()
{
   int nNextIndex = 1;
   std::map<int, OvlFont>::iterator it = m_mapFontData.begin();
   while (it != m_mapFontData.end())
   {
      if (it->first >= nNextIndex)
         nNextIndex = it->first + 1;
      it++;
   }

   return nNextIndex;
}

void CPointOverlayFonts::removeUnusedFonts()
{
   std::set<int> setFontLinks;
   setFontLinks.insert(m_nOverlayFontLink);

   std::map<std::string, int>::iterator it = m_mapGroupFontLinks.begin();
   while (it != m_mapGroupFontLinks.end())
      setFontLinks.insert((it++)->second);

   std::map<FVPOINT, int>::iterator itPoint = m_mapPointFontLinks.begin();
   while (itPoint != m_mapPointFontLinks.end())
      setFontLinks.insert((itPoint++)->second);

   std::map<int, OvlFont>::iterator it2 = m_mapFontData.begin();
   while (it2 != m_mapFontData.end())
   {
      std::map<int, OvlFont>::iterator oldIter = it2;
      it2++;
      if (setFontLinks.find(oldIter->first) == setFontLinks.end())
         m_mapFontData.erase(oldIter);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFontDataSet implementation

IMPLEMENT_DYNAMIC(CFontDataSet, CDaoRecordset)

CFontDataSet::CFontDataSet(CDaoDatabase* pdb)
   : CDaoRecordset(pdb)
{
   //{{AFX_FIELD_INIT(CLocalPointSet)
   m_FontID = 0;
   m_Size = 0;
   m_BackgroundEffect = 0;
   m_Strikeout = 0;
   m_Underline = 0;
   m_Bold = 0;
   m_Italic = 0;
   m_nFields = 10;

   //}}AFX_FIELD_INIT
   m_nDefaultType = dbOpenDynaset;
}

CString CFontDataSet::GetDefaultSQL()
{
   return _T("[FontData]");
}

CString CFontDataSet::GetDefaultDBName()
{
   return m_filename;
}

void CFontDataSet::set_filename(CString filename)
{
   m_filename = filename;
}

void CFontDataSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
   //{{AFX_FIELD_MAP(CFontDataSet)
   pFX->SetFieldType(CDaoFieldExchange::outputColumn);
   DFX_Short(pFX, _T("[FontID]"), m_FontID);
   DFX_Text(pFX, _T("[Name]"), m_Name);
   DFX_Short(pFX, _T("[Size]"), m_Size);
   DFX_Text(pFX, _T("[BackgroundColor]"), m_BackgroundColor);
   DFX_Text(pFX, _T("[ForegroundColor]"), m_ForegroundColor);
   DFX_Short(pFX, _T("[BackgroundEffect]"), m_BackgroundEffect);
   DFX_Short(pFX, _T("[Strikeout]"), m_Strikeout);
   DFX_Short(pFX, _T("[Underline]"), m_Underline);
   DFX_Short(pFX, _T("[Bold]"), m_Bold);
   DFX_Short(pFX, _T("[Italic]"), m_Italic);
   //}}AFX_FIELD_MAP

   // This code was added to modify the DAO binding structure for each field that
   // is a text field so that the new callback function is called instead of the 
   // MFC function.  This fix was required to correct a character buffer overwrite 
   // error introduced by the upgrade to DAO 3.6
   if (pFX->m_nOperation == CDaoFieldExchange::BindField)
   {
      for (int i = 0; i < this->m_nFields; i++)
      {
         LPDAOCOLUMNBINDING pcb = &m_prgDaoColBindInfo[i];
         if (pcb->dwDataType == DAO_CHAR)
         {
            pcb->cbDataOffset = (DWORD)MyDaoStringAllocCallback;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFontLinksSet implementation

IMPLEMENT_DYNAMIC(CFontLinksSet, CDaoRecordset)

CFontLinksSet::CFontLinksSet(CDaoDatabase* pdb)
   : CDaoRecordset(pdb)
{
   //{{AFX_FIELD_INIT(CLocalPointSet)
   m_LinkID = 0;
   m_FontID = 0;
   m_Type = 0;
   m_nFields = 5;

   //}}AFX_FIELD_INIT
   m_nDefaultType = dbOpenDynaset;
}

CString CFontLinksSet::GetDefaultSQL()
{
   return _T("[FontLinks]");
}

CString CFontLinksSet::GetDefaultDBName()
{
   return m_filename;
}

void CFontLinksSet::set_filename(CString filename)
{
   m_filename = filename;
}

void CFontLinksSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
   //{{AFX_FIELD_MAP(CFontLinksSet)
   pFX->SetFieldType(CDaoFieldExchange::outputColumn);
   DFX_Short(pFX, _T("[LinkID]"), m_LinkID);
   DFX_Short(pFX, _T("[FontID]"), m_FontID);
   DFX_Short(pFX, _T("[Type]"), m_Type);
   DFX_Text(pFX, _T("[PointID]"), m_PointID);
   DFX_Text(pFX, _T("[Group_Name]"), m_Group_Name);
   //}}AFX_FIELD_MAP

   // This code was added to modify the DAO binding structure for each field that
   // is a text field so that the new callback function is called instead of the 
   // MFC function.  This fix was required to correct a character buffer overwrite 
   // error introduced by the upgrade to DAO 3.6
   if (pFX->m_nOperation == CDaoFieldExchange::BindField)
   {
      for (int i = 0; i < this->m_nFields; i++)
      {
         LPDAOCOLUMNBINDING pcb = &m_prgDaoColBindInfo[i];
         if (pcb->dwDataType == DAO_CHAR)
         {
            pcb->cbDataOffset = (DWORD)MyDaoStringAllocCallback;
         }
      }
   }
}