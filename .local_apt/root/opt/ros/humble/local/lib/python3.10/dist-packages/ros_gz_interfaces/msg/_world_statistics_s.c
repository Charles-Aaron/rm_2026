// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "ros_gz_interfaces/msg/detail/world_statistics__struct.h"
#include "ros_gz_interfaces/msg/detail/world_statistics__functions.h"

ROSIDL_GENERATOR_C_IMPORT
bool std_msgs__msg__header__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * std_msgs__msg__header__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool builtin_interfaces__msg__time__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * builtin_interfaces__msg__time__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool builtin_interfaces__msg__time__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * builtin_interfaces__msg__time__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool builtin_interfaces__msg__time__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * builtin_interfaces__msg__time__convert_to_py(void * raw_ros_message);
bool ros_gz_interfaces__msg__log_playback_statistics__convert_from_py(PyObject * _pymsg, void * _ros_message);
PyObject * ros_gz_interfaces__msg__log_playback_statistics__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool builtin_interfaces__msg__time__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * builtin_interfaces__msg__time__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool ros_gz_interfaces__msg__world_statistics__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[56];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("ros_gz_interfaces.msg._world_statistics.WorldStatistics", full_classname_dest, 55) == 0);
  }
  ros_gz_interfaces__msg__WorldStatistics * ros_message = _ros_message;
  {  // header
    PyObject * field = PyObject_GetAttrString(_pymsg, "header");
    if (!field) {
      return false;
    }
    if (!std_msgs__msg__header__convert_from_py(field, &ros_message->header)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // sim_time
    PyObject * field = PyObject_GetAttrString(_pymsg, "sim_time");
    if (!field) {
      return false;
    }
    if (!builtin_interfaces__msg__time__convert_from_py(field, &ros_message->sim_time)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // pause_time
    PyObject * field = PyObject_GetAttrString(_pymsg, "pause_time");
    if (!field) {
      return false;
    }
    if (!builtin_interfaces__msg__time__convert_from_py(field, &ros_message->pause_time)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // real_time
    PyObject * field = PyObject_GetAttrString(_pymsg, "real_time");
    if (!field) {
      return false;
    }
    if (!builtin_interfaces__msg__time__convert_from_py(field, &ros_message->real_time)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // paused
    PyObject * field = PyObject_GetAttrString(_pymsg, "paused");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->paused = (Py_True == field);
    Py_DECREF(field);
  }
  {  // iterations
    PyObject * field = PyObject_GetAttrString(_pymsg, "iterations");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->iterations = PyLong_AsUnsignedLongLong(field);
    Py_DECREF(field);
  }
  {  // model_count
    PyObject * field = PyObject_GetAttrString(_pymsg, "model_count");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->model_count = (int32_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }
  {  // log_playback_statistics
    PyObject * field = PyObject_GetAttrString(_pymsg, "log_playback_statistics");
    if (!field) {
      return false;
    }
    if (!ros_gz_interfaces__msg__log_playback_statistics__convert_from_py(field, &ros_message->log_playback_statistics)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // real_time_factor
    PyObject * field = PyObject_GetAttrString(_pymsg, "real_time_factor");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->real_time_factor = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // step_size
    PyObject * field = PyObject_GetAttrString(_pymsg, "step_size");
    if (!field) {
      return false;
    }
    if (!builtin_interfaces__msg__time__convert_from_py(field, &ros_message->step_size)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * ros_gz_interfaces__msg__world_statistics__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of WorldStatistics */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("ros_gz_interfaces.msg._world_statistics");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "WorldStatistics");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  ros_gz_interfaces__msg__WorldStatistics * ros_message = (ros_gz_interfaces__msg__WorldStatistics *)raw_ros_message;
  {  // header
    PyObject * field = NULL;
    field = std_msgs__msg__header__convert_to_py(&ros_message->header);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "header", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // sim_time
    PyObject * field = NULL;
    field = builtin_interfaces__msg__time__convert_to_py(&ros_message->sim_time);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "sim_time", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // pause_time
    PyObject * field = NULL;
    field = builtin_interfaces__msg__time__convert_to_py(&ros_message->pause_time);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "pause_time", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // real_time
    PyObject * field = NULL;
    field = builtin_interfaces__msg__time__convert_to_py(&ros_message->real_time);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "real_time", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // paused
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->paused ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "paused", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // iterations
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLongLong(ros_message->iterations);
    {
      int rc = PyObject_SetAttrString(_pymessage, "iterations", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // model_count
    PyObject * field = NULL;
    field = PyLong_FromLong(ros_message->model_count);
    {
      int rc = PyObject_SetAttrString(_pymessage, "model_count", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // log_playback_statistics
    PyObject * field = NULL;
    field = ros_gz_interfaces__msg__log_playback_statistics__convert_to_py(&ros_message->log_playback_statistics);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "log_playback_statistics", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // real_time_factor
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->real_time_factor);
    {
      int rc = PyObject_SetAttrString(_pymessage, "real_time_factor", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // step_size
    PyObject * field = NULL;
    field = builtin_interfaces__msg__time__convert_to_py(&ros_message->step_size);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "step_size", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
