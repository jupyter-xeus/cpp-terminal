#include <cpp-terminal/input.hpp>
#include <cpp-terminal/prompt.hpp>
#include <cpp-terminal/base.hpp>
#include <iostream>
#include "private/conversion.hpp"
#include "private/platform.hpp"

Term::Result Term::prompt_blocking(std::string message,
                                   std::string first_option,
                                   std::string second_option,
                                   std::string prompt_indicator) {
    Terminal term(false, true, true);
    std::cout << message << " [" << first_option << '/' << second_option << ']'
              << prompt_indicator << ' ' << std::flush;

    if (!Term::is_stdin_a_tty()) {
        Term::write("\n");
        return Result::ERROR;
    }

    std::vector<char> input;
    unsigned short int length = 0;
    int key;
    while (true) {
        key = Term::read_key();
        if (key >= 'a' && key <= 'z') {
            std::cout << (char)key << std::flush;
            length++;
            input.push_back(static_cast<char>(key));
        } else if (key >= 'A' && key <= 'Z') {
            std::cout << (char)key << std::flush;
            length++;
            input.push_back(static_cast<char>(
                key + 32));  // convert upper case to lowercase
        } else if (key == Term::Key::CTRL + 'c') {
            std::cout << '\n';
            return Result::ABORT;
        } else if (key == Term::Key::BACKSPACE) {
            if (length != 0) {
                std::cout
                    << "\033[D \033[D"
                    << std::flush;  // erase last line and move the cursor back
                length--;
                input.pop_back();
            }
        } else if (key == Term::Key::ENTER) {
            if (Private::vector_to_string(input) == "y" ||
                Private::vector_to_string(input) == "yes") {
                Term::write("\n");
                return Result::YES;
            } else if (Private::vector_to_string(input) == "n" ||
                       Private::vector_to_string(input) == "no") {
                Term::write("\n");
                return Result::NO;
            } else if (length == 0) {
                Term::write("\n");
                return Result::NONE;
            } else {
                Term::write("\n");
                return Result::INVALID;
            }
        }
    }
    // should be unreachable
    return Result::ERROR;
}

Term::Result Term::prompt_non_blocking(std::string message,
                                       std::string first_option,
                                       std::string second_option,
                                       std::string prompt_indicator) {
    Terminal term(false, true, true);
    std::cout << message << " [" << first_option << '/' << second_option << ']'
              << prompt_indicator << ' ' << std::flush;

    if (!Term::is_stdin_a_tty()) {
        Term::write("\n");
        return Result::ERROR;
    }

    int key;
    while (true) {
        key = Term::read_key();
        if (key == 'y' || key == 'Y') {
            Term::write("\n");
            return Result::YES;
        } else if (key == 'n' || key == 'N') {
            Term::write("\n");
            return Result::NO;
        } else if (key == Term::Key::CTRL + 'c') {
            Term::write("\n");
            return Result::ABORT;
        } else if (key == Term::Key::ENTER) {
            Term::write("\n");
            return Result::NONE;
        } else {
            Term::write("\n");
            return Result::INVALID;
        }
    }
}

Term::Result_simple Term::prompt_simple(std::string message) {
    switch (prompt_blocking(message, "Y", "N", ":")) {
        case Result::YES:
            return Result_simple::YES;
        case Result::ABORT:
            return Result_simple::ABORT;
        case Result::NO:     // falls through
        case Result::ERROR:  // falls through
        case Result::NONE:   // falls through
        case Result::INVALID:
            return Result_simple::NO;
    }
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
    std::u32string s2 = Private::utf8_to_utf32(s);
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

std::string Term::prompt_multiline(
    Terminal& term,
    const std::string& prompt_string,
    std::vector<std::string>& history,
    std::function<bool(std::string)>& iscomplete) {
    int row, col;
    bool term_attached = Private::is_stdin_a_tty();
    if (term_attached) {
        Term::get_cursor_position(row, col);
    } else {
        row = 1;
        col = 1;
    }
    int rows, cols;
    if (!Private::get_term_size(rows, cols)) {
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
        key = Term::read_key();
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
