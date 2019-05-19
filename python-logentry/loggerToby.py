import struct

def loggerToby(b):
    return {"flightMode" : struct.unpack('i', b[305:309])[0], 
            "destination" : struct.unpack('ff', b[160:168]) + struct.unpack('f', b[12:16]), 
            "currentPosition" : struct.unpack('ff', b[176:184]) + struct.unpack('f', b[20:24]),
            "currentOrientation" : struct.unpack('ffff', b[96:112]),
            "thrust" : struct.unpack('f', b[245:249])[0],
            "millis" : struct.unpack('Q', b[313:321])[0]}