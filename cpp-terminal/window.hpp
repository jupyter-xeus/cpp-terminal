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
    std::size_t w, h;                // width and height of the window
    std::size_t cursor_x, cursor_y;  // current cursor position
    std::vector<char32_t> chars;     // the characters in row first order
    struct rgb {
        unsigned int r, g, b;
    };
    std::vector<rgb> m_fg;
    std::vector<rgb> m_bg;
    std::vector<bool> m_fg_reset;
    std::vector<bool> m_bg_reset;
    std::vector<Style> m_style;

    char32_t get_char(std::size_t, std::size_t);

    bool get_fg_reset(std::size_t, std::size_t);
    bool get_bg_reset(std::size_t, std::size_t);
    rgb get_fg(std::size_t, std::size_t);
    rgb get_bg(std::size_t, std::size_t);

    Style get_style(std::size_t, std::size_t);

   public:
    Window_24bit(std::size_t w, std::size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, {0, 0, 0}),
          m_bg(w * h, {0, 0, 0}),
          m_fg_reset(w * h, true),
          m_bg_reset(w * h, true),
          m_style(w * h, Style::RESET){};

    CPP_TERMINAL_NODISCARD std::size_t get_w() const;

    CPP_TERMINAL_NODISCARD std::size_t get_h() const;

    void set_char(std::size_t, std::size_t, char32_t);

    void set_fg_reset(std::size_t, std::size_t);

    void set_bg_reset(std::size_t, std::size_t);

    void set_fg(std::size_t,
                std::size_t,
                unsigned int,
                unsigned int,
                unsigned int);

    void set_bg(std::size_t,
                std::size_t,
                unsigned int,
                unsigned int,
                unsigned int);

    void set_style(std::size_t, std::size_t, Style);

    void set_cursor_pos(int, int);

    void set_h(std::size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_bg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_style(int, int, int, int, Style);

    void print_border(bool = true);

    void print_rect(std::size_t,
                    std::size_t,
                    std::size_t,
                    std::size_t,
                    bool = true);

    void clear();

    static bool rgb_equal(rgb&, rgb);

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
    std::size_t w{}, h{};            // width and height of the window
    std::size_t cursor_x, cursor_y;  // current cursor position
    std::vector<char32_t> chars;     // the characters in row first order
    std::vector<Color4> m_fg;
    std::vector<Color4> m_bg;
    std::vector<Style> m_style;

    char32_t get_char(std::size_t, std::size_t);

    Color4 get_fg(std::size_t, std::size_t);

    Color4 get_bg(std::size_t, std::size_t);

    Style get_style(std::size_t, std::size_t);

   public:
    Window(std::size_t w, std::size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, Color4::DEFAULT),
          m_bg(w * h, Color4::DEFAULT),
          m_style(w * h, Style::RESET){};

    CPP_TERMINAL_NODISCARD std::size_t get_w() const;

    CPP_TERMINAL_NODISCARD std::size_t get_h() const;

    void set_char(std::size_t, std::size_t, char32_t);

    void set_fg(std::size_t, std::size_t, Color4);

    void set_bg(std::size_t, std::size_t, Color4);

    void set_style(std::size_t, std::size_t, Style);

    void set_cursor_pos(int, int);

    void set_h(std::size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, Color4);

    void fill_bg(int, int, int, int, Color4);

    void fill_style(int, int, int, int, Style);

    void print_border(bool);

    void print_rect(std::size_t,
                    std::size_t,
                    std::size_t,
                    std::size_t,
                    bool = true);

    void clear();

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int, int, bool);
};
}  // namespace Term
