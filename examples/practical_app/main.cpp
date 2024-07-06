#include <brightcpp/brightcpp.hpp>
#include <iostream>

int main() {
    try {
        bgt::window wnd{
            bgt::window::settings{
                .title = "BrightCpp Sample App",
            },
        };

        while (bgt::frame_update()) {
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
