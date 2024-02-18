#ifndef FLOATOPS_HPP
#define FLOATOPS_HPP

#include "commons.hpp"

#include <array>

namespace nsp {
    float deg2rad(float degree);

    constexpr float PI = 3.14159f;

    class mat3x3 {
        private:
            std::array<std::array<float, 3>, 3> data;

        public:
            mat3x3(std::array<std::array<float, 3>, 3> init);
            ~mat3x3();

            static mat3x3 identity();
    };
}
#endif /* FLOATOPS_HPP */