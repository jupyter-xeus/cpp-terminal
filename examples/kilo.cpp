/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/args.hpp"
#include "cpp-terminal/color.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/mouse.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/terminal_initializer.hpp"
#include "cpp-terminal/tty.hpp"

#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

const std::string           KILO_VERSION{"0.0.1"};
const constexpr std::size_t KILO_TAB_STOP{8};
const constexpr std::size_t KILO_QUIT_TIMES{3};
const constexpr std::size_t HL_HIGHLIGHT_NUMBERS(1UL << 0UL);
const constexpr std::size_t HL_HIGHLIGHT_STRINGS(1UL << 1UL);

enum editorHighlight
{
  HL_NORMAL = 0,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_STRING,
  HL_NUMBER,
  HL_MATCH
};

/*** data ***/

class editorSyntax
{
public:
  editorSyntax() : keywords(nullptr) {}
  editorSyntax(const std::string& type, const std::vector<std::string>& match, const char** key, const std::string& sc, const std::string& mcs, const std::string& mce, const std::size_t& flag) : filetype(type), filematch(match), keywords(key), singleline_comment_start(sc), multiline_comment_start(mcs), multiline_comment_end(mce), flags(flag) { m_empty = false; }
  bool                     empty() const noexcept { return m_empty; }
  std::string              filetype;
  std::vector<std::string> filematch;
  const char**             keywords;
  std::string              singleline_comment_start;
  std::string              multiline_comment_start;
  std::string              multiline_comment_end;
  std::size_t              flags;

private:
  bool m_empty{true};
};

class erow
{
public:
  int            idx{0};
  int            size{0};
  int            rsize{0};
  char*          chars{nullptr};
  char*          render{nullptr};
  unsigned char* hl{nullptr};
  int            hl_open_comment{0};
};

class editorConfig
{
public:
  editorConfig()
  {
    static const Term::TerminalInitializer init;
    setScreenSize(Term::screen_size());
    statusmsg.reserve(screencols);
  }
  void setScreenSize(const Term::Screen& screen)
  {
    screenrows = screen.rows() - m_sizeBar;
    screencols = screen.columns();
    statusmsg.resize(screencols);
  }
  std::size_t  cx{0};
  std::size_t  cy{0};
  int          rx{0};
  std::size_t  rowoff{0};
  std::size_t  coloff{0};
  std::size_t  screenrows{0};
  std::size_t  screencols{0};
  std::size_t  numrows{0};
  erow*        row{nullptr};
  bool         dirty{false};
  std::string  filename;
  std::string  statusmsg;
  editorSyntax syntax;

private:
  static const constexpr std::size_t m_sizeBar{2};
};

class editorConfig E;

/*** filetypes ***/

const std::vector<std::string> C_HL_extensions = {".c", ".h", ".hpp", ".cpp"};
const char*                    C_HL_keywords[] = {"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class", "case", "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", "bool|", nullptr};

std::array<editorSyntax, 1> HLDB = {
  editorSyntax("c", C_HL_extensions, C_HL_keywords, "//", "/*", "*/", HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS),
};

/*** prototypes ***/

std::string editorPrompt(const std::string& prompt, void (*callback)(std::string&, const Term::Key&));

/*** syntax highlighting ***/

bool is_separator(const int& c) { return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != nullptr; }

void editorUpdateSyntax(erow* row)
{
  row->hl = reinterpret_cast<unsigned char*>(realloc(row->hl, static_cast<size_t>(row->rsize)));
  if(row->hl == nullptr) { exit(-1); /*TODO: better handler*/ }
  memset(row->hl, HL_NORMAL, static_cast<size_t>(row->rsize));

  if(E.syntax.empty()) return;

  const char** keywords = E.syntax.keywords;

  const std::string scs = E.syntax.singleline_comment_start;
  const std::string mcs = E.syntax.multiline_comment_start;
  const std::string mce = E.syntax.multiline_comment_end;

  int prev_sep   = 1;
  int in_string  = 0;
  int in_comment = (row->idx > 0) && (E.row[row->idx - 1].hl_open_comment != 0);

  size_t i = 0;
  while(i < static_cast<size_t>(row->rsize))
  {
    char          c       = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : static_cast<unsigned char>(HL_NORMAL);

    if(scs.size() && !in_string && !in_comment)
    {
      if(!strncmp(&row->render[i], &scs[0], scs.size()))
      {
        memset(&row->hl[i], HL_COMMENT, row->rsize - i);
        break;
      }
    }

    if(mcs.size() && mce.size() && !in_string)
    {
      if(in_comment)
      {
        row->hl[i] = HL_MLCOMMENT;
        if(!strncmp(&row->render[i], &mce[0], mce.size()))
        {
          memset(&row->hl[i], HL_MLCOMMENT, mce.size());
          i += mce.size();
          in_comment = 0;
          prev_sep   = 1;
          continue;
        }
        else
        {
          i++;
          continue;
        }
      }
      else if(!strncmp(&row->render[i], &mcs[0], mcs.size()))
      {
        memset(&row->hl[i], HL_MLCOMMENT, mcs.size());
        i += mcs.size();
        in_comment = 1;
        continue;
      }
    }

    if(E.syntax.flags & HL_HIGHLIGHT_STRINGS)
    {
      if(in_string)
      {
        row->hl[i] = HL_STRING;
        if(c == '\\' && i + 1 < static_cast<size_t>(row->rsize))
        {
          row->hl[i + 1] = HL_STRING;
          i += 2;
          continue;
        }
        if(c == in_string) in_string = 0;
        i++;
        prev_sep = 1;
        continue;
      }
      else
      {
        if(c == '"' || c == '\'')
        {
          in_string  = c;
          row->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }

    if(E.syntax.flags & HL_HIGHLIGHT_NUMBERS)
    {
      if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER))
      {
        row->hl[i] = HL_NUMBER;
        i++;
        prev_sep = 0;
        continue;
      }
    }

    if(prev_sep)
    {
      int j;
      for(j = 0; keywords[j]; j++)
      {
        int klen = static_cast<int>(strlen(keywords[j]));
        int kw2  = keywords[j][klen - 1] == '|';
        if(kw2) klen--;

        if(!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen]))
        {
          memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, static_cast<size_t>(klen));
          i += klen;
          break;
        }
      }
      if(keywords[j] != nullptr)
      {
        prev_sep = 0;
        continue;
      }
    }

    prev_sep = is_separator(c);
    i++;
  }

  int changed          = (row->hl_open_comment != in_comment);
  row->hl_open_comment = in_comment;
  if(changed && row->idx + 1 < E.numrows) editorUpdateSyntax(&E.row[row->idx + 1]);
}

Term::Color editorSyntaxToColor(const std::size_t& color)
{
  switch(color)
  {
    case HL_COMMENT: return Term::Color::Name::BrightCyan;
    case HL_MLCOMMENT: return Term::Color::Name::Cyan;
    case HL_KEYWORD1: return Term::Color::Name::Yellow;
    case HL_KEYWORD2: return Term::Color::Name::Green;
    case HL_STRING: return Term::Color::Name::Magenta;
    case HL_NUMBER: return Term::Color::Name::Red;
    case HL_MATCH: return Term::Color::Name::Blue;
    default: return Term::Color::Name::Gray;
  }
}

void editorSelectSyntaxHighlight()
{
  if(E.filename.empty()) { return; }
  const std::size_t found{E.filename.find_last_of('.')};
  if(found == std::string::npos) { return; }
  const std::string ext{E.filename.substr(found)};

  for(std::size_t j = 0; j != HLDB.size(); ++j)
  {
    for(std::size_t i = 0; i != HLDB[j].filematch.size(); ++i)
    {
      if(HLDB[j].filematch[i] == ext)
      {
        E.syntax = HLDB[j];
        for(std::size_t filerow = 0; filerow < E.numrows; filerow++) { editorUpdateSyntax(&E.row[filerow]); }
        return;
      }
    }
  }
}

/*** row operations ***/

int editorRowCxToRx(erow* row, int cx)
{
  int rx = 0;
  for(std::size_t j = 0; j < cx; ++j)
  {
    if(row->chars[j] == '\t') { rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP); }
    ++rx;
  }
  return rx;
}

int editorRowRxToCx(erow* row, int rx)
{
  int cur_rx = 0;
  int cx{};
  for(cx = 0; cx < row->size; cx++)
  {
    if(row->chars[cx] == '\t') cur_rx += (KILO_TAB_STOP - 1) - (cur_rx % KILO_TAB_STOP);
    cur_rx++;

    if(cur_rx > rx) return cx;
  }
  return cx;
}

void editorUpdateRow(erow* row)
{
  int tabs = 0;
  for(int j = 0; j < row->size; j++)
    if(row->chars[j] == '\t') tabs++;

  free(row->render);
  row->render = reinterpret_cast<char*>(malloc(static_cast<size_t>(row->size + tabs * (KILO_TAB_STOP - 1) + 1)));
  if(row->render == nullptr) { exit(-1); /*TODO: create better check*/ }

  int idx = 0;
  for(int j = 0; j < row->size; j++)
  {
    if(row->chars[j] == '\t')
    {
      row->render[idx++] = ' ';
      while(idx % KILO_TAB_STOP != 0) row->render[idx++] = ' ';
    }
    else { row->render[idx++] = row->chars[j]; }
  }
  row->render[idx] = '\0';
  row->rsize       = idx;

  editorUpdateSyntax(row);
}

void editorInsertRow(int at, const char* s, std::size_t len)
{
  if(at < 0 || at > E.numrows) return;

  E.row = reinterpret_cast<erow*>(realloc(E.row, sizeof(erow) * (static_cast<size_t>(E.numrows) + 1)));
  if(E.row == nullptr) { exit(-1); /*TODO: create better check*/ }
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (static_cast<size_t>(E.numrows) - at));
  for(int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;

  E.row[at].idx = at;

  E.row[at].size  = static_cast<int>(len);
  E.row[at].chars = (char*)malloc(len + 1);
  if(E.row[at].chars == nullptr)
  {
    // allocation could fail and return nullptr
    exit(-1);  // insert better error handling if needed
  }
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize           = 0;
  E.row[at].render          = nullptr;
  E.row[at].hl              = nullptr;
  E.row[at].hl_open_comment = 0;
  editorUpdateRow(&E.row[at]);

  E.numrows++;
  E.dirty = true;
}

void editorFreeRow(erow* row)
{
  free(row->render);
  free(row->chars);
  free(row->hl);
}

void editorDelRow(int at)
{
  if(at < 0 || at >= E.numrows) return;
  editorFreeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (static_cast<size_t>(E.numrows) - at - 1));
  for(int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
  E.numrows--;
  E.dirty = true;
}

void editorRowInsertChar(erow* row, int at, int c)
{
  if(at < 0 || at > row->size) at = row->size;
  row->chars = reinterpret_cast<char*>(realloc(row->chars, static_cast<size_t>(row->size) + 2));
  if(row->chars == nullptr) { exit(-1); /*TODO: create better check*/ }
  memmove(&row->chars[at + 1], &row->chars[at], static_cast<size_t>(row->size) - at + 1);
  row->size++;
  row->chars[at] = static_cast<char>(c);
  editorUpdateRow(row);
  E.dirty = true;
}

void editorRowAppendString(erow* row, char* s, std::size_t len)
{
  row->chars = reinterpret_cast<char*>(realloc(row->chars, static_cast<size_t>(row->size) + len + 1));
  if(row->chars == nullptr) { exit(-1); /*TODO: create better check*/ }
  memcpy(&row->chars[row->size], s, len);
  row->size += static_cast<int>(len);
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty = true;
}

void editorRowDelChar(erow* row, int at)
{
  if(at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], static_cast<size_t>(row->size) - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty = true;
}

/*** editor operations ***/

void editorInsertChar(const Term::Key& key)
{
  if(E.cy == E.numrows) { editorInsertRow(E.numrows, "", 0); }
  editorRowInsertChar(&E.row[E.cy], E.cx, key);
  E.cx++;
}

void editorInsertNewline()
{
  if(E.cx == 0) { editorInsertRow(E.cy, "", 0); }
  else
  {
    erow* row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, &row->chars[E.cx], static_cast<size_t>(row->size - E.cx));
    row                   = &E.row[E.cy];
    row->size             = E.cx;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
  }
  E.cy++;
  E.cx = 0;
}

void editorDelChar()
{
  if(E.cy == E.numrows) return;
  if(E.cx == 0 && E.cy == 0) return;

  erow* row = &E.row[E.cy];
  if(E.cx > 0)
  {
    editorRowDelChar(row, E.cx - 1);
    E.cx--;
  }
  else
  {
    E.cx = E.row[E.cy - 1].size;
    editorRowAppendString(&E.row[E.cy - 1], row->chars, static_cast<size_t>(row->size));
    editorDelRow(E.cy);
    E.cy--;
  }
}

/*** file i/o ***/

std::string editorRowsToString()
{
  std::string ret;
  for(std::size_t line = 0; line < E.numrows; ++line) { ret += std::string(E.row[line].chars, static_cast<std::size_t>(E.row[line].size)) + '\n'; }
  return ret;
}

void editorOpen(const std::string& filename)
{
  E.filename = filename;
  editorSelectSyntaxHighlight();

  std::ifstream file(E.filename);
  if(file.fail()) { throw std::runtime_error("File failed to open."); }
  std::string line;
  std::getline(file, line);
  while(file.rdstate() == std::ios_base::goodbit)
  {
    long linelen = static_cast<long>(line.size());
    while(linelen > 0 && (line[static_cast<size_t>(linelen - 1)] == '\n' || line[static_cast<size_t>(linelen - 1)] == '\r')) linelen--;
    editorInsertRow(E.numrows, line.c_str(), linelen);
    std::getline(file, line);
  }
  E.dirty = false;
}

void editorSetStatusMessage(const std::string fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(&E.statusmsg[0], E.statusmsg.capacity(), fmt.c_str(), ap);
  va_end(ap);
}

void editorSave()
{
  if(E.filename.empty())
  {
    E.filename = editorPrompt("Save as: %s (ESC to cancel)", nullptr);
    if(E.filename.empty())
    {
      editorSetStatusMessage("Save aborted");
      return;
    }
    editorSelectSyntaxHighlight();
  }

  std::string   text = editorRowsToString();
  std::ofstream out;
  out.open(E.filename);
  out << text;
  out.close();
  E.dirty = false;
  editorSetStatusMessage("%d bytes written to disk", text.size());
}

/*** find ***/

void editorFindCallback(std::string& query, const Term::Key& key)
{
  static int last_match = -1;
  static int direction  = 1;

  static int   saved_hl_line;
  static char* saved_hl = nullptr;

  if(saved_hl)
  {
    memcpy(E.row[saved_hl_line].hl, saved_hl, static_cast<size_t>(E.row[saved_hl_line].rsize));
    free(saved_hl);
    saved_hl = nullptr;
  }

  if(key == Term::Key::Enter || key == Term::Key::Esc)
  {
    last_match = -1;
    direction  = 1;
    return;
  }
  else if(key == Term::Key::ArrowRight || key == Term::Key::ArrowDown) { direction = 1; }
  else if(key == Term::Key::ArrowLeft || key == Term::Key::ArrowUp) { direction = -1; }
  else
  {
    last_match = -1;
    direction  = 1;
  }

  if(last_match == -1) direction = 1;
  int current = last_match;
  for(int i = 0; i < E.numrows; i++)
  {
    current += direction;
    if(current == -1) { current = E.numrows - 1; }
    else if(current == E.numrows) { current = 0; }

    erow* row   = &E.row[current];
    char* match = strstr(row->render, query.c_str());
    if(match)
    {
      last_match = current;
      E.cy       = current;
      E.cx       = editorRowRxToCx(row, static_cast<int>(match - row->render));
      E.rowoff   = E.numrows;

      saved_hl_line = current;
      saved_hl      = reinterpret_cast<char*>(malloc(static_cast<size_t>(row->rsize)));
      if(saved_hl == nullptr) { exit(-1); /*TODO: better handler*/ }
      memcpy(saved_hl, row->hl, row->rsize);
      memset(&row->hl[match - row->render], HL_MATCH, query.size());
      break;
    }
  }
}

void editorFind()
{
  const std::size_t saved_cx{E.cx};
  const std::size_t saved_cy{E.cy};
  const std::size_t saved_coloff{E.coloff};
  const std::size_t saved_rowoff{E.rowoff};

  std::string query = editorPrompt("Search: %s (Use ESC/Arrows/Enter)", editorFindCallback);

  if(query.empty())
  {
    E.cx     = saved_cx;
    E.cy     = saved_cy;
    E.coloff = saved_coloff;
    E.rowoff = saved_rowoff;
  }
}

/*** output ***/

void editorScroll()
{
  E.rx = 0;
  if(E.cy < E.numrows) { E.rx = editorRowCxToRx(&E.row[E.cy], E.cx); }

  if(E.cy < E.rowoff) { E.rowoff = E.cy; }
  if(E.cy >= E.rowoff + E.screenrows) { E.rowoff = E.cy - E.screenrows + 1; }
  if(E.rx < E.coloff) { E.coloff = E.rx; }
  if(E.rx >= E.coloff + E.screencols) { E.coloff = E.rx - E.screencols + 1; }
}

void editorDrawRows(std::string& screen)
{
  for(int y = 0; y < E.screenrows; y++)
  {
    int filerow = y + E.rowoff;
    if(filerow >= E.numrows)
    {
      if(E.numrows == 0 && y == E.screenrows / 3)
      {
        char welcome[80];
        int  welcomelen = snprintf(welcome, sizeof(welcome), "Kilo editor -- version %s", KILO_VERSION.c_str());
        if(welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if(padding)
        {
          screen.append("~");
          padding--;
        }
        while(padding--) screen.append(" ");
        screen.append(welcome);
      }
      else { screen.append("~"); }
    }
    else
    {
      int len = E.row[filerow].rsize - E.coloff;
      if(len < 0) len = 0;
      if(len > E.screencols) len = E.screencols;
      char*          c             = &E.row[filerow].render[E.coloff];
      unsigned char* hl            = &E.row[filerow].hl[E.coloff];
      Term::Color    current_color = Term::Color::Name::Black;  // black is not used in editorSyntaxToColor
      int            j;
      for(j = 0; j < len; j++)
      {
        if(iscntrl(c[j]))
        {
          char sym = (c[j] <= 26) ? '@' + c[j] : '?';
          screen.append(style(Term::Style::Reversed));
          screen.append(std::string(&sym, 1));
          screen.append(style(Term::Style::Reset));
          if(current_color != Term::Color::Name::Default) { screen.append(color_fg(current_color)); }
        }
        else if(hl[j] == HL_NORMAL)
        {
          if(current_color != Term::Color::Name::Black)
          {
            screen.append(color_fg(Term::Color::Name::Default));
            current_color = Term::Color::Name::Black;
          }
          screen.append(std::string(&c[j], 1));
        }
        else
        {
          Term::Color color = editorSyntaxToColor(hl[j]);
          if(color != current_color)
          {
            current_color = color;
            screen.append(Term::color_fg(color));
          }
          screen.append(std::string(&c[j], 1));
        }
      }
      screen.append(color_fg(Term::Color::Name::Default));
    }

    screen.append(Term::clear_eol());
    screen.append("\r\n");
  }
}

void editorDrawStatusBar(std::string& screen)
{
  screen.append(style(Term::Style::Reversed));
  std::string left1;
  if(!E.filename.empty()) { left1 += E.filename; }
  else { left1 += "[No Name]"; }
  std::string left2 = " - " + std::to_string(E.numrows) + " lines ";
  if(E.dirty) { left2 += "(modified)"; }
  std::string right;
  if(!E.syntax.empty()) { right += E.syntax.filetype + " | "; }
  else { right += "no ft | "; }
  right += std::to_string(E.cy + 1) + "/" + std::to_string(E.numrows);
  if((left1.size() + left2.size() + right.size()) < E.screencols) { screen += left1 + left2 + std::string(E.screencols - right.size() - left1.size() - left2.size(), ' ') + right; }
  else
  {
    const std::size_t size = right.size() + left2.size();
    if(E.screencols - size > 0 && (left1.size() - E.screencols + size) < left1.size()) screen += left1.substr(left1.size() - E.screencols + size, E.screencols - size) + left2 + right;
  }
  screen.append(style(Term::Style::Reset));
  screen.append("\r\n");
}

void editorDrawMessageBar(std::string& screen)
{
  screen.append(Term::clear_eol());
  if(E.statusmsg.size() > E.screencols) E.statusmsg.resize(E.screencols);
  if(!E.statusmsg.empty()) { screen.append(E.statusmsg); }
}

void editorRefreshScreen()
{
  editorScroll();
  std::string screen;
  screen.reserve(E.screencols * (E.screenrows + 2));  //FIXME Obtain 2 from E !
  screen.append(Term::cursor_off());
  screen.append(Term::cursor_move(1, 1));
  editorDrawRows(screen);
  editorDrawStatusBar(screen);
  editorDrawMessageBar(screen);
  screen.append(Term::cursor_move(static_cast<size_t>((E.cy - E.rowoff) + 1), static_cast<size_t>((E.rx - E.coloff) + 1)));
  screen.append(Term::cursor_on());
  Term::cout << screen << std::flush;
}

/*** input ***/

std::string editorPrompt(const std::string& prompt, void (*callback)(std::string&, const Term::Key&))
{
  std::string buf;
  buf.reserve(128);
  while(true)
  {
    editorSetStatusMessage(prompt, buf.c_str());
    editorRefreshScreen();

    Term::Key c = Term::read_event();

    if(c.empty()) continue;
    else if(c == Term::Key::Del || c == Term::Key::Ctrl_H || c == Term::Key::Backspace)
    {
      if(!buf.empty()) buf.pop_back();
    }
    else if(c == Term::Key::Esc)
    {
      editorSetStatusMessage("");
      if(callback) callback(buf, c);
      return {};
    }
    else if(c == Term::Key::Enter)
    {
      if(!buf.empty())
      {
        editorSetStatusMessage("");
        if(callback) callback(buf, c);
        return buf;
      }
    }
    else if(!c.iscntrl() && c.isASCII()) { buf += static_cast<char>(c); }

    if(callback) callback(buf, c);
  }
}

void editorMoveCursor(const Term::Key& key)
{
  erow* row = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];

  switch(key)
  {
    case Term::Key::ArrowLeft:
      if(E.cx != 0) { E.cx--; }
      else if(E.cy > 0)
      {
        E.cy--;
        E.cx = E.row[E.cy].size;
      }
      break;
    case Term::Key::ArrowRight:
      if(row && E.cx < row->size) { E.cx++; }
      else if(row && E.cx == row->size)
      {
        E.cy++;
        E.cx = 0;
      }
      break;
    case Term::Key::ArrowUp:
      if(E.cy != 0) { E.cy--; }
      break;
    case Term::Key::ArrowDown:
      if(E.cy < E.numrows) { E.cy++; }
      break;
    default: return;
  }

  row        = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];
  int rowlen = row ? row->size : 0;
  if(E.cx > rowlen) { E.cx = rowlen; }
}

bool editorProcessKeypress()
{
  static std::size_t quit_times{KILO_QUIT_TIMES};

  Term::Event event{Term::read_event()};
  switch(event.type())
  {
    case Term::Event::Type::Key:
    {
      Term::Key key = event;
      switch(key)
      {
        case Term::Key::Enter:
        {
          ::editorInsertNewline();
          break;
        }
        case Term::Key::Ctrl_Q:
        {
          if(E.dirty == true && quit_times > 0)
          {
            ::editorSetStatusMessage("WARNING!!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit_times);
            --quit_times;
            return true;
          }
          return false;
        }
        case Term::Key::Ctrl_S:
        {
          ::editorSave();
          break;
        }
        case Term::Key::Home:
        {
          E.cx = 0;
          break;
        }
        case Term::Key::End:
        {
          if(E.cy < E.numrows) { E.cx = E.row[E.cy].size; }
          break;
        }
        case Term::Key::Ctrl_F:
        {
          ::editorFind();
          break;
        }
        case Term::Key::Backspace:
        case Term::Key::Del:
        {
          if(key == Term::Key::Del) { ::editorMoveCursor(Term::Key::ArrowRight); }
          ::editorDelChar();
          break;
        }
        case Term::Key::PageUp:
        case Term::Key::PageDown:
        {
          if(key == Term::Key::PageUp) { E.cy = E.rowoff; }
          else if(key == Term::Key::PageDown)
          {
            E.cy = E.rowoff + E.screenrows - 1;
            if(E.cy > E.numrows) E.cy = E.numrows;
          }

          int times = E.screenrows;
          while(times--) { ::editorMoveCursor(key == Term::Key::PageUp ? Term::Key::ArrowUp : Term::Key::ArrowDown); }
          break;
        }

        case Term::Key::ArrowUp:
        case Term::Key::ArrowDown:
        case Term::Key::ArrowLeft:
        case Term::Key::ArrowRight:
        {
          ::editorMoveCursor(key);
          break;
        }
        case Term::Key::Ctrl_L:
        case Term::Key::Esc:
        {
          break;
        }
        case Term::Key::Tab:
        {
          ::editorInsertChar('\t');
          break;
        }
        default:
        {
          if((!key.hasCtrlAll()) && (key.isASCII())) { editorInsertChar(key); }  //FIXME UTF8 !
          break;
        }
      }
      break;
    }
    case Term::Event::Type::Screen:
    {
      E.setScreenSize(Term::Screen(event));
      break;
    }
    case Term::Event::Type::Mouse:
    {
      Term::Mouse mouse = event;
      switch(mouse.getButton().type())
      {
        case Term::Button::Type::Wheel:
        {
          if(mouse.getButton().action() == Term::Button::Action::RolledDown) { ::editorMoveCursor(Term::Key::ArrowDown); }
          else if(mouse.getButton().action() == Term::Button::Action::RolledUp) { ::editorMoveCursor(Term::Key::ArrowUp); }
          else { break; }
        }
        default: break;
      }
    }
    default: break;
  }
  return true;
}

}  // namespace

int main()
{
  // We must put all code in try/catch block, otherwise destructors are not
  // being called when exception happens and the terminal is not put into
  // correct state.
  try
  {
    // check if the terminal is capable of handling input
    if(!Term::is_stdin_a_tty())
    {
      std::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw);
    if(Term::argc >= 2) { editorOpen(Term::argv[1]); }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    editorRefreshScreen();
    while(editorProcessKeypress()) { editorRefreshScreen(); }
  }
  catch(const Term::Exception& exception)
  {
    std::cerr << "cpp-terminal error: " << exception.what() << std::endl;
    std::exit(2);
  }
  catch(const std::exception& exception)
  {
    std::cerr << "error: " << exception.what() << std::endl;
    std::exit(2);
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    std::exit(1);
  }
  return 0;
}
