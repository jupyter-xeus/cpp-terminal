#include <cpp-terminal/terminal.h>
#include <cpp-terminal/terminal_base.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

Term::Terminal::~Terminal() {
    restore_screen();
}

void Term::Terminal::restore_screen() {
    if (restore_screen_) {
        write("\033[?1049l");  // restore screen
        write(
            "\033"
            "8");  // restore current cursor position
        restore_screen_ = false;
    }
}

void Term::Terminal::save_screen() {
    restore_screen_ = true;
    write(
        "\033"
        "7");              // save current cursor position
    write("\033[?1049h");  // save screen
}

int Term::Terminal::read_key() const {
    int key{};
    while ((key = read_key0()) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return key;
}

int Term::Terminal::read_key0() const {
    char c{};
    if (!read_raw(&c))
        return 0;

    if (c == '\x1b') {
        char seq[4];

        if (!read_raw(&seq[0]))
            return Key::ESC;
        if (!read_raw(&seq[1])) {
            if (seq[0] >= 'a' && seq[0] <= 'z') {
                // gnome-term, Windows Console
                return Key::ALT + seq[0];
            }
            if (seq[0] == '\x0d') {
                // gnome-term
                return Key::ALT_ENTER;
            }
            return -1;
        }

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (!read_raw(&seq[2])) {
                    return -2;
                }
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1':
                            return Key::HOME;
                        case '2':
                            return Key::INSERT;
                        case '3':
                            return Key::DEL;
                        case '4':
                            return Key::END;
                        case '5':
                            return Key::PAGE_UP;
                        case '6':
                            return Key::PAGE_DOWN;
                        case '7':
                            return Key::HOME;
                        case '8':
                            return Key::END;
                    }
                } else if (seq[2] == ';') {
                    if (seq[1] == '1') {
                        if (!read_raw(&seq[2])) {
                            return -10;
                        }
                        if (!read_raw(&seq[3])) {
                            return -11;
                        }
                        if (seq[2] == '5') {
                            switch (seq[3]) {
                                case 'A':
                                    return Key::CTRL_UP;
                                case 'B':
                                    return Key::CTRL_DOWN;
                                case 'C':
                                    return Key::CTRL_RIGHT;
                                case 'D':
                                    return Key::CTRL_LEFT;
                            }
                        }
                        return -12;
                    }
                } else {
                    if (seq[2] >= '0' && seq[2] <= '9') {
                        if (!read_raw(&seq[3])) {
                            return -3;
                        }
                        if (seq[3] == '~') {
                            if (seq[1] == '1') {
                                switch (seq[2]) {
                                    case '5':
                                        return Key::F5;
                                    case '7':
                                        return Key::F6;
                                    case '8':
                                        return Key::F7;
                                    case '9':
                                        return Key::F8;
                                }
                            } else if (seq[1] == '2') {
                                switch (seq[2]) {
                                    case '0':
                                        return Key::F9;
                                    case '1':
                                        return Key::F10;
                                    case '3':
                                        return Key::F11;
                                    case '4':
                                        return Key::F12;
                                }
                            }
                        }
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A':
                        return Key::ARROW_UP;
                    case 'B':
                        return Key::ARROW_DOWN;
                    case 'C':
                        return Key::ARROW_RIGHT;
                    case 'D':
                        return Key::ARROW_LEFT;
                    case 'E':
                        return Key::NUMERIC_5;
                    case 'H':
                        return Key::HOME;
                    case 'F':
                        return Key::END;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'F':
                    return Key::END;
                case 'H':
                    return Key::HOME;
                case 'P':
                    return Key::F1;
                case 'Q':
                    return Key::F2;
                case 'R':
                    return Key::F3;
                case 'S':
                    return Key::F4;
            }
        }

        // std::cout << "Unsupported escape sequence:" << std::endl;
        // std::cout << seq[0] << seq[1] << seq[2] << seq[3] << std::endl;
        return -4;
    } else {
        switch (c) {
            case '\x09':  // TAB
                return Key::TAB;
            case '\x0a':  // LF; falls-through
            case '\x0d':  // CR
                return Key::ENTER;
            case '\x7f':  // DEL
                return Key::BACKSPACE;
        }
        if (c == '\xc3') {
            if (!read_raw(&c)) {
                return -8;
            } else {
                if (c >= '\xa1' && c <= '\xba') {
                    // xterm
                    return Key::ALT + (c + 'a' - '\xa1');
                }
                return -9;
            }
        } else if (c == '\xc2') {
            if (!read_raw(&c)) {
                return -10;
            } else {
                if (c == '\x8d') {
                    // xterm
                    return Key::ALT_ENTER;
                }
                return -11;
            }
        }
        return c;
    }
}

void Term::Terminal::get_cursor_position(int& rows, int& cols) const {
    char buf[32];
    write(cursor_position_report());
    for (unsigned int i = 0; i < sizeof(buf) - 1; i++) {
        while (!read_raw(&buf[i]))
            ;
        if (buf[i] == 'R') {
            if (i < 5) {
                throw std::runtime_error(
                    "get_cursor_position(): too short response");
            } else {
                buf[i] = '\0';
            }
            break;
        }
    }
    // Find the result in the response, drop the rest:
    for (unsigned int i = 0; i < sizeof(buf) - 6; i++) {
        if (buf[i] == '\x1b' && buf[i + 1] == '[') {
            if (convert_string_to_int(&buf[i + 2], "%d;%d", &rows, &cols) !=
                2) {
                throw std::runtime_error(
                    "get_cursor_position(): result could not be parsed");
            }
            return;
        }
        if (buf[i] == '\0')
            break;
    }
    throw std::runtime_error(
        "get_cursor_position(): result not found in the response");
}
void Term::Terminal::get_term_size_slow(int& rows, int& cols) {
    struct CursorOff {
        const Terminal& term;
        explicit CursorOff(const Terminal& term) : term{term} {
            write(cursor_off());
        }
        ~CursorOff() { write(cursor_on()); }
    };
    CursorOff cursor_off(*this);
    int old_row{}, old_col{};
    get_cursor_position(old_row, old_col);
    write(move_cursor_right(999) + move_cursor_down(999));
    get_cursor_position(rows, cols);
    write(move_cursor(old_row, old_col));
}

/*----------------------------------------------------------------------------*/

static constexpr uint8_t UTF8_ACCEPT = 0;
static constexpr uint8_t UTF8_REJECT = 0xf;

uint8_t Term::utf8_decode_step(uint8_t state, uint8_t octet, uint32_t* cpp) {
    static const uint32_t utf8_classtab[0x10] = {
        0x88888888UL, 0x88888888UL, 0x99999999UL, 0x99999999UL,
        0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL,
        0x222222ffUL, 0x22222222UL, 0x22222222UL, 0x22222222UL,
        0x3333333bUL, 0x33433333UL, 0xfff5666cUL, 0xffffffffUL,
    };

    static const uint32_t utf8_statetab[0x10] = {
        0xfffffff0UL, 0xffffffffUL, 0xfffffff1UL, 0xfffffff3UL,
        0xfffffff4UL, 0xfffffff7UL, 0xfffffff6UL, 0xffffffffUL,
        0x33f11f0fUL, 0xf3311f0fUL, 0xf33f110fUL, 0xfffffff2UL,
        0xfffffff5UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL,
    };

    const uint8_t reject = (state >> 3), nonascii = (octet >> 7);
    const uint8_t class_ =
        (!nonascii ? 0
                   : (0xf & (utf8_classtab[(octet >> 3) & 0xf] >>
                             (4 * (octet & 7)))));

    *cpp = (state == UTF8_ACCEPT ? (octet & (0xffU >> class_))
                                 : ((octet & 0x3fU) | (*cpp << 6)));

    return (reject ? 0xf
                   : (0xf & (utf8_statetab[class_] >> (4 * (state & 7)))));
}

/*----------------------------------------------------------------------------*/

void Term::codepoint_to_utf8(std::string& s, char32_t c) {
    if (c > 0x0010FFFF) {
        throw std::runtime_error("Invalid UTF32 codepoint.");
    }
    char bytes[4];
    int nbytes = 1;
    char32_t d = c;
    if (c >= 0x10000) {
        nbytes++;
        bytes[3] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x800) {
        nbytes++;
        bytes[2] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x80) {
        nbytes++;
        bytes[1] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    static const unsigned char mask[4] = {0x00, 0xC0, 0xE0, 0xF0};
    bytes[0] = static_cast<char>(d | mask[nbytes - 1]);
    s.append(bytes, nbytes);
}

/*----------------------------------------------------------------------------*/

std::u32string Term::utf8_to_utf32(const std::string& s) {
    uint32_t codepoint{};
    uint8_t state = UTF8_ACCEPT;
    std::u32string r{};
    for (char i : s) {
        state = utf8_decode_step(state, i, &codepoint);
        if (state == UTF8_ACCEPT) {
            r.push_back(codepoint);
        } else if (state == UTF8_REJECT) {
            throw std::runtime_error("Invalid byte in UTF8 encoded string");
        }
    }
    if (state != UTF8_ACCEPT) {
        throw std::runtime_error("Expected more bytes in UTF8 encoded string");
    }
    return r;
}

std::string Term::utf32_to_utf8(const std::u32string& s) {
    std::string r{};
    for (char32_t i : s) {
        codepoint_to_utf8(r, i);
    }
    return r;
}

std::string Term::concat(const std::vector<std::string>& lines) {
    std::string s;
    for (auto& line : lines) {
        s.append(line + "\n");
    }
    return s;
}

std::vector<std::string> Term::split(const std::string& s) {
    size_t j = 0;
    std::vector<std::string> lines;
    lines.emplace_back("");
    if (s[s.size() - 1] != '\n')
        throw std::runtime_error("\\n is required");
    for (size_t i = 0; i < s.size() - 1; i++) {
        if (s[i] == '\n') {
            j++;
            lines.emplace_back("");
        } else {
            lines[j].push_back(s[i]);
        }
    }
    return lines;
}

char32_t Term::U(const std::string& s) {
    std::u32string s2 = Term::utf8_to_utf32(s);
    if (s2.size() != 1)
        throw std::runtime_error("U(s): s not a codepoint.");
    return s2[0];
}

void Term::print_left_curly_bracket(Term::Window& scr, int x, int y1, int y2) {
    int h = y2 - y1 + 1;
    if (h == 1) {
        scr.set_char(x, y1, U("]"));
    } else {
        scr.set_char(x, y1, U("┐"));
        for (int j = y1 + 1; j <= y2 - 1; j++) {
            scr.set_char(x, j, U("│"));
        }
        scr.set_char(x, y2, U("┘"));
    }
}

void Term::render(Term::Window& scr, const Model& m, size_t cols) {
    scr.clear();
    print_left_curly_bracket(scr, cols, 1, m.lines.size());
    scr.print_str(
        cols - 6, m.lines.size(),
        std::to_string(m.cursor_row) + "," + std::to_string(m.cursor_col));
    for (size_t j = 0; j < m.lines.size(); j++) {
        if (j == 0) {
            scr.print_str(1, j + 1, m.prompt_string);
            scr.fill_fg(1, j + 1, m.prompt_string.size(), m.lines.size(),
                        Term::fg::green);
            scr.fill_style(1, j + 1, m.prompt_string.size(), m.lines.size(),
                           Term::style::bold);
        } else {
            for (size_t i = 0; i < m.prompt_string.size() - 1; i++) {
                scr.set_char(i + 1, j + 1, '.');
            }
        }
        scr.print_str(m.prompt_string.size() + 1, j + 1, m.lines[j]);
    }
    scr.set_cursor_pos(m.prompt_string.size() + m.cursor_col, m.cursor_row);
}

std::string Term::prompt(Terminal& term,
                         const std::string& prompt_string,
                         std::vector<std::string>& history,
                         std::function<bool(std::string)>& iscomplete) {
    int row, col;
    bool term_attached = Terminal::is_stdin_a_tty();
    if (term_attached) {
        term.get_cursor_position(row, col);
    } else {
        row = 1;
        col = 1;
    }
    int rows, cols;
    if (!term.get_term_size(rows, cols)) {
        rows = 25;
        cols = 80;
    }

    Model m;
    m.prompt_string = prompt_string;
    m.lines.emplace_back("");
    m.cursor_col = 1;
    m.cursor_row = 1;

    // Make a local copy of history that can be modified by the user. All
    // changes will be forgotten once a command is submitted.
    std::vector<std::string> hist = history;
    size_t history_pos = hist.size();
    hist.push_back(concat(m.lines));  // Push back empty input

    Term::Window scr(cols, 1);
    int key;
    render(scr, m, cols);
    std::cout << scr.render(1, row, term_attached) << std::flush;
    bool not_complete = true;
    while (not_complete) {
        key = term.read_key();
        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
            (!iscntrl(key) && key < 128)) {
            std::string before =
                m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
            std::string newchar;
            newchar.push_back(key);
            std::string after =
                m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
            m.lines[m.cursor_row - 1] = before += newchar += after;
            m.cursor_col++;
        } else if (key == Key::CTRL + 'd') {
            if (m.lines.size() == 1 && m.lines[m.cursor_row - 1].empty()) {
                m.lines[m.cursor_row - 1].push_back(Key::CTRL + 'd');
                std::cout << "\n" << std::flush;
                history.push_back(m.lines[0]);
                return m.lines[0];
            }
        } else {
            switch (key) {
                case Key::BACKSPACE:
                    if (m.cursor_col > 1) {
                        std::string before = m.lines[m.cursor_row - 1].substr(
                            0, m.cursor_col - 2);
                        std::string after =
                            m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
                        m.lines[m.cursor_row - 1] = before + after;
                        m.cursor_col--;
                    } else if (m.cursor_col == 1 && m.cursor_row > 1) {
                        m.cursor_col = m.lines[m.cursor_row - 2].size() + 1;
                        m.lines[m.cursor_row - 2] += m.lines[m.cursor_row - 1];
                        m.lines.erase(m.lines.begin() + m.cursor_row - 1);
                        m.cursor_row--;
                    }
                    break;
                case Key::DEL:
                    if (m.cursor_col <= m.lines[m.cursor_row - 1].size()) {
                        std::string before = m.lines[m.cursor_row - 1].substr(
                            0, m.cursor_col - 1);
                        std::string after =
                            m.lines[m.cursor_row - 1].substr(m.cursor_col);
                        m.lines[m.cursor_row - 1] = before + after;
                    }
                    break;
                case Key::ARROW_LEFT:
                    if (m.cursor_col > 1) {
                        m.cursor_col--;
                    }
                    break;
                case Key::ARROW_RIGHT:
                    if (m.cursor_col <= m.lines[m.cursor_row - 1].size()) {
                        m.cursor_col++;
                    }
                    break;
                case Key::HOME:
                    m.cursor_col = 1;
                    break;
                case Key::END:
                    m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                    break;
                case Key::ARROW_UP:
                    if (m.cursor_row == 1) {
                        if (history_pos > 0) {
                            hist[history_pos] = concat(m.lines);
                            history_pos--;
                            m.lines = split(hist[history_pos]);
                            m.cursor_row = m.lines.size();
                            if (m.cursor_col >
                                m.lines[m.cursor_row - 1].size() + 1) {
                                m.cursor_col =
                                    m.lines[m.cursor_row - 1].size() + 1;
                            }
                            if (m.lines.size() > scr.get_h()) {
                                scr.set_h(m.lines.size());
                            }
                        }
                    } else {
                        m.cursor_row--;
                        if (m.cursor_col >
                            m.lines[m.cursor_row - 1].size() + 1) {
                            m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                        }
                    }
                    break;
                case Key::ARROW_DOWN:
                    if (m.cursor_row == m.lines.size()) {
                        if (history_pos < hist.size() - 1) {
                            hist[history_pos] = concat(m.lines);
                            history_pos++;
                            m.lines = split(hist[history_pos]);
                            m.cursor_row = 1;
                            if (m.cursor_col >
                                m.lines[m.cursor_row - 1].size() + 1) {
                                m.cursor_col =
                                    m.lines[m.cursor_row - 1].size() + 1;
                            }
                            if (m.lines.size() > scr.get_h()) {
                                scr.set_h(m.lines.size());
                            }
                        }
                    } else {
                        m.cursor_row++;
                        if (m.cursor_col >
                            m.lines[m.cursor_row - 1].size() + 1) {
                            m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                        }
                    }
                    break;
                case Key::ENTER:
                    not_complete = !iscomplete(concat(m.lines));
                    if (not_complete) {
                        key = Key::ALT_ENTER;
                    } else {
                        break;
                    }
                    [[fallthrough]];
                case Key::CTRL + 'n':
                case Key::ALT_ENTER:
                    std::string before =
                        m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
                    std::string after =
                        m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
                    m.lines[m.cursor_row - 1] = before;
                    if (m.cursor_row < m.lines.size()) {
                        // Not at the bottom row, can't push back
                        m.lines.insert(m.lines.begin() + m.cursor_row, after);
                    } else {
                        m.lines.push_back(after);
                    }
                    m.cursor_col = 1;
                    m.cursor_row++;
                    if (m.lines.size() > scr.get_h()) {
                        scr.set_h(m.lines.size());
                    }
            }
        }
        render(scr, m, cols);
        std::cout << scr.render(1, row, term_attached) << std::flush;
        if (row + (int)scr.get_h() - 1 > rows) {
            row = rows - ((int)scr.get_h() - 1);
            std::cout << scr.render(1, row, term_attached) << std::flush;
        }
    }
    std::string line_skips;
    for (size_t i = 0; i <= m.lines.size() - m.cursor_row; i++) {
        line_skips += "\n";
    }
    std::cout << line_skips << std::flush;
    history.push_back(concat(m.lines));
    return concat(m.lines);
}
