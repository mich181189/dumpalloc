/**
 * dumpalloc-file.c
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

#include "../include/dumpalloc-file.h"
#include "../include/endian.h"

#include <assert.h>
#include <string.h>

static int read_bytes(int fd, char* buffer, size_t expected_len) {

	int ret = 0;
	int total_read = 0;

	while (expected_len > 0 && (ret = read(fd, buffer+total_read, expected_len)) > 0) {

		expected_len -= ret;

		total_read += ret;
	}

	return total_read;
}

int dumpalloc_file_read_record_header(int fd, char* type, int32_t* record_size) {
    
    assert(type != NULL);
    assert(record_size != NULL);
    
    if(type == NULL || record_size == NULL) {
        return 1;
    }
    
    if(read_bytes(fd, type, 4) != 4) {
        return 1;
    }
    
    if(read_bytes(fd, record_size, sizeof(int32_t)) != sizeof(int32_t)) {
        return 1;
    }
    
    *record_size = le32toh(*record_size);
    
    return 0;
}

char* dumpalloc_file_read_string(int fd) {
    
    uint32_t length = 0;
    
    if(read_bytes(fd, &length, sizeof(uint32_t)) != sizeof(uint32_t)) {
        return NULL;
    }
    
    length = le32toh(length);
    
    char* str = calloc(length, 1);
    if(!str) {
        return NULL;
    }
    
    if(read_bytes(fd, str, length) != length) {
        free(str);
        return NULL;
    }
    
    return str;
}

void dumpalloc_file_free_string(char* str) {
    free(str);
}

int dumpalloc_file_read_timestamp(int fd, int64_t* seconds, int32_t* nanoseconds) {
    //seconds is mandatory. nanoseconds is optional.
    assert(seconds != NULL);
    
    if(read_bytes(fd, seconds, sizeof(int64_t)) != sizeof(int64_t)) {
        return 1;
    }
    
    *seconds = le64toh(*seconds);
    
    if(nanoseconds != NULL) {
        if(read_bytes(fd, nanoseconds, sizeof(int32_t)) != sizeof(int32_t)) {
            return 1;
        }
    } else {
        // read nanoseconds and throw it away since the caller didn't want it.
        int32_t nanoseconds_dump;
        if(read_bytes(fd, &nanoseconds_dump, sizeof(int32_t)) != sizeof(int32_t)) {
            return 1;
        }
    }
    
    return 0;
}

dumpalloc_file_timestamp_t* dumpalloc_file_read_timestamp2(int fd) {
    dumpalloc_file_timestamp_t* rec = malloc(sizeof(dumpalloc_file_timestamp_t));
    
    if(!rec) {
        return NULL;
    }
    
    if(dumpalloc_file_read_timestamp(&rec->seconds, &rec->nanoseconds) != 0) {
        free(rec);
        return NULL;
    }
    
    return rec;
}

void dumpalloc_file_free_timestamp(dumpalloc_file_timestamp_t* ts) {
    free(rec);
}

int dumpalloc_file_read_process_record(int fd, int32_t* pid, char** name) {
    assert(pid);
    assert(name);
    if(pid == NULL || name == NULL)
    {
        return 1;
    }
    
    // if this assert fires, you might be leaking strings.
    assert(*name == NULL);
    
    if(read_bytes(fd, pid, sizeof(int32_t)) != sizeof(int32_t)) {
        return 1;
    }
    
    *name = dumpalloc_file_read_string(fd);
    if(*name == NULL) {
        return 1;
    }
    
    return 0;
}

int dumpalloc_file_read_object_record(int fd, char** name) {
    assert(name);
    
    if(name == NULL) {
        return 1;
    }
    
    *name = dumpalloc_file_read_string(fd);
    
    return (*name != NULL) ? 0 : 1;
}

int dumpalloc_file_read_allocation_record(int fd, uint64_t* address, dumpalloc_file_timestamp_t** timestamp) {
    assert(address);
    assert(timestamp);
    
    if(address == NULL || timestamp == NULL) {
        return 1;
    }
    
    // If this fires, you might be leaking timestamp structures.
    assert(*timestamp == NULL);
    
    if(read_bytes(fd, address, sizeof(uint64_t)) != sizeof(uint64_t)) {
        return 1;
    }
    
    *timestamp = dumpalloc_file_read_timestamp2(fd);
    
    return (*timestamp != NULL) ? 0 : 1;
}

dumpalloc_file_frame_record_t* dumpalloc_file_read_frame_record(int fd) {
    
}

dumpalloc_file_precalc_record_t* dumpalloc_file_read_precalc_frame(dumpalloc_file_frame_record_t* frame) {
    
}

void dumpalloc_file_free_precalc_record(dumpalloc_file_precalc_record_t* rec) {
    
}

void dumpalloc_file_free_frame_record(dumpalloc_file_frame_record_t* rec) {
    
}

int dumpalloc_file_read_deallocation_record(int fd, uint64_t* address) {
    
}
