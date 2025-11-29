#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_param_type.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Param Value (Msg ID 22).
/// @details Emit the value of a onboard parameter. The inclusion of param_count and param_index in the message allows
/// the recipient to keep track of received parameters and allows him to re-request missing parameters after a loss or
/// timeout. The parameter microservice is documented at https://mavlink.io/en/services/parameter.html
template <typename Traits>
struct ParamValue_T {
    static constexpr std::uint32_t MessageId = 22;
    static constexpr std::string_view MessageName = "PARAM_VALUE";

    typename Traits::template Field<float> param_value;          ///< Onboard parameter value
    typename Traits::template Field<std::uint16_t> param_count;  ///< Total number of onboard parameters
    typename Traits::template Field<std::uint16_t> param_index;  ///< Index of this onboard parameter
    typename Traits::template Field<std::array<char, 16>>
        param_id;  ///< Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and
                   ///< WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to
                   ///< provide 16+1 bytes storage if the ID is stored as string
    typename Traits::template Field<std::uint8_t> param_type;  ///< Onboard parameter type. (MAV_PARAM_TYPE)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<ParamValue_T<Traits>>(); }
};

using ParamValue = ParamValue_T<TxTraits>;
using LazyParamValue = ParamValue_T<RxTraits>;

}  // namespace mavlink::payloads
