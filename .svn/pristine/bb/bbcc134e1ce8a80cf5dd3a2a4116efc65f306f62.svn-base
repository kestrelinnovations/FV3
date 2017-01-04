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
/*
 *	$Header: /common/FileSpec.h 2     12/04/01 11:40p Admin $
 *
 *	$History: FileSpec.h $
 * 
 * *****************  Version 2  *****************
 * User: Admin        Date: 12/04/01   Time: 11:40p
 * Updated in $/common
 * Fixed some resource leaks.  Added code to update the server list via
 * Sir Prober.
 * 
 * *****************  Version 1  *****************
 * User: Admin        Date: 28/03/01   Time: 8:42p
 * Created in $/common
 */
class CFileSpec
{
public:
	enum FS_BUILTINS
	{
		FS_EMPTY,		//	Nothing
		FS_APP,			//  Full application path and name
		FS_APPDIR,		//	Application folder
		FS_WINDIR,		//	Windows folder
		FS_SYSDIR,		//	System folder
		FS_TMPDIR,		//	Temporary folder
		FS_DESKTOP,		//	Desktop folder
		FS_FAVOURITES,	//	Favourites folder
		FS_MEDIA,		//	Default media folder
		FS_CURRDIR,		//	Current folder
		FS_TEMPNAME		//	Create a temporary name
	};

					CFileSpec(FS_BUILTINS eSpec = FS_EMPTY);
					CFileSpec(FS_BUILTINS eSpec, LPCTSTR szFileame);
					CFileSpec(LPCTSTR szSpec, LPCTSTR szFilename);
					CFileSpec(LPCTSTR szFilename);

//	Operations
	BOOL			Exists() const;
	BOOL			IsUNCPath() const;
	BOOL			LoadArchive(CObject *pObj) const;
	BOOL			SaveArchive(CObject *pObj) const;

//	Access functions
	CString&		Drive()				{ return m_csDrive; }
	CString&		Path()				{ return m_csPath; }
	CString&		FileName()			{ return m_csFilename; }
	CString&		Extension()			{ return m_csExtension; }
	
	CString&		Extension2()		{	m_csExtension2 = m_csExtension; 
												m_csExtension2.TrimLeft("."); 
												return m_csExtension2; }
	
	const CString	FullPathNoExtension() const;
	const CString	GetFolder() const;
	const CString	GetFullSpec() const;
	const CString	GetFileName() const;
	const CString	ConvertToUNCPath() const;

	void			SetFullSpec(LPCTSTR szSpec);
	void			SetFullSpec(FS_BUILTINS eSpec = FS_EMPTY);
	void			SetFileName(LPCTSTR szSpec);

	void			Initialise(FS_BUILTINS eSpec);

private:
	BOOL			IsUNCPath(LPCTSTR szPath) const;
	void			WriteAble() const;
	void			ReadOnly() const;
	void			GetShellFolder(int iFolder);

	CString		m_csDrive,
					m_csPath,
					m_csFilename,
					m_csExtension,
					m_csExtension2;
};
