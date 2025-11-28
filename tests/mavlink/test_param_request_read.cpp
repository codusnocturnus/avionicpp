#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/param_request_read.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ParamRequestRead Serialization", "[mavlink][param_request_read]") {
    GIVEN("A populated ParamRequestRead message") {
        ParamRequestRead req;
        req.target_system.value = 1;
        req.target_component.value = 1;
        std::string id = "MY_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        req.param_id.value = param_id;
        req.param_index.value = -1;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(req, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());
                // Header 10 + Payload 20 + CRC 2 = 32
                // Payload: param_index(2) + target_system(1) + target_component(1) + param_id(16) = 20
                // Truncation: "MY_PARAM" is 8 chars.
                // param_id is char[16]. 8 non-zero, 8 zero.
                // param_id is sorted last (array).
                // So trailing 8 bytes are zeros.
                // Payload: 2+1+1 + 8 = 12 bytes.
                // Total: 10 + 12 + 2 = 24.
                REQUIRE(*res == 24);

                // param_id is offset 4 (after index, sys, comp)
                CHECK(buffer[10 + 4] == 'M');
            }
        }
    }
}

SCENARIO("ParamRequestRead Deserialization", "[mavlink][param_request_read]") {
    GIVEN("A buffer containing a ParamRequestRead payload") {
        ParamRequestRead req_in;
        req_in.target_system.value = 1;
        req_in.target_component.value = 1;
        std::string id = "MY_PARAM";
        std::array<char, 16> param_id = {};
        std::copy(id.begin(), id.end(), param_id.begin());
        req_in.param_id.value = param_id;
        req_in.param_index.value = -1;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(req_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 20;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ParamRequestRead>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& req = *res_d;
                CHECK(req.target_system.value == 1);
                CHECK(req.target_component.value == 1);
                CHECK(req.param_id.value[0] == 'M');
                CHECK(req.param_index.value == -1);
            }
        }
    }
}
