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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvOverlayDataObjectImpl.h
//

#pragma once

class Parser;

// Implements IFvTabularEditorParser defined in FalconViewOverlay.tlb
//
class CFvTabularEditorParserImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewTabularEditorLib::IFvTabularEditorParser, 
      &FalconViewTabularEditorLib::IID_IFvTabularEditorParser, &FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib>
{
   Parser* m_parser;

public:
   CFvTabularEditorParserImpl();
   ~CFvTabularEditorParserImpl();

   BEGIN_COM_MAP(CFvTabularEditorParserImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewTabularEditorLib::IFvTabularEditorParser)
   END_COM_MAP()

   void Initialize(Parser* parser);

   // IFvTabularEditorParser
public:

   STDMETHOD(raw_OpenFile)(long skip_to_start_row);
   STDMETHOD(raw_GetTokens)(SAFEARRAY** tokens);
   STDMETHOD(raw_GetColumnHeaders)(SAFEARRAY** column_headers);
   STDMETHOD(raw_GetNumFilters)(long* number_of_filters);
   STDMETHOD(raw_GetFilter)(int filter_index, FalconViewTabularEditorLib::ParserFilter* filter);
};
