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

// ReaderWriterGdiplus.h

#ifndef FALCONVIEW_READERWRITERGDIPLUS_H_
#define FALCONVIEW_READERWRITERGDIPLUS_H_

#include "osgDB/ReaderWriter"

class ReaderWriterGdiplus : public osgDB::ReaderWriter
{
public:

   ReaderWriterGdiplus();

   virtual const char* className() const { return "Gdiplus Image Reader"; }

   virtual ReadResult readObject(const std::string& file,
      const osgDB::ReaderWriter::Options* options = NULL) const
   {
      return readImage(file, options);
   }

   bool acceptsExtension(const std::string& extension) const;

   virtual ReadResult readImage(const std::string& file,
      const osgDB::ReaderWriter::Options* options) const;
};

#endif  // FALCONVIEW_READERWRITERGDIPLUS_H_