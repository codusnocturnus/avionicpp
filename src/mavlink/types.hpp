#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <vector>

namespace mavlink {

/// @brief Errors that can occur during Mavlink message processing.
enum class MavlinkError {
    None,
    ParseError,
    BufferOverrun,
    InvalidMagic,
    InvalidChecksum,
    InvalidSignature,
    ProtocolViolation
};

/// @brief Represents a parsed Mavlink message view with zero-copy payload.
struct MessageView {
    std::uint32_t msgid;                    ///< Message ID.
    std::uint8_t sysid;                     ///< System ID.
    std::uint8_t compid;                    ///< Component ID.
    std::uint8_t seq;                       ///< Sequence number.
    std::span<const std::uint8_t> payload;  ///< Payload view.
};

// --- Traits for Payload Fields ---

/// @brief Wrapper for Tx fields.
/// @tparam T The type of the field.
template <typename T>
struct TxField {
    using ValueType = T;
    T value;
};

/// @brief Wrapper for Rx fields.
/// @tparam T The type of the field.
template <typename T>
struct RxField {
    using ValueType = T;
    T value;
};

/// @brief Traits for Tx payload fields.
struct TxTraits {
    template <typename T>
    using Field = TxField<T>;
};

/// @brief Traits for Rx payload fields.
struct RxTraits {
    template <typename T>
    using Field = RxField<T>;
};

}  // namespace mavlink
