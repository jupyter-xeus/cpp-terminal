#pragma once

#include <vector>
#include "base.hpp"

namespace Term {
    /* Represents a rectangular window, as a 2D array of characters and their
 * attributes. The render method can convert this internal representation to a
 * string that when printed will show the Window on the screen.
 *
 * Note: the characters are represented by char32_t, representing their UTF-32
 * code point. The natural way to represent a character in a terminal would be
 * a "unicode grapheme cluster", but due to a lack of a good library for C++
 * that could handle those, we simply use a Unicode code point as a character.
 */
    class Window_24bit {
    private:
        size_t w, h;                  // width and height of the window
        size_t cursor_x, cursor_y;    // current cursor position
        std::vector<char32_t> chars;  // the characters in row first order
        struct rgb {
            unsigned int r, g, b;
        };
        std::vector<rgb> m_fg;
        std::vector<rgb> m_bg;
        std::vector<bool> m_fg_reset;
        std::vector<bool> m_bg_reset;
        std::vector<style> m_style;

        char32_t get_char(size_t x, size_t y);

        bool get_fg_reset(size_t x, size_t y);

        bool get_bg_reset(size_t x, size_t y);

        rgb get_fg(size_t x, size_t y);

        rgb get_bg(size_t x, size_t y);

        style get_style(size_t x, size_t y);

    public:
        Window_24bit(size_t w, size_t h)
                : w{w},
                  h{h},
                  cursor_x{1},
                  cursor_y{1},
                  chars(w * h, ' '),
                  m_fg(w * h, {0, 0, 0}),
                  m_bg(w * h, {0, 0, 0}),
                  m_fg_reset(w * h, true),
                  m_bg_reset(w * h, true),
                  m_style(w * h, style::reset) {}

        size_t get_w() const;

        size_t get_h() const;

        void set_char(size_t x, size_t y, char32_t c);

        void set_fg_reset(size_t x, size_t y);

        void set_bg_reset(size_t x, size_t y);

        void set_fg(size_t x,
                    size_t y,
                    unsigned int r,
                    unsigned int g,
                    unsigned int b);

        void set_bg(size_t x,
                    size_t y,
                    unsigned int r,
                    unsigned int g,
                    unsigned int b);

        void set_style(size_t x, size_t y, style c);

        void set_cursor_pos(int x, int y);

        void set_h(size_t new_h);

        void print_str(int x,
                       int y,
                       const std::string &s,
                       int indent = 0,
                       bool move_cursor = false);

        void fill_fg(int x1,
                     int y1,
                     int x2,
                     int y2,
                     unsigned int r,
                     unsigned int g,
                     unsigned int b);

        void fill_bg(int x1,
                     int y1,
                     int x2,
                     int y2,
                     unsigned int r,
                     unsigned int g,
                     unsigned int b);

        void fill_style(int x1, int y1, int x2, int y2, style color);

        void print_border(bool unicode = true);

        void print_rect(size_t x1,
                        size_t y1,
                        size_t x2,
                        size_t y2,
                        bool unicode = true);

        void clear();

        static bool rgb_equal(rgb &rgb_one, rgb rgb_two);

        // TODO: add Window/Screen parameter here, to be used like this:
        // old_scr = scr;
        // scr.print_str(...)
        // scr.render(1, 1, old_scr)
        std::string render(int x0, int y0, bool term);
    };

/* Represents a rectangular window, as a 2D array of characters and their
 * attributes. The render method can convert this internal representation to a
 * string that when printed will show the Window on the screen.
 *
 * Note: the characters are represented by char32_t, representing their UTF-32
 * code point. The natural way to represent a character in a terminal would be
 * a "unicode grapheme cluster", but due to a lack of a good library for C++
 * that could handle those, we simply use a Unicode code point as a character.
 */
    class Window {
    private:
        size_t w, h;                  // width and height of the window
        size_t cursor_x, cursor_y;    // current cursor position
        std::vector<char32_t> chars;  // the characters in row first order
        std::vector<fg> m_fg;
        std::vector<bg> m_bg;
        std::vector<style> m_style;

        char32_t get_char(size_t x, size_t y);

        fg get_fg(size_t x, size_t y);

        bg get_bg(size_t x, size_t y);

        style get_style(size_t x, size_t y);

    public:
        Window(size_t w, size_t h)
                : w{w},
                  h{h},
                  cursor_x{1},
                  cursor_y{1},
                  chars(w * h, ' '),
                  m_fg(w * h, fg::reset),
                  m_bg(w * h, bg::reset),
                  m_style(w * h, style::reset) {}

        size_t get_w() const;

        size_t get_h() const;

        void set_char(size_t x, size_t y, char32_t c);

        void set_fg(size_t x, size_t y, fg c);

        void set_bg(size_t x, size_t y, bg c);

        void set_style(size_t x, size_t y, style c);

        void set_cursor_pos(int x, int y);

        void set_h(size_t new_h);

        void print_str(int x,
                       int y,
                       const std::string &s,
                       int indent = 0,
                       bool move_cursor = false);

        void fill_fg(int x1, int y1, int x2, int y2, fg color);

        void fill_bg(int x1, int y1, int x2, int y2, bg color);

        void fill_style(int x1, int y1, int x2, int y2, style color);

        void print_border(bool unicode = true);

        void print_rect(size_t x1,
                        size_t y1,
                        size_t x2,
                        size_t y2,
                        bool unicode = true);

        void clear();

        // TODO: add Window/Screen parameter here, to be used like this:
        // old_scr = scr;
        // scr.print_str(...)
        // scr.render(1, 1, old_scr)
        std::string render(int x0, int y0, bool term);
    };
}
