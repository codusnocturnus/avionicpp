#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/change_operator_control.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ChangeOperatorControl Serialization", "[mavlink][change_operator_control]") {
    GIVEN("A populated ChangeOperatorControl message") {
        ChangeOperatorControl coc;
        coc.target_system.value = 1;
        coc.control_request.value = 0;
        coc.version.value = 0;
        std::array<char, 25> passkey = {};
        std::string key = "my_secret_key";
        std::copy(key.begin(), key.end(), passkey.begin());
        coc.passkey.value = passkey;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(coc, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 28 + CRC 2 = 40
                // Truncated: passkey has trailing zeros. "my_secret_key" is 13 bytes.
                // Remaining 12 bytes of passkey are zero.
                // Payload size: 3 (fields) + 13 (passkey) = 16.
                // Total: 10 + 16 + 2 = 28.
                REQUIRE(len == 28);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 16);  // LEN
                    CHECK(buffer[7] == 5);   // MSGID 5
                }

                AND_THEN("the payload is correct") {
                    // target_system (offset 0)
                    CHECK(buffer[10] == 1);

                    // passkey (offset 3)
                    CHECK(buffer[10 + 3] == 'm');
                    CHECK(buffer[10 + 4] == 'y');
                }
            }
        }
    }
}

SCENARIO("ChangeOperatorControl Deserialization", "[mavlink][change_operator_control]") {
    GIVEN("A buffer containing a ChangeOperatorControl payload") {
        ChangeOperatorControl coc_in;
        coc_in.target_system.value = 1;
        coc_in.control_request.value = 0;
        coc_in.version.value = 0;
        std::array<char, 25> passkey = {};
        std::string key = "my_secret_key";
        std::copy(key.begin(), key.end(), passkey.begin());
        coc_in.passkey.value = passkey;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(coc_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 5;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ChangeOperatorControl>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& coc = *res_d;
                CHECK(coc.target_system.value == 1);
                CHECK(coc.passkey.value[0] == 'm');
                CHECK(coc.passkey.value[1] == 'y');
            }
        }
    }
}
