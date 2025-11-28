#pragma once

namespace uart {

enum class BaudRate {
    b0 = 0,
    b50,
    b75,
    b110,
    b134,
    b150,
    b200,
    b300,
    b600,
    b1200,
    b1800,
    b2400,
    b4800,
    b9600,
    b14400,
    b19200,
    b38400,
    b56000,
    b57600,
    b115200,
    b128000,
    b230400,
    b256000,
    b460800,
    b921600,
    b1000000,
    b2000000,
    b3000000,
    b4000000
};

enum class CharacterSize { cs5 = 0, cs6, cs7, cs8, cs16 };

enum class Parity { none = 0, even, odd, mark, space };

enum class StopBits { sb1 = 0, sb1_5, sb2 };

}  // namespace uart
