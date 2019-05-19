import struct

def loggerToby(b, dict):
    for x in dict:
        if x == "flightMode":
            dict[x] = struct.unpack('i', b[305:309])[0]
        if x == "destination":
            dict[x] = struct.unpack('ff', b[160:168]) + struct.unpack('f', b[12:16])
        if x == "currentPosition":
            dict[x] = struct.unpack('ff', b[176:184]) + struct.unpack('f', b[20:24])
        if x == "currentOrientation":
            dict[x] = struct.unpack('ffff', b[96:112])
        if x == "thrust":
            dict[x] = struct.unpack('f', b[245:249])[0]
