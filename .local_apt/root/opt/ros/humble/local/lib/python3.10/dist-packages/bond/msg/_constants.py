# generated from rosidl_generator_py/resource/_idl.py.em
# with input from bond:msg/Constants.idl
# generated code does not contain a copyright notice


# Import statements for member types

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_Constants(type):
    """Metaclass of message 'Constants'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
        'DEAD_PUBLISH_PERIOD': 0.05,
        'DEFAULT_CONNECT_TIMEOUT': 10.0,
        'DEFAULT_HEARTBEAT_TIMEOUT': 4.0,
        'DEFAULT_DISCONNECT_TIMEOUT': 2.0,
        'DEFAULT_HEARTBEAT_PERIOD': 1.0,
        'DISABLE_HEARTBEAT_TIMEOUT_PARAM': '/bond_disable_heartbeat_timeout',
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('bond')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'bond.msg.Constants')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__constants
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__constants
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__constants
            cls._TYPE_SUPPORT = module.type_support_msg__msg__constants
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__constants

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
            'DEAD_PUBLISH_PERIOD': cls.__constants['DEAD_PUBLISH_PERIOD'],
            'DEFAULT_CONNECT_TIMEOUT': cls.__constants['DEFAULT_CONNECT_TIMEOUT'],
            'DEFAULT_HEARTBEAT_TIMEOUT': cls.__constants['DEFAULT_HEARTBEAT_TIMEOUT'],
            'DEFAULT_DISCONNECT_TIMEOUT': cls.__constants['DEFAULT_DISCONNECT_TIMEOUT'],
            'DEFAULT_HEARTBEAT_PERIOD': cls.__constants['DEFAULT_HEARTBEAT_PERIOD'],
            'DISABLE_HEARTBEAT_TIMEOUT_PARAM': cls.__constants['DISABLE_HEARTBEAT_TIMEOUT_PARAM'],
        }

    @property
    def DEAD_PUBLISH_PERIOD(self):
        """Message constant 'DEAD_PUBLISH_PERIOD'."""
        return Metaclass_Constants.__constants['DEAD_PUBLISH_PERIOD']

    @property
    def DEFAULT_CONNECT_TIMEOUT(self):
        """Message constant 'DEFAULT_CONNECT_TIMEOUT'."""
        return Metaclass_Constants.__constants['DEFAULT_CONNECT_TIMEOUT']

    @property
    def DEFAULT_HEARTBEAT_TIMEOUT(self):
        """Message constant 'DEFAULT_HEARTBEAT_TIMEOUT'."""
        return Metaclass_Constants.__constants['DEFAULT_HEARTBEAT_TIMEOUT']

    @property
    def DEFAULT_DISCONNECT_TIMEOUT(self):
        """Message constant 'DEFAULT_DISCONNECT_TIMEOUT'."""
        return Metaclass_Constants.__constants['DEFAULT_DISCONNECT_TIMEOUT']

    @property
    def DEFAULT_HEARTBEAT_PERIOD(self):
        """Message constant 'DEFAULT_HEARTBEAT_PERIOD'."""
        return Metaclass_Constants.__constants['DEFAULT_HEARTBEAT_PERIOD']

    @property
    def DISABLE_HEARTBEAT_TIMEOUT_PARAM(self):
        """Message constant 'DISABLE_HEARTBEAT_TIMEOUT_PARAM'."""
        return Metaclass_Constants.__constants['DISABLE_HEARTBEAT_TIMEOUT_PARAM']


class Constants(metaclass=Metaclass_Constants):
    """
    Message class 'Constants'.

    Constants:
      DEAD_PUBLISH_PERIOD
      DEFAULT_CONNECT_TIMEOUT
      DEFAULT_HEARTBEAT_TIMEOUT
      DEFAULT_DISCONNECT_TIMEOUT
      DEFAULT_HEARTBEAT_PERIOD
      DISABLE_HEARTBEAT_TIMEOUT_PARAM
    """

    __slots__ = [
    ]

    _fields_and_field_types = {
    }

    SLOT_TYPES = (
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)
