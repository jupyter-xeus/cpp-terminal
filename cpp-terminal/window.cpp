#include "window.hpp"
#include <stdexcept>

namespace Term {
    char32_t Term::Window_24bit::get_char(size_t x, size_t y) {
    return chars[(y - 1) * w + (x - 1)];
}

bool Term::Window_24bit::get_fg_reset(size_t x, size_t y) {
    return m_fg_reset[(y - 1) * w + (x - 1)];
}
bool Term::Window_24bit::get_bg_reset(size_t x, size_t y) {
    return m_bg_reset[(y - 1) * w + (x - 1)];
}
Term::Window_24bit::rgb Term::Window_24bit::get_fg(size_t x, size_t y) {
    return m_fg[(y - 1) * w + (x - 1)];
}
Term::Window_24bit::rgb Term::Window_24bit::get_bg(size_t x, size_t y) {
    return m_bg[(y - 1) * w + (x - 1)];
}

Term::style Term::Window_24bit::get_style(size_t x, size_t y) {
    return m_style[(y - 1) * w + (x - 1)];
}

size_t Term::Window_24bit::get_w() {
    return w;
}

size_t Term::Window_24bit::get_h() {
    return h;
}

void Term::Window_24bit::set_char(size_t x, size_t y, char32_t c) {
    if (x >= 1 && y >= 1 && x <= w && y <= h) {
        chars[(y - 1) * w + (x - 1)] = c;
    } else {
        throw std::runtime_error("set_char(): (x,y) out of bounds");
    }
}

void Term::Window_24bit::set_fg_reset(size_t x, size_t y) {
    m_fg_reset[(y - 1) * w + (x - 1)] = true;
    m_fg[(y - 1) * w + (x - 1)] = {256, 256, 256};
}

void Term::Window_24bit::set_bg_reset(size_t x, size_t y) {
    m_bg_reset[(y - 1) * w + (x - 1)] = true;
    m_fg[(y - 1) * w + (x - 1)] = {256, 256, 256};
}

void Term::Window_24bit::set_fg(size_t x,
                                size_t y,
                                unsigned int r,
                                unsigned int g,
                                unsigned int b) {
    m_fg_reset[(y - 1) * w + (x - 1)] = false;
    m_fg[(y - 1) * w + (x - 1)] = {r, g, b};
}

void Term::Window_24bit::set_bg(size_t x,
                                size_t y,
                                unsigned int r,
                                unsigned int g,
                                unsigned int b) {
    m_bg_reset[(y - 1) * w + (x - 1)] = false;
    m_bg[(y - 1) * w + (x - 1)] = {r, g, b};
}

void Term::Window_24bit::set_style(size_t x, size_t y, style c) {
    m_style[(y - 1) * w + (x - 1)] = c;
}

void Term::Window_24bit::set_cursor_pos(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void Term::Window_24bit::set_h(size_t new_h) {
    if (new_h == h) {
        return;
    } else if (new_h > h) {
        size_t dc = (new_h - h) * w;
        chars.insert(chars.end(), dc, ' ');
        m_fg_reset.insert(m_fg_reset.end(), dc, true);
        m_bg_reset.insert(m_bg_reset.end(), dc, true);
        m_fg.insert(m_fg.end(), dc, {0, 0, 0});
        m_bg.insert(m_bg.end(), dc, {0, 0, 0});
        m_style.insert(m_style.end(), dc, style::reset);
        h = new_h;
    } else {
        throw std::runtime_error("Shrinking height not supported.");
    }
}

void Term::Window_24bit::print_str(int x,
                                   int y,
                                   const std::string& s,
                                   int indent,
                                   bool move_cursor) {
    std::u32string s2 = utf8_to_utf32(s);
    size_t xpos = x;
    size_t ypos = y;
    for (char32_t i : s2) {
        if (i == U'\n') {
            xpos = x + indent;
            ypos++;
            if (xpos <= w && ypos <= h) {
                for (int j = 0; j < indent; j++) {
                    set_char(x + j, ypos, '.');
                }
            } else {
                // String is out of the window
                return;
            }
        } else {
            if (xpos <= w && ypos <= h) {
                set_char(xpos, y, i);
            } else {
                // String is out of the window
                return;
            }
            xpos++;
        }
    }
    if (move_cursor) {
        cursor_x = xpos;
        cursor_y = ypos;
    }
}

void Term::Window_24bit::fill_fg(int x1,
                                 int y1,
                                 int x2,
                                 int y2,
                                 unsigned int r,
                                 unsigned int g,
                                 unsigned int b) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_fg(i, j, r, g, b);
        }
    }
}

void Term::Window_24bit::fill_bg(int x1,
                                 int y1,
                                 int x2,
                                 int y2,
                                 unsigned int r,
                                 unsigned int g,
                                 unsigned int b) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_bg(i, j, r, g, b);
        }
    }
}

void Term::Window_24bit::fill_style(int x1,
                                    int y1,
                                    int x2,
                                    int y2,
                                    style color) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_style(i, j, color);
        }
    }
}

void Term::Window_24bit::print_border(bool unicode) {
    print_rect(1, 1, w, h, unicode);
}

void Term::Window_24bit::print_rect(size_t x1,
                                    size_t y1,
                                    size_t x2,
                                    size_t y2,
                                    bool unicode) {
    std::u32string border = utf8_to_utf32("│─┌┐└┘");
    if (unicode) {
        for (size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, border[0]);
            set_char(x2, j, border[0]);
        }
        for (size_t i = x1 + 1; i <= x2 - 1; i++) {
            set_char(i, y1, border[1]);
            set_char(i, y2, border[1]);
        }
        set_char(x1, y1, border[2]);
        set_char(x2, y1, border[3]);
        set_char(x1, y2, border[4]);
        set_char(x2, y2, border[5]);
    } else {
        for (size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, '|');
            set_char(x2, j, '|');
        }
        for (size_t i = x1 + 1; i <= x2 - 1; i++) {
            set_char(i, y1, '-');
            set_char(i, y2, '-');
        }
        set_char(x1, y1, '+');
        set_char(x2, y1, '+');
        set_char(x1, y2, '+');
        set_char(x2, y2, '+');
    }
}

void Term::Window_24bit::clear() {
    for (size_t j = 1; j <= h; j++) {
        for (size_t i = 1; i <= w; i++) {
            set_char(i, j, ' ');
            set_fg_reset(i, j);
            set_bg_reset(i, j);
            set_style(i, j, style::reset);
        }
    }
}

bool Term::Window_24bit::rgb_equal(rgb& rgb_one, rgb rgb_two) {
    return rgb_one.r == rgb_two.r && rgb_one.b == rgb_two.b &&
           rgb_one.g == rgb_two.g;
}
std::string Term::Window_24bit::render(int x0, int y0, bool term) {
    std::string out;
    if (term) {
        out.append(cursor_off());
    }
    rgb current_fg = {256, 256, 256};
    rgb current_bg = {256, 256, 256};
    bool current_fg_reset = true;
    bool current_bg_reset = true;
    style current_style = style::reset;
    for (size_t j = 1; j <= h; j++) {
        if (term) {
            out.append(move_cursor(y0 + j - 1, x0));
        }
        for (size_t i = 1; i <= w; i++) {
            bool update_fg = false;
            bool update_bg = false;
            bool update_fg_reset = false;
            bool update_bg_reset = false;
            bool update_style = false;
            if (current_fg_reset != get_fg_reset(i, j)) {
                current_fg_reset = get_fg_reset(i, j);
                if (current_fg_reset) {
                    update_fg_reset = true;
                    current_fg = {256, 256, 256};
                }
            }

            if (current_bg_reset != get_bg_reset(i, j)) {
                current_bg_reset = get_bg_reset(i, j);
                if (current_bg_reset) {
                    update_bg_reset = true;
                    current_bg = {256, 256, 256};
                }
            }

            if (current_fg_reset == false) {
                if (!rgb_equal(current_fg, get_fg(i, j))) {
                    current_fg = get_fg(i, j);
                    update_fg = true;
                }
            }

            if (current_fg_reset == false) {
                if (!rgb_equal(current_bg, get_bg(i, j))) {
                    current_bg = get_bg(i, j);
                    update_bg = true;
                }
            }
            if (current_style != get_style(i, j)) {
                current_style = get_style(i, j);
                update_style = true;
                if (current_style == style::reset) {
                    // style::reset resets fg and bg colors too, we have to
                    // set them again if they are non-default, but if fg or
                    // bg colors are reset, we do not update them, as
                    // style::reset already did that.
                    update_fg = (current_fg_reset == false);
                    update_bg = (current_bg_reset == false);
                }
            }
            // Set style first, as style::reset will reset colors too
            if (update_style)
                out.append(color(get_style(i, j)));
            if (update_fg_reset)
                out.append(color(fg::reset));
            else if (update_fg) {
                rgb color_tmp = get_fg(i, j);
                out.append(color24_fg(color_tmp.r, color_tmp.g, color_tmp.b));
            }
            if (update_bg_reset)
                out.append(color(bg::reset));
            else if (update_bg) {
                rgb color_tmp = get_bg(i, j);
                out.append(color24_bg(color_tmp.r, color_tmp.g, color_tmp.b));
            }
            codepoint_to_utf8(out, get_char(i, j));
        }
        if (j < h)
            out.append("\n");
    }
    if (!current_fg_reset)
        out.append(color(fg::reset));
    if (!current_bg_reset)
        out.append(color(bg::reset));
    if (current_style != style::reset)
        out.append(color(style::reset));
    if (term) {
        out.append(move_cursor(y0 + cursor_y - 1, x0 + cursor_x - 1));
        out.append(cursor_on());
    }
    return out;
};

char32_t Term::Window::get_char(size_t x, size_t y) {
    return chars[(y - 1) * w + (x - 1)];
}

Term::fg Term::Window::get_fg(size_t x, size_t y) {
    return m_fg[(y - 1) * w + (x - 1)];
}

Term::bg Term::Window::get_bg(size_t x, size_t y) {
    return m_bg[(y - 1) * w + (x - 1)];
}

Term::style Term::Window::get_style(size_t x, size_t y) {
    return m_style[(y - 1) * w + (x - 1)];
}

size_t Term::Window::get_w() {
    return w;
}

size_t Term::Window::get_h() {
    return h;
}

void Term::Window::set_char(size_t x, size_t y, char32_t c) {
    if (x >= 1 && y >= 1 && x <= w && y <= h) {
        chars[(y - 1) * w + (x - 1)] = c;
    } else {
        throw std::runtime_error("set_char(): (x,y) out of bounds");
    }
}

void Term::Window::set_fg(size_t x, size_t y, fg c) {
    m_fg[(y - 1) * w + (x - 1)] = c;
}

void Term::Window::set_bg(size_t x, size_t y, bg c) {
    m_bg[(y - 1) * w + (x - 1)] = c;
}

void Term::Window::set_style(size_t x, size_t y, style c) {
    m_style[(y - 1) * w + (x - 1)] = c;
}

void Term::Window::set_cursor_pos(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void Term::Window::set_h(size_t new_h) {
    if (new_h == h) {
        return;
    } else if (new_h > h) {
        size_t dc = (new_h - h) * w;
        chars.insert(chars.end(), dc, ' ');
        m_fg.insert(m_fg.end(), dc, fg::reset);
        m_bg.insert(m_bg.end(), dc, bg::reset);
        m_style.insert(m_style.end(), dc, style::reset);
        h = new_h;
    } else {
        throw std::runtime_error("Shrinking height not supported.");
    }
}

void Term::Window::print_str(int x,
                             int y,
                             const std::string& s,
                             int indent,
                             bool move_cursor) {
    std::u32string s2 = utf8_to_utf32(s);
    size_t xpos = x;
    size_t ypos = y;
    for (char32_t i : s2) {
        if (i == U'\n') {
            xpos = x + indent;
            ypos++;
            if (xpos <= w && ypos <= h) {
                for (int j = 0; j < indent; j++) {
                    set_char(x + j, ypos, '.');
                }
            } else {
                // String is out of the window
                return;
            }
        } else {
            if (xpos <= w && ypos <= h) {
                set_char(xpos, y, i);
            } else {
                // String is out of the window
                return;
            }
            xpos++;
        }
    }
    if (move_cursor) {
        cursor_x = xpos;
        cursor_y = ypos;
    }
}

void Term::Window::fill_fg(int x1, int y1, int x2, int y2, fg color) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_fg(i, j, color);
        }
    }
}

void Term::Window::fill_bg(int x1, int y1, int x2, int y2, bg color) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_bg(i, j, color);
        }
    }
}

void Term::Window::fill_style(int x1, int y1, int x2, int y2, style color) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_style(i, j, color);
        }
    }
}

void Term::Window::print_border(bool unicode = true) {
    print_rect(1, 1, w, h, unicode);
}

void Term::Window::print_rect(size_t x1,
                              size_t y1,
                              size_t x2,
                              size_t y2,
                              bool unicode) {
    std::u32string border = utf8_to_utf32("│─┌┐└┘");
    if (unicode) {
        for (size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, border[0]);
            set_char(x2, j, border[0]);
        }
        for (size_t i = x1 + 1; i <= x2 - 1; i++) {
            set_char(i, y1, border[1]);
            set_char(i, y2, border[1]);
        }
        set_char(x1, y1, border[2]);
        set_char(x2, y1, border[3]);
        set_char(x1, y2, border[4]);
        set_char(x2, y2, border[5]);
    } else {
        for (size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, '|');
            set_char(x2, j, '|');
        }
        for (size_t i = x1 + 1; i <= x2 - 1; i++) {
            set_char(i, y1, '-');
            set_char(i, y2, '-');
        }
        set_char(x1, y1, '+');
        set_char(x2, y1, '+');
        set_char(x1, y2, '+');
        set_char(x2, y2, '+');
    }
}

void Term::Window::clear() {
    for (size_t j = 1; j <= h; j++) {
        for (size_t i = 1; i <= w; i++) {
            set_char(i, j, ' ');
            set_fg(i, j, fg::reset);
            set_bg(i, j, bg::reset);
            set_style(i, j, style::reset);
        }
    }
}

std::string Term::Window::render(int x0, int y0, bool term) {
    std::string out;
    if (term) {
        out.append(cursor_off());
    }
    fg current_fg = fg::reset;
    bg current_bg = bg::reset;
    style current_style = style::reset;
    for (size_t j = 1; j <= h; j++) {
        if (term) {
            out.append(move_cursor(y0 + j - 1, x0));
        }
        for (size_t i = 1; i <= w; i++) {
            bool update_fg = false;
            bool update_bg = false;
            bool update_style = false;
            if (current_fg != get_fg(i, j)) {
                current_fg = get_fg(i, j);
                update_fg = true;
            }
            if (current_bg != get_bg(i, j)) {
                current_bg = get_bg(i, j);
                update_bg = true;
            }
            if (current_style != get_style(i, j)) {
                current_style = get_style(i, j);
                update_style = true;
                if (current_style == style::reset) {
                    // style::reset resets fg and bg colors too, we have to
                    // set them again if they are non-default, but if fg or
                    // bg colors are reset, we do not update them, as
                    // style::reset already did that.
                    update_fg = (current_fg != fg::reset);
                    update_bg = (current_bg != bg::reset);
                }
            }
            // Set style first, as style::reset will reset colors too
            if (update_style)
                out.append(color(get_style(i, j)));
            if (update_fg)
                out.append(color(get_fg(i, j)));
            if (update_bg)
                out.append(color(get_bg(i, j)));
            codepoint_to_utf8(out, get_char(i, j));
        }
        if (j < h)
            out.append("\n");
    }
    if (current_fg != fg::reset)
        out.append(color(fg::reset));
    if (current_bg != bg::reset)
        out.append(color(bg::reset));
    if (current_style != style::reset)
        out.append(color(style::reset));
    if (term) {
        out.append(move_cursor(y0 + cursor_y - 1, x0 + cursor_x - 1));
        out.append(cursor_on());
    }
    return out;
};
}