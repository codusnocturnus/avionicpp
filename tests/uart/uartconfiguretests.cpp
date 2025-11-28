#include <catch2/catch_test_macros.hpp>
#include <fakeit.hpp>

#include <chrono>

#include "uart/settings.hpp"
#include "uart/uart.hpp"

#include "mocks/fakeuart.hpp"

SCENARIO("UART configuration is retrieved from underlying uart object", "[uart][configuration]") {
    GIVEN("A UART interface with a mock implementation") {
        auto mockuart = MockUART{};
        auto puart = std::shared_ptr<FakeUART>(&mockuart.get());
        auto uut = uart::UART<FakeUART>(puart);

        WHEN("Configuration parameters are requested") {
            fakeit::When(Method(mockuart, devicename)).Return("");
            fakeit::When(Method(mockuart, baudrate)).Return(uart::BaudRate::b9600);
            fakeit::When(Method(mockuart, charactersize)).Return(uart::CharacterSize::cs8);
            fakeit::When(Method(mockuart, parity)).Return(uart::Parity::none);
            fakeit::When(Method(mockuart, stopbits)).Return(uart::StopBits::sb1);
            fakeit::When(Method(mockuart, timeout)).Return(std::chrono::milliseconds(42));

            THEN("The underlying getters are called") {
                CHECK(uut.devicename().empty());
                fakeit::Verify(Method(mockuart, devicename)).Once();
                CHECK(uut.baudrate() == uart::BaudRate::b9600);
                fakeit::Verify(Method(mockuart, baudrate)).Once();
                CHECK(uut.charactersize() == uart::CharacterSize::cs8);
                fakeit::Verify(Method(mockuart, charactersize)).Once();
                CHECK(uut.parity() == uart::Parity::none);
                fakeit::Verify(Method(mockuart, parity)).Once();
                CHECK(uut.stopbits() == uart::StopBits::sb1);
                fakeit::Verify(Method(mockuart, stopbits)).Once();
                CHECK(uut.timeout() == std::chrono::milliseconds(42));
                fakeit::Verify(Method(mockuart, timeout)).Once();

                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }
    }
}

SCENARIO("UART configuration is passed to the underlying uart object", "[uart][configuration]") {
    GIVEN("A UART interface with a mock implementation") {
        auto mockuart = MockUART{};
        auto puart = std::shared_ptr<FakeUART>(&mockuart.get());
        auto uut = uart::UART<FakeUART>(puart);

        WHEN("Configuration parameters are set") {
            fakeit::When(Method(mockuart, set_baudrate)).Return(true);
            fakeit::When(Method(mockuart, set_charactersize)).Return(true);
            fakeit::When(Method(mockuart, set_parity)).Return(true);
            fakeit::When(Method(mockuart, set_stopbits)).Return(true);
            fakeit::When(Method(mockuart, set_timeout)).Return(true);

            auto res_baud = uut.set_baudrate(uart::BaudRate::b57600);
            auto res_char = uut.set_charactersize(uart::CharacterSize::cs6);
            auto res_par = uut.set_parity(uart::Parity::odd);
            auto res_stop = uut.set_stopbits(uart::StopBits::sb2);
            auto res_time = uut.set_timeout(std::chrono::milliseconds(42));

            THEN("The underlying setters are called with correct values and return success") {
                fakeit::Verify(Method(mockuart, set_baudrate).Using(uart::BaudRate::b57600)).Once();
                CHECK(res_baud.has_value());
                fakeit::Verify(Method(mockuart, set_charactersize).Using(uart::CharacterSize::cs6)).Once();
                CHECK(res_char.has_value());
                fakeit::Verify(Method(mockuart, set_parity).Using(uart::Parity::odd)).Once();
                CHECK(res_par.has_value());
                fakeit::Verify(Method(mockuart, set_stopbits).Using(uart::StopBits::sb2)).Once();
                CHECK(res_stop.has_value());
                fakeit::Verify(Method(mockuart, set_timeout).Using(std::chrono::milliseconds(42))).Once();
                CHECK(res_time.has_value());

                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }
    }
}
