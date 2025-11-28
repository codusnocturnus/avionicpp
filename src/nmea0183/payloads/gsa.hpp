#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief GNSS DOP and Active Satellites (GSA).
template <typename Traits>
struct GSA_T {
    static constexpr std::string_view MessageId = "GSA"sv;

    /// @brief Selection mode (M=Manual, A=Automatic).
    typename Traits::template Enum<char> selection_mode;

    /// @brief Mode (1=Fix not available, 2=2D, 3=3D).
    typename Traits::template Enum<char> fix_mode;

    // IDs of SVs used in position fix (12 fields)
    // Null if unused.

    /// @brief Satellite ID 1.
    typename Traits::template Int<int, 2> sv_id_01;
    /// @brief Satellite ID 2.
    typename Traits::template Int<int, 2> sv_id_02;
    /// @brief Satellite ID 3.
    typename Traits::template Int<int, 2> sv_id_03;
    /// @brief Satellite ID 4.
    typename Traits::template Int<int, 2> sv_id_04;
    /// @brief Satellite ID 5.
    typename Traits::template Int<int, 2> sv_id_05;
    /// @brief Satellite ID 6.
    typename Traits::template Int<int, 2> sv_id_06;
    /// @brief Satellite ID 7.
    typename Traits::template Int<int, 2> sv_id_07;
    /// @brief Satellite ID 8.
    typename Traits::template Int<int, 2> sv_id_08;
    /// @brief Satellite ID 9.
    typename Traits::template Int<int, 2> sv_id_09;
    /// @brief Satellite ID 10.
    typename Traits::template Int<int, 2> sv_id_10;
    /// @brief Satellite ID 11.
    typename Traits::template Int<int, 2> sv_id_11;
    /// @brief Satellite ID 12.
    typename Traits::template Int<int, 2> sv_id_12;

    /// @brief PDOP (Position Dilution of Precision).
    typename Traits::template Float<float, 1> pdop;

    /// @brief HDOP (Horizontal Dilution of Precision).
    typename Traits::template Float<float, 1> hdop;

    /// @brief VDOP (Vertical Dilution of Precision).
    typename Traits::template Float<float, 1> vdop;
};

using GSA = GSA_T<TxTraits>;
using LazyGSA = GSA_T<RxTraits>;

}  // namespace nmea0183::payloads
