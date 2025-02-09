#include <brightcpp/brightcpp.hpp>
#include <brightcpp/renderer2d.hpp>
#include <brightcpp/audio.hpp>
#include <iostream>

int main() {
    try {
        bgt::system_initializer sys;

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

        // bgt::window wnd2{
        //     bgt::window::settings{
        //         .size = {
        //             .w = 300,
        //             .h = 400,
        //         },
        //         .is_resizable = false,
        //         .title = "Sample App Window 2",
        //     },
        // };

        bgt::image img{"examples/assets/test.png"};
        auto imgclip = img.clip(0, 0, 32, 32);

        bgt::renderer2d r(wnd1);

        bgt::audio test_sound("examples/assets/test.wav");

        bgt::audio_player player(test_sound);

        player.play_loop();

        while (bgt::frame_update()) {
            r.draw(img, 0, 100);
            r.draw(img, 100, 100, 1.57f);
            r.draw(img, 200, 100, 3.14f);
            r.draw(img, 0, 200, bgt::pivot_pos::center);
            r.draw(img, 100, 200, 1.57f, bgt::pivot_pos::center);
            r.draw(img, 200, 200, 3.14f, bgt::pivot_pos::center);
            r.draw(imgclip, 0, 0);
            r.flush();
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
