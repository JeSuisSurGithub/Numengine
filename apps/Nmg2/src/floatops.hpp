#ifndef FLOATOPS_HPP
#define FLOATOPS_HPP

#include "commons.hpp"

#include <array>

namespace nmg {
    float deg2rad(float degree);

    constexpr float PI = 3.14159f;

    class vec2 {
        private:
            std::array<float, 2> data;

        public:
            vec2(std::array<float, 2> init);
            ~vec2();
    };

    class mat3x3 {
        private:
            std::array<std::array<float, 3>, 3> data;

        public:
            mat3x3(std::array<std::array<float, 3>, 3> init);
            ~mat3x3();

            static mat3x3 identity();
            static mat3x3 rotation(float theta, vec2 pivot = vec2({0, 0}));
            static mat3x3 translation(vec2 translation);
            static mat3x3 scale(vec2 scale);
    };
}
#endif /* FLOATOPS_HPP */