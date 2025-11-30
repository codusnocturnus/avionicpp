#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "uart/posixuart.hpp"

SCENARIO("PosixUART Communication via PTY", "[uart][posix]") {
    GIVEN("A PTY pair") {
        // Create a pseudo-terminal pair manually
        int master_fd = posix_openpt(O_RDWR | O_NOCTTY);
        REQUIRE(master_fd != -1);

        if (grantpt(master_fd) == -1 || unlockpt(master_fd) == -1) {
            close(master_fd);
            FAIL("Failed to setup PTY");
        }

        char* pts_name = ptsname(master_fd);
        REQUIRE(pts_name != nullptr);

        uart::PosixUART serial(pts_name);

        WHEN("Opening the serial port") {
            auto result = serial.open();

            THEN("It opens successfully") {
                if (!result) {
                    FAIL("Failed to open serial: " << result.error().second);
                }
                REQUIRE(result.has_value());
                CHECK(serial.is_open());
                CHECK(serial.native_handle() > 0);
            }

            AND_WHEN("Configuring parameters") {
                auto res_baud = serial.set_baudrate(uart::BaudRate::b115200);

                THEN("Baudrate configuration succeeds") {
                    if (!res_baud) {
                        UNSCOPED_INFO("Baud error: " << res_baud.error().first << " " << res_baud.error().second);
                    }
                    CHECK(res_baud.has_value());
                }
                CHECK(serial.baudrate() == uart::BaudRate::b115200);

                // Note: Parity/CharSize/StopBits might fail on PTY, so we check or warn
                auto res_par = serial.set_parity(uart::Parity::even);
                if (!res_par) {
                    WARN("Parity configuration failed on PTY: " << res_par.error().second);
                }
                // Verify getter returns the set value (state is updated even if configure fails)
                CHECK(serial.parity() == uart::Parity::even);

                auto res_char = serial.set_charactersize(uart::CharacterSize::cs8);
                if (!res_char) {
                    WARN("CharacterSize configuration failed on PTY: " << res_char.error().second);
                }
                CHECK(serial.charactersize() == uart::CharacterSize::cs8);

                auto res_stop = serial.set_stopbits(uart::StopBits::sb1);
                if (!res_stop) {
                    WARN("StopBits configuration failed on PTY: " << res_stop.error().second);
                }
                CHECK(serial.stopbits() == uart::StopBits::sb1);
            }

            AND_WHEN("Setting invalid baudrate") {
                auto result = serial.set_baudrate(static_cast<uart::BaudRate>(9999));
                THEN("It returns error") {
                    CHECK_FALSE(result.has_value());
                    if (!result)
                        CHECK(result.error().first == EINVAL);
                }
            }

            AND_WHEN("Setting invalid charactersize") {
                auto result = serial.set_charactersize(static_cast<uart::CharacterSize>(99));
                THEN("It returns error") {
                    CHECK_FALSE(result.has_value());
                    if (!result)
                        CHECK(result.error().first == EINVAL);
                }
            }

            AND_WHEN("Reading and Writing data") {
                // Write to Serial (which writes to slave PTY) -> Read from Master FD
                std::array<uint8_t, 5> tx_buf = {0x01, 0x02, 0x03, 0x04, 0x05};
                auto write_res = serial.write(tx_buf);

                THEN("Write succeeds") {
                    REQUIRE(write_res.has_value());
                    CHECK(*write_res == 5);

                    uint8_t rx_raw[10];
                    ssize_t n = ::read(master_fd, rx_raw, sizeof(rx_raw));
                    REQUIRE(n == 5);
                    CHECK(rx_raw[0] == 0x01);
                    CHECK(rx_raw[4] == 0x05);
                }

                // Write to Master FD -> Read from Serial
                uint8_t master_tx[] = {0xAA, 0xBB, 0xCC};
                ::write(master_fd, master_tx, 3);

                std::array<uint8_t, 10> rx_buf;
                auto read_res = serial.read(rx_buf, 3);

                THEN("Read succeeds") {
                    REQUIRE(read_res.has_value());
                    CHECK(*read_res == 3);
                    CHECK(rx_buf[0] == 0xAA);
                    CHECK(rx_buf[2] == 0xCC);
                }
            }

            AND_WHEN("Setting timeout") {
                auto duration = std::chrono::milliseconds(100);
                serial.set_timeout(duration);
                THEN("It is set correctly") {
                    CHECK(serial.timeout() == duration);
                }
            }

            AND_WHEN("Setting invalid parity") {
                auto result = serial.set_parity(static_cast<uart::Parity>(99));
                THEN("It returns error") {
                    CHECK_FALSE(result.has_value());
                    if (!result) {
                        CHECK(result.error().first == EINVAL);
                    }
                }
            }

            AND_WHEN("Opening again") {
                auto result = serial.open();
                THEN("It closes and re-opens successfully") {
                    REQUIRE(result.has_value());
                    CHECK(serial.is_open());
                }
            }

            AND_WHEN("Closing the port") {
                serial.close();
                THEN("It is closed") {
                    CHECK_FALSE(serial.is_open());
                    // Reading from closed port should fail
                    std::array<uint8_t, 1> buf;
                    auto read_res = serial.read(buf, 1);
                    CHECK_FALSE(read_res.has_value());
                    if (!read_res) {
                        CHECK(read_res.error().first == EBADF);
                    }

                    auto write_res = serial.write(buf);
                    CHECK_FALSE(write_res.has_value());
                    if (!write_res) {
                        CHECK(write_res.error().first == EBADF);
                    }
                }
            }
        }

        WHEN("Attempting to open a non-existent port") {
            uart::PosixUART bad_serial("/dev/this/does/not/exist");
            auto result = bad_serial.open();

            THEN("It fails") {
                CHECK_FALSE(result.has_value());
                CHECK_FALSE(bad_serial.is_open());
            }
        }

        WHEN("Configuring a closed port") {
            uart::PosixUART closed_serial(pts_name);

            THEN("It is closed") {
                CHECK_FALSE(closed_serial.is_open());
            }

            THEN("Configuration returns false (success=false)") {
                auto result = closed_serial.set_baudrate(uart::BaudRate::b9600);
                CHECK(result.has_value());
                if (result.has_value()) {
                    CHECK(result.value() == false);
                }
            }

            THEN("Reading returns error (EBADF because uninitialized fd is -1)") {
                std::array<uint8_t, 1> buf;
                auto read_res = closed_serial.read(buf, 1);
                CHECK_FALSE(read_res.has_value());
                if (!read_res) {
                    // EBADF because we initialized to -1
                    CHECK(read_res.error().first == EBADF);
                }
            }
        }

        serial.close();
        close(master_fd);
    }
}
