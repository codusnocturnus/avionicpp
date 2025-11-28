#pragma once
#include <algorithm>
#include <boost/pfr.hpp>
#include <cstring>
#include <expected>
#include "mavlink/types.hpp"

namespace mavlink {

/// @brief Helper to set value to T
/// @tparam T The type to set
/// @param[out] dest Destination
/// @param[in] src Source value
template <typename T>
constexpr void set_value(T& dest, const T& src) {
    dest = src;
}

/// @brief Helper to set value to RxField
/// @tparam T The type to set
/// @param[out] dest Destination RxField
/// @param[in] src Source value
template <typename T>
constexpr void set_value(RxField<T>& dest, const T& src) {
    dest.value = src;
}

/// @brief Helper to set value to TxField
/// @tparam T The type to set
/// @param[out] dest Destination TxField
/// @param[in] src Source value
template <typename T>
constexpr void set_value(TxField<T>& dest, const T& src) {
    dest.value = src;
}

/// @brief Deserializes a MessageView into a payload struct.
/// @tparam MessageT The payload struct type.
/// @param[in] view The message view to deserialize.
/// @return The deserialized message struct or an error.
template <typename MessageT>
[[nodiscard]] std::expected<MessageT, MavlinkError> deserialize(const MessageView& view) {
    if (view.msgid != MessageT::MessageId) {
        return std::unexpected(MavlinkError::ParseError);
    }

    auto message = MessageT{};

    auto payload_ptr = view.payload.data();
    auto payload_len = view.payload.size();
    auto current_pos = payload_ptr;
    auto end_pos = payload_ptr + payload_len;

    // Iterate fields and copy
    boost::pfr::for_each_field(message, [&](auto& field) {
        using FieldType = std::remove_cvref_t<decltype(field)>;
        using ValueType = typename FieldType::ValueType;

        auto val = ValueType{};
        auto val_bytes = reinterpret_cast<std::uint8_t*>(&val);

        std::size_t remaining = end_pos - current_pos;
        if (remaining > 0) {
            std::size_t bytes_to_copy = std::min(remaining, sizeof(ValueType));
            std::copy(current_pos, current_pos + bytes_to_copy, val_bytes);
            current_pos += bytes_to_copy;
        }

        set_value(field, val);
    });

    return message;
}

}  // namespace mavlink
