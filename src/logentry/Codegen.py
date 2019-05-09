#!/usr/bin/env python3

import os
from os.path import join
import re
import sys
from CodegenSnippets import *

# Check the alignment of u64 entries!

datamembers = [
    # Logging
    ('u', "size", "$size"),
    ('u', "mode", "getFlightMode()"),
    ('u64', "frametime", "getMillis()"),
    ('u', "framecounter", "getTickCount()"),
    # Configuration
    ('i', "droneConfig", "configManager.getControllerConfiguration()"),

    # RC controls
    ('f', "rcTuning", "getTuner()"),
    ('f', "rcThrottle", "getThrottle()"),
    ('f', "rcRoll", "getRoll()"),
    ('f', "rcPitch", "getPitch()"),
    ('f', "rcYaw", "getYaw()"),

    # Reference 
    ('f', "referenceOrientation", 4, "toCppArray(attitudeController.getReferenceQuat())"),
    ('f', "referenceOrientationEuler", 3, "toCppArray(attitudeController.getReferenceEuler())"),
    ('f', "__pad0", "{0}"),
    ('f', "referenceHeight", "altitudeController.getReferenceHeight()"),
    ('f', "referenceLocation", 2, "toCppArray(position.getReferencePosition())"),

    # Measurements
    ('f', "measurementOrientation", 4 ,"getAHRSQuat()"),
    ('f', "measurementAngularVelocity", 3 ,"getGyroMeasurement()"),
    ('f', "measurementHeight", "getCorrectedSonarMeasurement()"),
    ('f', "measurementLocation", 2, "getCorrectedPosition()"),

    # Observers
    ('f', "attitudeObserverState", 10 ,"toCppArray(attitude.getState())"),
    ('f', "altitudeObserverState", 3, "toCppArray(altitude.getState())"),
    ('f', "navigationObserverState", 6, "toCppArray(position.getState())"),
    ('f', "attitudeYawOffset", "getYawJump()"),

    # Controller outputs
    ('f', "attitudeControlSignals", 3, "toCppArray(attitude.getControl())"),
    ('f', "altitudeControlSignal", "altitude.getControl()"),
    ('f', "positionControlSignal", 2, "position.getControl()"),
    ('f', "motorControlSignals", 4, "{} /* TODO */"), # TODO

    # Thrust
    ('f', "commonThrust", "{} /* TODO */"), # TODO
    ('f', "hoverThrust", "inputBias.getThrustBias()"), # TODO
]

if len(sys.argv) > 1:
    inputdir = sys.argv[1]
else:
    abspath = os.path.abspath(__file__)
    inputdir = os.path.dirname(abspath)

members = ""
cpp_methods = ""
c_functions = ""
c_assignments = ""
cpp_assignments = ""
c_prototypes = ""
c_python_functions = ""
c_python_getsets = ""

size = 0

typedefs = set()

types = {'u': "uint32_t", 'u64': "uint64_t", 'i': "int32_t", 'f': "float"}
sizes = {'u': 1, 'u64': 2, 'i': 1, 'f': 1}

for datamember in datamembers:
    type = types[datamember[0]]
    name = datamember[1]
    if size % sizes[datamember[0]] != 0:
        raise RuntimeError("""Alignment of `{} {}` is incorrect 
(should be aligned to {} bytes)""".format(type, name, sizes[datamember[0]] * 4))

    if len(datamember) == 3:  # no lenght → value type
        c_assign = datamember[2]
        cpp_methods += setter(name, type) + lineEnding
        cpp_methods += getter(name, type) + lineEnding
        c_functions += c_setter(name, type) + lineEnding
        c_functions += c_getter(name, type) + lineEnding
        c_prototypes += c_getter_prototype(name, type) + lineEnding
        c_prototypes += c_setter_prototype(name, type) + lineEnding
        # c_assignments += c_assignment(name, c_assign) + lineEnding
        cpp_assignments += cpp_assignment(name, c_assign) + lineEnding
        c_python_functions += c_python_getter(name, type) + 2 * lineEnding
        c_python_functions += c_python_setter(name, type) + 2 * lineEnding
        c_python_getsets += c_python_getset(name) + lineEnding
        members += "    " + type + ' ' + name + ";" + lineEnding
        size += sizes[datamember[0]]

    elif len(datamember) == 4:  # length → array type
        length = datamember[2]
        c_assign = datamember[3]
        cpp_methods += setter_array(name, type, length, typedefs) + lineEnding
        cpp_methods += getter_array(name, type, length, typedefs) + lineEnding
        c_functions += c_setter_array(name, type, length) + lineEnding
        c_functions += c_getter_array(name, type) + lineEnding
        c_functions += c_get_array_length(name) + lineEnding
        c_prototypes += c_getter_array_prototype(name, type) + lineEnding
        c_prototypes += c_setter_array_prototype(name, type) + lineEnding
        c_prototypes += c_get_array_length_prototype(name) + lineEnding
        # c_assignments += c_assignment(name, c_assign) + lineEnding
        cpp_assignments += cpp_assignment(name, c_assign) + lineEnding
        c_python_functions += c_python_getter_array(name, type, length) + 2 * lineEnding
        c_python_functions += c_python_setter_array(name, type, length) + 2 * lineEnding
        c_python_getsets += c_python_getset(name) + lineEnding
        members += "    " + type + ' ' + name + '[' + str(length) + "];" + lineEnding
        size += length * sizes[datamember[0]]


typedefs = map(lambda t: t[1], typedefs)
typedefs = lineEnding.join(typedefs)


if not os.path.exists('src-generated'):
    os.makedirs('src-generated')
if not os.path.exists('include-generated'):
    os.makedirs('include-generated')

template_dir = join(inputdir, 'templates')
with \
    open(join(template_dir, 'LogEntry.template.h'), 'r') as template_header_file, \
    open(join(template_dir, 'LogEntry.template.cpp'), 'r') as template_cpp_file, \
    open(join(template_dir, 'logger.drone.template.cpp'), 'r') as template_drone_cpp_file, \
    open(join(template_dir, 'DroneLogger.py.template.c'), 'r') as template_c_python_file, \
    open('include-generated/LogEntry.h', 'w') as header_file, \
    open('src-generated/LogEntry.cpp', 'w') as cpp_file, \
    open('src-generated/logger.drone.cpp', 'w') as drone_cpp_file, \
    open('src-generated/DroneLogger.py.c', 'w') as c_python_file:

    template_header = template_header_file.read()
    template_cpp = template_cpp_file.read()
    template_drone_cpp = template_drone_cpp_file.read()
    template_c_python = template_c_python_file.read()

    template_header = template_header.replace('$typedefs', typedefs)
    template_header = template_header.replace('$members', members)
    template_header = template_header.replace('$cpp_methods', cpp_methods)
    template_header = template_header.replace('$c_prototypes', c_prototypes)
    template_header = template_header.replace('$size', str(size))

    header_file.write(template_header)

    template_cpp = template_cpp.replace('$c_functions', c_functions)

    cpp_file.write(template_cpp)

    # c_assignments = c_assignments.replace('$size', str(size))
    cpp_assignments = cpp_assignments.replace('$size', str(size))
    template_drone_cpp = template_drone_cpp.replace('$cpp_assignments', cpp_assignments)

    drone_cpp_file.write(template_drone_cpp)

    template_c_python = template_c_python.replace('$c_python_functions', c_python_functions)
    template_c_python = template_c_python.replace('$c_python_getsets', c_python_getsets)

    c_python_file.write(template_c_python)
