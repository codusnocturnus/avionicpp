#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/param_value.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("ParamValue Serialization", "[mavlink][param_value]") {
    GIVEN("A populated ParamValue message") {
        ParamValue val;
        std::string id = "TEST_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        val.param_id.value = param_id;
        val.param_value.value = 123.456f;
        val.param_type.value = MavParamType::REAL32;
        val.param_count.value = 100;
        val.param_index.value = 5;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(val, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());
                // Header 10 + Payload 25 + CRC 2 = 37
                // Payload: value(4) + count(2) + index(2) + type(1) + id(16) = 25
                REQUIRE(*res == 37);

                // param_id is offset 8 (after value, count, index)
                // value(4) + count(2) + index(2) = 8
                CHECK(buffer[10 + 8] == 'T');
            }
        }
    }
}

SCENARIO("ParamValue Deserialization", "[mavlink][param_value]") {
    GIVEN("A buffer containing a ParamValue payload") {
        ParamValue val_in;
        std::string id = "TEST_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        val_in.param_id.value = param_id;
        val_in.param_value.value = 123.456f;
        val_in.param_type.value = MavParamType::REAL32;
        val_in.param_count.value = 100;
        val_in.param_index.value = 5;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(val_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        // param_id is sorted last in wire (array).
        // "TEST_PARAM" is 10 chars.
        // Trailing zeros in param_id.
        // param_id length 16. 10 non-zero, 6 zero.
        // Truncation removes trailing zeros.
        // Payload size: 25 - 6 = 19 bytes.
        // Serialized length: 10 + 19 + 2 = 31.
        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 22;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ParamValue>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& val = *res_d;
                CHECK_THAT(val.param_value.value, Catch::Matchers::WithinRel(123.456f));
                CHECK(val.param_count.value == 100);
                CHECK(val.param_index.value == 5);
                CHECK(val.param_type.value == MavParamType::REAL32);
                CHECK(val.param_id.value[0] == 'T');
            }
        }
    }
}
