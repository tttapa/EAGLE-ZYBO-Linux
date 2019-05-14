#include <Python.h>
#include <LogEntry.h>
#include <stdbool.h>

// Python type declaration
static PyTypeObject PythonDroneLogEntryType;

// C Type definition
typedef struct {
    PyObject_HEAD //
    LogEntry logEntry;
} PythonDroneLogEntry; 

$c_python_functions
static int 
DroneLogger_init(PythonDroneLogEntry *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"raw", NULL};
    PyObject *bytes = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", 
                                     kwlist, &PyBytes_Type, &bytes))
        return -1;
    if (!bytes) {// optional bytes argument
        memset(&self->logEntry, 0, sizeof(self->logEntry));
        return 0;
    }
    Py_ssize_t bytesSize = PyBytes_Size(bytes);
    if (bytesSize != sizeof(self->logEntry)) {
        PyErr_SetString(PyExc_IndexError, 
            "Error: bytes size does not match log entry size");
        return -1;
    }
    const char *bytesData = PyBytes_AsString(bytes);
    if (!bytesData) {
        PyErr_SetString(PyExc_IndexError, "Error: bytesData == NULL");
        return -1;
    }
    memcpy(&self->logEntry, bytesData, sizeof(self->logEntry));
    return 0;
}

static PyObject *
DroneLogger__bytes__(PythonDroneLogEntry *self, PyObject *Py_UNUSED(ignored)) {
    return PyBytes_FromStringAndSize((char *) &self->logEntry, 
                                     sizeof(self->logEntry));
}

#ifdef DRONELOGGER_RICH_COMPARE
static PyObject *
DroneLogger_richcmp(PythonDroneLogEntry *self, PyObject *obj2, int op) {
    if (!PyObject_TypeCheck(self, &PythonDroneLogEntryType)) {
        PyErr_SetString(PyExc_TypeError, 
            "Error: first argument is not derived from DroneLogger.LogEntry");
        return NULL;
    }
    if (!PyObject_TypeCheck(obj2, &PythonDroneLogEntryType)) {
        PyErr_SetString(PyExc_TypeError, 
            "Error: second argument is not derived from DroneLogger.LogEntry");
        return NULL;
    }

    PythonDroneLogEntry *l1 = self;
    PythonDroneLogEntry *l2 = obj2;

    float timestamp1 = l1->logEntry.frametime;
    float timestamp2 = l2->logEntry.frametime;

    bool c = 0;
    switch (op) {
        case Py_LT: c = timestamp1 < timestamp2; break;
        case Py_LE: c = timestamp1 <= timestamp2; break;
        case Py_EQ: c = timestamp1 == timestamp2; break;
        case Py_NE: c = timestamp1 != timestamp2; break;
        case Py_GT: c = timestamp1 > timestamp2; break;
        case Py_GE: c = timestamp1 >= timestamp2; break;
    }
    PyObject *result = c ? Py_True : Py_False;
    Py_INCREF(result);
    return result;
}
#endif // DRONELOGGER_RICH_COMPARE

// Python Method definitions
static PyMethodDef DroneLogger_methods[] = {
    {"__bytes__", (PyCFunction) DroneLogger__bytes__, METH_NOARGS,
     "Convert the log entry to its underlying bytes representation"},
    {NULL}  /* Sentinel */
};

// Python Getters and Setters definitions
static PyGetSetDef DroneLogger_getsetters[] = {
$c_python_getsets    {NULL} /* Sentinel */
};

// Python Type definition
static PyTypeObject PythonDroneLogEntryType = {
    PyVarObject_HEAD_INIT(NULL, 0) // 
    .tp_name        = "DroneLogger.LogEntry",
    .tp_doc         = "A log entry for the state of the drone",
    .tp_basicsize   = sizeof(PythonDroneLogEntry),
    .tp_itemsize    = 0,
    .tp_flags       = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new         = PyType_GenericNew,
    .tp_init        = (initproc) DroneLogger_init,
    .tp_getset      = DroneLogger_getsetters,
    .tp_methods     = DroneLogger_methods,
#ifdef DRONELOGGER_RICH_COMPARE
    .tp_richcompare = DroneLogger_richcmp,
#endif
};

// Python Module definition
static PyModuleDef DroneLoggermodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "DroneLogger",
    .m_doc  = "Module for drone logging",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_DroneLogger(void) {
    PyObject *m;
    if (PyType_Ready(&PythonDroneLogEntryType) < 0)
        return NULL;

    m = PyModule_Create(&DroneLoggermodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PythonDroneLogEntryType);
    PyModule_AddObject(m, "LogEntry", (PyObject *) &PythonDroneLogEntryType);
    return m;
}
