#ifndef COMMONS_HPP
#define COMMONS_HPP

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>

namespace nmg {
    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using usz = size_t;

    using i16 = int16_t;
    using i32 = int32_t;

    constexpr usz DEBUG_LOG_LEN = 1024;
    extern std::string* DEBUG_LOG;

    void write_log(const std::string& str);
    void reset_log();
    const std::string& read_log();
}

#endif /* COMMONS_HPP */