#include "commons.hpp"
#include "floatops.hpp"
#include "interface.hpp"

int main()
{
    auto mat = nsp::mat3x3::identity();
    nsp::write_log("Hein!");
    nsp::reset_log();
    nsp::write_log("Whaouh!");
    nsp::draw_rect(0, 0, 320, 240, 0xFFFF);
    nsp::print_text(nsp::read_log().c_str(), 0, 0);
    nsp::wait_for_key_released();
    nsp::wait_for_key_pressed();
    return 0;
}