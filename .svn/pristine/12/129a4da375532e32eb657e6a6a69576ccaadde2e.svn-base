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


//
// Header definitions required for application info modules.
//

#pragma once

enum PROJECT_TYPES
{
	PROJECT_APP_TYPE_SOFMAP = 0,
	PROJECT_APP_TYPE_FALCONVIEW
};

// maximum size for the version string returned by project_version_str
const UINT MAX_VERSION_SIZE = 16;

//
// Public function prototypes for returning build information about
// the current running application.
//

// This function returns the version string ProductName defined in the version
// resource file.for this project.
char *project_product_name(char *, size_t size);

// returns the common name (e.g., "FalconView")
CString appShortName();

// returns the application error file
CString appErrorFile();

// returns the build version numbers
char* project_version_str(char*, size_t size);
char* project_short_version_str(char*, size_t size);
char* project_build_type();
BOOL need_to_display_non_release_warning_dialog();

// returns the copyright string
char* project_copyright(char* target_str, size_t size);

//returns the build date of the exe
COleDateTime get_app_build_date();

// returns formatted build version number
