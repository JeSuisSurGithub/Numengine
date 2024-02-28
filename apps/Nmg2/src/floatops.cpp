#include "floatops.hpp"

namespace nmg {
	float deg2rad(float degree)
	{
		return degree * (PI / 180.0f);
	}

	mat3x3::mat3x3(std::array<std::array<float, 3>, 3> init)
	:
	data(init)
	{
	}

	mat3x3::~mat3x3() {}

	mat3x3 mat3x3::identity() {
        return mat3x3({{{1.0f, 0.0f, 0.0f},
                        {0.0f, 1.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f}}});
    }
}