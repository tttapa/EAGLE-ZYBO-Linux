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
    return strings

# region C++ Header File ........................................................


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
    initializers = map(lambda kv: generate_member(
        kv[0], kv[1]), members.items())
    return (endl * 2).join(initializers)


def generate_struct(name: str, struct: OrderedDict) -> str:
    return """\
// This is an automatically generated struct, edit it in the code generator
{documentation}
struct {name} {{
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


def generate_struct_header(structs: OrderedDict) -> str:
    return """\
#pragma once

#include <Quaternion.hpp>

using Vec2f = TColVector<float, 2>;
using Vec3f = TColVector<float, 3>;

{structs}\
""".format(structs=generate_structs(structs))

# endregion

# region LogEntry ..............................................................


def get_optional_documentation_string(member: OrderedDict) -> str:
    if 'documentation' in member:
        return member['documentation']
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
    other_members = map(lambda kv: generate_log_entry_other_member(*kv),
                        data['others'].items())
    return endl.join(struct_members) + endl + endl.join(other_members)


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
struct LogEntry {{
  private:
    size_t size;

  public:
    LogEntry() : size(sizeof(LogEntry)) {{}}

{members}
}};
""".format(members=generate_log_entry_members(data))

# endregion

# region Python Struct Bindings .................................................


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
    bindings = map(lambda kv: generate_python_binding(
        kv[0], kv[1]['members']), structs.items())
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
    other_members = map(lambda kv: generate_python_log_entry_member_binding(*kv),
                        data['others'].items())
    return endl.join(struct_members) + endl + endl.join(other_members)


def generate_python_log_entry_binding(data: OrderedDict) -> str:
    return """\
    pybind11::class_<LogEntry>(py_log_module, "LogEntry")
        .def(pybind11::init<>())
{member_bindings};\
""".format(member_bindings=generate_python_log_entry_member_bindings(data))


def generate_python_module(data: OrderedDict) -> str:
    return """\
#include <LogEntry.hpp>
#include <pybind11/pybind11.h>
#include <PyMatrix.hpp>

PYBIND11_MODULE(DroneLogger, py_log_module) {{
{struct_bindings}

{log_entry_bindings}
}}""".format(struct_bindings=generate_python_bindings(data['structs']),
             log_entry_bindings=generate_python_log_entry_binding(data))

# endregion


json_file_path = os.path.join(dir_path, "Codegen.json")
with open(json_file_path, 'r') as json_file, \
    open("src-generated/DroneLogEntry.Python.cpp", 'w') \
        as pythonmodulefile, \
    open("include-generated/LoggerStructs.hpp", 'w') \
        as structsheaderfile, \
    open("include-generated/LogEntry.hpp", 'w') \
        as logentryheaderfile:
    data = json.load(json_file, object_pairs_hook=OrderedDict)

    structsheaderfile.write(generate_struct_header(data['structs']))
    pythonmodulefile.write(generate_python_module(data))
    logentryheaderfile.write(generate_log_entry_header(data))
