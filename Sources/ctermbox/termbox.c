#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for wcstring, strcasestr
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#include "termbox.h"
#include "bytebuffer.inl"
#include "term.inl"
#include "input.inl"

struct cellbuf {
  int width;
  int height;
  struct tb_cell *cells;
};

#define CELL(buf, x, y) (buf)->cells[(y) * (buf)->width + (x)]
#define IS_CURSOR_HIDDEN(cx, cy) (cx == -1 || cy == -1)
#define LAST_COORD_INIT -1

static struct termios orig_tios;

static struct cellbuf back_buffer;
static struct cellbuf front_buffer;
static struct bytebuffer output_buffer;
static struct bytebuffer input_buffer;

static int termw = -1;
static int termh = -1;

static bool title_set = false;
static int initflags = TB_INIT_ALL;

static int inout;
static int winch_fds[2];

static int lastx = LAST_COORD_INIT;
static int lasty = LAST_COORD_INIT;
static int cursor_x = -1;
static int cursor_y = -1;

static int output_mode = TB_OUTPUT_NORMAL;
static tb_color background = TB_DEFAULT;
static tb_color foreground = TB_DEFAULT;

static void write_cursor(int x, int y);
static void write_title(const char * title);

static void cellbuf_init(struct cellbuf *buf, int width, int height);
static void cellbuf_resize(struct cellbuf *buf, int width, int height);
static void cellbuf_clear(struct cellbuf *buf);
static void cellbuf_free(struct cellbuf *buf);

static void update_term_size(void);
static void set_colors(tb_color fg, tb_color bg);
static void send_char(int x, int y, uint32_t c);
static void sigwinch_handler(int xxx);
static int wait_fill_event(struct tb_event *event, struct timeval *timeout);

/* may happen in a different thread */
static volatile int buffer_size_change_request;

/* -------------------------------------------------------- */

int tb_init_fd(int inout_) {
  inout = inout_;
  if (inout == -1) {
    return TB_EFAILED_TO_OPEN_TTY;
  }

  if (init_term() < 0) {
    close(inout);
    return TB_EUNSUPPORTED_TERMINAL;
  }

  if (pipe(winch_fds) < 0) {
    close(inout);
    return TB_EPIPE_TRAP_ERROR;
  }

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigwinch_handler;
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, 0);

  tcgetattr(inout, &orig_tios);
  struct termios tios;
  memcpy(&tios, &orig_tios, sizeof(tios));

  tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON); // INPCK
  tios.c_oflag &= ~OPOST;
  tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tios.c_cflag &= ~(CSIZE | PARENB);
  tios.c_cflag |= CS8;
  tios.c_cc[VMIN] = 0;  // Return each byte, or zero for timeout.
  tios.c_cc[VTIME] = 0; // 0ms timeout (unit is tens of second).

  tcsetattr(inout, TCSAFLUSH, &tios);
  return 0;
}

int tb_init_screen(int flags) {
  bytebuffer_init(&input_buffer, 128);
  bytebuffer_init(&output_buffer, 32 * 1024);

  initflags = flags;

  if (initflags & TB_INIT_DETECT_MODE)
    output_mode = detect_color_support();

  if (initflags & TB_INIT_NO_CURSOR)
    tb_hide_cursor();

  if (initflags & TB_INIT_KEYPAD)
    bytebuffer_puts(&output_buffer, funcs[T_ENTER_KEYPAD]);

  if (initflags & TB_INIT_ALTSCREEN) {
    bytebuffer_puts(&output_buffer, funcs[T_ENTER_CA]);
    tb_clear_screen(); // flushes output
  } else {
    bytebuffer_flush(&output_buffer, inout);
  }

  update_term_size();
  cellbuf_init(&back_buffer, termw, termh);
  cellbuf_init(&front_buffer, termw, termh);
  cellbuf_clear(&back_buffer);
  cellbuf_clear(&front_buffer);

  return 0;
}

int tb_init_file(const char* name) {
  return tb_init_fd(open(name, O_RDWR));
}

int tb_init_with(int flags) {
  int res = tb_init_file("/dev/tty");
  if (res != 0) return res;

  return tb_init_screen(flags);
}

int tb_init(void) {
  int res = tb_init_file("/dev/tty");
  if (res != 0) return res;

  return tb_init_screen(TB_INIT_ALL);
}

void tb_shutdown(void) {
  if (termw == -1) {
    fputs("term not initialized.", stderr);
    return;
  }

  if (title_set) write_title("");
  tb_show_cursor();
  bytebuffer_puts(&output_buffer, funcs[T_SGR0]); // reset attrs

  if (initflags & TB_INIT_ALTSCREEN) {
    bytebuffer_puts(&output_buffer, funcs[T_EXIT_CA]);

    // don't clear screen by default. if user wants to, he can
    // just call tb_clear_screen() anyway
    // bytebuffer_puts(&output_buffer, funcs[T_CLEAR_SCREEN]);
  }

  if (initflags & TB_INIT_KEYPAD)
    bytebuffer_puts(&output_buffer, funcs[T_EXIT_KEYPAD]);

  bytebuffer_puts(&output_buffer, funcs[T_EXIT_MOUSE]);
  bytebuffer_flush(&output_buffer, inout);
  tcsetattr(inout, TCSAFLUSH, &orig_tios);

  shutdown_term();
  close(inout);
  close(winch_fds[0]);
  close(winch_fds[1]);

  cellbuf_free(&back_buffer);
  cellbuf_free(&front_buffer);
  bytebuffer_free(&output_buffer);
  bytebuffer_free(&input_buffer);
  termw = termh = -1;
}

void tb_render(void) {
  int x,y,w,i;
  struct tb_cell *back, *front;

  /* invalidate cursor position */
  lastx = LAST_COORD_INIT;
  lasty = LAST_COORD_INIT;

  if (buffer_size_change_request)
    tb_resize();

  for (y = 0; y < front_buffer.height; ++y) {
    for (x = 0; x < front_buffer.width; ) {

      // get back and front cells for x/y position
      back = &CELL(&back_buffer, x, y);
      front = &CELL(&front_buffer, x, y);

      // get width of char
      w = wcwidth(back->ch); // tb_unicode_is_char_wide(back->ch) ? 2 : 1;
      if (w < 1) w = 1;

      // if back cell hasn't changed, then skip to next one
      if (memcmp(back, front, sizeof(struct tb_cell)) == 0) {
        x += w;
        continue;
      }

      // copy back cell to front and set attributes
      memcpy(front, back, sizeof(struct tb_cell));
      set_colors(back->fg, back->bg);

      // if we have a wide char, but x position + char width would exceed screen width
      if (w == 2 && x >= front_buffer.width-1) {

        send_char(x, y, ' ');

      // otherwise, if we have a regular char or if there's enough room
      } else {

        // then send the char
        send_char(x, y, back->ch);

        // and empty the following cells, if needed (wide char)
        for (i = 1; i < w; ++i) {
          front = &CELL(&front_buffer, x + i, y);
          front->ch = 0;
          front->fg = back->fg;
          front->bg = back->bg;
        }
      }

      x += w;
    }
  }

  if (!IS_CURSOR_HIDDEN(cursor_x, cursor_y))
    write_cursor(cursor_x, cursor_y);

  bytebuffer_flush(&output_buffer, inout);
}

void tb_set_cursor(int cx, int cy) {
  if (IS_CURSOR_HIDDEN(cursor_x, cursor_y) && !IS_CURSOR_HIDDEN(cx, cy))
    tb_show_cursor();

  if (!IS_CURSOR_HIDDEN(cursor_x, cursor_y) && IS_CURSOR_HIDDEN(cx, cy))
    tb_hide_cursor();

  cursor_x = cx;
  cursor_y = cy;

  if (!IS_CURSOR_HIDDEN(cursor_x, cursor_y))
    write_cursor(cursor_x, cursor_y);
}

void tb_set_title(const char * title) {
  title_set = true;
  write_title(title);
}

void tb_flush(void) {
  bytebuffer_flush(&output_buffer, inout);
}

void tb_send(const char * str) {
  bytebuffer_puts(&output_buffer, str); // same as append but without length
}

void tb_sendf(const char * fmt, ...) {
  char buf[1024];
  va_list vl;
  va_start(vl, fmt);
  vsnprintf(buf, sizeof(buf), fmt, vl);
  va_end(vl);
  return tb_send(buf);
}

void tb_cell(int x, int y, const struct tb_cell *cell) {
  if ((unsigned)x >= (unsigned)back_buffer.width)
    return;

  if ((unsigned)y >= (unsigned)back_buffer.height)
    return;

  CELL(&back_buffer, x, y) = *cell;
}

void tb_char(int x, int y, tb_color fg, tb_color bg, uint32_t ch) {
  struct tb_cell c = {ch, fg, bg};
  tb_cell(x, y, &c);
}

#define MAX_LIMIT 1024

int tb_string_with_limit(int x, int y, tb_color fg, tb_color bg, char *str, int limit) {
  uint32_t uni;
  int w, c = 0, l = 0;

  while (*str && l < limit) {
    str += tb_utf8_char_to_unicode(&uni, str);
    tb_char(x, y, fg, bg, uni);
    w = tb_unicode_is_char_wide(uni) ? 2 : 1;
    c++;
    x++;
    l = l + w;
  }

  return l;
}

int tb_string(int x, int y, tb_color fg, tb_color bg, char *str) {
  return tb_string_with_limit(x, y, fg, bg, str, MAX_LIMIT);
}

int tb_stringf(int x, int y, tb_color fg, tb_color bg, const char *fmt, ...) {
  char buf[MAX_LIMIT];
  va_list vl;
  va_start(vl, fmt);
  vsnprintf(buf, sizeof(buf), fmt, vl);
  va_end(vl);
  return tb_string(x, y, fg, bg, buf);
}

struct tb_cell *tb_cell_buffer(void) {
  return back_buffer.cells;
}

int tb_poll_event(struct tb_event *event) {
  return wait_fill_event(event, 0);
}

int tb_peek_event(struct tb_event *event, int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout / 1000;
  tv.tv_usec = (timeout - (tv.tv_sec * 1000)) * 1000;
  return wait_fill_event(event, &tv);
}

int tb_width(void) {
  return termw;
}

int tb_height(void) {
  return termh;
}

void tb_hide_cursor(void) {
  bytebuffer_puts(&output_buffer, funcs[T_HIDE_CURSOR]);
}

void tb_show_cursor(void) {
  bytebuffer_puts(&output_buffer, funcs[T_SHOW_CURSOR]);
}

void tb_enable_mouse(void) {
  bytebuffer_puts(&output_buffer, funcs[T_ENTER_MOUSE]);
  bytebuffer_flush(&output_buffer, inout);
}

void tb_disable_mouse(void) {
  bytebuffer_puts(&output_buffer, funcs[T_EXIT_MOUSE]);
  bytebuffer_flush(&output_buffer, inout);
}

int tb_select_output_mode(int mode) {
  if (mode) output_mode = mode;
  return output_mode;
}

void tb_set_clear_attributes(tb_color fg, tb_color bg) {
  foreground = fg;
  background = bg;
}

void tb_clear_screen(void) {
  set_colors(foreground, background);
  bytebuffer_puts(&output_buffer, funcs[T_CLEAR_SCREEN]);

  if (!IS_CURSOR_HIDDEN(cursor_x, cursor_y))
    write_cursor(cursor_x, cursor_y);

  bytebuffer_flush(&output_buffer, inout);

  /* we need to invalidate cursor position too and these two vars are
   * used only for simple cursor positioning optimization, cursor
   * actually may be in the correct place, but we simply discard
   * optimization once and it gives us simple solution for the case when
   * cursor moved */
  lastx = LAST_COORD_INIT;
  lasty = LAST_COORD_INIT;
}

void tb_clear_buffer(void) {
  if (buffer_size_change_request)
    tb_resize();

  cellbuf_clear(&back_buffer);
}

void tb_resize(void) {
  if (buffer_size_change_request) {
    buffer_size_change_request = 0;
  } else {
    update_term_size();
  }

  cellbuf_resize(&back_buffer, termw, termh);
  cellbuf_resize(&front_buffer, termw, termh);
  cellbuf_clear(&front_buffer);

  tb_clear_screen();
}

/* -------------------------------------------------------- */

static void cellbuf_init(struct cellbuf *buf, int width, int height) {
  buf->cells = (struct tb_cell*)malloc(sizeof(struct tb_cell) * width * height);
  assert(buf->cells);
  buf->width = width;
  buf->height = height;
}

static void cellbuf_resize(struct cellbuf *buf, int width, int height) {
  if (buf->width == width && buf->height == height)
    return;

  int oldw = buf->width;
  int oldh = buf->height;
  struct tb_cell *oldcells = buf->cells;

  cellbuf_init(buf, width, height);
  cellbuf_clear(buf);

  int minw = (width < oldw) ? width : oldw;
  int minh = (height < oldh) ? height : oldh;
  int i;

  for (i = 0; i < minh; ++i) {
    struct tb_cell *csrc = oldcells + (i * oldw);
    struct tb_cell *cdst = buf->cells + (i * width);
    memcpy(cdst, csrc, sizeof(struct tb_cell) * minw);
  }

  free(oldcells);
}

static void cellbuf_clear(struct cellbuf *buf) {
  int i;
  int ncells = buf->width * buf->height;

  for (i = 0; i < ncells; ++i) {
    buf->cells[i].ch = ' ';
    buf->cells[i].fg = foreground;
    buf->cells[i].bg = background;
  }
}

static void cellbuf_free(struct cellbuf *buf) {
  free(buf->cells);
}

static void update_term_size(void) {
  struct winsize sz;
  memset(&sz, 0, sizeof(sz));
  ioctl(inout, TIOCGWINSZ, &sz);

  termw = sz.ws_col;
  termh = sz.ws_row;
}

static uint8_t base_colors[8][3] = {
 { 1, 1, 1 }, // black
 { 1, 0, 0 }, // red
 { 0, 1, 0 }, // green
 { 1, 1, 0 }, // yellow
 { 0, 0, 1 }, // blue
 { 1, 0, 1 }, // magenta
 { 0, 1, 1 }, // cyan
 { 1, 1, 1 }  // white
};

// int levels = { 0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff };
static int steps[6] = { 47, 115, 155, 195, 235, 256 }; // in between of each level

uint8_t get_256_color(uint32_t color) {
  // extract rgb values from number (e.g. 0xffcc00 -> 16763904)
  uint8_t rgb[3] = { 0, 0, 0 }; // r, g, b
  rgb[0] = (color >> 16) & 0xFF; // e.g. 255
  rgb[1] = (color >> 8)  & 0xFF; // e.g. 204
  rgb[2] = (color >> 0)  & 0xFF; // e.g. 0

  // now, translate rgb to their most similar value between the 0-5 range
  int nums[3] = { 0, 0, 0 };
  for (int c = 0; c < 3; c++) {
    for (int i = 0; i < 6; i++) {
      if (steps[i] > rgb[c]) {
        nums[c] = i;
        break;
      }
    }
  }

  // 16 + (r * 36) + (g * 6) + b
  return 16 + (nums[0] * 36) + (nums[1] * 6) + nums[2];
}

uint8_t get_base_color(uint32_t color) {

  // extract rgb values and round them to either 0 or 1
  uint8_t rgb[3] = { 0, 0, 0 }; // r, g, b
  rgb[0] = ((color >> 16) & 0xFF) > 128 ? 1 : 0;
  rgb[1] = ((color >> 8)  & 0xFF) > 128 ? 1 : 0;
  rgb[2] = ((color >> 0)  & 0xFF) > 128 ? 1 : 0;

  uint8_t i;
  for (i = 0; i < 8; i++) {
    if (base_colors[i][0] == rgb[0] && base_colors[i][1] == rgb[1] && base_colors[i][2] == rgb[2]) {
      return i;
    }
  }

  return 0; // default
}

tb_color tb_rgb(uint32_t in) {
#ifdef WITH_TRUECOLOR
  if (output_mode == 2)
    return in;
#endif

  if (output_mode == 1) {
    return get_256_color(in);
  } else {
    return get_base_color(in);
  }
}

static int convertnum(uint8_t num, char* buf) {
  int i, l = 0;
  int ch;
  do {
    buf[l++] = '0' + (num % 10);
    num /= 10;
  } while (num);

  for(i = 0; i < l / 2; i++) {
    ch = buf[i];
    buf[i] = buf[l - 1 - i];
    buf[l - 1 - i] = ch;
  }

  return l;
}

uint8_t map_to_base_color(tb_color col) {
  if (col > 255)
    return TB_BASIC_WHITE; // TB_DEFAULT;
  else if (col > 244) // light grays
    return TB_BASIC_LIGHT_GRAY;
  else if (col > 231) // dark grays
    return TB_BASIC_MEDIUM_GRAY;
  else if (col == 16)
    return TB_BASIC_BLACK;
  else if ((col - 16) % 36 == 0)
    return TB_BASIC_RED;
  else if ((col - 16) % 6 == 0)
    return TB_BASIC_GREEN;
  else if ((col - 16) % 3 == 0) // totally unscientific
    return TB_BASIC_YELLOW;
  else if (col % 3 == 0) // even less
    return TB_BASIC_MAGENTA;
  else
    return TB_BASIC_BLUE;
}

#define WRITE_LITERAL(X) bytebuffer_append(&output_buffer, (X), sizeof(X)-1)
#define WRITE_INT(X) bytebuffer_append(&output_buffer, buf, convertnum((X), buf))

static tb_color lastfg = LAST_ATTR_INIT, lastbg = LAST_ATTR_INIT;

static void set_colors(tb_color fg, tb_color bg) {
  if (fg == lastfg && bg == lastbg)
    return;

  lastfg = fg;
  lastbg = bg;

  bytebuffer_puts(&output_buffer, funcs[T_SGR0]); // reset attrs

  if (fg & TB_BOLD) {
    bytebuffer_puts(&output_buffer, funcs[T_BOLD]);
  }

  //if (bg & TB_BOLD)
  //  bytebuffer_puts(&output_buffer, funcs[T_BLINK]);

  if (fg & TB_UNDERLINE) {
    bytebuffer_puts(&output_buffer, funcs[T_UNDERLINE]);
  }

  if ((fg & TB_REVERSE) || (bg & TB_REVERSE)) {
    bytebuffer_puts(&output_buffer, funcs[T_REVERSE]);
  }

  tb_color fgcol, bgcol;
  bool default_fg, default_bg;

#ifndef WITH_TRUECOLOR
  // remove attributes
  fgcol = fg & 0xFF;
  bgcol = bg & 0xFF;

  default_fg = fgcol == TB_DEFAULT;
  default_bg = bgcol == TB_DEFAULT;

  if (default_fg && default_bg)
    return;
#endif

  char buf[32];
  WRITE_LITERAL("\033[");

#ifdef WITH_TRUECOLOR

  default_fg = fg == TB_DEFAULT;
  default_bg = bg == TB_DEFAULT;

  if (output_mode != 2) {

    // convert rgb value to either 256 or 16 color
    fgcol = tb_rgb(fg);
    bgcol = tb_rgb(bg);

  } else {

    // write RGB color to buffer
    WRITE_LITERAL("38;2;");
    WRITE_INT(fg >> 16 & 0xFF); // fg R
    WRITE_LITERAL(";");
    WRITE_INT(fg >> 8 & 0xFF);  // fg G
    WRITE_LITERAL(";");
    WRITE_INT(fg & 0xFF);       // fg B
    WRITE_LITERAL(";48;2;");
    WRITE_INT(bg >> 16 & 0xFF); // bg R
    WRITE_LITERAL(";");
    WRITE_INT(bg >> 8 & 0xFF);  // bg G
    WRITE_LITERAL(";");
    WRITE_INT(bg & 0xFF);       // bg B

  }

#else // no truecolor support

  if (output_mode == 0) { // 16 colors
    fgcol = fgcol > 16 ? map_to_base_color(fgcol) : fgcol; // & 0x0F;
    bgcol = bgcol > 16 ? map_to_base_color(bgcol) : bgcol; // & 0x0F;
  }

#endif

  // 256 colors
  // num      fg          bg
  // 0-15     [38;5;(N)m  [48;5;(N)m -- 16 ANSI colors
  // 16-231   [38;5;(N)m  [48;5;(N)m -- 6x6x6 RGB
  // 232-255  [38;5;(N)m  [48;5;(N)m -- 24 grayscale

/*
  echo "256"
  echo -e "\e[38;5;3mnormal\e[0mtext"
  echo -e "\e[2;38;5;3mdim\e[0mtext"
  echo -e "\e[38;5;11mbright\e[0mtext"
  echo -e "\e[1;38;5;11mbold\e[0mtext (also bright)"
  echo "iso"
  echo -e "\e[33mnormal\e[0mtext"
  echo -e "\e[2;33mdim\e[0mtext"
  echo -e "\e[1;33mbold\e[0mtext"
*/

  if (output_mode == 1) {

    if (!default_fg) {
      WRITE_LITERAL("38;5;");
      WRITE_INT(fgcol);
      if (!default_bg) WRITE_LITERAL(";");
    }
    if (!default_bg) {
      WRITE_LITERAL("48;5;");
      WRITE_INT(bgcol);
    }

  // 16 color ISO
  // num   fg         bg
  // 0-7   3(N)m      4(N)m
  // 8-15  1;3(N-8)m  1;4(N-8)m

  // in bold
  // 0-7   9(N)m      10(N)m
  // 8-15  1;9(N-8)m  1;9(N-8)m

  } else if (output_mode == 0) {

    if (!default_fg) {
      if (fgcol > 7) { // upper 8
        WRITE_LITERAL("1;3");
        WRITE_INT(fgcol - 8);
      } else {
        WRITE_LITERAL("3");
        WRITE_INT(fgcol);
      }
      if (!default_bg) WRITE_LITERAL(";");
    }

    if (!default_bg) {
      if (bgcol > 7) { // upper 8
        WRITE_LITERAL("10"); // "1;4"
        WRITE_INT(bgcol - 8);
      } else {
        WRITE_LITERAL("4");
        WRITE_INT(bgcol);
      }
    }
  }

  WRITE_LITERAL("m");
}

static void write_cursor(int x, int y) {
  char buf[32];
  WRITE_LITERAL("\033[");
  WRITE_INT(y+1);
  WRITE_LITERAL(";");
  WRITE_INT(x+1);
  WRITE_LITERAL("H");
}

static void write_title(const char * title) {
  tb_sendf("%c]0;%s%c\n", '\033', title, '\007');
}

static void send_char(int x, int y, uint32_t c) {
  char buf[7];
  int bw = tb_utf8_unicode_to_char(buf, c);

  if (x-1 != lastx || y != lasty) {
    write_cursor(x, y);
  }

  lastx = x; lasty = y;
  if (!c) buf[0] = ' '; // replace 0 with whitespace

  bytebuffer_append(&output_buffer, buf, bw);
}

static void sigwinch_handler(int xxx) {
  (void) xxx;
  const int zzz = 1;

  int unused __attribute__((unused));
  unused = write(winch_fds[1], &zzz, sizeof(int));
}

static int cutesc = 0;
#define MAXSEQ 14 // need to make room for urxvt mouse sequences
static char seq[MAXSEQ];

static int decode_char(struct tb_event * event, uint32_t ch) {
  if (ch == 127) {
    event->key = TB_KEY_BACKSPACE2;
  } else if (ch < 32) { // ctrl + a-z or number up to 7
    event->meta = ch == 13 ? 0 : TB_META_CTRL;
    event->key = ch;
    // event->ch  = ch + 97; // we don't want it to be printed
  } else { // a-z -- A-Z -- 0-9
    event->meta = ('A' <= ch && ch <= 'Z') ? TB_META_SHIFT : 0;
    event->ch = ch;
  }
  return 1;
}

static int decode_utf8(struct tb_event * event, char c) {
  int rs, nread = 1;
  // uint8_t len = (c >> 7 == 0) ? 1 : (c >> 5 == 0x6) ? 2 : (c >> 4 == 0xE) ? 3 : (c >> 5 == 0x1E) ? 4 : 0;
  uint8_t len = tb_utf8_char_length(c);
  uint32_t ch;

  seq[0] = c;
  while (nread < len) {
    // if read error, or if didn't read end of sequence, return -1
    rs = read(inout, seq + nread++, 1);
    if (rs < 1)
      return -1;
  }

  seq[nread] = '\0';
  len = tb_utf8_char_to_unicode(&ch, seq);
  decode_char(event, ch);
  // bytebuffer_truncate(&input_buffer, tb_utf8_char_length(seq[0]));

  return len;
}


static int decode_esc(struct tb_event * event) {
  int rs, nread = 1;
  seq[0] = 27;

  while (nread < MAXSEQ) {
    rs = read(inout, seq + nread++, 1);
    if (rs == -1) return -1;
    if (rs == 0) break;

    // handle urxvt alt + keys
    if (seq[nread-1] == 27) { // found another escape char!
      if (seq[nread-2] == 27) { // double esc
        if (read(inout, seq + nread++, 1) == 0) { // end of the road, so it's alt+esc
          event->key  = TB_KEY_ESC;
          event->meta = TB_META_ALT;
          return 1;
        } // if not end of road, then it must be ^[^[[A (urxvt alt+arrows)
      } else {
        cutesc = 1;
        break;
      }
    }
  }

  if (nread == MAXSEQ) return 0;
  seq[nread] = '\0';

  int mouse_parsed = parse_mouse_event(event, seq, nread-1);
  if (mouse_parsed != 0)
    return mouse_parsed;

  return parse_esc_seq(event, seq, nread-1);
}

static int read_and_extract_event(struct tb_event * event) {
  int nread, c = 0;

  if (cutesc) {
    c = 27;
    cutesc = 0;
  } else {
    while ((nread = read(inout, &c, 1)) == 0);
    if (nread == -1) return -1;
  }

  event->type = TB_EVENT_KEY;
  event->meta = 0;
  event->ch   = 0;

  if (c == 27) { // escape
    return decode_esc(event);

  } else if (0 <= c && c <= 127) { // from ctrl-a to z, not esc
    return decode_char(event, c);

  } else { // utf8 sequence
    return decode_utf8(event, c);
  }
}

static int wait_fill_event(struct tb_event *event, struct timeval *timeout) {
  int n;
  fd_set events;
  memset(event, 0, sizeof(struct tb_event));

  if (cutesc) { // there's a part of an escape sequence left!
    n = read_and_extract_event(event);
    if (n < 0) return -1;
    if (n > 0) return event->type;
  }

  while (1) {
    FD_ZERO(&events);
    FD_SET(inout, &events);
    FD_SET(winch_fds[0], &events);
    int maxfd  = (winch_fds[0] > inout) ? winch_fds[0] : inout;
    int result = select(maxfd+1, &events, 0, 0, timeout);
    if (!result) return 0;

    if (FD_ISSET(winch_fds[0], &events)) {
      event->type = TB_EVENT_RESIZE;
      int zzz = 0;
      n = read(winch_fds[0], &zzz, sizeof(int));
      buffer_size_change_request = 1;

      update_term_size();
      event->w = termw;
      event->h = termh;
      return TB_EVENT_RESIZE;
    }

    if (FD_ISSET(inout, &events)) {
      n = read_and_extract_event(event) > 0;
      if (n < 0) return -1;
      if (n > 0) return event->type;
    }
  }
}
