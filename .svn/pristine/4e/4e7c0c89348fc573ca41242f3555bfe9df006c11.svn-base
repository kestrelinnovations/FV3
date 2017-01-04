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

// PointOverlayFonts.h
//
#pragma once

#include "FalconView/include/ovlelem.h"             // for OvlFont

class CFontDataSet;
class CFontLinksSet;
class PointsSpatialiteDriver;

struct FVPOINT
{
   FVPOINT(std::string id, std::string group)
   { 
      strPointId = id; 
      strGroupName = group; 
   }   
   std::string strPointId;
   std::string strGroupName;
   bool operator<(const FVPOINT p) const
   {
      std::string s1 = strPointId + strGroupName;
      std::string s2 = p.strPointId + p.strGroupName;
      return s1 < s2;
   }
};

class CPointOverlayFonts
{
   std::map<int, OvlFont> m_mapFontData;

   int m_nOverlayFontLink;
   std::map<std::string, int> m_mapGroupFontLinks;
   std::map<FVPOINT, int> m_mapPointFontLinks;

public:
   CPointOverlayFonts(); 

   void SetOverlayFont(OvlFont& font, bool bClearGroupAndPointFonts = false);
   void SetGroupFont(OvlFont& font, std::string strGroupName);
   void SetPointFont(OvlFont& font, FVPOINT p);

   void RemovePointFont(FVPOINT p);

   bool DoesPointFontExist(std::string strPointId, std::string strGroupName);
   bool DoesGroupFontExist(std::string strGroupName);
   OvlFont& GetFont(std::string strPointId, std::string strGroupName);

   void ReadFontsFromFile(CFontDataSet *pFontData, CFontLinksSet *pFontLinks);
   void SaveFontsToFile(CFontDataSet *pFontData, CFontLinksSet *pFontLinks);

   void SetFont(int fontId, CString name, int size, CString bgColor,
      CString fgColor, short bgEffect, bool strikeout, bool underline, bool bold, bool italic);

   void SetOverlayFontId(int fontId);
   void SetGroupFontId(std::string strGroupName, int fontId);
   void SetPointFontId(std::string strPointId, std::string strGroupName, int fontId);

protected:
   int getFontIndex(OvlFont& font);
   int getGetNextIndex();
   void removeUnusedFonts();
};

class CFontDataSet : public CDaoRecordset
{
public:
   CFontDataSet(CDaoDatabase* pDatabase = NULL);
   DECLARE_DYNAMIC(CFontDataSet)

// Field/Param Data
   //{{AFX_FIELD(CFontDataSet, CDaoRecordset)
   short m_FontID;
   CString m_Name;
   short m_Size;
   CString m_BackgroundColor;
   CString m_ForegroundColor;
   short m_BackgroundEffect;
   short m_Strikeout;
   short m_Underline;
   short m_Bold;
   short m_Italic;
   //}}AFX_FIELD

   CString m_filename;
   void set_filename(CString filename);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFontDataSet)
   public:
   virtual CString GetDefaultDBName();
   virtual CString GetDefaultSQL();
   virtual void DoFieldExchange(CDaoFieldExchange* pFX);
   //}}AFX_VIRTUAL
};

class CFontLinksSet : public CDaoRecordset
{
public:
   CFontLinksSet(CDaoDatabase* pDatabase = NULL);
   DECLARE_DYNAMIC(CFontLinksSet)

// Field/Param Data
   //{{AFX_FIELD(CFontLinksSet, CDaoRecordset)
   short m_LinkID;
   short m_FontID;
   short m_Type;
   CString m_PointID;
   CString m_Group_Name;
   //}}AFX_FIELD

   CString m_filename;
   void set_filename(CString filename);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFontLinksSet)
   public:
   virtual CString GetDefaultDBName();
   virtual CString GetDefaultSQL();
   virtual void DoFieldExchange(CDaoFieldExchange* pFX);
   //}}AFX_VIRTUAL
};