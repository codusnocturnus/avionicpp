#pragma once

#include <array>
#include <format>
#include <functional>
#include <numeric>
#include <ranges>
#include <tuple>

#include <boost/pfr.hpp>

#include "concepts.hpp"
#include "types.hpp"

namespace nmea0183 {

// --- Wrapper for Zero-Allocation Formatting ---
/// @brief Helper to allow formatting of std::optional fields.
template <typename T, std::uint8_t P, std::uint8_t W>
struct OptionalWrapper {
    const std::optional<T>& opt;
};

}  // namespace nmea0183

// --- Custom Formatter ---
/// @brief Specialization of std::formatter for OptionalWrapper.
template <typename T, std::uint8_t P, std::uint8_t W>
struct std::formatter<nmea0183::OptionalWrapper<T, P, W>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const nmea0183::OptionalWrapper<T, P, W>& wrapper, std::format_context& ctx) const {
        if (wrapper.opt) {
            if constexpr (std::is_floating_point_v<T>) {
                if constexpr (W > 0) {
                    return std::format_to(ctx.out(), "{:0{}.{}f}", *wrapper.opt, W, P);
                } else {
                    return std::format_to(ctx.out(), "{:.{}f}", *wrapper.opt, P);
                }
            } else if constexpr (std::is_enum_v<T>) {
                return std::format_to(ctx.out(), "{}", static_cast<char>(*wrapper.opt));
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, char>) {
                if constexpr (W > 0) {
                    return std::format_to(ctx.out(), "{:0{}d}", *wrapper.opt, W);
                } else {
                    return std::format_to(ctx.out(), "{}", *wrapper.opt);
                }
            } else {
                return std::format_to(ctx.out(), "{}", *wrapper.opt);
            }
        }
        return ctx.out();
    }
};

namespace nmea0183 {

// --- Compile-Time Format String Generation ---

// Specifier for TxField: "{}" (Formatter handles logic)
template <typename T, std::uint8_t P, std::uint8_t W>
consteval auto get_specifier(TxField<T, P, W>*) {
    return FixedString("{}");
}

template <typename Tuple, size_t Index = 0>
consteval auto build_payload_fmt() {
    if constexpr (Index >= std::tuple_size_v<Tuple>)
        return FixedString("");
    else {
        using FieldType = std::tuple_element_t<Index, Tuple>;
        constexpr auto current = get_specifier(static_cast<FieldType*>(nullptr));
        if constexpr (Index == 0)
            return current + build_payload_fmt<Tuple, Index + 1>();
        else
            return FixedString(",") + current + build_payload_fmt<Tuple, Index + 1>();
    }
}

// --- Runtime Converters ---

template <typename T, std::uint8_t P, std::uint8_t W>
auto to_formattable(const TxField<T, P, W>& field) {
    return OptionalWrapper<T, P, W>{field.value};
}

// --- Message Wrapper & Serializer ---

/// @brief Holds a payload and defines formatting logic for serialization.
/// @tparam TalkerID The 2-character talker ID (e.g., "GP").
/// @tparam Payload The message payload struct.
template <FixedString TalkerID, Aggregate Payload>
struct Message {
    Payload payload;

    using PayloadTuple = decltype(boost::pfr::structure_to_tuple(std::declval<Payload>()));

    static constexpr auto HEADER = FixedString("${}{},");
    static constexpr auto BODY = build_payload_fmt<PayloadTuple>();
    static constexpr auto FULL_FMT = HEADER + BODY;
};

/// @brief Serializes a message into a provided buffer.
/// @tparam TalkerID The talker ID.
/// @tparam Payload The payload type.
/// @param[in] msg The message to serialize.
/// @param[out] buffer The buffer to write to (must be contiguous range).
/// @return The number of bytes written.
template <FixedString TalkerID, Aggregate Payload>
[[nodiscard]] auto serialize(const Message<TalkerID, Payload>& msg, std::ranges::contiguous_range auto&& buffer) {
    auto ptr = buffer.data();
    auto end = buffer.data() + buffer.size();

    // 1. Prepare Arguments
    auto fields = boost::pfr::structure_to_tuple(msg.payload);
    auto args = std::apply([](auto const&... f) { return std::make_tuple(to_formattable(f)...); }, fields);

    // Use Payload::MessageId per guideline update
    auto full_args = std::tuple_cat(std::make_tuple(TalkerID.view(), Payload::MessageId), args);

    // 2. Format Body
    auto res = std::apply(
        [&](auto const&... a) { return std::format_to_n(ptr, std::distance(ptr, end), msg.FULL_FMT.view(), a...); },
        full_args);
    ptr = res.out;

    // 3. Checksum
    auto span = std::span(buffer.data() + 1, ptr);
    auto checksum = std::ranges::fold_left(span, std::uint8_t{0}, std::bit_xor<>{});

    // 4. Footer
    res = std::format_to_n(ptr, std::distance(ptr, end), "*{:02X}\r\n", checksum);
    return std::distance(buffer.data(), res.out);
}

}  // namespace nmea0183
