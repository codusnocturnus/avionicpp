#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/system_time.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("SystemTime Serialization", "[mavlink][system_time]") {
    GIVEN("A populated SystemTime message") {
        SystemTime st;
        st.time_unix_usec.value = 1672531200000000;  // 2023-01-01 00:00:00 UTC
        st.time_boot_ms.value = 0x12345678;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized with specific system and component IDs") {
            // SysID 1, CompID 1, Seq 0
            auto res = serialize(st, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 12 (8+4) + CRC 2 = 24
                REQUIRE(len == 24);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);  // STX
                    CHECK(buffer[1] == 12);    // LEN
                    CHECK(buffer[2] == 0);     // INC
                    CHECK(buffer[3] == 0);     // CMP
                    CHECK(buffer[4] == 0);     // SEQ
                    CHECK(buffer[5] == 1);     // SYS
                    CHECK(buffer[6] == 1);     // COMP
                    // MSGID 2
                    CHECK(buffer[7] == 2);
                    CHECK(buffer[8] == 0);
                    CHECK(buffer[9] == 0);
                }

                AND_THEN("the payload is correct") {
                    // time_unix_usec (LE) 1672531200000000 = 0x0005F128840D4000
                    CHECK(buffer[10] == 0x00);
                    CHECK(buffer[11] == 0x40);
                    CHECK(buffer[12] == 0x0D);
                    CHECK(buffer[13] == 0x84);
                    CHECK(buffer[14] == 0x28);
                    CHECK(buffer[15] == 0xF1);
                    CHECK(buffer[16] == 0x05);
                    CHECK(buffer[17] == 0x00);

                    // time_boot_ms (LE) 0x12345678
                    CHECK(buffer[18] == 0x78);
                    CHECK(buffer[19] == 0x56);
                    CHECK(buffer[20] == 0x34);
                    CHECK(buffer[21] == 0x12);
                }
            }
        }
    }
}

SCENARIO("SystemTime Deserialization", "[mavlink][system_time]") {
    GIVEN("A buffer containing a SystemTime payload") {
        std::array<std::uint8_t, 12> payload = {
            0x00, 0x40, 0x0D, 0x84, 0x28, 0xF1, 0x05, 0x00,  // time_unix_usec
            0x78, 0x56, 0x34, 0x12                           // time_boot_ms
        };

        MessageView view;
        view.msgid = 2;  // SystemTime
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<SystemTime>(view);

            THEN("the message matches the payload") {
                REQUIRE(res.has_value());
                const auto& st = *res;
                CHECK(st.time_unix_usec.value == 1672531200000000);
                CHECK(st.time_boot_ms.value == 0x12345678);
            }
        }
    }

    GIVEN("A truncated buffer (zero truncation)") {
        std::array<std::uint8_t, 8> payload = {
            0x00, 0x40, 0x0D, 0x84, 0x28,
            0xF1, 0x05, 0x00  // time_unix_usec only
            // time_boot_ms zeros implied
        };

        MessageView view;
        view.msgid = 2;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<SystemTime>(view);

            THEN("missing fields are zero-initialized") {
                REQUIRE(res.has_value());
                const auto& st = *res;
                CHECK(st.time_unix_usec.value == 1672531200000000);
                CHECK(st.time_boot_ms.value == 0);
            }
        }
    }
}
