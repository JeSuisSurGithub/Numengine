#include "commons.hpp"

namespace nmg {
    // ARMv7-none-eabi doesn't support global ctor/dtor
    std::string* DEBUG_LOG = nullptr;

    void write_log(const std::string& str) {
        if (DEBUG_LOG->length() + str.length() + 1 < DEBUG_LOG_LEN) {
            DEBUG_LOG->append(str);
            DEBUG_LOG->push_back('\n');
        } else {
            reset_log();
        }
    }

    void reset_log() {
        DEBUG_LOG->clear();
    }

    const std::string& read_log() {
        return (*DEBUG_LOG);
    }
}