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

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// ReaderWriterGdiplus.cpp

#include "stdafx.h"
#include "ReaderWriterGdiplus.h"

#include "osgDB/FileNameUtils"

#include "FalconView/include/OverlayElements.h"

ReaderWriterGdiplus::ReaderWriterGdiplus()
{
   supportsExtension("bmp", "BMP image format");
   supportsExtension("gif", "GIF image format");
   supportsExtension("jpeg", "JPEG image format");
   supportsExtension("jpg", "JPG image format");
   supportsExtension("png", "PNG image format");
   supportsExtension("tga", "TGA image format");
   supportsExtension("ico", "ICO image format");
}

bool ReaderWriterGdiplus::acceptsExtension(const std::string& extension) const
{
   return
      osgDB::equalCaseInsensitive(extension, "bmp") ||
      osgDB::equalCaseInsensitive(extension, "gif") ||
      osgDB::equalCaseInsensitive(extension, "jpeg") ||
      osgDB::equalCaseInsensitive(extension, "jpg") ||
      osgDB::equalCaseInsensitive(extension, "png") ||
      osgDB::equalCaseInsensitive(extension, "tga") ||
      osgDB::equalCaseInsensitive(extension, "ico");
}

osgDB::ReaderWriter::ReadResult ReaderWriterGdiplus::readImage(
   const std::string& file,
   const osgDB::ReaderWriter::Options* options) const
{
   std::string ext = osgDB::getLowerCaseFileExtension(file);
   if (!acceptsExtension(ext))
      return ReadResult::FILE_NOT_HANDLED;

   return osg_image_utils::ImageFromFilename(_bstr_t(file.c_str()));
}