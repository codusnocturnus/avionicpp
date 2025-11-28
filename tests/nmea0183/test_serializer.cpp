#include <array>
#include <optional>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

// Include the library headers
#include "nmea0183/payloads/hdt.hpp"
#include "nmea0183/payloads/rot.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

// =========================================================
// Helpers
// =========================================================

// Helper to calculate NMEA checksum of a string (between $ and *) to verify test expectations
uint8_t calculate_checksum_ref(std::string_view content) {
    uint8_t sum = 0;
    for (char c : content) {
        sum ^= static_cast<uint8_t>(c);
    }
    return sum;
}

// Helper to verify a serialized buffer matches expected string
void REQUIRE_NMEA_MATCH(const std::array<char, 100>& buffer, size_t len, std::string expected) {
    std::string result(buffer.data(), len);
    REQUIRE(result == expected);
}

// =========================================================
// SCENARIO 1: TxField and Low-Level Types
// =========================================================

SCENARIO("TxField holds data and precision information", "[TxField]") {
    GIVEN("A TxField for a float with precision 1") {
        using RateField = nmea0183::TxField<float, 1>;
        RateField field;

        WHEN("Assigned a value") {
            field.value = 123.456f;

            THEN("It holds the value") {
                REQUIRE(field.value == 123.456f);
            }
            THEN("It exposes the correct precision constant") {
                REQUIRE(RateField::precision == 1);
            }
        }

        WHEN("Default constructed") {
            THEN("It is empty") {
                REQUIRE_FALSE(field.value.has_value());
            }
        }
    }

    GIVEN("A TxField for an Enum") {
        using StatusField = nmea0183::TxField<char>;
        StatusField field;

        WHEN("Assigned a value") {
            field.value = nmea0183::enumerations::StatusIndicator::Active;
            THEN("It holds the value") {
                REQUIRE(field.value == nmea0183::enumerations::StatusIndicator::Active);
            }
        }
    }
}

// (Specific message tests moved to test_nmea0183rot.cpp and test_nmea0183hdt.cpp)

// =========================================================
// SCENARIO 4: Buffer Handling and Edge Cases
// =========================================================

SCENARIO("Serializer handles buffer boundaries safely", "[Serializer][Safety]") {
    auto msg = nmea0183::Message<"GP", nmea0183::payloads::ROT>{
        .payload = {.rate_of_turn = {10.0f}, .status = {nmea0183::enumerations::StatusIndicator::Active}}};
    // Full msg: "$GPROT,10.0,A*1C\r\n" (Length 17)

    WHEN("Buffer is exactly large enough") {
        std::array<char, 18> exact_buffer;
        auto len = nmea0183::serialize(msg, exact_buffer);

        THEN("It writes successfully") {
            REQUIRE(len == 18);
            REQUIRE(exact_buffer[16] == '\r');
            REQUIRE(exact_buffer.back() == '\n');
        }
    }

    WHEN("Buffer is too small") {
        std::array<char, 10> small_buffer;
        auto len = nmea0183::serialize(msg, small_buffer);

        THEN("It fills the buffer up to its size") {
            REQUIRE(len == 10);
        }

        THEN("It truncates output safely without crashing") {
            std::string result(small_buffer.data(), 10);
            // Output would likely be "$GPROT,10."
            REQUIRE(result.starts_with("$GPROT"));
        }
    }
}

// =========================================================
// SCENARIO 5: Compile-Time Format String Verification
// =========================================================

SCENARIO("Message Format strings are generated correctly at compile time", "[Serializer][Format]") {
    // This test ensures that the 'fixed string' logic in Serializer.hpp
    // is correctly building the format string for std::format.

    GIVEN("The ROT message type") {
        using ROTMsg = nmea0183::Message<"GP", nmea0183::payloads::ROT>;

        WHEN("Inspecting the generated format string") {
            // Expected: "${}{},{},{}" (because we use custom formatter for all fields)
            constexpr auto fmt = ROTMsg::FULL_FMT.view();

            THEN("It matches the expected pattern") {
                REQUIRE(std::string(fmt) == "${}{},{},{}");
            }
        }
    }

    GIVEN("The HDT message type") {
        using HDTMsg = nmea0183::Message<"HE", nmea0183::payloads::HDT>;

        WHEN("Inspecting the generated format string") {
            // Expected: "${}{},{},{}"
            constexpr auto fmt = HDTMsg::FULL_FMT.view();

            THEN("It matches the expected pattern") {
                REQUIRE(std::string(fmt) == "${}{},{},{}");
            }
        }
    }
}

// =========================================================
// SCENARIO 6: Optional Fields (Empty)
// =========================================================

SCENARIO("Serializing messages with empty optional fields", "[Serializer][Optional]") {
    GIVEN("An ROT message with empty status") {
        std::array<char, 100> buffer;
        // Explicit nullopt for status
        auto msg = nmea0183::Message<"GP", nmea0183::payloads::ROT>{
            .payload = {.rate_of_turn = {35.5f}, .status = {std::nullopt}}};

        WHEN("Serialized") {
            auto len = serialize(msg, buffer);
            THEN("The empty field is represented as nothing between commas") {
                // Expected content: "GPROT,35.5,"
                // Checksum logic: XOR of "GPROT,35.5,"
                // We don't hardcode checksum here as it changes with emptiness
                std::string res(buffer.data(), len);
                REQUIRE(res.starts_with("$GPROT,35.5,*"));
            }
        }
    }

    GIVEN("An ROT message with all empty fields") {
        std::array<char, 100> buffer;
        auto msg = nmea0183::Message<"GP", nmea0183::payloads::ROT>{
            .payload = {.rate_of_turn = {std::nullopt}, .status = {std::nullopt}}};

        WHEN("Serialized") {
            auto len = serialize(msg, buffer);
            THEN("It produces empty fields") {
                // "GPROT,,"
                std::string res(buffer.data(), len);
                REQUIRE(res.starts_with("$GPROT,,*"));
            }
        }
    }
}
