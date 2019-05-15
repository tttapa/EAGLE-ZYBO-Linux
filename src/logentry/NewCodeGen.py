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


def generate_python_binding(name: str, members: OrderedDict) -> str:
    return """\
    pybind11::class_<{name}>(module, "{name}")
        {member_bindings};\
""".format(name=name,
           member_bindings=generate_python_member_bindings(name, members))


def generate_python_bindings(structs: OrderedDict) -> str:
    bindings = map(lambda kv: generate_python_binding(
        kv[0], kv[1]['members']), structs.items())
    return endl.join(bindings)


def generate_python_module(structs: OrderedDict) -> str:
    return """\
PYBIND11_MODULE(module, "DroneLogger") {{
{bindings}
}}""".format(bindings=generate_python_bindings(structs))


json_file_path = os.path.join(dir_path, "Codegen.json")
with open(json_file_path, 'r') as f:
    data = json.load(f, object_pairs_hook=OrderedDict)
    for name, struct in data['structs'].items():
        print(generate_struct(name, struct))
    print(generate_python_module(data['structs']))
