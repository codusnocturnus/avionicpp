#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>

#include "concepts.hpp"

namespace nmea0183 {

using namespace std::string_view_literals;

/// @brief Helper for creating overloaded lambdas/functors (e.g., for std::visit or dispatch).
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/// @brief Errors that can occur during NMEA message processing.
enum class NMEAError {
    None,
    ParseError,
    FieldCountMismatch,
    InvalidStartDelimiter,
    InvalidEndDelimiter,
    ChecksumMismatch
};

/// @brief A compile-time string wrapper for defining message IDs.
/// @tparam N The size of the string buffer including null terminator.
template <size_t N>
struct FixedString {
    std::array<char, N> buf{};

    consteval FixedString() = default;
    consteval FixedString(const char (&arg)[N]) { std::copy_n(arg, N, buf.begin()); }
    constexpr std::string_view view() const { return {buf.data(), N - 1}; }
};

/// @brief Concatenates two FixedStrings at compile time.
template <size_t N1, size_t N2>
consteval auto operator+(const FixedString<N1>& s1, const FixedString<N2>& s2) {
    // N1 + N2 - 1 because we subtract one null terminator (from s1)
    // but keep the one from s2.
    FixedString<N1 + N2 - 1> result;

    // Copy s1 (excluding its null terminator)
    std::copy_n(s1.buf.begin(), N1 - 1, result.buf.begin());

    // Copy s2 (including its null terminator) at the offset
    std::copy_n(s2.buf.begin(), N2, result.buf.begin() + N1 - 1);

    return result;
}

/// @brief Represents a parsed NMEA message view with zero-copy fields.
struct MessageView {
    static constexpr size_t MaxFields = 32;
    std::string_view talker_id;
    std::string_view message_type;
    std::array<std::string_view, MaxFields> fields;
    size_t field_count = 0;
};

/// @brief Wrapper for eager values used in transmission.
/// @tparam T The value type.
/// @tparam Precision Floating point precision (digits after decimal).
/// @tparam Width Minimum field width.
template <typename T, std::uint8_t Precision = 0, std::uint8_t Width = 0>
struct TxField {
    using ValueType = T;
    static constexpr std::uint8_t precision = Precision;
    static constexpr std::uint8_t width = Width;
    std::optional<T> value;
};

/// @brief Wrapper for lazy parsing of received fields.
/// @tparam T The value type to parse into.
/// @tparam Precision Floating point precision (informational).
/// @tparam Width Minimum field width (informational).
template <typename T, std::uint8_t Precision = 0, std::uint8_t Width = 0>
struct RxField {
    using ValueType = T;
    static constexpr std::uint8_t precision = Precision;
    static constexpr std::uint8_t width = Width;

    std::string_view token;  // Zero-copy reference

    /// @brief Parses the token into the target type.
    /// @return std::expected containing optional value or error.
    [[nodiscard]] auto value() const -> std::expected<std::optional<T>, NMEAError> {
        if (token.empty())
            return std::optional<T>{std::nullopt};

        if constexpr (std::is_same_v<T, std::string_view>) {
            return std::optional<T>{token};
        } else if constexpr (std::is_same_v<T, char>) {
            return std::optional<T>{token[0]};
        } else {
            T val{};
            if constexpr (std::is_enum_v<T>) {
                val = static_cast<T>(token[0]);  // Simple 1-char enum map
            } else {
                auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), val);
                if (ec != std::errc())
                    return std::unexpected(NMEAError::ParseError);
            }
            return std::optional<T>{val};
        }
    }
};

// --- Traits to Select Mode ---

/// @brief Traits for defining transmission payloads with eager values.
struct TxTraits {
    template <typename T, std::uint8_t P = 0, std::uint8_t W = 0>
    using Float = TxField<T, P, W>;
    template <typename T>
    using Enum = TxField<T>;
    template <typename T, std::uint8_t Width = 0>
    using Int = TxField<T, 0, Width>;
    using String = TxField<std::string_view>;
};

/// @brief Traits for defining reception payloads with lazy parsing.
struct RxTraits {
    template <typename T, std::uint8_t P = 0, std::uint8_t W = 0>
    using Float = RxField<T, P, W>;
    template <typename T>
    using Enum = RxField<T>;
    template <typename T, std::uint8_t Width = 0>
    using Int = RxField<T, 0, Width>;
    using String = RxField<std::string_view>;
};

}  // namespace nmea0183
