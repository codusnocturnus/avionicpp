#include <benchmark/benchmark.h>
#include <minmea.h>

#include <array>
#include <cstring>
#include <string>
#include <string_view>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/framer.hpp"
#include "nmea0183/payloads/gga.hpp"
#include "nmea0183/payloads/rmc.hpp"

using namespace std::string_view_literals;

static const char* GGA_MSG = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
static const char* RMC_MSG = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A\r\n";

// --- Helpers ---

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

// --- Benchmarks ---

static void BM_Minmea_GGA(benchmark::State& state) {
    struct minmea_sentence_gga frame;
    for (auto _ : state) {
        enum minmea_sentence_id id = minmea_sentence_id(GGA_MSG, false);
        if (id == MINMEA_SENTENCE_GGA) {
            minmea_parse_gga(&frame, GGA_MSG);
            benchmark::DoNotOptimize(frame);
        }
    }
}
BENCHMARK(BM_Minmea_GGA);

static void BM_Nmea0183_GGA(benchmark::State& state) {
    // Buffer for the framer
    std::array<char, 256> buffer;
    std::span<char> span(buffer);
    auto framer = nmea0183::create_framer(&span);
    std::string_view msg = GGA_MSG;

    for (auto _ : state) {
        // Push byte by byte to simulate stream
        for (char c : msg) {
            auto result = framer.push_byte(c);
            if (result) {
                if (result->has_value()) {
                    auto& view = result->value();
                    auto payload = nmea0183::bind<nmea0183::payloads::LazyGGA>(view);
                    benchmark::DoNotOptimize(payload);
                }
            }
        }
    }
}
BENCHMARK(BM_Nmea0183_GGA);

static void BM_Nmea0183_GGA_Bind(benchmark::State& state) {
    auto view =
        make_view("GP", "GGA",
                  {"123519", "4807.038", "N", "01131.000", "E", "1", "08", "0.9", "545.4", "M", "46.9", "M", "", ""});

    for (auto _ : state) {
        auto result = nmea0183::bind<nmea0183::payloads::LazyGGA>(view);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Nmea0183_GGA_Bind);

static void BM_Minmea_RMC(benchmark::State& state) {
    struct minmea_sentence_rmc frame;
    for (auto _ : state) {
        enum minmea_sentence_id id = minmea_sentence_id(RMC_MSG, false);
        if (id == MINMEA_SENTENCE_RMC) {
            minmea_parse_rmc(&frame, RMC_MSG);
            benchmark::DoNotOptimize(frame);
        }
    }
}
BENCHMARK(BM_Minmea_RMC);

static void BM_Nmea0183_RMC(benchmark::State& state) {
    std::array<char, 256> buffer;
    std::span<char> span(buffer);
    auto framer = nmea0183::create_framer(&span);
    std::string_view msg = RMC_MSG;

    for (auto _ : state) {
        for (char c : msg) {
            auto result = framer.push_byte(c);
            if (result) {
                if (result->has_value()) {
                    auto& view = result->value();
                    auto payload = nmea0183::bind<nmea0183::payloads::LazyRMC>(view);
                    benchmark::DoNotOptimize(payload);
                }
            }
        }
    }
}
BENCHMARK(BM_Nmea0183_RMC);

static void BM_Nmea0183_RMC_Bind(benchmark::State& state) {
    auto view = make_view("GP", "RMC",
                          {"123519", "A", "4807.038", "N", "01131.000", "E", "022.4", "084.4", "230394", "003.1", "W"});

    for (auto _ : state) {
        auto result = nmea0183::bind<nmea0183::payloads::LazyRMC>(view);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Nmea0183_RMC_Bind);

BENCHMARK_MAIN();
