#pragma once
#include <algorithm>
#include <boost/pfr.hpp>
#include <cstdint>
#include <expected>
#include <span>
#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink {

// Helper to get value from TxField or direct type
template <typename T>
constexpr const T& get_value(const T& val) {
    return val;
}

template <typename T>
constexpr const T& get_value(const TxField<T>& val) {
    return val.value;
}

// Serializes a message into a provided buffer.
// Returns the number of bytes written or an error.
template <typename MessageT>
[[nodiscard]] std::expected<std::size_t, MavlinkError> serialize(const MessageT& message,
                                                                 std::uint8_t sysid,
                                                                 std::uint8_t compid,
                                                                 std::uint8_t seq,
                                                                 std::span<std::uint8_t> buffer) {
    // Max Mavlink packet size is 280 bytes.
    if (buffer.size() < 280) {
        return std::unexpected(MavlinkError::BufferOverrun);
    }

    auto ptr = buffer.data();

    // Header offsets (Mavlink v2)
    // STX: 0
    // LEN: 1
    // INC: 2
    // CMP: 3
    // SEQ: 4
    // SYS: 5
    // COMP: 6
    // MSGID: 7, 8, 9
    // Payload: 10

    ptr[0] = 0xFD;  // STX
    ptr[2] = 0;     // INC
    ptr[3] = 0;     // CMP
    ptr[4] = seq;
    ptr[5] = sysid;
    ptr[6] = compid;

    auto msgid = MessageT::MessageId;
    ptr[7] = msgid & 0xFF;
    ptr[8] = (msgid >> 8) & 0xFF;
    ptr[9] = (msgid >> 16) & 0xFF;

    // Payload serialization using PFR
    auto payload_start = ptr + 10;
    auto current_pos = payload_start;

    boost::pfr::for_each_field(message, [&](const auto& field) {
        const auto& val = get_value(field);
        using ValType = std::decay_t<decltype(val)>;

        auto val_bytes = reinterpret_cast<const std::uint8_t*>(&val);
        std::copy(val_bytes, val_bytes + sizeof(ValType), current_pos);
        current_pos += sizeof(ValType);
    });

    auto full_payload_len = static_cast<std::size_t>(current_pos - payload_start);

    // Zero truncation
    auto payload_len = full_payload_len;
    while (payload_len > 1 && payload_start[payload_len - 1] == 0) {
        payload_len--;
    }

    ptr[1] = static_cast<std::uint8_t>(payload_len);

    // CRC
    auto crc = std::uint16_t{0xFFFF};
    // CRC over header (excluding STX) and payload
    // Header len without STX is 9 bytes.
    crc_accumulate_buffer(crc, reinterpret_cast<const char*>(ptr + 1), 9);
    crc_accumulate_buffer(crc, reinterpret_cast<const char*>(payload_start), payload_len);

    crc_accumulate(MessageT::CrcExtra, crc);

    auto crc_pos = payload_start + payload_len;
    crc_pos[0] = crc & 0xFF;
    crc_pos[1] = (crc >> 8) & 0xFF;

    return 10 + payload_len + 2;  // Header + Payload + CRC
}

}  // namespace mavlink
