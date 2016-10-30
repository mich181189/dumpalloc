/**
 * dumpalloc-file.h
 *
 * Copyright (c) 2016 Michael Cullen <michael181189@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DUMPALLOC_FILE_H
#define DUMPALLOC_FILE_H

typedef struct {
    int64_t seconds;
    uint32_t nanoseconds;
} dumpalloc_file_timestamp_t;

typedef struct {
    char* function_name;
    char* source_file_name;
    uint32_t line_number;
} dumpalloc_file_precalc_record_t

typedef struct {
    char type[4];
    void* payload;
    size_t payload_length;
} dumpalloc_file_frame_record_t;

/**
 * \param[in] fd file descriptor to read from
 * \param[out] type buffer allocated by caller to place four characters in
 * \param[out] record_size size of record
 * \return zero on success, non-zero on failure
 */
int dumpalloc_file_read_record_header(int fd, char* type, int32_t* record_size);

/**
 * \brief reads a string from the provided FD
 * 
 * \return zero terminated string on success, NULL on failure. Caller is responsible for freeing.
 * \param[in] fd input fd to read from
 */
char* dumpalloc_file_read_string(int fd);

/**
 * \brief free a string allocated by dumpalloc_file_read_string
 * 
 * \param[in] string pointer to string to free.
 */
void dumpalloc_file_free_string(char* string);

/**
 * \brief read a timestamp from the provided FD
 * 
 * \param[in] fd file descriptor to read from
 * \param[out] seconds seconds part of timestamp
 * \param[out] nanoseconds optional nanoseconds part of timestamp. Pass NULL to ignore.
 * \return zero on success, non-zero on failure.
 */
int dumpalloc_file_read_timestamp(int fd, int64_t* seconds, int32_t* nanoseconds);

/**
 * \brief read a timestamp from the provided FD, to a structure
 * 
 * \param[in] fd file descriptor to read from
 * \return pointer to timestamp struct on success, NULL on failure. Free with dumpalloc_file_free_timestamp
 */
dumpalloc_file_timestamp_t* dumpalloc_file_read_timestamp2(int fd);

/**
 * \brief free a timestamp allocated with dumpalloc_file_read_timestamp2
 * 
 * \param[in] ts pointer to timestamp struct to  free.
 */
void dumpalloc_file_free_timestamp(dumpalloc_file_timestamp_t* ts);

/**
 * \brief read a process record from the provided FD
 * 
 * \param[in] fd file descriptor to read from
 * \param[out] pid Process Id field of record
 * \param[out] name name of process. To be deallocated by dumpalloc_file_free_string.
 * \return zero on success, non-zero on failure.
 */
int dumpalloc_file_read_process_record(int fd, int32_t* pid, char** name);

/**
 * \brief read an object record from the provided FD
 * 
 * \param[in] fd file descriptor to read from
 * \param[out] name name of object. To be deallocated by dumpalloc_file_free_string.
 * \return zero on success, non-zero on failure.
 */
int dumpalloc_file_read_object_record(int fd, char** name);

/**
 * \brief read an allocation record from the provided FD
 * 
 * \param[in] fd file descriptor to read from
 * \param[out] address Memory address allocated
 * \param[out] timestamp timestamp of allocation
 * \return zero on success, non-zero on failure.
 */
int dumpalloc_file_read_allocation_record(int fd, uint64_t* address, dumpalloc_file_timestamp_t** timestamp);

/**
 * \brief read a frame record from the provided FD
 * \note Interpretation of the payload is dependant on type.
 * 
 * \return frame record on success, NULL on failure. Returned pointer must be freed with dumpalloc_file_free_frame_record
 */
dumpalloc_file_frame_record_t* dumpalloc_file_read_frame_record(int fd);

/** 
 * \brief free structure returned from dumpalloc_file_read_frame_record
 * 
 * \param[in] rec pointer to free
 */
void dumpalloc_file_free_frame_record(dumpalloc_file_frame_record_t* rec);

/**
 * \brief read a deallocation record from the provided FD
 * 
 * \param[in] fd file descriptor to read from
 * \param[out] address Memory address deallocated
 * \return zero on success, non-zero on failure.
 */
int dumpalloc_file_read_deallocation_record(int fd, uint64_t* address);

#endif
