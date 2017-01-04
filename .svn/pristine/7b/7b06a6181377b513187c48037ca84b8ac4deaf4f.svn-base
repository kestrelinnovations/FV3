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
// IndexFil.h
//

// CIndexFile defines a standard mechanism for reading and writing indexed
// files containing two indices per record, a header block, an indexed
// record collection, a non-indexed record collection, and the actual
// index lists. In addition, actual reading and writing of the stored
// records is done via callbacks to the utilizing code.

// The primary index is a lat/lon bucket format, where the bucket pointers
// are stored in a sorted order and point to the beginning of the file
// area where all points contained within that bucket are sequentially
// stored in non-sorted sequence. In other words, the lat/lon indices
// are only indices to buckets, not to records.

// The alternate index is a record-address form of index where there is
// a one-to-one correspondance between an index and a record. This 
// necessitates that the alternate indices be unique by record. A write
// error will occur if the caller attempts to write two indexed records
// with the same alternate index.

//
// -----------------------------------------------------------------------------
//

// The format of a CIndexFile is as follows:
//
// Reserved Header Block (RHB, 512 bytes, 16 32-byte slots)
//		slot 0	-	file information (date, version, build #)
//		slot 1	-	file position of Primary Data Block (PDB, indexed)
//		slot 2	-	file position of Alternate Data Block (ADB, non-indexed)
//		slot 3	-	Index Information Block (IIB) of primary index
//							(includes size of index value (in bytes) and file position
//							of the Alternate Index Block)
//		slot 4	-	Index Information Block (IIB) of alternate index
//							(includes size of index value (in bytes) and file position
//							of the Alternate Index Block)
//		slot 5 - slot 15
//					-	reserved
//
//	Application Header Block (AHB, 512 bytes)
//
// Alternate Data Block  (ADB, arbitrary number of arbitrary-sized records, 
//						written-to and read-from via call back functions) -- placed
//						first because it can be written on the fly during file
//						creation.
//
// Pimrary Data Block (PDB, arbitrary number of arbitrary-sized records, 
//						written to and read from via call back functions)
//
// Primary Index Block (PIB, size according to IIB for Primary index)
//
//	Alternate Index Block (AIB, size according to IIB for Alternate index)

#ifndef INDEX_FIL_H
#define INDEX_FIL_H 1


#define CINDEXFILE_VERSION 1
#define BUFFER_LENGTH		8192
//
// -----------------------------------------------------------------------------
//

#include "geo_tool\common.h" // for boolean_t, SUCCESS, and FAILURE


class CIndexFile : public CFile
{
public:
	struct filePosChain {
		long						filePos;
		struct filePosChain*	nextPos;
	};

	struct altIndexEntry {
		char		index_value[20];
		long		filePos;
	};

	struct primaryIndexEntry {
		int		lat;
		int		lon;
		long		filePos;
	};

	BOOL		m_opened;					// file currently opened or not

	BYTE*		m_buffer;					// buffered record pointer
	BYTE*		m_buffer_pos;				// current write position in buffer
	int		m_rec_size;					// buffered input record size

	int		m_version;					// file version number (format of records)
	struct filePosChain**	 
				m_lonLatChain;				// pointer to malloc'd array
	int		m_total_buckets;			// total available buckets
	int		m_bucket_size;				// requested bucket size, must be evenly 
												// divisible into 360
	int		m_lat_buckets;				// number of latitude buckets
	int		m_lon_buckets;				// number of longitude buckets
	int		m_alternate_index_size;	// number of bytes in each alternate index
	CFile*	m_ndx_file;					// temporary file for index building
	CString	m_tmp_filename;			// name of the temporary file
	int		m_adb_count;				// count of adb records
	int		m_ndx_count;				// count of indexed records
	long		m_adb_location;			// location of the adb
	long		m_pdb_location;			// location of the pdb
	long		m_current_adb_location;	// current write position in the adb record area
	long		m_save_pdb_location;		// used for push/pop of pdb location
	long		m_pib_location;			// location of the pib
	long		m_aib_location;			// location of the aib
	int		m_max_pdb_rec_length;	// maximum record length found during create
	int		m_max_adb_rec_length;	// maximum record length found during create

	int		m_used_buckets;			// number of used buckets
	int		m_smallest_bucket;		// statistics gathered to aid bucket size
	int		m_largest_bucket;			//			determination.
	int		m_average_bucket;			//				"
	int		m_largest_bucket_lat;
	int		m_largest_bucket_lon;

public:

	// --------------------------------------------------------------------------
	// Standard constructor and destructor --------------------------------------
	// --------------------------------------------------------------------------
	CIndexFile();
	~CIndexFile();

	// --------------------------------------------------------------------------
	// Standard open and close CFile overrides ----------------------------------
	// --------------------------------------------------------------------------
	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, 
		CFileException* pError = NULL);
	void Close();

	// --------------------------------------------------------------------------
	// File creation routines ---------------------------------------------------
	// --------------------------------------------------------------------------

	// Create a new file with the given file specification. The size of the
	// alternate index must be specified as well as the size of the lat/lon
	// bucket (in degrees). Returns SUCCESS or FAILURE.
	int create(CString file_specification, int bucket_size, int alt_index_size, 
		int version);

	// Signals the end of all file creation activity. This function will 
	// (1) write the PDB (primary data block) in bucket sequence with each bucket
	// preceded by the count of records in the bucket, (2) write the PIB (primary
	// index block) for all indices that have content available, (3) write the
	// AIB (alternate index block), and (4) write the header slots with the
	// built information.
	// Returns SUCCESS or FAILURE.
	int end_create();

	//	Writes a record to the temporary build area, builds the index entries
	//	and calls the virtual function write_record (see below). The actual file 
	// consumption will be record_size + sizeof(int) because we always write 
	// the record size before the record. Returns SUCCESS or FAILURE.
	int write_record(void* record, int record_size, double lat, double lon, 
		char* alternate_index);

	// Writes a record to the Alternate Data Block area of the file -- not
	// indexed in any way. The actual file consumption will be record_size +
	// sizeof(int) because we always write the record size before the record.
	//
	// NOTE: We write this out at the current file position -- because 
	// CIndexFile makes sure that during file creation, the file does not
	// get written to until a CLOSE occurs, EXCEPT through this routine.
	//
	// Returns SUCCESS or FAILURE.
	int write_adb_record(void* record, int record_size);

	//	Function for writing the 512-byte application header block. 
	// Returns SUCCESS or FAILURE.
	int write_application_header(void* pAppHeaderBlock);

	// --------------------------------------------------------------------------
	// Buffered i/o routines ----------------------------------------------------
	// --------------------------------------------------------------------------
	void write_buffered_start();
	void write_buffered_byte(BYTE byte_val);
	void write_buffered_int(int int_val);
	void write_buffered_short(short short_val);
	void write_buffered_double(double double_val);
	void write_buffered_void(void* void_val, int void_len);
	void write_buffered_bool(boolean_t bool_val);
	void write_buffered_ctime(CTime time_val);
	void write_buffered_coledatetime(COleDateTime time_val);
	void write_buffered_cstring(CString str_val);
	int write_buffered(double lat, double lon, char* alternate_index);
	int write_adb_buffered();

	void read_buffered_start(boolean_t primary);
	void read_buffered_byte(BYTE* byte_val);
	void read_buffered_int(int* int_val);
	void read_buffered_short(short* short_val);
	void read_buffered_double(double* double_val);
	void read_buffered_void(void* void_val, int* void_len);
	void read_buffered_bool(boolean_t* bool_val);
	void read_buffered_ctime(CTime* time_val);
	void read_buffered_coledatetime(COleDateTime* time_val);
	void read_buffered_cstring(CString* str_val);

	// --------------------------------------------------------------------------
	// File information routines ------------------------------------------------
	// --------------------------------------------------------------------------

	// Return the opened file's alternate_index_size.
	int get_alt_index_size() const { return m_alternate_index_size; }

	// Return the opened file's alternate_index_size.
	int get_alt_index_count() const { return m_ndx_count; }

	// Return the opened file's bucket size.
	int get_bucket_size() const { return m_bucket_size; }

	// Return the opened file's version number.
	int get_format_version() const { return m_version; }

	// Return the opened file's count of alternate records
	int get_alt_record_count() const { return m_adb_count; }

	// Return the opened file's count of indexed records
	int get_primary_record_count() const { return m_ndx_count; }

	// Return the opened file's longest alternate record length
	int get_alt_record_max_length() const { return m_max_adb_rec_length; }

	// Return the opened file's longest alternate record length
	int get_primary_record_max_length() const { return m_max_pdb_rec_length; }

	// Return the current adb record pointer (used primarily when non-"point"
	// (lat/lon) data is indexed using cell/grid methodology, like Echum's 
	// powerlines.)
	long get_adb_position() const { return m_current_adb_location; }

	// --------------------------------------------------------------------------
	// File read routines -------------------------------------------------------
	// --------------------------------------------------------------------------

	// Open the specified file for reading. This function will also read in the
	// header information blocks that contain the file management values.
	// Returns SUCCESS or FAILURE.
	int open(CString file_specification);
	
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
	int begin_read_index(int lat_degrees, int lon_degrees);

	//	Moves the file position pointer to begin a read of a record
	//	whose unique index value is passed. 
	//
	//	Returns the size of the located record: 0 for no record found
	//
	// Note that the record pointer will be at the first byte of the found
	// record, which will always be _after_ the record_size field
	// 
	int begin_alt_index_read(const char* alt_index_value);

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
	// "primary_record" indicates whether the record being read
	// is an PDB record or an ADB record.
	//
	// Returns record_size on a successful read, or -1 otherwise.
	int read_record(void* pRecord, BOOL primary_record = TRUE);

	// Locates the record which corresponds to the alternate index
	// value requested and reads the record into the area pointed to
	// by pRecord. 
	//
	// NOTE that all notes stipulated in "read_record" above apply here
	// with the exception of note (3).
	//
	//	Returns SUCCESS or FAILURE.
	int read_index_record(char* alt_index_value, void* pRecord);

	//	Function for reading the 512-byte application header block. 
	// Returns SUCCESS or FAILURE.
	int read_application_header(void* pAppHeaderBlock);

	// Push and Pop location read mechanisms for interruptions in the
	// block read methodologies. This is used primarily 
	// when non-"point" (lat/lon) data is indexed using cell/grid methodology, 
	// like Echum's powerlines.) Returns SUCCESS or FAILURE.
	int push_pdb_loc();
	int pop_pdb_loc();



	// --------------------------------------------------------------------------
	// Bucket (sequential) read mode --------------------------------------------
	// --------------------------------------------------------------------------

	// set the file pointer to the first bucket (block of PDB records) -- returns
	// the number of blocks/buckets to read
	int reset_block_reads();

	// retrieves the number of records in the next block -- 0 indicates no
	// more blocks -- must be called to properly set i/o pointer to beginning of
	// the block (past the block header)
	int next_block_size();

	// retrieves the Nth alternate index value -- used to sequentially read through
	// the records by alternate index sort value
	int get_alt_index(int index_num, char* alt_index_value, int alt_index_value_len);

	// Sets the read position to the beginning of an adb record (used primarily 
	// when non-"point" (lat/lon) data is indexed using cell/grid methodology, 
	// like Echum's powerlines.) Returns SUCCESS or FAILURE.
	int set_adb_read_position(long new_pos);

private:
	// Writes a 32-byte slot record for the CIndexFile header. Returns SUCCESS
	// or FAILURE.
	int write_header_slot(int slot, void* slot_value);

	// Reads a 32-byte slot record from the CIndexFile header. Returns SUCCESS
	// or FAILURE.
	int read_header_slot(int slot, void* slot_value);

	// Add a record position (temporary file) to our lat/lon index chain.
	// Returns SUCCESS or FAILURE.
	int add_to_chain(int lat_degrees, int lon_degrees, DWORD dwPosition);
};


#endif
