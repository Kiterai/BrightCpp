#include <brightcpp/brightcpp.hpp>
#include <brightcpp/renderer2d.hpp>
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

        bgt::renderer2d r(wnd1);

        bgt::image img{"examples/assets/test.png"};

        while (bgt::frame_update()) {
            r.draw(img, 50, 50);
            r.draw(img, 100, 100);
            r.draw(img, 150, 150);
            r.flush();
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
