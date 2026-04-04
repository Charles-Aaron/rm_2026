# generated from rosidl_generator_py/resource/_idl.py.em
# with input from ros_gz_interfaces:msg/WorldStatistics.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_WorldStatistics(type):
    """Metaclass of message 'WorldStatistics'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('ros_gz_interfaces')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'ros_gz_interfaces.msg.WorldStatistics')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__world_statistics
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__world_statistics
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__world_statistics
            cls._TYPE_SUPPORT = module.type_support_msg__msg__world_statistics
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__world_statistics

            from builtin_interfaces.msg import Time
            if Time.__class__._TYPE_SUPPORT is None:
                Time.__class__.__import_type_support__()

            from ros_gz_interfaces.msg import LogPlaybackStatistics
            if LogPlaybackStatistics.__class__._TYPE_SUPPORT is None:
                LogPlaybackStatistics.__class__.__import_type_support__()

            from std_msgs.msg import Header
            if Header.__class__._TYPE_SUPPORT is None:
                Header.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class WorldStatistics(metaclass=Metaclass_WorldStatistics):
    """Message class 'WorldStatistics'."""

    __slots__ = [
        '_header',
        '_sim_time',
        '_pause_time',
        '_real_time',
        '_paused',
        '_iterations',
        '_model_count',
        '_log_playback_statistics',
        '_real_time_factor',
        '_step_size',
    ]

    _fields_and_field_types = {
        'header': 'std_msgs/Header',
        'sim_time': 'builtin_interfaces/Time',
        'pause_time': 'builtin_interfaces/Time',
        'real_time': 'builtin_interfaces/Time',
        'paused': 'boolean',
        'iterations': 'uint64',
        'model_count': 'int32',
        'log_playback_statistics': 'ros_gz_interfaces/LogPlaybackStatistics',
        'real_time_factor': 'double',
        'step_size': 'builtin_interfaces/Time',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.NamespacedType(['std_msgs', 'msg'], 'Header'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['builtin_interfaces', 'msg'], 'Time'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['builtin_interfaces', 'msg'], 'Time'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['builtin_interfaces', 'msg'], 'Time'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['ros_gz_interfaces', 'msg'], 'LogPlaybackStatistics'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['builtin_interfaces', 'msg'], 'Time'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        from std_msgs.msg import Header
        self.header = kwargs.get('header', Header())
        from builtin_interfaces.msg import Time
        self.sim_time = kwargs.get('sim_time', Time())
        from builtin_interfaces.msg import Time
        self.pause_time = kwargs.get('pause_time', Time())
        from builtin_interfaces.msg import Time
        self.real_time = kwargs.get('real_time', Time())
        self.paused = kwargs.get('paused', bool())
        self.iterations = kwargs.get('iterations', int())
        self.model_count = kwargs.get('model_count', int())
        from ros_gz_interfaces.msg import LogPlaybackStatistics
        self.log_playback_statistics = kwargs.get('log_playback_statistics', LogPlaybackStatistics())
        self.real_time_factor = kwargs.get('real_time_factor', float())
        from builtin_interfaces.msg import Time
        self.step_size = kwargs.get('step_size', Time())

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
        if self.header != other.header:
            return False
        if self.sim_time != other.sim_time:
            return False
        if self.pause_time != other.pause_time:
            return False
        if self.real_time != other.real_time:
            return False
        if self.paused != other.paused:
            return False
        if self.iterations != other.iterations:
            return False
        if self.model_count != other.model_count:
            return False
        if self.log_playback_statistics != other.log_playback_statistics:
            return False
        if self.real_time_factor != other.real_time_factor:
            return False
        if self.step_size != other.step_size:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def header(self):
        """Message field 'header'."""
        return self._header

    @header.setter
    def header(self, value):
        if __debug__:
            from std_msgs.msg import Header
            assert \
                isinstance(value, Header), \
                "The 'header' field must be a sub message of type 'Header'"
        self._header = value

    @builtins.property
    def sim_time(self):
        """Message field 'sim_time'."""
        return self._sim_time

    @sim_time.setter
    def sim_time(self, value):
        if __debug__:
            from builtin_interfaces.msg import Time
            assert \
                isinstance(value, Time), \
                "The 'sim_time' field must be a sub message of type 'Time'"
        self._sim_time = value

    @builtins.property
    def pause_time(self):
        """Message field 'pause_time'."""
        return self._pause_time

    @pause_time.setter
    def pause_time(self, value):
        if __debug__:
            from builtin_interfaces.msg import Time
            assert \
                isinstance(value, Time), \
                "The 'pause_time' field must be a sub message of type 'Time'"
        self._pause_time = value

    @builtins.property
    def real_time(self):
        """Message field 'real_time'."""
        return self._real_time

    @real_time.setter
    def real_time(self, value):
        if __debug__:
            from builtin_interfaces.msg import Time
            assert \
                isinstance(value, Time), \
                "The 'real_time' field must be a sub message of type 'Time'"
        self._real_time = value

    @builtins.property
    def paused(self):
        """Message field 'paused'."""
        return self._paused

    @paused.setter
    def paused(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'paused' field must be of type 'bool'"
        self._paused = value

    @builtins.property
    def iterations(self):
        """Message field 'iterations'."""
        return self._iterations

    @iterations.setter
    def iterations(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'iterations' field must be of type 'int'"
            assert value >= 0 and value < 18446744073709551616, \
                "The 'iterations' field must be an unsigned integer in [0, 18446744073709551615]"
        self._iterations = value

    @builtins.property
    def model_count(self):
        """Message field 'model_count'."""
        return self._model_count

    @model_count.setter
    def model_count(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'model_count' field must be of type 'int'"
            assert value >= -2147483648 and value < 2147483648, \
                "The 'model_count' field must be an integer in [-2147483648, 2147483647]"
        self._model_count = value

    @builtins.property
    def log_playback_statistics(self):
        """Message field 'log_playback_statistics'."""
        return self._log_playback_statistics

    @log_playback_statistics.setter
    def log_playback_statistics(self, value):
        if __debug__:
            from ros_gz_interfaces.msg import LogPlaybackStatistics
            assert \
                isinstance(value, LogPlaybackStatistics), \
                "The 'log_playback_statistics' field must be a sub message of type 'LogPlaybackStatistics'"
        self._log_playback_statistics = value

    @builtins.property
    def real_time_factor(self):
        """Message field 'real_time_factor'."""
        return self._real_time_factor

    @real_time_factor.setter
    def real_time_factor(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'real_time_factor' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'real_time_factor' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._real_time_factor = value

    @builtins.property
    def step_size(self):
        """Message field 'step_size'."""
        return self._step_size

    @step_size.setter
    def step_size(self, value):
        if __debug__:
            from builtin_interfaces.msg import Time
            assert \
                isinstance(value, Time), \
                "The 'step_size' field must be a sub message of type 'Time'"
        self._step_size = value
