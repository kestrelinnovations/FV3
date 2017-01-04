// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// FvOverlayManagerImpl.cpp
//

#include "stdafx.h"
#include "FvTabularEditorParserImpl.h"
#include "FalconView\TabularEditor\Parser.h"

CFvTabularEditorParserImpl::CFvTabularEditorParserImpl()
   : m_parser(nullptr)
{
}

CFvTabularEditorParserImpl::~CFvTabularEditorParserImpl()
{
}

void CFvTabularEditorParserImpl::Initialize(Parser* parser)
{
   m_parser = parser;
}

STDMETHODIMP CFvTabularEditorParserImpl::raw_OpenFile(long skip_to_start_row)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_parser->open_file(skip_to_start_row);

   return S_OK;
}

STDMETHODIMP CFvTabularEditorParserImpl::raw_GetTokens(SAFEARRAY** tokens)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CList<CString, CString> token_list;
   m_parser->get_tokens(token_list);

   BstrSafeArray sa_tokens;

   POSITION token_position = token_list.GetHeadPosition();

   while (token_position != nullptr)
   {
      _bstr_t token = token_list.GetNext(token_position);

      sa_tokens.Append(token.Detach());
   }

   SafeArrayCopy(&sa_tokens, tokens);

   return S_OK;
}

STDMETHODIMP CFvTabularEditorParserImpl::raw_GetColumnHeaders(SAFEARRAY** column_headers)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CList<CString, CString>& column_header_list = m_parser->get_properties()->get_column_header_list();

   BstrSafeArray sa_column_headers;

   POSITION header_position = column_header_list.GetHeadPosition();

   while (header_position != nullptr)
   {
      _bstr_t header = column_header_list.GetNext(header_position);

      sa_column_headers.Append(header.Detach());
   }

   SafeArrayCopy(&sa_column_headers, column_headers);

   return S_OK;
}

STDMETHODIMP CFvTabularEditorParserImpl::raw_GetNumFilters(long* number_of_filters)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *number_of_filters = m_parser->get_properties()->get_num_filters();

   return S_OK;
}


STDMETHODIMP CFvTabularEditorParserImpl::raw_GetFilter(int filter_index, FalconViewTabularEditorLib::ParserFilter* parser_filter)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   filter_t filter = m_parser->get_properties()->get_filter(filter_index);

   parser_filter->color = filter.color;
   parser_filter->column_header = _bstr_t(filter.column_header);
   parser_filter->filter_string = _bstr_t(filter.filter_string);
   parser_filter->group_name = _bstr_t(filter.group_name);
   parser_filter->icon_assigned = _bstr_t(filter.icon_assigned);
   parser_filter->shape_type = _bstr_t(filter.shape_type);

   return S_OK;
}

