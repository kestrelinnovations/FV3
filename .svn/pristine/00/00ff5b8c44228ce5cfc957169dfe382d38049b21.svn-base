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



#include "StdAfx.h"
#include "CommandLine.h"


CFVCommandLineInfo::CFVCommandLineInfo() :
   m_run_all_tests(false)
{
	m_bRegServerOption = FALSE;
   m_bUnRegServerOption = FALSE;
   m_bRestoreOverlays = TRUE;
   m_bCleanCoverageDatabase = FALSE;

	m_next_parameter_will_be_name_of_favorite = false;
	m_next_parameter_will_be_name_of_overlay = false;
}

void CFVCommandLineInfo::ParseParam(const TCHAR* pszParam,BOOL bFlag,BOOL bLast)
{
   // ignore remaining parameters if /RUNALLTESTS was given
   if (m_run_all_tests)
      return;

	if (m_next_parameter_will_be_name_of_favorite)
	{
		if (m_favorite_name.GetLength())
			m_favorite_name += CString(pszParam);
		else
			m_favorite_name = CString(pszParam);
	}

	if (m_next_parameter_will_be_name_of_overlay)
	{
		if (m_overlay_name.GetLength())
			m_overlay_name += " " + CString(pszParam);
		else
			m_overlay_name = CString(pszParam);
	}

	/* if the parameter is REGSERVER, then handle it, otherwise,
	   let CCommandLineInfo::ParseParam take care of the parameter
	*/
	if (_strcmpi(pszParam, "REGSERVER") == 0)
	{
		m_next_parameter_will_be_name_of_favorite = false;
		m_next_parameter_will_be_name_of_overlay = false;
		m_bRegServerOption = TRUE;
	}
	else if (_strcmpi(pszParam, "UNREGSERVER") == 0)
	{
		m_next_parameter_will_be_name_of_favorite = false;
		m_next_parameter_will_be_name_of_overlay = false;
		m_bUnRegServerOption = TRUE;
	}
   else if (_strcmpi(pszParam, "CLEANCOVDB") == 0)
   {
      m_next_parameter_will_be_name_of_favorite = false;
      m_next_parameter_will_be_name_of_overlay = false;
      m_bCleanCoverageDatabase = TRUE;
   }
	else if (_strcmpi(pszParam, "F") == 0)
	{
		m_next_parameter_will_be_name_of_overlay = false;
		m_next_parameter_will_be_name_of_favorite = true;
	}
	else if (_strcmpi(pszParam, "O") == 0)
	{
		m_next_parameter_will_be_name_of_favorite = false;
		m_next_parameter_will_be_name_of_overlay = true;
	}
	else if (_strcmpi(pszParam, "NOOVERLAYS") == 0)
	{
		m_next_parameter_will_be_name_of_favorite = false;
		m_next_parameter_will_be_name_of_overlay = false;
		m_bRestoreOverlays = FALSE;
	}
   else if (_strcmpi(pszParam, "RUNALLTESTS") == 0)
   {
      m_run_all_tests = true;
   }
	else
		CCommandLineInfo::ParseParam(pszParam,bFlag,bLast);
}

//should be inline, but inline expansion is disabled by default
//in the compiler options

BOOL CFVCommandLineInfo::IsRegServer() 
{
	return m_bRegServerOption; 
}

BOOL CFVCommandLineInfo::IsUnRegServer()
{
   return m_bUnRegServerOption;
}

BOOL CFVCommandLineInfo::CleanCoverageDB()
{
   return m_bCleanCoverageDatabase;
}

BOOL CFVCommandLineInfo::IsRestoreOverlays()
{
	return m_bRestoreOverlays;
}

