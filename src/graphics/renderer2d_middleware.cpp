#pragma once

#include "renderer2d_middleware.hpp"
#include "../interfaces/rendertarget_factory.hpp"
#include "../interfaces/texture_factory.hpp"
#include "shader_input.hpp"

BRIGHTCPP_START

namespace internal {

using g_vbuffer = global_module<vbuffer_factory_backend>;
using g_tex = global_module<texture_factory_backend>;
using g_rt = global_module<rendertarget_factory_backend>;
using g_rendererfactory_t = global_module<renderer2d_factory_backend>;

renderer2d_middleware::renderer2d_middleware(rendertarget rt)
    : rect_buffer{sizeof(renderer2d_vertex) * 4}, empty_texture{4, 4}, rt{rt} {
    renderer2d_vertex rect_buffer_data[4] = {
        renderer2d_vertex{
            .pos{0.0, 0.0},
            .uv{0.0, 0.0},
        },
        renderer2d_vertex{
            .pos{1.0, 0.0},
            .uv{1.0, 0.0},
        },
        renderer2d_vertex{
            .pos{0.0, 1.0},
            .uv{0.0, 1.0},
        },
        renderer2d_vertex{
            .pos{1.0, 1.0},
            .uv{1.0, 1.0},
        },
    };
    g_vbuffer::get().update_data(rect_buffer.handle(), reinterpret_cast<const uint8_t *>(rect_buffer_data),
                                 sizeof(renderer2d_vertex) * 4);

    p_renderer = g_rendererfactory_t::get().make(rt);
    p_renderer->render_begin();
}
renderer2d_middleware::~renderer2d_middleware() { p_renderer->render_end(); }

void renderer2d_middleware::draw_texture(const image &image, int x, int y, float rotation, float scale_x, float scale_y,
                                         pivot image_pivot) {
    draw_texture(image.clip(), x, y, rotation, scale_x, scale_y, image_pivot);
}

void renderer2d_middleware::draw_texture(const image_clip &clip, int x, int y, float rotation, float scale_x,
                                         float scale_y, pivot clip_pivot) {
    const auto clip_rect = clip.clipping_area();

    p_renderer->attach_texture(clip);

    // gcc not supports cosf, sinf
    const auto cos_th = std::cos(rotation), sin_th = std::sin(rotation);

    const auto anchor_x = clip_pivot.x.value;
    const auto anchor_y = clip_pivot.y.value;
    const auto w = clip_rect.size().width() * scale_x;
    const auto h = clip_rect.size().height() * scale_y;

    mat3 pivot_mat{.m{
        {1.0f, 0.0f, -anchor_x},
        {0.0f, 1.0f, -anchor_y},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 scale_mat{.m{
        {w, 0.0f, 0.0f},
        {0.0f, h, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 rotate_mat{.m{
        {cos_th, -sin_th, 0.0f},
        {sin_th, cos_th, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 move_mat{.m{
        {1.0f, 0.0f, float(x)},
        {0.0f, 1.0f, float(y)},
        {0.0f, 0.0f, 1.0f},
    }};

    const auto tex_size = g_tex::get().texture_size(clip);

    const renderer2d_uniform data{
        .draw_matrix{move_mat * rotate_mat * scale_mat * pivot_mat},
        .screen_size{
            // .v{float(g_rt::get(). rt.get().extent().width), float(rt.get().extent().height)},
        },
        .tex_clip_pos{
            .v{float(clip_rect.left()) / tex_size.width(), float(clip_rect.top()) / tex_size.height()},
        },
        .tex_clip_size{
            .v{float(clip_rect.width()) / tex_size.width(), float(clip_rect.height()) / tex_size.height()},
        },
        .color{},
    };

    p_renderer->draw_polygon(4, rect_buffer, data);
};

void renderer2d_middleware::draw_rect(color fill_color, int x, int y, float rotation, float width, float height,
                                      pivot pivot) {
    p_renderer->attach_texture(empty_texture);

    // gcc not supports cosf, sinf
    const auto cos_th = std::cos(rotation), sin_th = std::sin(rotation);

    const auto anchor_x = pivot.x.value;
    const auto anchor_y = pivot.y.value;

    mat3 pivot_mat{.m{
        {1.0f, 0.0f, -anchor_x},
        {0.0f, 1.0f, -anchor_y},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 scale_mat{.m{
        {width, 0.0f, 0.0f},
        {0.0f, height, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 rotate_mat{.m{
        {cos_th, -sin_th, 0.0f},
        {sin_th, cos_th, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 move_mat{.m{
        {1.0f, 0.0f, float(x)},
        {0.0f, 1.0f, float(y)},
        {0.0f, 0.0f, 1.0f},
    }};

    const renderer2d_uniform data{
        .draw_matrix{move_mat * rotate_mat * scale_mat * pivot_mat},
        .tex_clip_pos{
            .v{0, 0},
        },
        .tex_clip_size{
            .v{1, 1},
        },
        .color{.v{fill_color.r, fill_color.g, fill_color.b, fill_color.a}},
    };

    p_renderer->draw_polygon(4, rect_buffer, data);
}
void renderer2d_middleware::draw_circle() {
    // TODO
}

void renderer2d_middleware::flush() {
    p_renderer->render_end();
    p_renderer->render_begin();
}

} // namespace internal

BRIGHTCPP_END
