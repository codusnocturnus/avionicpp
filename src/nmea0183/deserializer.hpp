#pragma once
#include <array>
#include <functional>

#include <boost/pfr.hpp>

#include "concepts.hpp"
#include "types.hpp"

namespace nmea0183 {

// --- Binder ---

/// @brief Binds a raw MessageView to a strongly-typed LazyPayload struct.
/// @tparam LazyPayload The aggregate type to bind to (must satisfy Aggregate concept).
/// @param[in] view The parsed message view containing fields.
/// @return std::expected containing the bound payload or an error.
template <Aggregate LazyPayload>
[[nodiscard]] auto bind(const MessageView& view) noexcept -> std::expected<LazyPayload, NMEAError> {
    LazyPayload payload{};

    // Map view fields to struct members
    boost::pfr::for_each_field(payload, [&](auto& field, size_t idx) {
        if (idx < view.field_count)
            field.token = view.fields[idx];
        else
            field.token = {};
    });

    return payload;
}

// --- Dispatcher ---

/// @brief compile-time association of a message ID with its payload type.
/// @tparam ID The message ID string (e.g., "GGA").
/// @tparam RxPayload The payload type to deserialize into.
template <FixedString ID, Aggregate RxPayload>
struct MessageHandler {
    static constexpr std::string_view id = ID.view();
    using PayloadType = RxPayload;
};

/// @brief Dispatches a message view to the appropriate handler based on message ID.
/// @tparam Handlers Variadic list of MessageHandler types.
template <typename... Handlers>
struct Dispatcher {
    /// @brief Tries to match the view's message type against registered handlers and invokes the visitor.
    /// @tparam Visitor Callable type accepting std::expected<Payload, NMEAError>.
    /// @param[in] view The message view to dispatch.
    /// @param[in] visitor The visitor to invoke with the result.
    /// @return true if a handler matched the message ID, false otherwise.
    template <typename Visitor>
    [[nodiscard]] static bool dispatch(const MessageView& view, Visitor&& visitor) noexcept {
        return ((try_match<Handlers>(view, visitor)) || ...);
    }

   private:
    template <typename Handler, typename Visitor>
    static bool try_match(const MessageView& view, Visitor&& visitor) noexcept {
        if (view.message_type == Handler::id) {
            auto result = bind<typename Handler::PayloadType>(view);
            std::invoke(std::forward<Visitor>(visitor), std::move(result));
            return true;
        }
        return false;
    }
};

}  // namespace nmea0183
