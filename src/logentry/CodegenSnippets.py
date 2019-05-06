import os, re

lineEnding = "\n"
if os.name == 'nt': # Windows
    print("Windows")
    # lineEnding = "\r\n"


def capitalize_first(s: str) -> str:
    return s[:1].upper() + s[1:]


def settername(name: str):
    return "set" + capitalize_first(name)


def gettername(name: str):
    return "get" + capitalize_first(name)

first_cap_re = re.compile('(.)([A-Z][a-z]+)')
all_cap_re = re.compile('([a-z0-9])([A-Z])')
def pythonname(name: str):
    s1 = first_cap_re.sub(r'\1_\2', name)
    return all_cap_re.sub(r'\1_\2', s1).lower()


def getsizename(name: str):
    return "get" + capitalize_first(name) + "Size"


def setter(name: str, type: str):
    return "    void {settername}({type} {name}) {{ this->{name} = {name}; }}"\
        .format(settername=settername(name), type=type, name=name)


def setter_array(name: str, type: str, length: int, typedefs: set):
    td = make_typedef(type, length, ref=True, const=True)
    typedefs.add(td)
    const_ref_type = td[0]
    return """    void {settername}({type} {name}) {{
        std::copy(std::begin({name}),
              std::end({name}), 
              std::begin(this->{name}));
    }}""".format(settername=settername(name), type=const_ref_type, name=name)


def c_setter_prototype(name: str, type: str):
    return "void {settername}(LogEntry *logEntry, {type} {name});" \
        .format(settername=settername(name), type=type, name=name)


def c_setter(name: str, type: str):
    return "void {settername}(LogEntry *logEntry, {type} {name}) {{ logEntry->{name} = {name}; }}" \
        .format(settername=settername(name), type=type, name=name)


def c_setter_array_prototype(name: str, type: str):
    return "void {settername}(LogEntry *logEntry, const {type} *{name});" \
        .format(settername=settername(name), type=type, name=name)


def c_setter_array(name: str, type: str, length: int):
    return """void {settername}(LogEntry *logEntry, const {type} *{name}) {{ 
    memcpy(logEntry->{name}, {name}, {length} * sizeof({type})); 
}}""" .format(settername=settername(name), type=type, name=name, length=length)


def getter(name: str, type: str):
    return "    {type} {gettername}() const {{ return this->{name}; }}" \
        .format(gettername=gettername(name), type=type, name=name)


def getter_array(name: str, type: str, length: int, typedefs: set):
    td = make_typedef(type, length, ref=True, const=True)
    typedefs.add(td)
    const_ref_type = td[0]
    return "    {type} {gettername}() const {{ return this->{name}; }}" \
        .format(gettername=gettername(name), type=const_ref_type, name=name)


def c_getter_prototype(name: str, type: str):
    return "{type} {gettername}(const LogEntry *logEntry);" \
        .format(gettername=gettername(name), type=type)


def c_getter(name: str, type: str):
    return "{type} {gettername}(const LogEntry *logEntry) {{ return logEntry->{name}; }}" \
        .format(gettername=gettername(name), type=type, name=name)


def c_getter_array_prototype(name: str, type: str):
    return "const {type} *{gettername}(const LogEntry *logEntry);" \
        .format(gettername=gettername(name), type=type)


def c_getter_array(name: str, type: str):
    return "const {type} *{gettername}(const LogEntry *logEntry) {{ return logEntry->{name}; }}" \
        .format(gettername=gettername(name), type=type, name=name)


def c_get_array_length_prototype(name: str):
    return "size_t {getsizename}(void);" \
        .format(getsizename=getsizename(name))


def c_get_array_length(name: str):
    return "size_t {getsizename}() {{ return sizeof(((LogEntry *)0)->{name}) / sizeof(*((LogEntry *)0)->{name}); }}" \
        .format(getsizename=getsizename(name), name=name)


def c_assignment(name: str, c_assign: str):
    return "    " + settername(name) + "(logEntry, " + c_assign + ");"


def cpp_assignment(name: str, cpp_assign: str):
    return "    " + "logEntry." + settername(name) + "(" + cpp_assign + ");"


c_python_types = {"float": "PyFloat", "uint32_t": "PyLong", "uint64_t": "PyLong", "int32_t": "PyLong"}
to_c_python_types = {"float": "PyFloat_FromDouble", "uint32_t": "PyLong_FromUnsignedLong", "uint64_t": "PyLong_FromUnsignedLongLong", "int32_t": "PyLong_FromLong"}
from_c_python_types = {"float": "PyFloat_AsDouble", "uint32_t": "PyLong_AsUnsignedLong", "uint64_t": "PyLong_AsUnsignedLongLong", "int32_t": "PyLong_AsLong"}


def c_python_getter(name: str, type: str):
    return """static PyObject *DroneLogger_{gettername}(PythonDroneLogEntry *self, void *closure) {{
    return {to_c_python_type}({gettername}(&(self->logEntry)));
}}""".format(to_c_python_type=to_c_python_types[type], gettername=gettername(name))


def c_python_setter(name: str, type: str):
    return """static int DroneLogger_{settername}(PythonDroneLogEntry *self, PyObject *ppy, void *closure) {{
    if (!{c_python_type}_Check(ppy)) {{
        PyErr_SetString(PyExc_TypeError, "Error: {c_python_type} expected");
        return -1;
    }}
    {type} val = ({type}) {from_c_python_type}(ppy);
    if (PyErr_Occurred())
        return -1;
    {settername}(&(self->logEntry), val); 
    return 0;
}}""".format(settername=settername(name), c_python_type=c_python_types[type], type=type, from_c_python_type=from_c_python_types[type])


def c_python_getter_array(name: str, type: str, length: int):
    return """static PyObject *DroneLogger_{gettername}(PythonDroneLogEntry *self, void *closure) {{
    const Py_ssize_t length = {length};
    PyObject *tup = PyTuple_New(length);
    if (!tup) {{
        PyErr_SetString(PyExc_RuntimeError, "Error creating tuple");
        return NULL;
    }}
    const {type} *p = {gettername}(&self->logEntry);
    Py_ssize_t i;
    for (i = 0; i < length; ++i) {{
        PyObject *ppy = {to_c_python_type}(p[i]);
        if (!ppy) {{
            Py_DECREF(tup);
            return NULL;
        }}
        if (PyTuple_SetItem(tup, i, ppy)) {{
            PyErr_SetString(PyExc_RuntimeError, "Error adding {c_python_type} to tuple");
            Py_DECREF(tup);
            return NULL;
        }}
    }}
    return tup;
}}""".format(gettername=gettername(name), length=length, type=type, to_c_python_type=to_c_python_types[type], c_python_type=c_python_types[type])


def c_python_setter_array(name: str, type: str, length: int):
    return """static int DroneLogger_{settername}(PythonDroneLogEntry *self, PyObject *tup, void *closure) {{
    if (!PyTuple_Check(tup)) {{
        PyErr_SetString(PyExc_TypeError, "Error: tuple of {c_python_type} expected");
        return -1;
    }}
    const Py_ssize_t length = {length};
    const Py_ssize_t tupLength = PyTuple_Size(tup);
    if (tupLength != length) {{
        PyErr_SetString(PyExc_TypeError, "Error: number of elements is incorrect");
        return -1;
    }}
    {type} buffer[{length}];
    Py_ssize_t i;
    for (i = 0; i < length; ++i) {{
        PyObject *el = PyTuple_GetItem(tup, i); // borrowed reference
        if (!{c_python_type}_Check(el)) {{
            PyErr_SetString(PyExc_TypeError, "Error: element is not a {c_python_type}");
            return -1;
        }}
        buffer[i] = ({type}) {from_c_python_type}(el);
        if (PyErr_Occurred())
            return -1;
    }}
    {settername}(&self->logEntry, buffer);
    return 0;
}}""".format(settername=settername(name), c_python_type=c_python_types[type], length=length, type=type, from_c_python_type=from_c_python_types[type])

def c_python_getset(name: str, doc: str = None):
    pname = pythonname(name)
    if doc is None:
        doc = name
    return "    {{\"{pythonname}\", (getter) DroneLogger_{gettername}, (setter) DroneLogger_{settername}, \"{doc}\"}}," \
        .format(pythonname=pname, gettername=gettername(name), settername=settername(name), doc=doc)


def make_typedef(type, length, ref=True, const=True):
    name = type
    if length > 1:
        name += "_" + str(length)
    if const:
        name += "_const"
    if ref:
        name += "_ref"

    typedef = "using " + name + " = "
    if const:
        typedef += "const "
    typedef += type
    if ref:
        if length > 1:
            typedef += "(&)[" + str(length) + "]"
        else:
            typedef += "&"
    else:
        if length > 1:
            typedef += "[" + str(length) + "]"
        else:
            pass
    typedef += ";"
    return (name, typedef)


def make_type_def_data(data: dict, ref=True, const=True):
    return make_typedef(data['name'], data['length'], ref=ref, const=const)