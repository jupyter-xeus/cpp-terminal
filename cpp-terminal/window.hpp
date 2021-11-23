#pragma once

#include <cpp-terminal/base.hpp>
#include <vector>

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

    char32_t get_char(size_t, size_t);

    bool get_fg_reset(size_t, size_t);
    bool get_bg_reset(size_t, size_t);
    rgb get_fg(size_t, size_t);
    rgb get_bg(size_t, size_t);

    style get_style(size_t, size_t);

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
          m_style(w * h, style::reset){};

    size_t get_w();

    size_t get_h();

    void set_char(size_t, size_t, char32_t);

    void set_fg_reset(size_t, size_t);

    void set_bg_reset(size_t, size_t);

    void set_fg(size_t, size_t, unsigned int, unsigned int, unsigned int);

    void set_bg(size_t, size_t, unsigned int, unsigned int, unsigned int);

    void set_style(size_t, size_t, style);

    void set_cursor_pos(int, int);

    void set_h(size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_bg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_style(int, int, int, int, style);

    void print_border(bool = true);

    void print_rect(size_t, size_t, size_t, size_t, bool = true);

    void clear();

    bool rgb_equal(rgb&, rgb);

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int, int, bool);
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
    size_t w{}, h{};              // width and height of the window
    size_t cursor_x, cursor_y;    // current cursor position
    std::vector<char32_t> chars;  // the characters in row first order
    std::vector<fg> m_fg;
    std::vector<bg> m_bg;
    std::vector<style> m_style;

    char32_t get_char(size_t, size_t);

    fg get_fg(size_t, size_t);

    bg get_bg(size_t, size_t);

    style get_style(size_t, size_t);

   public:
    Window(size_t w, size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, fg::reset),
          m_bg(w * h, bg::reset),
          m_style(w * h, style::reset){};

    size_t get_w();

    size_t get_h();

    void set_char(size_t, size_t, char32_t);

    void set_fg(size_t, size_t, fg);

    void set_bg(size_t, size_t, bg);

    void set_style(size_t, size_t, style);

    void set_cursor_pos(int, int);

    void set_h(size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, fg);

    void fill_bg(int, int, int, int, bg);

    void fill_style(int, int, int, int, style);

    void print_border(bool);

    void print_rect(size_t, size_t, size_t, size_t, bool = true);

    void clear();

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int, int, bool);
};
}  // namespace Term
