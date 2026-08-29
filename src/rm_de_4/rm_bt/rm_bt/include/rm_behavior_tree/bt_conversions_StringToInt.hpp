#ifndef RM_BEHAVIOR_TREE__bt_CONVERSIONS_STRINGTOINT_HPP_
#define RM_BEHAVIOR_TREE__bt_CONVERSIONS_STRINGTOINT_HPP_

#include "behaviortree_cpp/basic_types.h"
#include "std_msgs/msg/int16.hpp"

namespace BT
{

/**
 * @brief Parse XML string to std_msgs::msg::Int16
 * @param key XML string
 * @return std_msgs::msg::Int16
 */
template <>
inline std_msgs::msg::Int16 convertFromString<std_msgs::msg::Int16>(BT::StringView str)
{
    std_msgs::msg::Int16 msg;
    try
    {
        msg.data = std::stoi(std::string(str));  // 将 StringView 转换为 std::string 后再转换为整数
    }
    catch (const std::invalid_argument& e)
    {
        // 处理转换失败的情况
        throw BT::RuntimeError("Failed to convert string to Int16: " + std::string(e.what()));
    }
    return msg;
}
}  // namespace BT

#endif  //RM_BEHAVIOR_TREE__bt_CONVERSIONS_STRINGTOINT_HPP_
