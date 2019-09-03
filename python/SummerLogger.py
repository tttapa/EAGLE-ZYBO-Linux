import struct

"""
Data structure:
    
     0        1        2        3
+--------+--------+--------+--------+
| id len |  type  |   data length   |
+--------+--------+--------+--------+
|             identifier            |
+--------+--------+--------+--------+
|                ...                |
+--------+--------+--------+--------+
|        identifier        |  NULL  |
+--------+--------+--------+--------+
|               data                |
+--------+--------+--------+--------+
|                ...                |
+--------+--------+--------+--------+

The format of the header is always the same. The identifier starts at the second
word.
There is at least one NULL byte after the identifier, and at most four.
The data always starts at a word boundary, right after the null bytes after the
identifier.
"""


def nextWord(i: int) -> int:
    """
    Get the offset of the next 4-byte word.

    0 → 4
    1 → 4
    2 → 4
    3 → 4
    4 → 8
    5 → 8

    etc.
    """
    return i - (i % 4) + 4


def roundUpToWordSizeMultiple(i: int) -> int:
    """
    Round up a size to a multiple of 4-byte words.

    0 → 0
    1 → 4
    2 → 4
    3 → 4
    4 → 4
    5 → 8

    etc.
    """
    return i + 3 - ((i + 3) % 4)


def bytesToType(datatype: int, data: bytes):
    """
    Convert an array of bytes to the given data type.
    """
    return {
        1: lambda d: struct.unpack('<b', d)[0],  # char
        2: lambda d: struct.unpack('<B', d)[0],  # uint8_t
        3: lambda d: struct.unpack('<h', d)[0],  # int16_t
        4: lambda d: struct.unpack('<H', d)[0],  # uint16_t
        5: lambda d: struct.unpack('<i', d)[0],  # int32_t
        6: lambda d: struct.unpack('<I', d)[0],  # uint32_t
        7: lambda d: struct.unpack('<q', d)[0],  # int64_t
        8: lambda d: struct.unpack('<Q', d)[0],  # uint64_t
        9: lambda d: struct.unpack('<f', d)[0],  # float
        10: lambda d: struct.unpack('<d', d)[0],  # double
        11: lambda d: struct.unpack('<?', d)[0],  # bool
        12: lambda d: struct.unpack('<c', d)[0],  # char
        100: lambda d: struct.unpack('<ffff', d),  # Quaternion
        101: lambda d: struct.unpack('<fff', d),  # EulerAngles
        102: lambda d: struct.unpack('<ff', d),  # Vec2f
        103: lambda d: struct.unpack('<fff', d),  # Vec3f
    }[datatype](data)


def typeToSize(datatype: int):
    """
    Get the length in bytes of the given data type.
    """
    return {
        1: 1,  # int8_t
        2: 1,  # uint8_t
        3: 2,  # int16_t
        4: 2,  # uint16_t
        5: 4,  # int32_t
        6: 4,  # uint32_t
        7: 8,  # int64_t
        8: 8,  # uint64_t
        9: 4,  # float
        10: 8,  # double
        11: 1,  # bool
        12: 1,  # char
        100: 16,  # Quaternion
        101: 12,  # EulerAngles
        102: 8,  # Vec2f
        103: 12,  # Vec3f
    }[datatype]


def parse(buffer: bytes) -> dict:
    """
    Parse a buffer of bytes containing a log entry into a dict containing the 
    key-value pairs of the log.
    """
    result = dict()
    index = 0
    while index < len(buffer):
        # An identifier / key length of zero indicates the end of the log entry
        if buffer[index] == 0x00:
            break
        # The first byte is the length of the identifier
        idLen = int(buffer[index + 0])
        # The second byte is the type ID
        datatype = int(buffer[index + 1])
        # The third and fourth byte are the size of the element
        length = (buffer[index + 2] << 0) | (buffer[index + 3] << 8)
        # The data starts after the four byte header, and on the next word
        # boundary after the identifier
        datastart = 4 + nextWord(idLen)

        # Read the identifier / key as bytes
        identifierbytes = buffer[index + 4: index + 4 + idLen]
        # Convert the identifier / key to a string
        identifier = identifierbytes.decode('utf-8')

        # If the data type is 9 (char), then interpret the data as a string
        if datatype == 12:  # if it's a char array
            absdatastart = index + datastart
            string = buffer[absdatastart:absdatastart + length].decode('utf-8')
            result[identifier] = string
        # If it's a different data type, store the elements in a list
        else:
            # Look up the size of a single element of the array
            elementsize = typeToSize(datatype)
            data = []
            elementindex = index + datastart
            for i in range(length // elementsize):
                # Extract the bytes of the current element
                elementbytes = buffer[elementindex:elementindex + elementsize]
                # Convert them to the correct data type
                element = bytesToType(datatype, elementbytes)
                # Add them to the list
                data.append(element)
                # Increment the index to read the next array element next time
                elementindex += elementsize
            # Save the data in a dict
            result[identifier] = data
        # Increment the index to read the next log element next time
        index += datastart + roundUpToWordSizeMultiple(length)
    return result


# This data was generated by Logger-example.cpp
exampledata = bytes((
    0x02, 0x0A, 0x08, 0x00,
    0xCF, 0x80, 0x00, 0x00,
    0x18, 0x2D, 0x44, 0x54,
    0xFB, 0x21, 0x09, 0x40,
    0x30, 0x05, 0x04, 0x00,
    0x54, 0x68, 0x65, 0x20,
    0x6D, 0x65, 0x61, 0x6E,
    0x69, 0x6E, 0x67, 0x20,
    0x6F, 0x66, 0x20, 0x6C,
    0x69, 0x66, 0x65, 0x2C,
    0x20, 0x74, 0x68, 0x65,
    0x20, 0x75, 0x6E, 0x69,
    0x76, 0x65, 0x72, 0x73,
    0x65, 0x20, 0x61, 0x6E,
    0x64, 0x20, 0x65, 0x76,
    0x65, 0x72, 0x79, 0x74,
    0x68, 0x69, 0x6E, 0x67,
    0x00, 0x00, 0x00, 0x00,
    0x2A, 0x00, 0x00, 0x00,
    0x07, 0x0C, 0x14, 0x00,
    0x6D, 0x65, 0x73, 0x73,
    0x61, 0x67, 0x65, 0x00,
    0x54, 0x68, 0x65, 0x20,
    0x45, 0x41, 0x47, 0x4C,
    0x45, 0x20, 0x68, 0x61,
    0x73, 0x20, 0x6C, 0x61,
    0x6E, 0x64, 0x65, 0x64,
    0x07, 0x0B, 0x01, 0x00,
    0x73, 0x75, 0x63, 0x63,
    0x65, 0x73, 0x73, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0x04, 0x64, 0x10, 0x00,
    0x71, 0x75, 0x61, 0x74,
    0x00, 0x00, 0x00, 0x00,
    0xCE, 0x58, 0x63, 0x3F,
    0xF3, 0x6D, 0xA6, 0x3E,
    0xF3, 0x6D, 0xA6, 0xBE,
    0x00, 0x00, 0x00, 0x00,
    0x05, 0x65, 0x0C, 0x00,
    0x65, 0x75, 0x6C, 0x65,
    0x72, 0x00, 0x00, 0x00,
    0x93, 0x0A, 0x86, 0xBE,
    0x14, 0x90, 0x1D, 0xBF,
    0xDB, 0x0F, 0x49, 0x3F,
    0x05, 0x06, 0x04, 0x00,
    0x73, 0x74, 0x65, 0x61,
    0x6B, 0x00, 0x00, 0x00,
    0xEF, 0xBE, 0xAD, 0xDE,
    0x0B, 0x09, 0x0C, 0x00,
    0x63, 0x6F, 0x6F, 0x72,
    0x64, 0x69, 0x6E, 0x61,
    0x74, 0x65, 0x73, 0x00,
    0xCD, 0xCC, 0xCC, 0x3D,
    0x66, 0x66, 0xA6, 0x40,
    0x9A, 0x99, 0x59, 0x40,
    0x0D, 0x09, 0x10, 0x00,
    0x6D, 0x6F, 0x74, 0x6F,
    0x72, 0x20, 0x6F, 0x75,
    0x74, 0x70, 0x75, 0x74,
    0x73, 0x00, 0x00, 0x00,
    0x29, 0x5C, 0x0F, 0x3F,
    0xCD, 0xCC, 0x0C, 0x3F,
    0x71, 0x3D, 0x0A, 0x3F,
    0x85, 0xEB, 0x11, 0x3F,
    0x06, 0x0A, 0x28, 0x00,
    0x76, 0x65, 0x63, 0x74,
    0x6F, 0x72, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x20,
    0x5F, 0xA0, 0x02, 0x42,
    0x00, 0x00, 0x00, 0xE8,
    0x76, 0x48, 0x37, 0x42,
    0x00, 0x00, 0x00, 0xA2,
    0x94, 0x1A, 0x6D, 0x42,
    0x00, 0x00, 0x40, 0xE5,
    0x9C, 0x30, 0xA2, 0x42,
    0x00, 0x00, 0x90, 0x1E,
    0xC4, 0xBC, 0xD6, 0x42,
    0x08, 0x05, 0x10, 0x00,
    0x69, 0x6E, 0x74, 0x65,
    0x67, 0x65, 0x72, 0x73,
    0x00, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x00, 0x00,
    0x14, 0x00, 0x00, 0x00,
    0x1E, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x00, 0x00,
))

if __name__ == "__main__":
    import pprint
    pprint.pprint(parse(exampledata))
