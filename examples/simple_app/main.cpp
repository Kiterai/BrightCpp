#include <brightcpp/brightcpp.hpp>
#include <iostream>

int main() {
    try {
        bgt::system_initializer sys;

        bgt::window wnd;

        while (bgt::frame_update()) {
            
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
