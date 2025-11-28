#include <catch2/catch_test_macros.hpp>
#include <fakeit.hpp>

#include "uart/uart.hpp"

#include "mocks/fakeuart.hpp"

SCENARIO("UART passes calls to open, is_open, and close to the underlying uart", "[uart][access]") {
    GIVEN("A UART interface with a mock implementation") {
        auto mockuart = MockUART{};
        auto puart = std::shared_ptr<FakeUART>(&mockuart.get());
        auto uut = uart::UART<FakeUART>(puart);

        WHEN("The UART is opened") {
            fakeit::When(Method(mockuart, open)).Return(true);
            auto result = uut.open();

            THEN("The underlying open method is called") {
                fakeit::Verify(Method(mockuart, open)).Once();
                CHECK(result.has_value());
                CHECK(result.value());
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }

        WHEN("The UART status is checked") {
            fakeit::When(Method(mockuart, is_open)).Return(true);
            auto result = uut.is_open();

            THEN("The underlying is_open method is called") {
                fakeit::Verify(Method(mockuart, is_open)).Once();
                CHECK(result);
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }

        WHEN("The UART is closed") {
            fakeit::Fake(Method(mockuart, close));
            uut.close();

            THEN("The underlying close method is called") {
                fakeit::Verify(Method(mockuart, close)).Once();
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }
    }
}
