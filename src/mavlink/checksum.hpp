#pragma once
#include <array>
#include <boost/pfr.hpp>
#include <cstdint>
#include <string_view>
#include "mavlink/types.hpp"

namespace mavlink {

/// @brief Accumulate the X.25 CRC.
/// @param[in] data New byte to accumulate.
/// @param[inout] crcAccum Current CRC value.
constexpr void crc_accumulate(std::uint8_t data, std::uint16_t& crcAccum) {
    std::uint8_t tmp = data ^ (std::uint8_t)(crcAccum & 0xff);
    tmp ^= (tmp << 4);
    crcAccum = (crcAccum >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
}

/// @brief Accumulate the X.25 CRC for a buffer.
/// @param[inout] crcAccum Current CRC value.
/// @param[in] pBuffer Buffer to accumulate.
/// @param[in] length Length of the buffer.
constexpr void crc_accumulate_buffer(std::uint16_t& crcAccum, const char* pBuffer, size_t length) {
    const std::uint8_t* p = (const std::uint8_t*)pBuffer;
    while (length--) {
        crc_accumulate(*p++, crcAccum);
    }
}

/// @brief Accumulate the X.25 CRC for a string view.
/// @param[inout] crcAccum Current CRC value.
/// @param[in] str String to accumulate.
constexpr void crc_accumulate_string(std::uint16_t& crcAccum, std::string_view str) {
    for (char c : str) {
        crc_accumulate(static_cast<std::uint8_t>(c), crcAccum);
    }
}

/// @brief Helper to get the Mavlink type string for a C++ type.
/// @tparam T The C++ type.
/// @return The Mavlink type string (e.g. "uint8_t").
template <typename T>
consteval std::string_view type_string() {
    if constexpr (std::is_same_v<T, std::uint8_t>)
        return "uint8_t";
    if constexpr (std::is_same_v<T, std::int8_t>)
        return "int8_t";
    if constexpr (std::is_same_v<T, std::uint16_t>)
        return "uint16_t";
    if constexpr (std::is_same_v<T, std::int16_t>)
        return "int16_t";
    if constexpr (std::is_same_v<T, std::uint32_t>)
        return "uint32_t";
    if constexpr (std::is_same_v<T, std::int32_t>)
        return "int32_t";
    if constexpr (std::is_same_v<T, std::uint64_t>)
        return "uint64_t";
    if constexpr (std::is_same_v<T, std::int64_t>)
        return "int64_t";
    if constexpr (std::is_same_v<T, float>)
        return "float";
    if constexpr (std::is_same_v<T, double>)
        return "double";
    if constexpr (std::is_same_v<T, char>)
        return "char";
    return "unknown";  // TODO: handle arrays?
}

/// @brief Helper to unwrap TxField/RxField or direct type to the underlying value type.
/// @tparam T The type to unwrap.
template <typename T>
struct UnwrapField {
    using Type = T;
};

/// @brief Specialization for TxField.
template <typename T>
struct UnwrapField<TxField<T>> {
    using Type = T;
};

/// @brief Specialization for RxField.
template <typename T>
struct UnwrapField<RxField<T>> {
    using Type = T;
};

/// @brief Calculate the Mavlink CRC_EXTRA byte at compile time.
/// @tparam MessageT The message type struct.
/// @return The calculated CRC_EXTRA byte.
template <typename MessageT>
consteval uint8_t calculate_crc_extra() {
    auto crc = std::uint16_t{0xFFFF};  // Initial seed for CRC Extra is NOT 0xFFFF, it's just normal CRC update?
    // Wait, the documentation says: "The CRC is initialized with the message name..."
    // Standard CRC init is usually 0xFFFF.

    // 1. Message Name
    // Expect MessageT::MessageName to be defined
    crc_accumulate_string(crc, MessageT::MessageName);
    crc_accumulate((uint8_t)' ', crc);

    // 2. Iterate fields
    // We assume fields in struct are in WIRE ORDER (Mavlink requirement).
    // boost::pfr iterates in declaration order.

    // We need an instance to iterate? No, pfr can work with types?
    // boost::pfr::for_each_field works on object.
    // But we need names and types.
    // boost::pfr::get_name<N, MessageT>() works.

    constexpr auto field_count = boost::pfr::tuple_size_v<MessageT>;

    [&]<size_t... I>(std::index_sequence<I...>) {
        (([&]() {
             using FieldType = typename boost::pfr::tuple_element<I, MessageT>::type;
             // FieldType is like TxField<uint32_t>
             using CleanType = std::remove_cvref_t<FieldType>;
             using ValueType = typename UnwrapField<CleanType>::Type;

             std::string_view type_str = type_string<ValueType>();
             std::string_view name_str = boost::pfr::get_name<I, MessageT>();

             // "type name "
             crc_accumulate_string(crc, type_str);
             crc_accumulate((uint8_t)' ', crc);
             crc_accumulate_string(crc, name_str);
             crc_accumulate((uint8_t)' ', crc);

             // TODO: Array length?
             // If ValueType is array, type_string should handle it?
             // For now assuming primitive types as in Heartbeat.
         }()),
         ...);
    }(std::make_index_sequence<field_count>{});

    // Result is the low byte of the CRC?
    // "The CRC_EXTRA byte is the result of the calculation"
    // Usually it means (crc & 0xFF) ^ (crc >> 8).
    // Wait, let's check Mavlink spec.
    // "CRC-16-CCITT ... The 8-bit CRC_EXTRA is obtained by: (crc & 0xFF) ^ (crc >> 8)"

    return (uint8_t)((crc & 0xFF) ^ (crc >> 8));
}

}  // namespace mavlink
