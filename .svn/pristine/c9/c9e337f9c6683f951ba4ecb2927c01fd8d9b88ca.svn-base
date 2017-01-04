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
#include "CugCellImpl.h"
#include "TabularEditorDlg.h"


CCugCellImpl::CCugCellImpl()
   : m_cell(nullptr)
{
}

CCugCellImpl::~CCugCellImpl()
{
}

void CCugCellImpl::Initialize(CUGCell* cell)
{
   m_cell = cell;
}

STDMETHODIMP CCugCellImpl::raw_SetCellType(long cell_type)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetCellType(cell_type);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetCellTypeEx(long cell_type)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetCellTypeEx(cell_type);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetDataType(short data_type)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetDataType(data_type);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetLabelText(BSTR label_text)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetLabelText((char*)_bstr_t(label_text));

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetText(BSTR text)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetText((char*)_bstr_t(text));

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetTextColor(COLORREF color)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetTextColor(color);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetBackColor(COLORREF color)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetBackColor(color);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetReadOnly(long read_only)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   
   m_cell->SetReadOnly(read_only);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetAlignment(short alignment)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetAlignment(alignment);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetNumberDecimals(long number_of_decimals)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetNumberDecimals(number_of_decimals);

   return S_OK;
}

STDMETHODIMP CCugCellImpl::raw_SetNumber(double number)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_cell->SetNumber(number);

   return S_OK;
}
