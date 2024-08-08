#include <brightcpp/brightcpp.hpp>
#include <iostream>

int main() {
    try {
        bgt::window wnd1{
            bgt::window::settings{
                .size = {
                    .w = 600,
                    .h = 400,
                },
                .is_resizable = true,
                .title = "Sample App Window 1",
            },
        };
        
        bgt::window wnd2{
            bgt::window::settings{
                .size = {
                    .w = 300,
                    .h = 400,
                },
                .is_resizable = false,
                .title = "Sample App Window 2",
            },
        };

        while (bgt::frame_update()) {
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
