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



#pragma once

class CFVCommandLineInfo : public CCommandLineInfo
{
	BOOL m_bRegServerOption; //was the /REGSERVER option chosen
	BOOL m_bUnRegServerOption; // was the /UNREGSERVER option chosen
	BOOL m_bRestoreOverlays; // was NORESTORE option chosen
   BOOL m_bCleanCoverageDatabase; // was the /CLEANCOVDB option choosen
   BOOL m_run_all_tests;            // set if /RUNALLTESTS command-line option was given

	// a favorite (map center, projection, overlays, etc...) can be
	// automatically launched at startup using the /F <favorite name>
	// option
	bool m_next_parameter_will_be_name_of_favorite;
	bool m_next_parameter_will_be_name_of_overlay;
	CString m_favorite_name;
	CString m_overlay_name;
	
public:
   CFVCommandLineInfo();
	void ParseParam(const TCHAR* pszParam,BOOL bFlag,BOOL bLast);
	BOOL IsRegServer();
   BOOL IsUnRegServer();
   BOOL CleanCoverageDB();
	CString get_favorite_name() { return m_favorite_name; }
	CString get_overlay_name() { return m_overlay_name; }
   BOOL IsRestoreOverlays();
   BOOL get_run_all_tests() { return m_run_all_tests; }
};
