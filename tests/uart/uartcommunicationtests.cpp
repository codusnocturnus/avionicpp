#include <catch2/catch_test_macros.hpp>
#include <fakeit.hpp>

#include <ranges>

#include "uart/uart.hpp"

#include "mocks/fakeuart.hpp"

SCENARIO("UART communication handles read operations", "[uart][communication][read]") {
    GIVEN("A UART interface with a mock implementation") {
        auto mockuart = MockUART{};
        auto puart = std::shared_ptr<FakeUART>(&mockuart.get());
        auto uut = uart::UART<FakeUART>(puart);
        auto buffer = std::array<std::uint8_t, 1024>{};

        WHEN("Reading fails in the underlying uart") {
            fakeit::When(Method(mockuart, read))
                .Return(std::unexpected<std::pair<int, std::string>>(std::make_pair(-1, "Error")));
            auto result = uut.read(buffer, 0);

            THEN("The error is propagated") {
                fakeit::Verify(Method(mockuart, read)).Once();
                CHECK(not result.has_value());
                CHECK(result.error().first == -1);
                CHECK(result.error().second == "Error");
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }

        WHEN("Reading succeeds in the underlying uart") {
            fakeit::When(Method(mockuart, read)).AlwaysDo([](auto& buffer, std::size_t readsize) {
                *buffer.data() = '4';
                *(buffer.data() + 1) = '2';
                return 2;
            });
            auto result = uut.read(buffer, 2);

            THEN("Success and data are returned") {
                fakeit::Verify(Method(mockuart, read)).Once();
                REQUIRE(result.has_value());
                CHECK(result.value() == 2U);
                CHECK(buffer[0] == '4');
                CHECK(buffer[1] == '2');
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }
    }
}

SCENARIO("UART communication handles write operations", "[uart][communication][write]") {
    GIVEN("A UART interface with a mock implementation") {
        auto mockuart = MockUART{};
        auto puart = std::shared_ptr<FakeUART>(&mockuart.get());
        auto uut = uart::UART<FakeUART>(puart);
        auto buffer = std::array<std::uint8_t, 1024>{1, 2, 3, 4, 0};

        WHEN("Writing fails in the underlying uart") {
            fakeit::When(Method(mockuart, write))
                .Return(std::unexpected<std::pair<int, std::string>>(std::make_pair(-1, "Error")));
            auto result = uut.write(std::span{buffer.data(), 4});

            THEN("The error is propagated") {
                fakeit::Verify(Method(mockuart, write)).Once();
                CHECK(not result.has_value());
                CHECK(result.error().first == -1);
                CHECK(result.error().second == "Error");
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }

        WHEN("Writing succeeds in the underlying uart") {
            fakeit::When(Method(mockuart, write)).Return(3);
            auto result = uut.write(std::span{buffer.data(), 3});

            THEN("Success and bytes written are returned") {
                fakeit::Verify(Method(mockuart, write).Matching([](const std::span<std::uint8_t>& buffer) {
                    return (std::ranges::size(buffer) == 3U) and (*std::ranges::cdata(buffer) == 1) and
                           (*(std::ranges::cdata(buffer) + 1) == 2) and (*(std::ranges::cdata(buffer) + 2) == 3);
                })).Once();
                REQUIRE(result.has_value());
                CHECK(result.value() == 3);
                fakeit::VerifyNoOtherInvocations(mockuart);
            }
        }
    }
}
