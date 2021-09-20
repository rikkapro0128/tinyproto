/*
    Copyright 2021 (C) Alexey Dynda

    This file is part of Tiny Protocol Library.

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "structmember.h"
//#include "proto/hdlc/low_level/hdlc.h"
#include "interface/TinySerial.h"
#include "py_serial.h"

typedef struct
{
    PyObject_HEAD // no semicolon
    tinyproto::Serial *serial;
    int error_flag;
} PySerial;

static PyMemberDef Serial_members[] = {
    {NULL} /* Sentinel */
};

/////////////////////////////// ALLOC/DEALLOC

static void Serial_dealloc(PySerial *self)
{
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int Serial_init(PySerial *self, PyObject *args, PyObject *kwds)
{
    const char *s = NULL;
    if ( !PyArg_ParseTuple(args, "s", &s) )
    {
        return 1;
    }
    self->serial = new tinyproto::Serial(s);
    self->error_flag = 0;
    return 0;
}

static PyObject *Serial_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PySerial *self;

    self = (PySerial *)type->tp_alloc(type, 0);
    if ( self != NULL )
    {
        if ( Serial_init(self, args, kwds) )
        {
             Py_RETURN_NONE;
        }
    }
    return (PyObject *)self;
}

////////////////////////////// METHODS

static PyObject *Serial_begin(PySerial *self, PyObject *args)
{
    unsigned int i = 115200;
    if ( !PyArg_ParseTuple(args, "I", &i) )
    {
        return NULL;
    }
    int result = self->serial->begin(i); // TODO:
    return PyLong_FromLong((long)result);
}

static PyObject *Serial_end(PySerial *self)
{
    self->serial->end();
    Py_RETURN_NONE;
}

static PyObject *Serial_send(PySerial *self, PyObject *args)
{
    Py_buffer buffer{};
    if ( !PyArg_ParseTuple(args, "s*", &buffer) )
    {
        return NULL;
    }
    int result = self->serial->write( (const uint8_t *)buffer.buf, buffer.len );
    PyBuffer_Release(&buffer);
    return PyLong_FromLong((long)result);
}

static PyObject *Serial_read(PySerial *self, PyObject *args)
{
    int result;
    Py_buffer buffer{};
    if ( !PyArg_ParseTuple(args, "|s*", &buffer) )
    {
        return NULL;
    }
    if ( buffer.buf == NULL )
    {
        void *data = PyObject_Malloc( 4096 ); // TODO: Magic number
        result = self->serial->readBytes( (uint8_t *)data, 4096 );
        PyObject *to_send = PyByteArray_FromStringAndSize((const char *)data, result);
        PyObject_Free(data);
        return to_send;
    }
    else
    {
        result = self->serial->readBytes( (uint8_t *)buffer.buf, buffer.len );
        PyBuffer_Release(&buffer);
        return PyLong_FromLong((long)result);
    }
}

static PyGetSetDef Serial_getsetters[] = {
    {NULL} /* Sentinel */
};

///////////////////////////////// BINDINGS

static PyMethodDef Serial_methods[] = {
    {"begin", (PyCFunction)Serial_begin, METH_VARARGS, "Starts serial communication"},
    {"end", (PyCFunction)Serial_end, METH_NOARGS, "Stops serial communication"},
    {"write", (PyCFunction)Serial_send, METH_VARARGS, "Sends new bytes to remote side"},
    {"read", (PyCFunction)Serial_read, METH_VARARGS, "Reads new bytes from remote side"},
    {NULL} /* Sentinel */
};

PyTypeObject SerialType = {
    PyVarObject_HEAD_INIT(NULL, 0) "tinyproto.Serial", /* tp_name */
    sizeof(PySerial),                              /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor)Serial_dealloc,                    /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_reserved */
    0,                                             /* tp_repr */
    0,                                             /* tp_as_number */
    0,                                             /* tp_as_sequence */
    0,                                             /* tp_as_mapping */
    0,                                             /* tp_hash  */
    0,                                             /* tp_call */
    0,                                             /* tp_str */
    0,                                             /* tp_getattro */
    0,                                             /* tp_setattro */
    0,                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,      /* tp_flags */
    "Serial object",                               /* tp_doc */
    0,                                             /* tp_traverse */
    0,                                             /* tp_clear */
    0,                                             /* tp_richcompare */
    0,                                             /* tp_weaklistoffset */
    0,                                             /* tp_iter */
    0,                                             /* tp_iternext */
    Serial_methods,                                /* tp_methods */
    Serial_members,                                /* tp_members */
    Serial_getsetters,                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Serial_init,                         /* tp_init */
    0,                                             /* tp_alloc */
    Serial_new,                                    /* tp_new */
};
