#include <brightcpp/audio.hpp>
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

        bgt::audio se_powa("examples/assets/powa.wav");
        bgt::oneshot_audio_player se_player;

        bgt::streaming_audio test_sound("examples/assets/test.mp3");
        bgt::audio_player player(test_sound);

        player.play_loop();

        while (bgt::frame_update()) {
            if (bgt::key_z.pressed()) {
                se_player.play_oneshot(se_powa);
            }
            r.draw(img, 0, 0);

            r.flush();
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
