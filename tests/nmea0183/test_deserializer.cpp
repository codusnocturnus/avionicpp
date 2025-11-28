#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// Include the library headers
#include "nmea0183//payloads/rot.hpp"
#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/hdt.hpp"

// =========================================================
// Helpers
// =========================================================

// Helper to create a view for testing without a full framer
nmea0183::MessageView make_view(std::string_view talker,
                                std::string_view type,
                                std::initializer_list<std::string_view> args) {
    nmea0183::MessageView view;
    view.talker_id = talker;
    view.message_type = type;
    size_t i = 0;
    for (auto arg : args) {
        if (i < nmea0183::MessageView::MaxFields) {
            view.fields[i++] = arg;
        }
    }
    view.field_count = i;
    return view;
}

// Define a Registry for the Dispatcher tests
using TestRegistry = nmea0183::Dispatcher<nmea0183::MessageHandler<"ROT", nmea0183::payloads::LazyROT>,
                                          nmea0183::MessageHandler<"HDT", nmea0183::payloads::LazyHDT> >;

// =========================================================
// SCENARIO 1: Low-Level RxField Behavior
// =========================================================

SCENARIO("RxField handles lazy parsing and optionality", "[RxField]") {
    GIVEN("An RxField<float>") {
        // Direct instantiation of the wrapper used in LazyROT
        nmea0183::RxField<float> field;

        WHEN("Initialized with a valid float string") {
            field.token = "123.456";

            THEN("value() parses it correctly") {
                auto result = field.value();
                REQUIRE(result.has_value());   // Expected<...> is valid
                REQUIRE(result->has_value());  // Optional<...> is valid
                REQUIRE(**result == Catch::Approx(123.456f));
            }
        }

        WHEN("Initialized with an empty string (NMEA null field)") {
            field.token = "";  // Represents ",,"

            THEN("value() returns a valid nullopt") {
                auto result = field.value();
                REQUIRE(result.has_value());         // Parse was "successful" (empty is valid)
                REQUIRE_FALSE(result->has_value());  // But it contains no value
            }
        }

        WHEN("Initialized with garbage data") {
            field.token = "NOT_A_NUMBER";

            THEN("value() returns a ParseError") {
                auto result = field.value();
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == nmea0183::NMEAError::ParseError);
            }
        }
    }

    GIVEN("An RxField<char>") {
        nmea0183::RxField<char> field;

        WHEN("Initialized with a valid char 'A'") {
            field.token = "A";
            THEN("It casts to the Enum") {
                auto result = field.value();
                REQUIRE(result.has_value());
                REQUIRE(**result == nmea0183::enumerations::StatusIndicator::Active);
            }
        }

        WHEN("Initialized with a valid char 'V'") {
            field.token = "V";
            THEN("It casts to the Enum") {
                auto result = field.value();
                REQUIRE(result.has_value());
                REQUIRE(**result == nmea0183::enumerations::StatusIndicator::Void);
            }
        }
    }
}

// =========================================================
// SCENARIO 2: Binding Views to Aggregates
// =========================================================

SCENARIO("Binding NmeaMessageViews to Lazy Types", "[Binder]") {
    GIVEN("A view representing a standard ROT message") {
        auto view = make_view("GP", "ROT", {"35.5", "A"});

        WHEN("Binding to LazyROT") {
            auto result = bind<nmea0183::payloads::LazyROT>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());

                nmea0183::payloads::LazyROT& rot = *result;

                // Check token assignment (Zero Copy verification)
                REQUIRE(std::string(rot.rate_of_turn.token) == "35.5");
                REQUIRE(std::string(rot.status.token) == "A");

                // Check Parsing
                REQUIRE(**rot.rate_of_turn.value() == Catch::Approx(35.5f));
                REQUIRE(**rot.status.value() == nmea0183::enumerations::StatusIndicator::Active);
            }
        }
    }

    GIVEN("A view with missing trailing fields") {
        // ROT expects 2 fields, we give 1
        auto view = make_view("GP", "ROT", {"10.0"});

        WHEN("Binding to LazyROT") {
            auto result = bind<nmea0183::payloads::LazyROT>(view);
            REQUIRE(result.has_value());
            nmea0183::payloads::LazyROT& rot = *result;

            THEN("Present fields are valid") {
                REQUIRE(**rot.rate_of_turn.value() == Catch::Approx(10.0f));
            }

            THEN("Missing fields are treated as empty/nullopt") {
                auto status = rot.status.value();
                REQUIRE(status.has_value());         // Parse OK
                REQUIRE_FALSE(status->has_value());  // Empty
            }
        }
    }
}

// =========================================================
// SCENARIO 3: Dispatcher Integration
// =========================================================

SCENARIO("Dispatcher Error Handling", "[Dispatcher]") {
    GIVEN("A populated Registry") {
        // (Success cases moved to message-specific test files)

        WHEN("Dispatching an unknown message ID") {
            auto view = make_view("GP", "UNK", {"1", "2"});

            bool handled =
                TestRegistry::dispatch(view, nmea0183::overloaded{[](auto) { FAIL("Should not be called"); }});

            THEN("Dispatch returns false") {
                REQUIRE(handled == false);
            }
        }
    }
}
