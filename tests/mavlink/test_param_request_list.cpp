#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/param_request_list.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ParamRequestList Serialization", "[mavlink][param_request_list]") {
    GIVEN("A populated ParamRequestList message") {
        ParamRequestList req;
        req.target_system.value = 1;
        req.target_component.value = 1;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(req, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());
                // Header 10 + Payload 2 + CRC 2 = 14
                REQUIRE(*res == 14);

                CHECK(buffer[10] == 1);
                CHECK(buffer[11] == 1);
            }
        }
    }
}

SCENARIO("ParamRequestList Deserialization", "[mavlink][param_request_list]") {
    GIVEN("A buffer containing a ParamRequestList payload") {
        ParamRequestList req_in;
        req_in.target_system.value = 1;
        req_in.target_component.value = 1;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(req_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 21;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ParamRequestList>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& req = *res_d;
                CHECK(req.target_system.value == 1);
                CHECK(req.target_component.value == 1);
            }
        }
    }
}
