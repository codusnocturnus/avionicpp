#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/param_set.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("ParamSet Serialization", "[mavlink][param_set]") {
    GIVEN("A populated ParamSet message") {
        ParamSet set;
        set.target_system.value = 1;
        set.target_component.value = 1;
        std::string id = "SET_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        set.param_id.value = param_id;
        set.param_value.value = 42.0f;
        set.param_type.value = MavParamType::REAL32;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(set, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());
                // Header 10 + Payload 23 + CRC 2 = 35
                // Payload: value(4) + target_sys(1) + target_comp(1) + type(1) + id(16) = 23
                REQUIRE(*res == 35);

                // param_id offset: value(4) + sys(1) + comp(1) = 6
                CHECK(buffer[10 + 6] == 'S');
            }
        }
    }
}

SCENARIO("ParamSet Deserialization", "[mavlink][param_set]") {
    GIVEN("A buffer containing a ParamSet payload") {
        ParamSet set_in;
        set_in.target_system.value = 1;
        set_in.target_component.value = 1;
        std::string id = "SET_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        set_in.param_id.value = param_id;
        set_in.param_value.value = 42.0f;
        set_in.param_type.value = MavParamType::REAL32;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(set_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        // param_id "SET_PARAM" is 9 chars.
        // Trailing 7 bytes zero.
        // Truncation removes 7 zeros.
        // Payload size: 23 - 7 = 16.
        // Total: 10 + 16 + 2 = 28.
        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 23;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ParamSet>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& set = *res_d;
                CHECK_THAT(set.param_value.value, Catch::Matchers::WithinRel(42.0f));
                CHECK(set.target_system.value == 1);
                CHECK(set.target_component.value == 1);
                CHECK(set.param_type.value == MavParamType::REAL32);
                CHECK(set.param_id.value[0] == 'S');
            }
        }
    }
}
