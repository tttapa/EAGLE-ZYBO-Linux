#!/usr/bin/env python3

import textwrap
import json
from collections import OrderedDict
from typing import Union, List, Tuple
import os

dir_path = os.path.dirname(os.path.realpath(__file__))

endl = '\n'


def json_text_list_to_str(strings: Union[str, List[str]]) -> str:
    if isinstance(strings, list):
        strings = " ".join(strings)
    return strings.replace('"', '\\"')

# region C++ Header File .......................................................


def generate_documentation(documentation: Union[str, List[str]],
                           indent: int = 0, linewidth: int = 80) -> str:
    documentation = json_text_list_to_str(documentation)
    indentation = indent * " "
    prefix = endl + indentation + " * "
    width = linewidth - 3 - indent
    doc = prefix.join(textwrap.wrap(documentation,
                                    width=width,
                                    replace_whitespace=True))
    return indentation + "/**" + prefix + doc + endl + indentation + " */"


def generate_arguments(members: OrderedDict) -> str:
    arguments = map(lambda kv: kv[1]['type']+" "+kv[0], members.items())
    return ", ".join(arguments)


def generate_initializers(members: OrderedDict) -> str:
    initializers = map(lambda kv: kv[0]+"{"+kv[0]+"}", members.items())
    return ", ".join(initializers)


def generate_constructors(name: str, struct: OrderedDict) -> str:
    return """\
    {name}() = default;
    {name}({arguments}) : {initializers} {{}}
    """.format(name=name,
               arguments=generate_arguments(struct['members']),
               initializers=generate_initializers(struct['members']))


def generate_member(name: str, member: OrderedDict) -> str:
    documentation = generate_documentation(member['documentation'], 4)
    return """{documentation}
    {type} {name} = {default};""".format(name=name,
                                         type=member['type'],
                                         default=member['default'],
                                         documentation=documentation)


def generate_members(members: dict) -> str:
    initializers = map(lambda kv: generate_member(*kv), members.items())
    return (endl * 2).join(initializers)


def generate_struct(name: str, struct: OrderedDict) -> str:
    return """\
// This is an automatically generated struct, edit it in the code generator
{documentation}
struct __attribute__((packed)) {name} {{
{constructors}
{members}
}};
""".format(name=name,
           documentation=generate_documentation(struct['documentation']),
           constructors=generate_constructors(name, struct),
           members=generate_members(struct['members']))


def generate_structs(structs: OrderedDict) -> str:
    structs_code = map(lambda kv: generate_struct(*kv), structs.items())
    return endl.join(structs_code)


def generate_enum_value(name: str, value: OrderedDict) -> str:
    documentation = generate_documentation(value['documentation'], 4)
    return """{documentation}
    {name} = {value},""".format(name=name,
                                value=value['value'],
                                documentation=documentation)


def generate_enum_values(values: dict) -> str:
    initializers = map(lambda kv: generate_enum_value(*kv), values.items())
    return (endl * 2).join(initializers)


def generate_enum(name: str, enum: OrderedDict) -> str:
    return """\
// This is an automatically generated enum, edit it in the code generator
{documentation}
enum class {name} : int32_t {{
{values}
}};
""".format(name=name,
           documentation=generate_documentation(enum['documentation']),
           values=generate_enum_values(enum['values']))


def generate_enums(enums: OrderedDict) -> str:
    enums_code = map(lambda kv: generate_enum(*kv), enums.items())
    return endl.join(enums_code)


def generate_struct_header(data: OrderedDict) -> str:
    return """\
#pragma once

#include <cstdint>
#include <Quaternion.hpp>
#include <real_t.h>

{enums}
{structs}\
""".format(structs=generate_structs(data['structs']),
           enums=generate_enums(data['enums']))

# endregion

# region LogEntry ..............................................................


def get_optional_documentation_string(member: OrderedDict) -> str:
    if 'documentation' in member:
        return json_text_list_to_str(member['documentation'])
    else:
        return ""


def generate_optional_documentation(member: OrderedDict) -> str:
    if 'documentation' in member:
        documentation = \
            generate_documentation(member['documentation'], 4) + endl
    else:
        documentation = ""
    return documentation


def generate_log_entry_struct_member(type: str, name: str,
                                     member: OrderedDict) -> str:
    return """{documentation}\
    {type} {name};"""\
        .format(name=name,
                type=type,
                documentation=generate_optional_documentation(member))


def generate_log_entry_struct_members(name: str, member: OrderedDict) -> str:
    if (not 'logger' in member) or len(member['logger']) == 0:
        return ""
    members = map(
        lambda kv: generate_log_entry_struct_member(name, kv[0], kv[1]),
        member['logger'].items())
    return endl.join(members)


def generate_log_entry_other_member(name: str, member: OrderedDict) -> str:
    return """{documentation}\
    {type} {name};"""\
        .format(name=name,
                type=member['type'],
                documentation=generate_optional_documentation(member))


def generate_log_entry_members(data: OrderedDict) -> str:
    struct_members = map(lambda kv: generate_log_entry_struct_members(*kv),
                         data['structs'].items())
    enum_members = map(lambda kv: generate_log_entry_struct_members(*kv),
                       data['enums'].items())
    other_members = map(lambda kv: generate_log_entry_other_member(*kv),
                        data['others'].items())
    return endl.join(struct_members) + endl \
        + endl.join(enum_members) + endl \
        + endl.join(other_members)


def generate_log_entry_header(data: OrderedDict) -> str:
    return """\
#pragma once

#include <LoggerStructs.hpp>

/**
 * This is the struct for logging data between the ZYBO Baremetal ANC controller
 * and the computer.
 *
 * @note    For simplicity, we'll assume that the byte order and padding of both
 *          machines is the same.
 */
struct __attribute__((packed)) LogEntry {{
  // private:
  //   size_t size;
  //
  // public:
  //   LogEntry() : size(sizeof(LogEntry)) {{}}

{members}
}};
""".format(members=generate_log_entry_members(data))

# endregion

# region Python Struct Bindings ................................................


def generate_python_member_binding(classname: str, membername: str,
                                   member: OrderedDict) -> str:
    return ".def_readwrite(\"{name}\", &{classname}::{name}, \"{doc}\")"\
        .format(name=membername,
                classname=classname,
                doc=json_text_list_to_str(member['documentation']))


def generate_python_member_bindings(name: str, members: OrderedDict) -> str:
    bindings = map(lambda kv: generate_python_member_binding(
        name, kv[0], kv[1]), members.items())
    return (endl + " " * 8).join(bindings)


def generate_constructor_types(members: OrderedDict) -> str:
    arguments = map(lambda m: m['type'], members.values())
    return ", ".join(arguments)


def generate_python_constructors(name: str, members: OrderedDict) -> str:
    return """.def(pybind11::init<>())
        .def(pybind11::init<{constructor_types}>())\
""".format(constructor_types=generate_constructor_types(members))


def generate_python_binding(name: str, members: OrderedDict) -> str:
    return """\
    pybind11::class_<{name}>(py_log_module, "{name}")
        {constructor_bindings}
        {member_bindings};\
""".format(name=name,
           constructor_bindings=generate_python_constructors(name, members),
           member_bindings=generate_python_member_bindings(name, members))


def generate_python_bindings(structs: OrderedDict) -> str:
    bindings = map(
        lambda kv: generate_python_binding(kv[0], kv[1]['members']),
        structs.items())
    return (endl * 2).join(bindings)


def generate_python_enum_value_binding(enumname: str, valuename: str,
                                       value: OrderedDict) -> str:
    return ".value(\"{name}\", {enumname}::{name}, \"{doc}\")"\
        .format(name=valuename,
                enumname=enumname,
                doc=json_text_list_to_str(value['documentation']))


def generate_python_enum_value_bindings(name: str, values: OrderedDict) -> str:
    bindings = map(lambda kv: generate_python_enum_value_binding(
        name, kv[0], kv[1]), values.items())
    return (endl + " " * 8).join(bindings)


def generate_python_enum_binding(name: str, values: OrderedDict) -> str:
    return """\
    pybind11::enum_<{name}>(py_log_module, "{name}", pybind11::arithmetic())
        {value_bindings};\
""".format(name=name,
           value_bindings=generate_python_enum_value_bindings(name, values))


def generate_python_enum_bindings(enums: OrderedDict) -> str:
    bindings = map(
        lambda kv: generate_python_enum_binding(kv[0], kv[1]['values']),
        enums.items())
    return (endl * 2).join(bindings)


def generate_python_log_entry_member_binding(name: str,
                                             member: OrderedDict) -> str:
    return """\
        .def_readwrite("{name}", &LogEntry::{name}, "{documentation}")"""\
        .format(name=name,
                documentation=get_optional_documentation_string(member))


def generate_python_log_entry_struct_bindings(member: OrderedDict) -> str:
    if (not 'logger' in member) or len(member['logger']) == 0:
        return ""
    members = map(
        lambda kv: generate_python_log_entry_member_binding(kv[0], kv[1]),
        member['logger'].items())
    return endl.join(members)


def generate_python_log_entry_member_bindings(data: OrderedDict) -> str:
    struct_members = map(
        lambda kv: generate_python_log_entry_struct_bindings(kv[1]),
        data['structs'].items())
    enum_members = map(
        lambda kv: generate_python_log_entry_struct_bindings(kv[1]),
        data['enums'].items())
    other_members = map(lambda kv: generate_python_log_entry_member_binding(*kv),
                        data['others'].items())
    return endl.join(struct_members) + endl \
        + endl.join(enum_members) + endl \
        + endl.join(other_members) + endl


def generate_python_log_entry_binding(data: OrderedDict) -> str:
    return """\
    pybind11::class_<LogEntry>(py_log_module, "LogEntry")
        .def(pybind11::init<>())
        .def("__init__",
             [](LogEntry &self, pybind11::bytes bytes) {{
                 const std::string v = bytes;
                 size_t size         = pybind11::len(bytes);
                 if (size != sizeof(LogEntry))
                     throw std::length_error(
                         std::string(
                             "The length of the bytes is not the same as the "
                             "size of the object (len(bytes) = ") +
                         std::to_string(size) + ", sizeof(T) = " +
                         std::to_string(sizeof(LogEntry)) + ")");
                 memcpy(&self, v.c_str(), sizeof(LogEntry));
             }})
        .def("__bytes__",
             [](const LogEntry &self) {{
                 const std::string str = {{
                     (const uint8_t *) &self, 
                     (const uint8_t *) &self + sizeof(self), 
                 }};
                 return pybind11::bytes(str);
             }})
{member_bindings};\
""".format(member_bindings=generate_python_log_entry_member_bindings(data))


def generate_python_module(data: OrderedDict) -> str:
    return """\
#include <LogEntry.hpp>
#include <pybind11/pybind11.h>
#include <PyMatrix.hpp>

PYBIND11_MODULE(DroneLogger, py_log_module) {{
        pybind11::module::import("PyQuaternion");

{struct_bindings}

{enum_bindings}

{log_entry_bindings}
}}""".format(struct_bindings=generate_python_bindings(data['structs']),
             enum_bindings=generate_python_enum_bindings(data['enums']),
             log_entry_bindings=generate_python_log_entry_binding(data))

# endregion

# region GetLogData for Baremetal ..............................................


def generate_getlogdata_assignment(name: str, entry: OrderedDict) -> str:
    return """\
    logentry.{name} = {value};""".format(name=name, value=entry['value'])


def generate_getlogdata_assignments(logger: OrderedDict):
    assignments = map(lambda kv: generate_getlogdata_assignment(*kv),
                      filter(lambda kv: 'value' in kv[1], logger.items()))
    return endl.join(assignments)


def generate_getlogdata_all_assignments(data: OrderedDict) -> str:
    struct_assignments = map(
        lambda struct: generate_getlogdata_assignments(struct['logger']),
        filter(lambda struct: 'logger' in struct and len(struct['logger']) > 0,
               data['structs'].values()))
    other_assignments = generate_getlogdata_assignments(data['others'])
    return endl.join(struct_assignments) + endl + other_assignments


def generate_getlogdata_includes(includes: List[str]) -> str:
    includes = map(lambda inc: "#include <" + inc + ">", includes)
    return endl.join(includes)


def generate_getlogdata(data: OrderedDict) -> str:
    return """\
#include <GetLogData.hpp>

{includes}

LogEntry getLogData() {{
    LogEntry logentry;
{assignments}
    return logentry;
}}""".format(includes=generate_getlogdata_includes(data['includes']),
             assignments=generate_getlogdata_all_assignments(data))

# endregion


for folder in ["src-generated", "include-generated"]:
    if not os.path.exists(folder):
        os.makedirs(folder)

json_file_path = os.path.join(dir_path, "Codegen.json")
with open(json_file_path, 'r') as json_file, \
    open("src-generated/DroneLogEntry.Python.cpp", 'w') \
        as pythonmodulefile, \
    open("include-generated/LoggerStructs.hpp", 'w') \
        as structsheaderfile, \
    open("include-generated/LogEntry.hpp", 'w') \
        as logentryheaderfile, \
    open("src-generated/GetLogData.cpp", 'w') \
        as getlogdatafile:
    data = json.load(json_file, object_pairs_hook=OrderedDict)

    structsheaderfile.write(generate_struct_header(data))
    pythonmodulefile.write(generate_python_module(data))
    logentryheaderfile.write(generate_log_entry_header(data))
    getlogdatafile.write(generate_getlogdata(data))
