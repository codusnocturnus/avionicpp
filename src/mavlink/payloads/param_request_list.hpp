#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Param Request List (Msg ID 21).
/// @details Request all parameters of this component. After this request, all parameters are emitted. The parameter
/// microservice is documented at https://mavlink.io/en/services/parameter.html
template <typename Traits>
struct ParamRequestList_T {
    static constexpr std::uint32_t MessageId = 21;
    static constexpr std::string_view MessageName = "PARAM_REQUEST_LIST";

    typename Traits::template Field<std::uint8_t> target_system;     ///< System ID
    typename Traits::template Field<std::uint8_t> target_component;  ///< Component ID

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<ParamRequestList_T<Traits>>(); }
};

using ParamRequestList = ParamRequestList_T<TxTraits>;
using LazyParamRequestList = ParamRequestList_T<RxTraits>;

}  // namespace mavlink::payloads
