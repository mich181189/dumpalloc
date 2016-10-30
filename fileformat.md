# Dumpalloc File Format

## Introduction
This file intends to explain the (planned) dumpalloc file format. It is intended to be a relatively simple, yet extensible file format, allowing possible future plugins, and allowing readers to skip sections they do not understand.

## Overall Design Principals
* All integers shall be little endian
* All pointers shall be expressed in 8 bytes
* Each record shall contain a length field, allowing readers to skip it
* The format shall allow for other, as yet unknown section types
* Borrowing from RIFF format and FOURCC, section headers shall be semi-human readable

## Composite Types
### String
Strings are expressed as a 4 byte length followed by the literal characters.
Any non-ascii values should be expressed as UTF-8 code points.
Strings are not NULL terminated.

Byte | type | description
--- | --- | ---
0-3 | integer | length
4-n | char | string contents

### Timestamp
Byte | type | description
--- | --- | ---
0-7 | integer (64-bit) | UNIX timestamp
8-11 | integer (32-bit) | nanoseconds since last second

## Record Types
In general, all records take the format of a type field and a length header.

Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 4 character ID
Record Length | integer | 4 | length of record after this point
Remainder of record | variable | variable |

hence the reader may skip "length" bytes to jump over the record.

### Process Record
Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 'PROC'
Record Length | integer | 4 | length of record after this point
Process ID | integer | 4 | process ID of the process
Process Name | string | variable | full path to object

### Object Record
Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 'OBJE'
Record Length | integer | 4 | length of record after this point
Object Name | string | variable | full path to object

It's worth noting in this case that actually, there will be two length fields next to each other (the record and the string). This is considered to be an acceptable side effect of a predictable file format.

### Allocation Record
Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 'ALOC'
Record Length | integer | 4 | length of record after this point
Memory Address | pointer | 8 | address of memory allocated
Allocation TIme | timestamp | 12 | timestamp of memory allocation
| | |
Stack Entries | Frame Records | variable | Frames of allocation

Note that the frame records are not included in the count of the record length. There may be zero or more frame records after an allocation record, ending in a frame record of type TERM.

### Frame Record
Frame records are a little different in that there are multiple subtypes, including a termination marker

Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 'FRAM'
Record Length | integer | 4 | length of record after this point
Frame Record Type | char | 4 | type of the rest of this record
Payload | Variable | Variable | possible payload of this frame record

If the frame record type is unknown, the remainder of the Frame Record may be skipped (since the length is already known from the header)

#### Frame Record Types
Type ID | Type Name | Payload
--- | --- | ---
'NTVE' | Native Frame | pointer
'PCAL' | Precalculated record, e.g interpreted code | precalculated/literal record
'TERM' | Termination Frame | None

A Precalculated frame is defined as a frame that does not require a lookup on the side of the reader. This might come from interpreted code, or perhaps the unwinder is doing its own symbol lookup. It is defined as:

Field | type | length
--- | --- | ---
Function Name | string | variable
Source Filename | string | variable
Line Number | integer | 4

A termination frame ends this allocation call trace.

### Deallocation Record
Name | Type | Length | Value
---- | --- | --- | ---
Record Type | char | 4 | 'DALC'
Record Length | integer | 4 | length of record after this point
Memory Address | pointer | 8 | address of memory deallocated

