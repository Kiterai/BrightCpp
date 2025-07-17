#include <brightcpp/audio.hpp>
#include <brightcpp/brightcpp.hpp>
#include <brightcpp/renderer2d.hpp>
#include <chrono>
#include <iostream>
#include <thread>

bool blocks_map[15][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int main() {
    try {
        bgt::audio jump_se{"examples/assets/powa.wav"};
        bgt::oneshot_audio_player se_player;

        bgt::image game_img{"examples/assets/game.png"};
        auto player_img = game_img.clip(0, 0, 32, 32);
        auto block_img = game_img.clip(32, 0, 32, 32);
        auto flag_img = game_img.clip(64, 0, 32, 32);
        auto clear_img = game_img.clip(0, 32, 96, 32);

        bgt::window mainWindow{
            bgt::window::settings{
                .size = {640, 480},
                .title = "Sample Game",
            }};

        bgt::renderer2d r(mainWindow);

        int fx = 6 * 32, fy = 4 * 32;

        int x, y, vx, vy;
        bool onground;
        bool cleared;

        auto game_reset = [&]() {
            x = 4 * 32;
            y = 11 * 32;
            vx = 0;
            vy = 0;
            onground = false;
            cleared = false;
        };

        game_reset();

        // for fps adjust
        auto base = std::chrono::system_clock::now();
        int count = 0;
        constexpr int fps = 60;

        while (bgt::frame_update()) {
            if (!cleared) {
                // action
                if (bgt::key_left.pressing()) {
                    vx = -6;
                } else if (bgt::key_right.pressing()) {
                    vx = +6;
                } else {
                    vx = 0;
                }
                if (bgt::key_space.pressed() && onground) {
                    vy = -15;
                    se_player.play_oneshot(jump_se);
                }

                // collision
                x += vx;
                if (blocks_map[y / 32][x / 32] || blocks_map[(y + 31) / 32][x / 32])
                    x = ((x / 32) + 1) * 32;
                if (blocks_map[y / 32][(x + 31) / 32] || blocks_map[(y + 31) / 32][(x + 31) / 32])
                    x = (((x + 31) / 32) - 1) * 32;
                vy += 1;
                y += vy;
                if (blocks_map[y / 32][x / 32] || blocks_map[y / 32][(x + 31) / 32]) {
                    vy = -vy;
                    y = ((y / 32) + 1) * 32;
                }
                if (blocks_map[(y + 31) / 32][x / 32] || blocks_map[(y + 31) / 32][(x + 31) / 32]) {
                    vy = 0;
                    y = (((y + 31) / 32) - 1) * 32;
                    onground = true;
                } else {
                    onground = false;
                }

                if (std::max(x, fx) < std::min(x + 31, fx + 31) && std::max(y, fy) < std::min(y + 31, fy + 31)) {
                    cleared = true;
                }
            } else {
                if (bgt::key_r.pressed()) {
                    game_reset();
                }
            }

            // render
            r.draw(flag_img, fx, fy);
            r.draw(player_img, x, y);
            for (int yi = 0; yi < 15; yi++)
                for (int xi = 0; xi < 20; xi++)
                    if (blocks_map[yi][xi])
                        r.draw(block_img, xi * 32, yi * 32);
            if (cleared)
                r.draw(clear_img, 272, 224);
            r.flush();

            // fps adjust
            auto now = std::chrono::system_clock::now();
            std::this_thread::sleep_for(base + std::chrono::microseconds(1'000'000 * count / fps) - now);
            count++;
            if (count >= 60) {
                count = 0;
                base = now;
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}