#include <stdexcept>

#include "cpp-terminal/private/conversion.hpp"
#include "cpp-terminal/window.hpp"

namespace Term {

char32_t Term::Window::get_char(std::size_t x, std::size_t y) {
    return chars[(y - 1) * w + (x - 1)];
}

bool Term::Window::get_fg_reset(std::size_t x, std::size_t y) {
    return m_fg_reset[(y - 1) * w + (x - 1)];
}

bool Term::Window::get_bg_reset(std::size_t x, std::size_t y) {
    return m_bg_reset[(y - 1) * w + (x - 1)];
}

Term::RGB Term::Window::get_fg(std::size_t x, std::size_t y) {
    return m_fg[(y - 1) * w + (x - 1)];
}

Term::RGB Term::Window::get_bg(std::size_t x, std::size_t y) {
    return m_bg[(y - 1) * w + (x - 1)];
}

Term::Style Term::Window::get_style(std::size_t x, std::size_t y) {
    return m_style[(y - 1) * w + (x - 1)];
}

std::size_t Term::Window::get_w() const {
    return w;
}

std::size_t Term::Window::get_h() const {
    return h;
}

void Term::Window::set_char(std::size_t x, std::size_t y, char32_t c) {
    if (x >= 1 && y >= 1 && x <= w && y <= h) {
        chars[(y - 1) * w + (x - 1)] = c;
    } else {
        throw std::runtime_error("set_char(): (x,y) out of bounds");
    }
}

void Term::Window::set_fg_reset(std::size_t x, std::size_t y) {
    m_fg_reset[(y - 1) * w + (x - 1)] = true;
    m_fg[(y - 1) * w + (x - 1)] = {255, 255, 255};
}

void Term::Window::set_bg_reset(std::size_t x, std::size_t y) {
    m_bg_reset[(y - 1) * w + (x - 1)] = true;
    m_fg[(y - 1) * w + (x - 1)] = {255, 255, 255};
}

void Term::Window::set_fg(std::size_t x, std::size_t y, const RGB& rgb) {
    m_fg_reset[(y - 1) * w + (x - 1)] = false;
    m_fg[(y - 1) * w + (x - 1)] = rgb;
}

void Term::Window::set_bg(std::size_t x, std::size_t y, const RGB& rgb) {
    m_bg_reset[(y - 1) * w + (x - 1)] = false;
    m_bg[(y - 1) * w + (x - 1)] = rgb;
}

void Term::Window::set_style(std::size_t x, std::size_t y, Style c) {
    m_style[(y - 1) * w + (x - 1)] = c;
}

void Term::Window::set_cursor_pos(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void Term::Window::set_h(std::size_t new_h) {
    if (new_h == h) {
        return;
    } else if (new_h > h) {
        std::size_t dc = (new_h - h) * w;
        chars.insert(chars.end(), dc, ' ');
        m_fg_reset.insert(m_fg_reset.end(), dc, true);
        m_bg_reset.insert(m_bg_reset.end(), dc, true);
        m_fg.insert(m_fg.end(), dc, {0, 0, 0});
        m_bg.insert(m_bg.end(), dc, {0, 0, 0});
        m_style.insert(m_style.end(), dc, Style::RESET);
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
    std::u32string s2 = Private::utf8_to_utf32(s);
    std::size_t xpos = x;
    std::size_t ypos = y;
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

void Term::Window::fill_fg(int x1, int y1, int x2, int y2, const RGB& rgb) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_fg(i, j, rgb);
        }
    }
}

void Term::Window::fill_bg(int x1, int y1, int x2, int y2, const RGB& rgb) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_bg(i, j, rgb);
        }
    }
}

void Term::Window::fill_style(int x1, int y1, int x2, int y2, Style color) {
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            set_style(i, j, color);
        }
    }
}

void Term::Window::print_border(bool unicode) {
    print_rect(1, 1, w, h, unicode);
}

void Term::Window::print_rect(std::size_t x1,
                              std::size_t y1,
                              std::size_t x2,
                              std::size_t y2,
                              bool unicode) {
    std::u32string border = Private::utf8_to_utf32("│─┌┐└┘");
    if (unicode) {
        for (std::size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, border[0]);
            set_char(x2, j, border[0]);
        }
        for (std::size_t i = x1 + 1; i <= x2 - 1; i++) {
            set_char(i, y1, border[1]);
            set_char(i, y2, border[1]);
        }
        set_char(x1, y1, border[2]);
        set_char(x2, y1, border[3]);
        set_char(x1, y2, border[4]);
        set_char(x2, y2, border[5]);
    } else {
        for (std::size_t j = y1 + 1; j <= y2 - 1; j++) {
            set_char(x1, j, '|');
            set_char(x2, j, '|');
        }
        for (std::size_t i = x1 + 1; i <= x2 - 1; i++) {
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
    for (std::size_t j = 1; j <= h; j++) {
        for (std::size_t i = 1; i <= w; i++) {
            set_char(i, j, ' ');
            set_fg_reset(i, j);
            set_bg_reset(i, j);
            set_style(i, j, Style::RESET);
        }
    }
}

bool Term::Window::rgb_equal(const RGB& rgb_one, const RGB& rgb_two) {
    return rgb_one.r == rgb_two.r && rgb_one.b == rgb_two.b &&
           rgb_one.g == rgb_two.g;
}

std::string Term::Window::render(int x0, int y0, bool term) {
    std::string out;
    if (term) {
        out.append(cursor_off());
    }
    RGB current_fg = {255, 255, 255};
    RGB current_bg = {255, 255, 255};
    bool current_fg_reset = true;
    bool current_bg_reset = true;
    Style current_style = Style::RESET;
    for (std::size_t j = 1; j <= h; j++) {
        if (term) {
            out.append(cursor_move(y0 + j - 1, x0));
        }
        for (std::size_t i = 1; i <= w; i++) {
            bool update_fg = false;
            bool update_bg = false;
            bool update_fg_reset = false;
            bool update_bg_reset = false;
            bool update_style = false;
            if (current_fg_reset != get_fg_reset(i, j)) {
                current_fg_reset = get_fg_reset(i, j);
                if (current_fg_reset) {
                    update_fg_reset = true;
                    current_fg = {255, 255, 255};
                }
            }

            if (current_bg_reset != get_bg_reset(i, j)) {
                current_bg_reset = get_bg_reset(i, j);
                if (current_bg_reset) {
                    update_bg_reset = true;
                    current_bg = {255, 255, 255};
                }
            }

            if (!current_fg_reset) {
                if (!rgb_equal(current_fg, get_fg(i, j))) {
                    current_fg = get_fg(i, j);
                    update_fg = true;
                }
            }

            if (!current_fg_reset) {
                if (!rgb_equal(current_bg, get_bg(i, j))) {
                    current_bg = get_bg(i, j);
                    update_bg = true;
                }
            }
            if (current_style != get_style(i, j)) {
                current_style = get_style(i, j);
                update_style = true;
                if (current_style == Style::RESET) {
                    // style::reset: reset fg and bg colors too, we have to
                    // set them again if they are non-default, but if fg or
                    // bg colors are reset, we do not update them, as
                    // style::reset already did that.
                    update_fg = !current_fg_reset;
                    update_bg = !current_bg_reset;
                }
            }
            // Set style first, as style::reset will reset colors too
            if (update_style)
                out.append(style(get_style(i, j)));
            if (update_fg_reset)
                out.append(color_fg(Color4::DEFAULT));
            else if (update_fg) {
                Term::RGB color_tmp = get_fg(i, j);
                out.append(color_fg(color_tmp.r, color_tmp.g, color_tmp.b));
            }
            if (update_bg_reset)
                out.append(color_bg(Color4::DEFAULT));
            else if (update_bg) {
                Term::RGB color_tmp = get_bg(i, j);
                out.append(color_bg(color_tmp.r, color_tmp.g, color_tmp.b));
            }
            Private::codepoint_to_utf8(out, get_char(i, j));
        }
        if (j < h)
            out.append("\n");
    }
    if (!current_fg_reset)
        out.append(color_fg(Color4::DEFAULT));
    if (!current_bg_reset)
        out.append(color_bg(Color4::DEFAULT));
    if (current_style != Style::RESET)
        out.append(style(Style::RESET));
    if (term) {
        out.append(cursor_move(y0 + cursor_y - 1, x0 + cursor_x - 1));
        out.append(cursor_on());
    }
    return out;
};

}  // namespace Term
