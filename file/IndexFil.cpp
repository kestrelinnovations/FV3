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

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.



//
// IndexFil.cpp
//

// Implementation file for the CIndexFile class.

/*
-
- IMPORTANT NOTE:
-     All calls to MEM_malloc(), MEM_realloc(), and MEM_free() have been
-     commented out and the replaced with calls to malloc(), realloc() and
-     free().  This is due to the VERY large number of items located in
-     files created by the ECHUM importer.  Apparently, due to the fact
-     that the MEM library functions maintain a list of all allocated memory
-     and do error checking on allocations and deallocations, when a large
-     number of allocations/deallocations are made, performance degrades to
-     the point that calling end_create() on the JOG.ech file takes 25 minutes
-     to run.  Without the MEM library calls, the same call to end_create()
-     takes approximately 30 seconds.
-
-     Should one wish to run the CIndexFile class with the MEM library calls,
-     all places where calls to the MEM library occurred still have the
-     original line commented out, located directly below the new line.
-
- DATE: 
-     August 6th, 1998
-
- PROGRAMMER:
-     Edward C. Robertson
-
*/

#include "stdafx.h"
//#include "common.h"

#include "IndexFil.h"
#include "ComErrorObject.h"
//#include "err.h"



// --------------------------------------------------------------------------
// Standard constructor and destructor --------------------------------------
// --------------------------------------------------------------------------
CIndexFile::CIndexFile() : m_lonLatChain(NULL), m_ndx_file(NULL)
{
	m_opened = FALSE;
	m_buffer = NULL;
}

CIndexFile::~CIndexFile()
{
   // Just in case it was not freed , free up our allocated memory
	if (m_lonLatChain)
   {
      // This can happen if the import was aborted and end_create was never called
      
	   struct filePosChain*			current = NULL;
	   struct filePosChain*			count_ptr = NULL;
	   struct filePosChain*			free_block = NULL;

      // loop through all buckets and delete PIB entry and free its memory chain.
		for (int lat = 0; lat < (180 / m_bucket_size); lat++)
		{
			for (int lon = 0; lon < (360 / m_bucket_size); lon++)
			{
				int offset = (lat * m_lon_buckets) + lon;
				
				// test our boundaries
				ASSERT(offset >= 0  &&  offset <= m_total_buckets);
				
				current = count_ptr = *(m_lonLatChain + offset);
				
				if (current)
				{
					// loop through all of the memory blocks and free them
					while (current)
					{
						free_block = current;
						current = current->nextPos;
						free(free_block);
               }

               *(m_lonLatChain + offset) = NULL;
				}
			}
      }

		free(m_lonLatChain);
      m_lonLatChain = NULL;
   }


	if (m_opened)
		Close();

	if (m_buffer)
		free(m_buffer);

	if (m_ndx_file)
      delete m_ndx_file;
}



// --------------------------------------------------------------------------
// Standard open and close CFile overrides ----------------------------------
// ---------------------------------------------------ww-----------------------
BOOL CIndexFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, 
			 CFileException* pError)
{
   ASSERT(strlen(lpszFileName) > 3);

   // if the file is already opened, the close it first
   if (m_opened)
      Close();

	m_opened = CFile::Open(lpszFileName, nOpenFlags, pError);

	// check return code
	if (m_opened == 0)
	{
/*
		CString	err_msg;
		CString	file_name = lpszFileName;

		err_msg = "Unable to open file ("+file_name+"):: ";

		switch (pError->m_cause)
		{
		case CFileException::none: 
			err_msg += "No error occurred";
			break;
		case CFileException::fileNotFound: 
			err_msg += "The file could not be located.";
			break;
		case CFileException::badPath: 
			err_msg += "All or part of the path is invalid.";
			break;
		case CFileException::tooManyOpenFiles: 
			err_msg += "The permitted number of open files was exceeded.";
			break;
		case CFileException::accessDenied: 
			err_msg += "The file could not be accessed.";
			break;
		case CFileException::invalidFile: 
			err_msg += "There was an attempt to use an invalid file handle.";
			break;
		case CFileException::removeCurrentDir: 
			err_msg += "The current working directory cannot be removed.";
			break;
		case CFileException::directoryFull: 
			err_msg += "There are no more directory entries.";
			break;
		case CFileException::badSeek: 
			err_msg += "There was an error trying to set the file pointer.";
			break;
		case CFileException::hardIO: 
			err_msg += "There was a hardware error.";
			break;
		case CFileException::sharingViolation: 
			err_msg += "SHARE.EXE was not loaded, or a shared region was locked.";
			break;
		case CFileException::lockViolation: 
			err_msg += "There was an attempt to lock a region that was already locked.";
			break;
		case CFileException::diskFull: 
			err_msg += "The disk is full.";
			break;
		case CFileException::endOfFile: 
			err_msg += "The end of file";
			break;
      default:
			err_msg += "An unspecified error occurred.";
			break;
		}
		
		//ERR_report(err_msg);
		WriteToLogFile(err_msg);
*/
		std::wstringstream	err_msg_stream;

		err_msg_stream << "Unable to open file (" << lpszFileName << "):: ";

		switch (pError->m_cause)
		{
		case CFileException::none: 
			err_msg_stream << "No error occurred";
			break;
		case CFileException::fileNotFound: 
			err_msg_stream << "The file could not be located.";
			break;
		case CFileException::badPath: 
			err_msg_stream << "All or part of the path is invalid.";
			break;
		case CFileException::tooManyOpenFiles: 
			err_msg_stream << "The permitted number of open files was exceeded.";
			break;
		case CFileException::accessDenied: 
			err_msg_stream << "The file could not be accessed.";
			break;
		case CFileException::invalidFile: 
			err_msg_stream << "There was an attempt to use an invalid file handle.";
			break;
		case CFileException::removeCurrentDir: 
			err_msg_stream << "The current working directory cannot be removed.";
			break;
		case CFileException::directoryFull: 
			err_msg_stream << "There are no more directory entries.";
			break;
		case CFileException::badSeek: 
			err_msg_stream << "There was an error trying to set the file pointer.";
			break;
		case CFileException::hardIO: 
			err_msg_stream << "There was a hardware error.";
			break;
		case CFileException::sharingViolation: 
			err_msg_stream << "SHARE.EXE was not loaded, or a shared region was locked.";
			break;
		case CFileException::lockViolation: 
			err_msg_stream << "There was an attempt to lock a region that was already locked.";
			break;
		case CFileException::diskFull: 
			err_msg_stream << "The disk is full.";
			break;
		case CFileException::endOfFile: 
			err_msg_stream << "The end of file";
			break;
		default:
			err_msg_stream << "An unspecified error occurred.";
			break;
		}
		
		WriteToLogFile(err_msg_stream.str().c_str());
	}


	return m_opened;
}

void CIndexFile::Close()
{
	CFile::Close();
	m_opened = FALSE;
}



//
// -----------------------------------------------------------------------------
//
// FILE CREATION ROUTINES
//
// -----------------------------------------------------------------------------
//

// Create a new file with the given file specification. The size of the
// alternate index must be specified as well as the size of the lat/lon
// bucket (in degrees). Returns SUCCESS or FAILURE.
int CIndexFile::create(CString file_specification, int bucket_size, 
							  int alt_index_size, int version)
{
	CFileException	open_exception;
	UINT				mode = modeCreate | modeReadWrite | shareExclusive 
									| typeBinary;

	// initialize the record counters
	m_adb_count = 0;
	m_ndx_count = 0;
	m_used_buckets = 0;
	m_smallest_bucket = INT_MAX;
	m_largest_bucket = INT_MIN;
	m_average_bucket = 0;
	m_max_pdb_rec_length = INT_MIN;
	m_max_adb_rec_length = INT_MIN;

	// create the requested file now, so that we know that we can do it
	// (access privileges are ok, path is ok, etc.)
	if (Open(file_specification, mode, &open_exception) == 0)
	{
		std::wstringstream err_msg_stream;
		err_msg_stream << "Error in file create: " << file_specification << ", code " << open_exception.m_cause;
		WriteToLogFile(err_msg_stream.str().c_str());
/*
		CString	err_msg;
		err_msg.Format("Error in file create: %s, code %d", file_specification,
			open_exception.m_cause);
		ERR_report(err_msg);
*/
		return FAILURE;
	}

	// set the length to already contain the header area so that we can
	// write the ADB (alternate data block) on the fly to the actual location
	// the records will occupy in the final data file.
	SetLength(1024);

	// place the file pointer at the end of the file so that ADB record
	// writes will occur correctly (appending to the file during file
	// build).
	SeekToEnd();
	m_adb_location = static_cast<long>(GetPosition());
	
	// save our class variables
	m_bucket_size = bucket_size;
	m_alternate_index_size = alt_index_size;
	m_version = version; 

	// Make sure that the alt_index_size is not greater than 20 bytes.
	// For the current implementation, this is the max value.
	ASSERT(alt_index_size <= 20  &&  alt_index_size > 0);

	// We now know everything to set up the indexing for this file.
	// We should create buckets for each index chain. The bucket size must be
	// evenly divisible into 180.
	ASSERT((180 % bucket_size) == 0);

	// now compute the number of buckets
	m_lat_buckets = 180 / bucket_size;
	m_lon_buckets = 360 / bucket_size;

	// create the memory area for our bucket chain heads.
	m_total_buckets = m_lat_buckets * m_lon_buckets * sizeof(struct filePosChain*);
   if (m_lonLatChain != NULL)
      free (m_lonLatChain);
	m_lonLatChain = (struct filePosChain**)malloc(m_total_buckets);

	if (m_lonLatChain == NULL)
	{
		//ERR_report("Unable to allocate bucket memory.");
		WriteToLogFile(L"Unable to allocate bucket memory.");
		return FAILURE;
	}

	// initialize the buckets to NULL so there will be no initial chains
	memset(m_lonLatChain, 0, 
		m_lat_buckets * m_lon_buckets * sizeof(struct filePosChain*));


	// now, create the temporary file that will be used to create
	// the initial write records as we build our lat/lon indices
	m_ndx_file = new CFile();
	
	// get the system's temporary directory
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
	char* temp_dir;
   size_t len;
   errno_t err = _dupenv_s(&temp_dir, &len, "TEMP");
	if (temp_dir == NULL || err)
	{
		err = _dupenv_s(&temp_dir, &len, "TMP");
		if (temp_dir == NULL || err)
		{
			//ERR_report("Unable to read location of system TEMP directory.");
			WriteToLogFile(L"Unable to read location of system TEMP directory.");
         free(temp_dir); // OK to free NULL
			return FAILURE;
		}
	}

	// create the filename and open the file
   GetTempFileName(temp_dir,                             // directory for temp. files 
                   "NDX",                                // temp filename prefix 
                   0,                                    // create unique name 
                   m_tmp_filename.GetBuffer(_MAX_PATH)); // buffer for name

   free(temp_dir); // OK to free NULL
   m_tmp_filename.ReleaseBuffer();

	if (!m_ndx_file->Open(m_tmp_filename, mode, &open_exception))
	{
		std::wstringstream err_msg_stream;
		err_msg_stream << "Error in file create: " << m_tmp_filename << ", code " << open_exception.m_cause;
		WriteToLogFile(err_msg_stream.str().c_str());

/*
		CString	err_msg;
		err_msg.Format("Error in file create: %s, code %d", m_tmp_filename,
			open_exception.m_cause);
		ERR_report(err_msg);
*/
		return FAILURE;
	}

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

//	Writes a record to the temporary build area, builds the index entries
//	and calls the virtual function write_record (see below). 
//	Returns SUCCESS or FAILURE.
int CIndexFile::write_record(void* record, int record_size, 
									  double lat, double lon, 
									  char* alternate_index)
{
	int	lat_degrees = int(lat);
	int	lon_degrees = int(lon);

	// make sure we've got a reasonable record size
	ASSERT(record_size > 0);

	// make sure that the lat/lon coordinates are reasonable
	if (lat < -90  ||  lat > 90  ||  lon < -180  ||  lon > 180)
	{
		//ERR_report("Invalid latitude or longitude specified.");
		WriteToLogFile(L"Invalid latitude or longitude specified.");
		return FAILURE;
	}

	// make sure the alternate_index is not NULL
	if (alternate_index == NULL  ||  strlen(alternate_index) == 0)
	{
		//ERR_report("Invalid alternate indiex specified.");
		WriteToLogFile(L"Invalid alternate indiex specified.");
		return FAILURE;
	}

	// keep the max record length field up to date
	m_max_pdb_rec_length = max(m_max_pdb_rec_length, record_size);

	// note the file position where we'll write this record into the tmp file
	DWORD dwPosition = static_cast<DWORD>(m_ndx_file->GetPosition());

	// write out the number of bytes that we're writing (every record
	// will begin with the record length)
	m_ndx_file->Write(&record_size, sizeof(int));

	// write out the alternate index string... we'll collect these again during
	// end_create() function.
	m_ndx_file->Write(alternate_index, m_alternate_index_size);

	// write out the actual record
	m_ndx_file->Write(record, record_size);

	// add this record to our lat/lon index chain
	if (add_to_chain(lat_degrees, lon_degrees, dwPosition) != SUCCESS)
		return FAILURE;

	// increment the count of indexed records
	m_ndx_count++;

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

// Writes a record to the Alternate Data Block area of the file -- not
// indexed in any way. The actual file consumption will be record_size +
// sizeof(int) because we always write the record size before the record.
//
// NOTE: We write this out at the current file position -- because 
// CIndexFile makes sure that during file creation, the file does not
// get written to until a CLOSE occurs, EXCEPT through this routine.
//
// Returns SUCCESS or FAILURE.
int CIndexFile::write_adb_record(void* record, int record_size)
{
	// make sure we've got a reasonable record size
	ASSERT(record_size > 0);

	// Write out the number of bytes that we're writing (every record
	// will begin with the record length)
	Write(&record_size, sizeof(int));

	// write out the actual record
	Write(record, record_size);

	// update our statistics
	m_max_adb_rec_length = max(m_max_adb_rec_length, record_size);

	// increment the count of adb records
	m_adb_count++;

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

// Add a record position (temporary file) to our lat/lon index chain.
// Returns SUCCESS or FAILURE.
int CIndexFile::add_to_chain(int lat_degrees, int lon_degrees, DWORD dwPosition)
{
	int							offset;
	struct filePosChain*		new_entry;
	int							lat_bucket = (lat_degrees + 90)  / m_bucket_size;
	int							lon_bucket = (lon_degrees + 180) / m_bucket_size;

	// generate the offset into the lat/lon memory block that we'll be adding to
	offset = (lat_bucket * m_lon_buckets) + lon_bucket;

	// test our boundaries
	ASSERT(offset >= 0  &&  offset <= m_total_buckets);
	
	// the method will be to always add the new entry to the head of the chain
	// and assign the old value to the next position pointer
	new_entry = (struct filePosChain*)malloc(sizeof(struct filePosChain));
	//new_entry = (struct filePosChain*)MEM_malloc(sizeof(struct filePosChain));
	if (new_entry == NULL)
	{
		//ERR_report("Unable to allocate memory for chain entry.");
		WriteToLogFile(L"Unable to allocate memory for chain entry.");
		return FAILURE;
	}

	// save the file pointer
	new_entry->filePos = dwPosition;

	// our new entry next pos should contain the previous bucket head
	new_entry->nextPos = *(m_lonLatChain + offset);

	// make our new entry the head of the bucket chain
	*(m_lonLatChain + offset) = new_entry;

	return SUCCESS;
}
  
//
// -----------------------------------------------------------------------------
//

int index_compare(const void *arg1, const void *arg2)
{
   /* Compare all of both strings: */
   return _stricmp( ( char* ) arg1, ( char* ) arg2 );
}

//
// -----------------------------------------------------------------------------
//

//	Function for writing the 512-byte application header block. 
// Returns SUCCESS or FAILURE.
int CIndexFile::write_application_header(void* pAppHeaderBlock)
{
	DWORD		cur_pos = static_cast<DWORD>(GetPosition());

	Seek(512, begin);
	Write(pAppHeaderBlock, 512);
	Seek(cur_pos, begin);

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

// Signals the end of all file creation activity. This function will 
// (1) write the PDB (primary data block) in bucket sequence with each bucket
// preceded by the count of records in the bucket, (2) write the PIB (primary
// index block) for all indices that have content available, (3) write the
// AIB (alternate index block), and (4) write the header slots with the
// built information.
// Returns SUCCESS or FAILURE.
int CIndexFile::end_create()
{
	int								offset;
	struct filePosChain*			current = NULL;
	struct filePosChain*			count_ptr = NULL;
	struct filePosChain*			free_block = NULL;
	DWORD								filePos;
	DWORD								bucketPos;
	int								counter;
	int								record_size;
	char								alt_ndx_field[20];
	BYTE*								record = (BYTE*)malloc(8192);
	struct altIndexEntry*		alt_ndx_array = NULL;
	struct altIndexEntry*		current_ndx_entry = NULL;
	int								lat, lon;
	struct primaryIndexEntry	pib_entry;
	int								slot_entries[8];
	int								i;
	//int								bucket_sizes[5000];


	// zero our statistics buckets
	//memset(bucket_sizes, 0, 5000 * sizeof(int));

	// grab the file location so that we can fill in the header slot for
	// the start of the pbd
	m_pdb_location = static_cast<long>(GetPosition());

	// if we have no indexed points, then we need to skip this and set
	// reasonable 0'd defaults
	if (m_ndx_count > 0)
	{
		// allocate our block of memory for building the array of alternate
		// indices.
		int	ndx_array_size = m_ndx_count * sizeof(struct altIndexEntry);
		alt_ndx_array = (struct altIndexEntry*)malloc(ndx_array_size);
		//alt_ndx_array = (struct altIndexEntry*)MEM_malloc(ndx_array_size);
		memset(alt_ndx_array, 0, ndx_array_size);
		
		current_ndx_entry = alt_ndx_array;
		
		// loop through all of the buckets and move all of the records from the 
		// temporary collection file and into the final file.
		for (lat = 0; lat < (180 / m_bucket_size); lat++)
		{
			for (lon = 0; lon < (360 / m_bucket_size); lon++)
			{
				offset = (lat * m_lon_buckets) + lon;
				
				// test our boundaries
				ASSERT(offset >= 0  &&  offset <= m_total_buckets);
				
				current = count_ptr = *(m_lonLatChain + offset);
				counter = 0;
				
				// save the file position for this bucket position in the PIB
				bucketPos = static_cast<DWORD>(GetPosition());
				
				// loop through just to determine how many there are for this bucket
				while (count_ptr)
				{
					counter++;
					count_ptr = count_ptr->nextPos;
				}
				
				// save it for writting out the starting file position
				count_ptr = current;
				
				// accumulate our statistics
				//bucket_sizes[counter]++;
				
				if (counter > 0)
				{
					// accumulate our statistics on mins and maxs
					m_smallest_bucket = min(m_smallest_bucket, counter);
					m_largest_bucket = max(m_largest_bucket, counter);
					
					if (m_largest_bucket == counter)
					{
						m_largest_bucket_lat = lat * m_bucket_size;
						m_largest_bucket_lon = lon * m_bucket_size;
					}
					
					// write out the record count at the biginning of this bucket
					Write(&counter, sizeof(int));
					
					// if there is a chain at the current lat/lon bucket location,
					// then loop through all ofthe records, reading them and 
					// writing them to the final file location, and also building
					// the AIB entries as you go.
					while (current)
					{
						// save the current file position for the AIB entry.
						filePos = static_cast<DWORD>(GetPosition());
						
						// seek to the beginning of the temporary file record for 
						// this bucket entry.
						m_ndx_file->Seek(current->filePos, begin);
						
						// read the record length field fromthe temporary file for this 
						// record.
						m_ndx_file->Read(&record_size, sizeof(int));
						
						// read the alternate index entry for this record
						m_ndx_file->Read(alt_ndx_field, m_alternate_index_size);
						
						// read in the record
						m_ndx_file->Read(record, record_size);
						
						// write out the record size field
						Write(&record_size, sizeof(int));
						
						// write out the record
						Write(record, record_size);
						
						// create our AIB entry for this record
						strncpy_s(current_ndx_entry->index_value, 20, alt_ndx_field, m_alternate_index_size);
						current_ndx_entry->filePos = filePos;
						
						// set to write to the next entry
						current_ndx_entry++;
						
						// loop to next record
						current = current->nextPos;
					}
					
					// write out the starting position for this bucket
					count_ptr->filePos = bucketPos;
					
					// keep count of the used buckets
					m_used_buckets++;		
				}
			}
		}
		
		// finalize our statistics
		m_average_bucket = m_ndx_count / m_used_buckets;
		
		// now, save the current file position for the start of the PIB
		m_pib_location = static_cast<long>(GetPosition());
		
		// loop again through all buckets and write is PIB entry and free its memory
		// chain.
		for (lat = 0; lat < (180 / m_bucket_size); lat++)
		{
			for (lon = 0; lon < (360 / m_bucket_size); lon++)
			{
				offset = (lat * m_lon_buckets) + lon;
				
				// test our boundaries
				ASSERT(offset >= 0  &&  offset <= m_total_buckets);
				
				current = count_ptr = *(m_lonLatChain + offset);
				
				if (current)
				{
					// write out the lat/lon bucket address and the file position for 
					// beginning area of that bucket's record collection.
					pib_entry.lat     = lat;
					pib_entry.lon     = lon;
					pib_entry.filePos = current->filePos;
					Write(&pib_entry, sizeof(pib_entry));
					
					// loop through all of the memory blocks and free them
					while (current)
					{
						free_block = current;
						current = current->nextPos;
						free(free_block);
               }

               *(m_lonLatChain + offset) = NULL;
				}
			}
		}
		
		// By this point, the entire array of alternate index entries is built
		// and ready for sorting and writing.
		qsort(alt_ndx_array, m_ndx_count, sizeof(struct altIndexEntry), 
			index_compare);
	}

	// save the current file position as the start of the AIB
	m_aib_location = static_cast<long>(GetPosition());

	// last file creation act: loop through all AIB entries and write them to the
	// AIB of the primary file.
	current_ndx_entry = alt_ndx_array;
	for (i = 0; i < m_ndx_count; i++)
	{
		// Write out the index value
		Write(current_ndx_entry->index_value, m_alternate_index_size);

		// Write out the file position for that record
		filePos = current_ndx_entry->filePos;
		Write(&filePos, sizeof(long));

		// Skip to the next entry
		current_ndx_entry++;
	}

	// finally, free up our allocated memory
	if (m_lonLatChain)
   {
		free(m_lonLatChain);
      m_lonLatChain = NULL;
   }

   if (alt_ndx_array)
		free(alt_ndx_array);

   if (record)
		free(record);

	delete m_ndx_file;
   m_ndx_file = NULL;

	CFile::Remove(m_tmp_filename);


	// write out the header slots
	//		slot 0	-	file information (date, version, build_type, build_no)
	CTime now = CTime::GetCurrentTime();
	memset(slot_entries, 0, sizeof(slot_entries));
	slot_entries[0] = now.GetYear();			// first comes creation date
	slot_entries[1] = now.GetMonth();
	slot_entries[2] = now.GetDay();
	slot_entries[3] = now.GetYear();			// then last save date
	slot_entries[4] = now.GetMonth();
	slot_entries[5] = now.GetDay();
	slot_entries[6] = m_version;				// data format version
	slot_entries[7] = CINDEXFILE_VERSION;	// structural version
	if (write_header_slot(0, slot_entries) != SUCCESS)
	{
		//ERR_report("Unable to write slot 0 in header block.");
		WriteToLogFile(L"Unable to write slot 0 in header block.");
		return FAILURE;
	}

	//		slot 1	-	file position of Primary Data Block (PDB, indexed)
	memset(slot_entries, 0, sizeof(slot_entries));
	slot_entries[0] = m_pdb_location;
	slot_entries[1] = m_ndx_count;
	slot_entries[2] = m_max_pdb_rec_length;
	if (write_header_slot(1, slot_entries) != SUCCESS)
	{
		//ERR_report("Unable to write slot 1 in header block.");
		WriteToLogFile(L"Unable to write slot 1 in header block.");
		return FAILURE;
	}

	//		slot 2	-	file position of Alternate Data Block (ADB, non-indexed)
	memset(slot_entries, 0, sizeof(slot_entries));
	slot_entries[0] = m_adb_location;
	slot_entries[1] = m_adb_count;
	slot_entries[2] = m_max_adb_rec_length;
	if (write_header_slot(2, slot_entries) != SUCCESS)
	{
		//ERR_report("Unable to write slot 2 in header block.");
		WriteToLogFile(L"Unable to write slot 2 in header block.");
		return FAILURE;
	}

	//		slot 3	-	Index Information Block (IIB) of primary index
	//							(includes size of index value (in bytes) and file position
	//							of the Alternate Index Block)
	memset(slot_entries, 0, sizeof(slot_entries));
	slot_entries[0] = m_pib_location;
	slot_entries[1] = m_bucket_size;
	slot_entries[2] = m_used_buckets;
	if (write_header_slot(3, slot_entries) != SUCCESS)
	{
		//ERR_report("Unable to write slot 3 in header block.");
		WriteToLogFile(L"Unable to write slot 3 in header block.");
		return FAILURE;
	}

	//		slot 4	-	Index Information Block (IIB) of alternate index
	//							(includes size of index value (in bytes) and file position
	//							of the Alternate Index Block)
	memset(slot_entries, 0, sizeof(slot_entries));
	slot_entries[0] = m_aib_location;
	slot_entries[1] = m_alternate_index_size;
	if (write_header_slot(4, slot_entries) != SUCCESS)
	{
		//ERR_report("Unable to write slot 4 in header block.");
		WriteToLogFile(L"Unable to write slot 4 in header block.");
		return FAILURE;
	}

	// flush any remaining buffered file data
	Flush();

	return SUCCESS;
}
  
//
// -----------------------------------------------------------------------------
//

// Writes a 32-byte slot record for the CIndexFile header. Returns SUCCESS
// or FAILURE.
int CIndexFile::write_header_slot(int slot, void* slot_value)
{
	DWORD		orig_loc = static_cast<DWORD>(GetPosition());
	DWORD		byte_loc = slot * 32;

	Seek(byte_loc, begin);
	Write(slot_value, 32);
	Seek(orig_loc, begin);

	return SUCCESS;
}

// --------------------------------------------------------------------------
// Buffered i/o routines ----------------------------------------------------
// --------------------------------------------------------------------------

void CIndexFile::write_buffered_start()
{
	// allocate the memory if it hasn't been already
	if (!m_buffer)
		m_buffer = (BYTE*)malloc(BUFFER_LENGTH);
		//m_buffer = (BYTE*)MEM_malloc(BUFFER_LENGTH);

	// rewind our buffer position pointer
	m_buffer_pos = m_buffer;

	// assign the pointer to the current alternate database records area
	// in case this 
	m_current_adb_location = static_cast<long>(GetPosition());
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_byte(BYTE byte_val)
{
	ASSERT((m_buffer_pos - m_buffer + 1) <= BUFFER_LENGTH);
	*m_buffer_pos++ = byte_val;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_int(int int_val)
{
	ASSERT((m_buffer_pos - m_buffer + 4) <= BUFFER_LENGTH);

	memcpy(m_buffer_pos, &int_val, 4);
	m_buffer_pos += 4;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_short(short short_val)
{
	ASSERT((m_buffer_pos - m_buffer + 2) <= BUFFER_LENGTH);

	memcpy(m_buffer_pos, &short_val, 2);
	m_buffer_pos += 2;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_double(double double_val)
{
	ASSERT((m_buffer_pos - m_buffer + 8) <= BUFFER_LENGTH);

	memcpy(m_buffer_pos, &double_val, 8);
	m_buffer_pos += 8;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_void(void* void_val, int void_len)
{
	ASSERT((m_buffer_pos - m_buffer + void_len) <= BUFFER_LENGTH);

	write_buffered_int(void_len);
	memcpy(m_buffer_pos, void_val, void_len);
	m_buffer_pos += void_len;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_bool(boolean_t bool_val)
{
	ASSERT((m_buffer_pos - m_buffer + 1) <= BUFFER_LENGTH);

	*m_buffer_pos++ = bool_val;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_ctime(CTime time_val)
{
	ASSERT((m_buffer_pos - m_buffer + 9) <= BUFFER_LENGTH);

	write_buffered_int(time_val.GetYear());
	write_buffered_byte(time_val.GetMonth());
	write_buffered_byte(time_val.GetDay());
	write_buffered_byte(time_val.GetHour());
	write_buffered_byte(time_val.GetMinute());
	write_buffered_byte(time_val.GetSecond());
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_coledatetime(COleDateTime time_val)
{
   ASSERT((m_buffer_pos - m_buffer + 8) <= BUFFER_LENGTH);

   write_buffered_double((DATE)time_val);
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::write_buffered_cstring(CString str_val)
{
	write_buffered_void((void*)LPCTSTR(str_val), str_val.GetLength());
}

//
// -----------------------------------------------------------------------------
//

int CIndexFile::write_buffered(double lat, double lon, char* alternate_index)
{
	return write_record(m_buffer, m_buffer_pos - m_buffer, 
		lat, lon, alternate_index);
}

//
// -----------------------------------------------------------------------------
//

int CIndexFile::write_adb_buffered()
{
	return write_adb_record(m_buffer, m_buffer_pos - m_buffer);
}


//
// -----------------------------------------------------------------------------
//
// FILE CREATION ROUTINES
//
// -----------------------------------------------------------------------------
//


// Open the specified file for reading. This function will also read in the
// header information blocks that contain the file management values.
// Returns SUCCESS or FAILURE.
int CIndexFile::open(CString file_specification)
{
	int					slot_entries[8];
	CFileException		open_exception;
	CFileStatus			rStatus;
	UINT					mode = modeRead | typeBinary | shareDenyWrite;

	// make sure that the file exists first
	if (!GetStatus(file_specification, rStatus))
		return FAILURE;

	// create the requested file now, so that we know that we can do it
	// (access privileges are ok, path is ok, etc.)
	if (!Open(file_specification, mode, &open_exception))
	{
		std::wstringstream err_msg_stream;
		err_msg_stream << "Error in file open: " << file_specification << ", code " << open_exception.m_cause;
		WriteToLogFile(err_msg_stream.str().c_str());
/*
		CString	err_msg;
		err_msg.Format("Error in file open: %s, code %d", file_specification,
			open_exception.m_cause);
		ERR_report(err_msg);
*/
		return FAILURE;
	}

	// read in the header slots
	//		slot 0	-	file information (date, version, build_type, build_no)
	if (read_header_slot(0, slot_entries) == SUCCESS)
		m_version = slot_entries[6];					// data format version
	else
	{
		//ERR_report("Unable to read slot 0 in header block.");
		WriteToLogFile(L"Unable to read slot 0 in header block.");
		return FAILURE;
	}

	//		slot 1	-	file position of Primary Data Block (PDB, indexed)
	if (read_header_slot(1, slot_entries) == SUCCESS)
	{
		m_pdb_location       = slot_entries[0]; 
		m_ndx_count          = slot_entries[1];
		m_max_pdb_rec_length = slot_entries[2];
	}
	else
	{
		//ERR_report("Unable to read slot 1 in header block.");
		WriteToLogFile(L"Unable to read slot 1 in header block.");
		return FAILURE;
	}

	//		slot 2	-	file position of Alternate Data Block (ADB, non-indexed)
	if (read_header_slot(2, slot_entries) == SUCCESS)
	{
		m_adb_location			= slot_entries[0];
		m_adb_count				= slot_entries[1];
		m_max_adb_rec_length = slot_entries[2];
	}
	else
	{
		//ERR_report("Unable to read slot 2 in header block.");
		WriteToLogFile(L"Unable to read slot 2 in header block.");
		return FAILURE;
	}

	//		slot 3	-	Index Information Block (IIB) of primary index
	//							(includes size of index value (in bytes) and file position
	//							of the Alternate Index Block)
	if (read_header_slot(3, slot_entries) == SUCCESS)
	{
		m_pib_location = slot_entries[0];
		m_bucket_size  = slot_entries[1];
		m_used_buckets = slot_entries[2];
	}
	else
	{
		//ERR_report("Unable to read slot 3 in header block.");
		WriteToLogFile(L"Unable to read slot 3 in header block.");
		return FAILURE;
	}

	//		slot 4	-	Index Information Block (IIB) of alternate index
	//							(includes size of index value (in bytes) and file position
	//							of the Alternate Index Block)
	if (read_header_slot(4, slot_entries) == SUCCESS)
	{
		m_aib_location         = slot_entries[0];
		m_alternate_index_size = slot_entries[1];
	}
	else
	{
		//ERR_report("Unable to read slot 4 in header block.");
		WriteToLogFile(L"Unable to read slot 4 in header block.");
		return FAILURE;
	}

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

//	Moves the file position pointer to begin a read of the bucket for the
//	corresponding bucket for lat/lon. NOTE: The caller is responsible for
// correctly managing the buckets that are requested; in other words,
// if the bucket_size is 3, then the user will get the same return
// values for lat/lons of 0/0, 0/1, 0/2, 1/0, 1/1, 1/2, 2/0, 2/1 and 2/2,
// thus resulting in duplicate reads if all such blocks are requested.
//
//	Returns the number of records available for the bucket: 0 for no
//	records in bucket, -1 for error condition.
//
// Note that the record pointer will be at the first byte of the first
// record block, which always begins with the record_size for that
// record. The INT record_size MUST be read before the record.
int CIndexFile::begin_read_index(int lat_degrees, int lon_degrees)
{
	int		lat_bucket = (lat_degrees + 90)  / m_bucket_size;
	int		lon_bucket = (lon_degrees + 180) / m_bucket_size;
	
   int		prevHigh, prevLow, currentRec, currentPos, errSet, setPos;
	int		found = FALSE;
	struct	primaryIndexEntry		pib_entry;
	int		bucket_count;
   
	// set our initial bounds
   prevHigh = m_used_buckets;
   prevLow = 0;
   currentRec = (int)(prevHigh / 2);
   
	// perform the binary search
   while (prevLow <= prevHigh) {
      currentPos = currentRec * sizeof(struct primaryIndexEntry);

		// do the index seek by including the PIB offset
      setPos = static_cast<int>(Seek(currentPos + m_pib_location, begin));

      // file error condition... return sick
      if ((currentPos + m_pib_location) != setPos) {
         errSet = errno;
         //ERR_report("Failure on read position set.\n");
         WriteToLogFile(L"Failure on read position set.\n");
			return -2;
      }
      
		// read the current entry and compare it, adjusting the next entry read
		// as warrented.
      Read(&pib_entry, sizeof(struct primaryIndexEntry));
      if ((pib_entry.lat == lat_bucket) && (pib_entry.lon == lon_bucket))
		{
			found = TRUE;
         break;
		}
      else if ((pib_entry.lat < lat_bucket) || ((pib_entry.lat == lat_bucket) && (pib_entry.lon < lon_bucket)))
         prevLow = currentRec + 1;
      else if ((pib_entry.lat > lat_bucket) || ((pib_entry.lat == lat_bucket) && (pib_entry.lon > lon_bucket)))
         prevHigh = currentRec - 1;
      
      currentRec = (int) ((prevHigh + prevLow) / 2);
   }
   
	// if we weren't able to find the entry, then return that condition to caller
	if (!found)
		return -1;

	// move to the beginning of the bucket block
	Seek(pib_entry.filePos, begin);
	if (GetPosition() != (UINT)pib_entry.filePos)
	{
		//ERR_report("Unable to seek to beginning of bucket block.");
		WriteToLogFile(L"Unable to seek to beginning of bucket block.");
		return -2;
	}

	// read the record count
	if (Read(&bucket_count, sizeof(int)) != sizeof(int))
	{
		//ERR_report("Unable to read the bucket block count.");
		WriteToLogFile(L"Unable to read the bucket block count.");
		return -3;
	}

	return bucket_count;
}

//
// -----------------------------------------------------------------------------
//

//	Moves the file position pointer to begin a read of a record
//	whose unique index value is passed. 
//
//	Returns the size of the located record: 0 for no record found
//
// Note that the record pointer will be at the first byte of the found
// record, which will always be _after_ the record_size field
// 
int CIndexFile::begin_alt_index_read(const char* alt_index_value)
{
   int		prevHigh, prevLow, currentRec, currentPos, errSet, setPos;
	int		found = FALSE;
	struct	altIndexEntry	alt_ndx_entry;
	int		compare;
	int		index_size = m_alternate_index_size + sizeof(long);
   
	// set our initial bounds
   prevHigh = m_ndx_count;
   prevLow = 0;
   currentRec = (int)(prevHigh / 2);
   
	// perform the binary search
   while (prevLow <= prevHigh) {
      currentPos = currentRec * index_size;

		// do the index seek by including the PIB offset
      setPos = static_cast<int>(Seek(currentPos + m_aib_location, begin));

      // file error condition... return sick
      if ((currentPos + m_aib_location) != setPos) {
         errSet = errno;
         //ERR_report("Failure on read position set.\n");
         WriteToLogFile(L"Failure on read position set.\n");
			return FAILURE;
      }
      
		// read the current entry and compare it, adjusting the next entry read
		// as warrented.
      Read(&alt_ndx_entry.index_value, m_alternate_index_size);

		compare = strncmp(alt_ndx_entry.index_value, alt_index_value, 
			m_alternate_index_size);

      if (compare == 0)
		{
			found = TRUE;
         break;
		}
      else if (compare < 0)
         prevLow = currentRec + 1;
      else if (compare > 0)
         prevHigh = currentRec - 1;
      
      currentRec = (int) ((prevHigh + prevLow) / 2);
   }
   
	// if we weren't able to find the entry, then return that condition to caller
	if (!found)
		return FAILURE;

	// Read the file position as the next sizeof(long) bytes
	Read(&alt_ndx_entry.filePos, sizeof(long));

	// move to the beginning of the bucket block
	Seek(alt_ndx_entry.filePos, begin);
	if (GetPosition() != (UINT)alt_ndx_entry.filePos)
	{
		//ERR_report("Unable to seek to indexed record.");
		WriteToLogFile(L"Unable to seek to indexed record.");
		return FAILURE;
	}

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

//	Reads the record at the current file position. Notes:
//	(1)	The structure	of the record must be handled by the caller. 
//	(2)	The first sizeof(int) bytes read is the record_size of the 
//			original record.
//	(3)	An erroneous read can occur if if the record position
//			is not correctly handled by the user. This means that
//			the user should not to attempt to read more records than
//			are in the current block. The only check that the read
//			can really do is to check the record_size against the
//			system's known max record length.
//	(4)	The caller is responsible for providing an appropriate
//			data block into which the data is read. This means that
//			that the caller should have allocated a block that is 
//			at least get_primary_record_max_length() bytes long
//			if reading a regular indexed record and at least
//			get_alt_record_max_length() bytes long if reading
//			an alternate record. 
// (5)	The record location is undefined and must be reset
//			in the event of an error return.
//
// Returns record_size on a successful read, or -1 otherwise.
int CIndexFile::read_record(void* pRecord, BOOL primary_record)
{
	// Start by reading the record size and validating it.
	int	record_size;
	Read(&record_size, sizeof(int));

	if (primary_record)
	{
		if (record_size > m_max_pdb_rec_length)
		{
			//ERR_report("Record misalignment suspected. Record not read.");
			WriteToLogFile(L"Record misalignment suspected. Record not read.");
			return -1;
		}
	}
	else
	{
		if (record_size > m_max_adb_rec_length)
		{
			//ERR_report("Record misalignment suspected. Record not read.");
			WriteToLogFile(L"Record misalignment suspected. Record not read.");
			return -1;
		}
	}

	// Read the next "record_size" bytes into pRecord.
	if (Read(pRecord, record_size) != (UINT)record_size)
	{
		//ERR_report("Unable to read an entire record properly. "
		//	"Undetermined record contents.");
		WriteToLogFile(L"Unable to read an entire record properly. "
			L"Undetermined record contents.");
		return -1;
	}

	return record_size;
}


//
// -----------------------------------------------------------------------------
//

// Locates the record which corresponds to the alternate index
// value requested and reads the record into the area pointed to
// by pRecord. 
//
// NOTE that all notes stipulated in "read_record" above apply here
// with the exception of note (3).
//
//	Returns SUCCESS or FAILURE.
int CIndexFile::read_index_record(char* alt_index_value, void* pRecord)
{
   int		prevHigh, prevLow, currentRec, currentPos, errSet, setPos;
	int		found = FALSE;
	struct	altIndexEntry	alt_ndx_entry;
	int		compare;
	int		record_size;
	int		index_size = m_alternate_index_size + sizeof(long);
   
	// set our initial bounds
   prevHigh = m_ndx_count;
   prevLow = 0;
   currentRec = (int)(prevHigh / 2);
   
	// perform the binary search
   while (prevLow <= prevHigh) {
      currentPos = currentRec * index_size;

		// do the index seek by including the PIB offset
      setPos = static_cast<int>(Seek(currentPos + m_aib_location, begin));

      // file error condition... return sick
      if ((currentPos + m_aib_location) != setPos) {
         errSet = errno;
         //ERR_report("Failure on read position set.\n");
         WriteToLogFile(L"Failure on read position set.\n");
			return -2;
      }
      
		// read the current entry and compare it, adjusting the next entry read
		// as warrented.
      Read(&alt_ndx_entry.index_value, m_alternate_index_size);

		compare = strncmp(alt_ndx_entry.index_value, alt_index_value, 
			m_alternate_index_size);

      if (compare == 0)
		{
			found = TRUE;
         break;
		}
      else if (compare == -1)
         prevLow = currentRec + 1;
      else if (compare == 1)
         prevHigh = currentRec - 1;
      
      currentRec = (int) ((prevHigh + prevLow) / 2);
   }
   
	// if we weren't able to find the entry, then return that condition to caller
	if (!found)
		return -1;

	// Read the file position as the next sizeof(long) bytes
	Read(&alt_ndx_entry.filePos, sizeof(long));

	// move to the beginning of the bucket block
	Seek(alt_ndx_entry.filePos, begin);
	if (GetPosition() != (UINT)alt_ndx_entry.filePos)
	{
		//ERR_report("Unable to seek to indexed record.");
		WriteToLogFile(L"Unable to seek to indexed record.");
		return -2;
	}

	// read the record
	record_size = read_record(pRecord, SUCCESS);
	return record_size;
}

//
// -----------------------------------------------------------------------------
//

// set the file pointer to the first bucket (block of PDB records) -- returns
// the number of blocks/buckets to read
int CIndexFile::reset_block_reads()
{
	Seek(m_pdb_location, begin);
	if (GetPosition() != (UINT)m_pdb_location)
	{
		//ERR_report("Unable to seek to beginning of bucket block.");
		WriteToLogFile(L"Unable to seek to beginning of bucket block.");
		return -2;
	}

	return m_used_buckets;
}

//
// -----------------------------------------------------------------------------
//

// retrieves the number of records in the next block -- 0 indicates no
// more blocks -- must be called to properly set i/o pointer to beginning of
// the block (past the block header)
int CIndexFile::next_block_size()
{
	int		bucket_count;
   
	// read the record count
	if (Read(&bucket_count, sizeof(int)) != sizeof(int))
	{
		//ERR_report("Unable to read the bucket block count.");
		WriteToLogFile(L"Unable to read the bucket block count.");
		return -3;
	}

	return bucket_count;
}

//
// -----------------------------------------------------------------------------
//

// retrieves the Nth alternate index value -- used to sequentially read through
// the records by alternate index sort value
int CIndexFile::get_alt_index(int index_num, char* alt_index_value, int alt_index_value_len)
{
	int	index_size = m_alternate_index_size + sizeof(long);
	int	currentPos = index_num * index_size;

	// do the index seek by including the PIB offset
   int	setPos = static_cast<int>(Seek(currentPos + m_aib_location, begin));

	// file error condition... return sick
	if ((currentPos + m_aib_location) != setPos) {
		//ERR_report("Failure on read position set.\n");
		WriteToLogFile(L"Failure on read position set.\n");
		return -2;
	}
	
	// read the current entry and compare it, adjusting the next entry read
	// as warrented.
	struct	altIndexEntry	alt_ndx_entry;
	const int nBytesRead = Read(&alt_ndx_entry.index_value, m_alternate_index_size);
   if (nBytesRead != m_alternate_index_size)
   {
      //ERR_report("Failure on read position set.\n");
      WriteToLogFile(L"Failure on read position set.\n");
      return -2;
   }

   alt_ndx_entry.index_value[m_alternate_index_size] = '\0';
	
	// Read the file position as the next sizeof(long) bytes
	Read(&alt_ndx_entry.filePos, sizeof(long));
	
	// move to the beginning of the bucket block
	Seek(alt_ndx_entry.filePos, begin);
	if (GetPosition() != (UINT)alt_ndx_entry.filePos)
	{
		//ERR_report("Unable to seek to indexed record.");
		WriteToLogFile(L"Unable to seek to indexed record.");
		return -2;
	}

	strncpy_s(alt_index_value, alt_index_value_len, alt_ndx_entry.index_value, m_alternate_index_size);

	return 1;
}

//
// -----------------------------------------------------------------------------
//

//	Function for reading the 512-byte application header block. 
// Returns SUCCESS or FAILURE.
int CIndexFile::read_application_header(void* pAppHeaderBlock)
{
	DWORD		cur_pos = static_cast<DWORD>(GetPosition());

	Seek(512, begin);
	Read(pAppHeaderBlock, 512);
	Seek(cur_pos, begin);

	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

// Writes a 32-byte slot record for the CIndexFile header. Returns SUCCESS
// or FAILURE.
int CIndexFile::read_header_slot(int slot, void* slot_value)
{
	DWORD		orig_loc = static_cast<DWORD>(GetPosition());
	DWORD		byte_loc = slot * 32;

	Seek(byte_loc, begin);
	Read(slot_value, 32);
	Seek(orig_loc, begin);

	return SUCCESS;
}

// --------------------------------------------------------------------------
// Buffered i/o routines ----------------------------------------------------
// --------------------------------------------------------------------------

void CIndexFile::read_buffered_start(boolean_t primary)
{
	// allocate the memory if it hasn't been already
	if (!m_buffer)
		m_buffer = (BYTE*)malloc(BUFFER_LENGTH);
		//m_buffer = (BYTE*)MEM_malloc(BUFFER_LENGTH);

	// rewind our buffer position pointer
	m_buffer_pos = m_buffer;

	ASSERT(get_primary_record_max_length() < BUFFER_LENGTH);

	m_rec_size = read_record(m_buffer, primary);
	m_buffer_pos = m_buffer;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_byte(BYTE* byte_val)
{
	ASSERT((m_buffer_pos - m_buffer + 1) <= m_rec_size);
	*byte_val = *m_buffer_pos++;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_int(int* int_val)
{
	ASSERT((m_buffer_pos - m_buffer + 4) <= m_rec_size);

	memcpy((void*)int_val, m_buffer_pos, 4);
	m_buffer_pos += 4;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_short(short* short_val)
{
	ASSERT((m_buffer_pos - m_buffer + 2) <= m_rec_size);

	memcpy((void*)short_val, m_buffer_pos, 2);
	m_buffer_pos += 2;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_double(double* double_val)
{
	ASSERT((m_buffer_pos - m_buffer + 8) <= m_rec_size);

	memcpy((void*)double_val, m_buffer_pos, 8);
	m_buffer_pos += 8;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_void(void* void_val, int* void_len)
{
	int	len;

	ASSERT((m_buffer_pos - m_buffer + 4) <= m_rec_size);
	read_buffered_int(&len);

	ASSERT(len <= *void_len);
	*void_len = len;

	ASSERT((m_buffer_pos - m_buffer + *void_len) <= m_rec_size);
	memcpy(void_val, m_buffer_pos, *void_len);
	m_buffer_pos += *void_len;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_bool(boolean_t* bool_val)
{
	ASSERT((m_buffer_pos - m_buffer + 1) <= m_rec_size);

	*bool_val = *m_buffer_pos++;
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_ctime(CTime* time_val )
{
	ASSERT((m_buffer_pos - m_buffer + 9) <= BUFFER_LENGTH);

	int nYear;
	BYTE nMonth;
	BYTE nDay;
	BYTE nHour;
	BYTE nMin;
	BYTE nSec;

	read_buffered_int(&nYear);
	read_buffered_byte(&nMonth);
	read_buffered_byte(&nDay);
	read_buffered_byte(&nHour);
	read_buffered_byte(&nMin);
	read_buffered_byte(&nSec);

	*time_val = CTime(nYear, nMonth, nDay, nHour, nMin, nSec);
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_coledatetime(COleDateTime* time_val)
{
   ASSERT((m_buffer_pos - m_buffer + 8) <= BUFFER_LENGTH);

   DATE date;

   read_buffered_double(&date);

   *time_val = COleDateTime(date);
}

//
// -----------------------------------------------------------------------------
//

void CIndexFile::read_buffered_cstring(CString* str_val)
{
	int	t_len = 2048;
	char*	t = (char*)malloc(t_len);
	//char*	t = (char*)MEM_malloc(t_len);

	read_buffered_void((void*)t, &t_len);

	t[t_len] = '\0';
	*str_val = t;
	free(t);
	//MEM_free(t);
}

//
// -----------------------------------------------------------------------------
//

// Sets the read position to the beginning of an adb record (used primarily 
// when non-"point" (lat/lon) data is indexed using cell/grid methodology, 
// like Echum's powerlines.) Returns SUCCESS or FAILURE.
int CIndexFile::set_adb_read_position(long new_pos)
{
	if (Seek(new_pos, CFile::begin) != new_pos)
		return FAILURE;
	else
		return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

// Push and Pop location read mechanisms for interruptions in the
// block read methodologies. This is used primarily 
// when non-"point" (lat/lon) data is indexed using cell/grid methodology, 
// like Echum's powerlines.) Returns SUCCESS or FAILURE.
int CIndexFile::push_pdb_loc()
{
	m_save_pdb_location = static_cast<long>(GetPosition());
	return SUCCESS;
}

//
// -----------------------------------------------------------------------------
//

int CIndexFile::pop_pdb_loc()
{
	if (Seek(m_save_pdb_location, CFile::begin) != m_save_pdb_location)
		return FAILURE;
	else
		return SUCCESS;
}


