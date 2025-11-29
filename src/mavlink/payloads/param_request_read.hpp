#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Param Request Read (Msg ID 20).
/// @details Request to read the onboard parameter with the param_id string id. Onboard parameters are stored as
/// key[const char*] -> value[float]. This allows to send a parameter to any other component (such as the GCS) without
/// the need of previous knowledge of possible parameter names. Thus the same GCS can store different parameters for
/// different autopilots.
template <typename Traits>
struct ParamRequestRead_T {
    static constexpr std::uint32_t MessageId = 20;
    static constexpr std::string_view MessageName = "PARAM_REQUEST_READ";

    typename Traits::template Field<std::int16_t> param_index;  ///< Parameter index. Send -1 to use the param ID field
                                                                ///< as identifier (else the param id will be ignored)
    typename Traits::template Field<std::uint8_t> target_system;     ///< System ID
    typename Traits::template Field<std::uint8_t> target_component;  ///< Component ID
    typename Traits::template Field<std::array<char, 16>>
        param_id;  ///< Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and
                   ///< WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to
                   ///< provide 16+1 bytes storage if the ID is stored as string

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<ParamRequestRead_T<Traits>>(); }
};

using ParamRequestRead = ParamRequestRead_T<TxTraits>;
using LazyParamRequestRead = ParamRequestRead_T<RxTraits>;

}  // namespace mavlink::payloads
