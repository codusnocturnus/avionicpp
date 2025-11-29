#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_param_type.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Param Set (Msg ID 23).
/// @details Set a parameter value (write new value to permanent storage). The receiving component should acknowledge
/// the new parameter value by broadcasting a PARAM_VALUE message (broadcasting ensures that multiple GCS all have an
/// up-to-date list of all parameters). If the sending GCS did not receive a PARAM_VALUE within its timeout time, it
/// should re-send the PARAM_SET message. The parameter microservice is documented at
/// https://mavlink.io/en/services/parameter.html.
template <typename Traits>
struct ParamSet_T {
    static constexpr std::uint32_t MessageId = 23;
    static constexpr std::string_view MessageName = "PARAM_SET";

    typename Traits::template Field<float> param_value;              ///< Onboard parameter value
    typename Traits::template Field<std::uint8_t> target_system;     ///< System ID
    typename Traits::template Field<std::uint8_t> target_component;  ///< Component ID
    typename Traits::template Field<std::array<char, 16>>
        param_id;  ///< Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and
                   ///< WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to
                   ///< provide 16+1 bytes storage if the ID is stored as string
    typename Traits::template Field<std::uint8_t> param_type;  ///< Onboard parameter type. (MAV_PARAM_TYPE)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<ParamSet_T<Traits>>(); }
};

using ParamSet = ParamSet_T<TxTraits>;
using LazyParamSet = ParamSet_T<RxTraits>;

}  // namespace mavlink::payloads
