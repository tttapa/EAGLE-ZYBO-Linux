#!/usr/bin/env python3

import textwrap
import json
from collections import OrderedDict
from typing import Union, List

endl = '\n'


def generate_documentation(documentation: Union[str, List[str]],
                           indent: int = 0, linewidth: int = 80):
    if isinstance(documentation, list):
        documentation = " ".join(documentation)
    indentation = indent * " "
    prefix = endl + indentation + " * "
    width = linewidth - 3 - indent
    doc = prefix.join(textwrap.wrap(documentation,
                                    width=width,
                                    replace_whitespace=True))
    return indentation + "/**" + prefix + doc + endl + indentation + " */"


def generate_arguments(members: dict):
    arguments = map(lambda kv: kv[1]['type']+" "+kv[0], members.items())
    return ", ".join(arguments)


def generate_initializers(members: dict):
    initializers = map(lambda kv: kv[0]+"{"+kv[0]+"}", members.items())
    return ", ".join(initializers)


def generate_constructors(name: str, struct: dict):
    return """\
    {name}() = default;
    {name}({arguments}) : {initializers} {{}}
    """.format(name=name,
               arguments=generate_arguments(struct['members']),
               initializers=generate_initializers(struct['members']))


def generate_member(name: str, member: dict):
    documentation = generate_documentation(member['documentation'], 4)
    return """{documentation}
    {type} {name} = {default};""".format(name=name,
                                         type=member['type'],
                                         default=member['default'],
                                         documentation=documentation)


def generate_members(members: dict):
    initializers = map(lambda kv: generate_member(
        kv[0], kv[1]), members.items())
    return (endl * 2).join(initializers)


def generate_struct(name: str, struct: dict):
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


def generate_python_bindings():
    pass


with open("Codegen.json", 'r') as f:
    data = json.load(f, object_pairs_hook=OrderedDict)
    for name, struct in data['structs'].items():
        print(generate_struct(name, struct))
