#include "commons.hpp"
#include "floatops.hpp"
#include "interface.hpp"

int main()
{
    auto mat = nmg::mat3x3::identity();
    nmg::write_log("Hein!");
    nmg::reset_log();
    nmg::write_log("Whaouh!");
    nmg::draw_rect(0, 0, 320, 240, 0xFFFF);
    nmg::print_text(nmg::read_log().c_str(), 0, 0);
    nmg::wait_for_key_released();
    nmg::wait_for_key_pressed();
    return 0;
}