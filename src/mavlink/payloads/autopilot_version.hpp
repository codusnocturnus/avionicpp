#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_protocol_capability.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Autopilot Version (Msg ID 148).
/// @details Version and capability of autopilot software. This should be emitted in response to a request with
/// MAV_CMD_REQUEST_MESSAGE.
template <typename Traits>
struct AutopilotVersion_T {
    static constexpr std::uint32_t MessageId = 148;
    static constexpr std::string_view MessageName = "AUTOPILOT_VERSION";

    typename Traits::template Field<std::uint64_t> capabilities;           ///< Bitmap of capabilities
    typename Traits::template Field<std::uint64_t> uid;                    ///< UID if provided by hardware (see uid2)
    typename Traits::template Field<std::uint32_t> flight_sw_version;      ///< Firmware version number.
    typename Traits::template Field<std::uint32_t> middleware_sw_version;  ///< Middleware version number
    typename Traits::template Field<std::uint32_t> os_sw_version;          ///< Operating system version number
    typename Traits::template Field<std::uint32_t>
        board_version;  ///< HW / board version (last 8 bytes should be silicon ID, if any).
    typename Traits::template Field<std::uint16_t> vendor_id;   ///< ID of the board vendor
    typename Traits::template Field<std::uint16_t> product_id;  ///< ID of the product
    typename Traits::template Field<std::array<std::uint8_t, 8>>
        flight_custom_version;  ///< Custom version field, commonly the first 8 bytes of the git hash.
    typename Traits::template Field<std::array<std::uint8_t, 8>>
        middleware_custom_version;  ///< Custom version field, commonly the first 8 bytes of the git hash.
    typename Traits::template Field<std::array<std::uint8_t, 8>>
        os_custom_version;  ///< Custom version field, commonly the first 8 bytes of the git hash.
    typename Traits::template Field<std::array<std::uint8_t, 18>>
        uid2;  ///< UID if provided by hardware (supersedes the uid field. If this is non-zero, use this field,
               ///< otherwise use uid)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<AutopilotVersion_T<Traits>>(); }
};

using AutopilotVersion = AutopilotVersion_T<TxTraits>;
using LazyAutopilotVersion = AutopilotVersion_T<RxTraits>;

}  // namespace mavlink::payloads
