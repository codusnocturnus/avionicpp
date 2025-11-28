#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/auth_key.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("AuthKey Serialization", "[mavlink][auth_key]") {
    GIVEN("A populated AuthKey message") {
        AuthKey ak;
        std::string key = "my_secret_key_32_bytes_long_xx";  // 31 chars + 1
        std::array<char, 32> key_arr = {};
        std::copy(key.begin(), key.end(), key_arr.begin());
        ak.key.value = key_arr;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(ak, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 32 + CRC 2 = 44
                // Truncated: key has 2 trailing zeros (length 30).
                // Payload size: 30.
                // Total: 10 + 30 + 2 = 42.
                REQUIRE(len == 42);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 30);  // LEN
                    CHECK(buffer[7] == 7);   // MSGID 7
                }

                AND_THEN("the payload is correct") {
                    CHECK(buffer[10] == 'm');
                }
            }
        }
    }
}

SCENARIO("AuthKey Deserialization", "[mavlink][auth_key]") {
    GIVEN("A buffer containing a AuthKey payload") {
        AuthKey ak_in;
        std::string key = "my_secret_key";
        std::array<char, 32> key_arr = {};
        std::copy(key.begin(), key.end(), key_arr.begin());
        ak_in.key.value = key_arr;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(ak_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        // Truncation note: "my_secret_key" is 13 chars. rest 0.
        // Payload size: 13 bytes non-zero.
        // Serialized length will be 10 + 13 + 2 = 25.
        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 7;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<AuthKey>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& ak = *res_d;
                CHECK(ak.key.value[0] == 'm');
                CHECK(ak.key.value[1] == 'y');
            }
        }
    }
}
