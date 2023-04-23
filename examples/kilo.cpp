#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iostream>

const std::string KILO_VERSION{"0.0.1"};
const int         KILO_TAB_STOP{8};
const int         KILO_QUIT_TIMES{3};

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

#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)

/*** data ***/

struct editorSyntax
{
  const char*  filetype;
  const char** filematch;
  const char** keywords;
  const char*  singleline_comment_start;
  const char*  multiline_comment_start;
  const char*  multiline_comment_end;
  int          flags;
};

typedef struct erow
{
  int            idx;
  int            size;
  int            rsize;
  char*          chars;
  char*          render;
  unsigned char* hl;
  int            hl_open_comment;
} erow;

struct editorConfig
{
  int                  cx, cy;
  int                  rx;
  int                  rowoff;
  int                  coloff;
  int                  screenrows;
  int                  screencols;
  int                  numrows;
  erow*                row;
  int                  dirty;
  char*                filename;
  char                 statusmsg[80];
  time_t               statusmsg_time;
  struct editorSyntax* syntax;
};

struct editorConfig E;

/*** filetypes ***/

const char* C_HL_extensions[] = {".c", ".h", ".cpp", nullptr};
const char* C_HL_keywords[]   = {"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class", "case", "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", nullptr};

struct editorSyntax HLDB[] = {
  {"c", C_HL_extensions, C_HL_keywords, "//", "/*", "*/", HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/*** prototypes ***/

char* editorPrompt(const char* prompt, void (*callback)(char*, int));

/*** syntax highlighting ***/

int is_separator(int c) { return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != nullptr; }

void editorUpdateSyntax(erow* row)
{
  row->hl = (unsigned char*)realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);

  if(E.syntax == nullptr) return;

  const char** keywords = E.syntax->keywords;

  const char* scs = E.syntax->singleline_comment_start;
  const char* mcs = E.syntax->multiline_comment_start;
  const char* mce = E.syntax->multiline_comment_end;

  int scs_len = scs ? strlen(scs) : 0;
  int mcs_len = mcs ? strlen(mcs) : 0;
  int mce_len = mce ? strlen(mce) : 0;

  int prev_sep   = 1;
  int in_string  = 0;
  int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);

  int i = 0;
  while(i < row->rsize)
  {
    char          c       = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : (char)HL_NORMAL;

    if(scs_len && !in_string && !in_comment)
    {
      if(!strncmp(&row->render[i], scs, scs_len))
      {
        memset(&row->hl[i], HL_COMMENT, row->rsize - i);
        break;
      }
    }

    if(mcs_len && mce_len && !in_string)
    {
      if(in_comment)
      {
        row->hl[i] = HL_MLCOMMENT;
        if(!strncmp(&row->render[i], mce, mce_len))
        {
          memset(&row->hl[i], HL_MLCOMMENT, mce_len);
          i += mce_len;
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
      else if(!strncmp(&row->render[i], mcs, mcs_len))
      {
        memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
        i += mcs_len;
        in_comment = 1;
        continue;
      }
    }

    if(E.syntax->flags & HL_HIGHLIGHT_STRINGS)
    {
      if(in_string)
      {
        row->hl[i] = HL_STRING;
        if(c == '\\' && i + 1 < row->rsize)
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

    if(E.syntax->flags & HL_HIGHLIGHT_NUMBERS)
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
        int klen = strlen(keywords[j]);
        int kw2  = keywords[j][klen - 1] == '|';
        if(kw2) klen--;

        if(!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen]))
        {
          memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
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

Term::Color editorSyntaxToColor(int hl)
{
  switch(hl)
  {
    case HL_COMMENT:
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
  E.syntax = nullptr;
  if(E.filename == nullptr) return;

  char* ext = strrchr(E.filename, '.');

  for(std::size_t j = 0; j != HLDB_ENTRIES; ++j)
  {
    struct editorSyntax* s = &HLDB[j];
    unsigned int         i = 0;
    while(s->filematch[i])
    {
      int is_ext = (s->filematch[i][0] == '.');
      if((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(E.filename, s->filematch[i])))
      {
        E.syntax = s;

        int filerow;
        for(filerow = 0; filerow < E.numrows; filerow++) { editorUpdateSyntax(&E.row[filerow]); }

        return;
      }
      i++;
    }
  }
}

/*** row operations ***/

int editorRowCxToRx(erow* row, int cx)
{
  int rx = 0;
  for(int j = 0; j < cx; j++)
  {
    if(row->chars[j] == '\t') rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
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
  row->render = (char*)malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);

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

  E.row = (erow*)realloc(E.row, sizeof(erow) * (E.numrows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));
  for(int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;

  E.row[at].idx = at;

  E.row[at].size  = len;
  E.row[at].chars = (char*)malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize           = 0;
  E.row[at].render          = nullptr;
  E.row[at].hl              = nullptr;
  E.row[at].hl_open_comment = 0;
  editorUpdateRow(&E.row[at]);

  E.numrows++;
  E.dirty++;
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
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
  for(int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
  E.numrows--;
  E.dirty++;
}

void editorRowInsertChar(erow* row, int at, int c)
{
  if(at < 0 || at > row->size) at = row->size;
  row->chars = (char*)realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowAppendString(erow* row, char* s, std::size_t len)
{
  row->chars = (char*)realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowDelChar(erow* row, int at)
{
  if(at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}

/*** editor operations ***/

void editorInsertChar(int c)
{
  if(E.cy == E.numrows) { editorInsertRow(E.numrows, "", 0); }
  editorRowInsertChar(&E.row[E.cy], E.cx, c);
  E.cx++;
}

void editorInsertNewline()
{
  if(E.cx == 0) { editorInsertRow(E.cy, "", 0); }
  else
  {
    erow* row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
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
    editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
    editorDelRow(E.cy);
    E.cy--;
  }
}

/*** file i/o ***/

char* editorRowsToString(int* buflen)
{
  int totlen = 0;
  for(int j = 0; j < E.numrows; j++) totlen += E.row[j].size + 1;
  *buflen = totlen;

  char* buf = (char*)malloc(totlen);
  char* p   = buf;
  for(int j = 0; j < E.numrows; j++)
  {
    memcpy(p, E.row[j].chars, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }

  return buf;
}

void editorOpen(char* filename)
{
  free(E.filename);
#ifdef _WIN32
  E.filename = _strdup(filename);
#else
  E.filename = strdup(filename);
#endif
  editorSelectSyntaxHighlight();

  std::ifstream f(filename);
  if(f.fail()) throw std::runtime_error("File failed to open.");
  std::string line;
  std::getline(f, line);
  while(f.rdstate() == std::ios_base::goodbit)
  {
    int linelen = line.size();
    while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) linelen--;
    editorInsertRow(E.numrows, line.c_str(), linelen);
    std::getline(f, line);
  }
  E.dirty = 0;
}

void editorSetStatusMessage(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(nullptr);
}

void editorSave()
{
  if(E.filename == nullptr)
  {
    E.filename = editorPrompt("Save as: %s (ESC to cancel)", nullptr);
    if(E.filename == nullptr)
    {
      editorSetStatusMessage("Save aborted");
      return;
    }
    editorSelectSyntaxHighlight();
  }

  int         len;
  char*       buf = editorRowsToString(&len);
  std::string s   = std::string(buf, len);
  free(buf);

  std::ofstream out;
  out.open(E.filename);
  out << s;
  out.close();
  E.dirty = 0;
  editorSetStatusMessage("%d bytes written to disk", len);
}

/*** find ***/

void editorFindCallback(char* query, int key)
{
  static int last_match = -1;
  static int direction  = 1;

  static int   saved_hl_line;
  static char* saved_hl = nullptr;

  if(saved_hl)
  {
    memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize);
    free(saved_hl);
    saved_hl = nullptr;
  }

  if(key == Term::Key::ENTER || key == Term::Key::ESC)
  {
    last_match = -1;
    direction  = 1;
    return;
  }
  else if(key == Term::Key::ARROW_RIGHT || key == Term::Key::ARROW_DOWN) { direction = 1; }
  else if(key == Term::Key::ARROW_LEFT || key == Term::Key::ARROW_UP) { direction = -1; }
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
    if(current == -1) current = E.numrows - 1;
    else if(current == E.numrows)
      current = 0;

    erow* row   = &E.row[current];
    char* match = strstr(row->render, query);
    if(match)
    {
      last_match = current;
      E.cy       = current;
      E.cx       = editorRowRxToCx(row, match - row->render);
      E.rowoff   = E.numrows;

      saved_hl_line = current;
      saved_hl      = (char*)malloc(row->rsize);
      memcpy(saved_hl, row->hl, row->rsize);
      memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
      break;
    }
  }
}

void editorFind()
{
  int saved_cx     = E.cx;
  int saved_cy     = E.cy;
  int saved_coloff = E.coloff;
  int saved_rowoff = E.rowoff;

  char* query = editorPrompt("Search: %s (Use ESC/Arrows/Enter)", editorFindCallback);

  if(query) { free(query); }
  else
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

void editorDrawRows(std::string& ab)
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
          ab.append("~");
          padding--;
        }
        while(padding--) ab.append(" ");
        ab.append(welcome);
      }
      else { ab.append("~"); }
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
          ab.append(style(Term::Style::REVERSED));
          ab.append(std::string(&sym, 1));
          ab.append(style(Term::Style::RESET));
          if(current_color != Term::Color::Name::Default) { ab.append(color_fg(current_color)); }
        }
        else if(hl[j] == HL_NORMAL)
        {
          if(current_color != Term::Color::Name::Black)
          {
            ab.append(color_fg(Term::Color::Name::Default));
            current_color = Term::Color::Name::Black;
          }
          ab.append(std::string(&c[j], 1));
        }
        else
        {
          Term::Color color = editorSyntaxToColor(hl[j]);
          if(color != current_color)
          {
            current_color = color;
            ab.append(Term::color_fg(color));
          }
          ab.append(std::string(&c[j], 1));
        }
      }
      ab.append(color_fg(Term::Color::Name::Default));
    }

    ab.append(Term::clear_eol());
    ab.append("\r\n");
  }
}

void editorDrawStatusBar(std::string& ab)
{
  ab.append(style(Term::Style::REVERSED));
  char status[80], rstatus[80];
  int  len  = snprintf(status, sizeof(status), "%.20s - %d lines %s", E.filename ? E.filename : "[No Name]", E.numrows, E.dirty ? "(modified)" : "");
  int  rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows);
  if(len > E.screencols) len = E.screencols;
  ab.append(std::string(status, len));
  while(len < E.screencols)
  {
    if(E.screencols - len == rlen)
    {
      ab.append(std::string(rstatus, rlen));
      break;
    }
    else
    {
      ab.append(" ");
      len++;
    }
  }
  ab.append(style(Term::Style::RESET));
  ab.append("\r\n");
}

void editorDrawMessageBar(std::string& ab)
{
  ab.append(Term::clear_eol());
  int msglen = strlen(E.statusmsg);
  if(msglen > E.screencols) msglen = E.screencols;
  if(msglen && time(nullptr) - E.statusmsg_time < 5) ab.append(std::string(E.statusmsg, msglen));
}

void editorRefreshScreen()
{
  editorScroll();

  std::string ab;
  ab.reserve(16 * 1024);

  ab.append(Term::cursor_off());
  ab.append(Term::cursor_move(1, 1));

  editorDrawRows(ab);
  editorDrawStatusBar(ab);
  editorDrawMessageBar(ab);

  ab.append(Term::cursor_move((E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1));

  ab.append(Term::cursor_on());

  std::cout << ab << std::flush;
}

/*** input ***/

char* editorPrompt(const char* prompt, void (*callback)(char*, int))
{
  std::size_t bufsize = 128;
  char*       buf     = (char*)malloc(bufsize);

  std::size_t buflen = 0;
  buf[0]             = '\0';

  while(true)
  {
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();

    Term::Key c = Term::read_event();

    if(c) continue;
    if(c == Term::Key::DEL || c == Term::Key::CTRL + 'h' || c == Term::Key::BACKSPACE)
    {
      if(buflen != 0) buf[--buflen] = '\0';
    }
    else if(c == Term::Key::ESC)
    {
      editorSetStatusMessage("");
      if(callback) callback(buf, c);
      free(buf);
      return nullptr;
    }
    else if(c == Term::Key::ENTER)
    {
      if(buflen != 0)
      {
        editorSetStatusMessage("");
        if(callback) callback(buf, c);
        return buf;
      }
    }
    else if(!iscntrl(c) && c < 128)
    {
      if(buflen == bufsize - 1)
      {
        bufsize *= 2;
        buf = (char*)realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen]   = '\0';
    }

    if(callback) callback(buf, c);
  }
}

void editorMoveCursor(int key)
{
  erow* row = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];

  switch(key)
  {
    case Term::Key::ARROW_LEFT:
      if(E.cx != 0) { E.cx--; }
      else if(E.cy > 0)
      {
        E.cy--;
        E.cx = E.row[E.cy].size;
      }
      break;
    case Term::Key::ARROW_RIGHT:
      if(row && E.cx < row->size) { E.cx++; }
      else if(row && E.cx == row->size)
      {
        E.cy++;
        E.cx = 0;
      }
      break;
    case Term::Key::ARROW_UP:
      if(E.cy != 0) { E.cy--; }
      break;
    case Term::Key::ARROW_DOWN:
      if(E.cy < E.numrows) { E.cy++; }
      break;
  }

  row        = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];
  int rowlen = row ? row->size : 0;
  if(E.cx > rowlen) { E.cx = rowlen; }
}

bool editorProcessKeypress()
{
  static int quit_times = KILO_QUIT_TIMES;

  Term::Key c;
  while((c = Term::read_event()).empty()) continue;  //NEEDED for windows (FIXME ?)
  switch(c)
  {
    case Term::Key::ENTER: editorInsertNewline(); break;

    case Term::Key::CTRL_Q:
      if(E.dirty && quit_times > 0)
      {
        editorSetStatusMessage("WARNING!!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit_times);
        quit_times--;
        return true;
      }
      return false;
      break;

    case Term::Key::CTRL_S: editorSave(); break;

    case Term::Key::HOME: E.cx = 0; break;

    case Term::Key::END:
      if(E.cy < E.numrows) E.cx = E.row[E.cy].size;
      break;

    case Term::Key::CTRL_F: editorFind(); break;

    case Term::Key::BACKSPACE:
    case Term::Key::DEL:
      if(c == Term::Key::DEL) editorMoveCursor(Term::Key::ARROW_RIGHT);
      editorDelChar();
      break;

    case Term::Key::PAGE_UP:
    case Term::Key::PAGE_DOWN:
    {
      if(c == Term::Key::PAGE_UP) { E.cy = E.rowoff; }
      else if(c == Term::Key::PAGE_DOWN)
      {
        E.cy = E.rowoff + E.screenrows - 1;
        if(E.cy > E.numrows) E.cy = E.numrows;
      }

      int times = E.screenrows;
      while(times--) editorMoveCursor(c == Term::Key::PAGE_UP ? Term::Key::ARROW_UP : Term::Key::ARROW_DOWN);
    }
    break;

    case Term::Key::ARROW_UP:
    case Term::Key::ARROW_DOWN:
    case Term::Key::ARROW_LEFT:
    case Term::Key::ARROW_RIGHT: editorMoveCursor(c); break;

    case Term::Key::CTRL_L:
    case Term::Key::ESC: break;

    case Term::Key::TAB: editorInsertChar('\t'); break;

    default: editorInsertChar(c); break;
  }

  quit_times = KILO_QUIT_TIMES;
  return true;
}

/*** init ***/

void initEditor()
{
  E.cx                = 0;
  E.cy                = 0;
  E.rx                = 0;
  E.rowoff            = 0;
  E.coloff            = 0;
  E.numrows           = 0;
  E.row               = nullptr;
  E.dirty             = 0;
  E.filename          = nullptr;
  E.statusmsg[0]      = '\0';
  E.statusmsg_time    = 0;
  E.syntax            = nullptr;
  Term::Screen screen = Term::screen_size();
  E.screenrows        = screen.rows();
  E.screencols        = screen.columns();
  E.screenrows -= 2;
}

int main(int argc, char* argv[])
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
    Term::terminal.setOptions({Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw});
    initEditor();
    if(argc >= 2) { editorOpen(argv[1]); }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    editorRefreshScreen();
    while(editorProcessKeypress()) { editorRefreshScreen(); }
  }
  catch(const Term::Exception& re)
  {
    std::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
