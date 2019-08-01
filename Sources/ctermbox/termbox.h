#pragma once

#include <stdint.h>

/* for shared objects */
#if __GNUC__ >= 4
 #define SO_IMPORT __attribute__((visibility("default")))
#else
 #define SO_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WITH_TRUECOLOR

typedef uint32_t tb_color;
#define LAST_ATTR_INIT  0xFFFFFFFF

// attributes
#define TB_BOLD         0x01000000
#define TB_UNDERLINE    0x04000000
#define TB_REVERSE      0x08000000
// #define TB_ITALIC    0x03000000
// #define TB_STRIKE    0x09000000

// colors
#define TB_DEFAULT       16777216 // 0xFFFFFF + 1

#define TB_RED           0xFF0000
#define TB_GREEN         0x00FF00
#define TB_YELLOW        0xFFFF00
#define TB_BLUE          0x0000FF
#define TB_MAGENTA       0xFF00FF
#define TB_CYAN          0x00FFFF
#define TB_LIGHTER_GRAY  0xC0C0C0
#define TB_MEDIUM_GRAY   0x808080
#define TB_LIGHT_RED     0xff5555
#define TB_LIGHT_GREEN   0x55ff55
#define TB_LIGHT_YELLOW  0xffff55
#define TB_LIGHT_BLUE    0x5555ff
#define TB_LIGHT_MAGENTA 0xff55ff
#define TB_LIGHT_CYAN    0x55ffff
#define TB_WHITE         0xFFFFFF
#define TB_BLACK         0x000000

#define TB_DARKEST_GRAY  0x202020
#define TB_DARKER_GRAY   0x404040
#define TB_DARK_GRAY     0x606060
// medium and lighter are above
#define TB_LIGHT_GRAY    0xa0a0a0
#define TB_LIGHTEST_GRAY 0xe0e0e0

#else // no truecolor

typedef uint16_t tb_color;
#define LAST_ATTR_INIT  0xFFFF

// attributes
#define TB_BOLD         0x0100
#define TB_UNDERLINE    0x0400
#define TB_REVERSE      0x0800
// #define TB_ITALIC    0x0300
// #define TB_STRIKE    0x0900

// colors
#define TB_DEFAULT       0x00
#define TB_RED           0x01
#define TB_GREEN         0x02
#define TB_YELLOW        0x03
#define TB_BLUE          0x04
#define TB_MAGENTA       0x05
#define TB_CYAN          0x06
#define TB_LIGHT_GRAY    0x07
#define TB_MEDIUM_GRAY   0x08
#define TB_LIGHT_RED     0x09
#define TB_LIGHT_GREEN   0x0A
#define TB_LIGHT_YELLOW  0x0B
#define TB_LIGHT_BLUE    0x0C
#define TB_LIGHT_MAGENTA 0x0D
#define TB_LIGHT_CYAN    0x0E
#define TB_WHITE         0x0F
#define TB_BLACK         0x10

#define TB_DARKEST_GRAY   234
#define TB_DARKER_GRAY    236
#define TB_DARK_GRAY      241
// medium and light are above
#define TB_LIGHTER_GRAY   251
#define TB_LIGHTEST_GRAY  254

#endif

#define TB_BASIC_RED           0x01
#define TB_BASIC_GREEN         0x02
#define TB_BASIC_YELLOW        0x03
#define TB_BASIC_BLUE          0x04
#define TB_BASIC_MAGENTA       0x05
#define TB_BASIC_LIGHT_GRAY    0x07
#define TB_BASIC_MEDIUM_GRAY   0x08
#define TB_BASIC_WHITE         0x0F
#define TB_BASIC_BLACK         0x10

#define TB_NONE           TB_DEFAULT
#define TB_DARKEST_GREY   TB_DARKEST_GRAY
#define TB_DARKER_GREY    TB_DARKER_GRAY
#define TB_DARK_GREY      TB_DARK_GRAY
#define TB_MEDIUM_GREY    TB_MEDIUM_GRAY
#define TB_LIGHT_GREY     TB_LIGHT_GRAY
#define TB_LIGHTER_GREY   TB_LIGHTER_GRAY
#define TB_LIGHTEST_GREY  TB_LIGHTEST_GRAY

/* A cell, single conceptual entity on the terminal screen. The terminal screen
 * is basically a 2d array of cells. It has the following fields:
 *  - 'ch' is a unicode character
 *  - 'fg' foreground color and attributes
 *  - 'bg' background color and attributes
 */

typedef uint32_t tb_chr;

struct tb_cell {
	tb_chr ch;
	tb_color fg;
	tb_color bg;
};

#define TB_EVENT_KEY    1
#define TB_EVENT_RESIZE 2
#define TB_EVENT_MOUSE  3

/* An event, single interaction from the user. The 'mod' and 'ch' fields are
 * valid if 'type' is TB_EVENT_KEY. The 'w' and 'h' fields are valid if 'type'
 * is TB_EVENT_RESIZE. The 'x' and 'y' fields are valid if 'type' is
 * TB_EVENT_MOUSE. The 'key' field is valid if 'type' is either TB_EVENT_KEY
 * or TB_EVENT_MOUSE. The fields 'key' and 'ch' are mutually exclusive; only
 * one of them can be non-zero at a time.
 */
struct tb_event {
	uint8_t type;
	// uint8_t mod; /* modifiers to either 'key' or 'ch' below */
	uint16_t key; /* one of the TB_KEY_* constants */
	uint32_t ch; /* unicode character */
	uint8_t meta;
	uint16_t w;
	uint16_t h;
	uint16_t x;
	uint16_t y;
};

/* Error codes returned by tb_init(). All of them are self-explanatory, except
 * the pipe trap error. Termbox uses unix pipes in order to deliver a message
 * from a signal handler (SIGWINCH) to the main event reading loop. Honestly in
 * most cases you should just check the returned code as < 0.
 */
#define TB_EUNSUPPORTED_TERMINAL -1
#define TB_EFAILED_TO_OPEN_TTY   -2
#define TB_EPIPE_TRAP_ERROR      -3

/* Flags passed to tb_init_with() to specify which features should be enabled.
 */
#define TB_INIT_ALL          -1
#define TB_INIT_ALTSCREEN     1
#define TB_INIT_KEYPAD        2
#define TB_INIT_NO_CURSOR     3
#define TB_INIT_DETECT_MODE   4

/* Initializes the termbox library. This function should be called before any
 * other functions. Function tb_init is same as tb_init_file("/dev/tty").
 * After successful initialization, the library must be
 * finalized using the tb_shutdown() function.
 */
SO_IMPORT int tb_init(void);
SO_IMPORT int tb_init_screen(int flags);
SO_IMPORT int tb_init_with(int flags);
SO_IMPORT int tb_init_file(const char* name);
SO_IMPORT int tb_init_fd(int inout);
SO_IMPORT void tb_shutdown(void);

/* Returns the size of the internal back buffer (which is the same as
 * terminal's window size in characters). The internal buffer can be resized
 * after tb_resize() or tb_render() function calls. Both dimensions have an
 * unspecified negative value when called before tb_init() or after
 * tb_shutdown().
 */
SO_IMPORT int tb_width(void);
SO_IMPORT int tb_height(void);

/* Clears the internal back buffer using TB_DEFAULT color or the
 * color/attributes set by tb_set_clear_attributes() function.
 */
SO_IMPORT void tb_clear_buffer(void);
SO_IMPORT void tb_set_clear_attributes(tb_color fg, tb_color bg);

// Clear screen.
SO_IMPORT void tb_clear_screen(void);

/* Syncronize the internal back buffer with the terminal. */
SO_IMPORT void tb_render(void);

SO_IMPORT tb_color tb_rgb(uint32_t in);

/* Sets the position of the cursor. Upper-left character is (0, 0). If you pass
 * TB_HIDE_CURSOR as both coordinates, then the cursor will be hidden. Cursor
 * is hidden by default.
 */

#define TB_HIDE_CURSOR -1
SO_IMPORT void tb_set_cursor(int cx, int cy);

SO_IMPORT void tb_set_title(const char * title);

// Flush output buffer to file descriptor (stdout).
SO_IMPORT void tb_flush(void);

/* Append string directly to output */
SO_IMPORT void tb_send(const char * str);

/* Same as above but with format and arguments (printf-style) */
SO_IMPORT void tb_sendf(const char * fmt, ...);

/* Set string starting at specific position */
SO_IMPORT int tb_string(int x, int y, tb_color fg, tb_color bg, char * str);

SO_IMPORT int tb_string_with_limit(int x, int y, tb_color fg, tb_color bg, char * str, int limit);

/* Same as above but with format and arguments (printf-style) */
SO_IMPORT int tb_stringf(int x, int y, tb_color fg, tb_color bg, const char * fmt, ...);

/* Set char at specific position */
SO_IMPORT void tb_char(int x, int y, tb_color fg, tb_color bg, tb_chr ch);

/* Changes cell's parameters in the internal back buffer at the specified
 * position.
 */
SO_IMPORT void tb_cell(int x, int y, const struct tb_cell *cell);

/* Returns a pointer to internal cell back buffer. You can get its dimensions
 * using tb_width() and tb_height() functions. The pointer stays valid as long
 * as no tb_resize() and tb_render() calls are made. The buffer is
 * one-dimensional buffer containing lines of cells starting from the top.
 */
SO_IMPORT struct tb_cell *tb_cell_buffer(void);

SO_IMPORT void tb_hide_cursor(void);
SO_IMPORT void tb_show_cursor(void);

SO_IMPORT void tb_enable_mouse(void);
SO_IMPORT void tb_disable_mouse(void);

/* Wait for an event up to 'timeout' milliseconds and fill the 'event'
 * structure with it, when the event is available. Returns the type of the
 * event (one of TB_EVENT_* constants) or -1 if there was an error or 0 in case
 * there were no event during 'timeout' period.
 */
SO_IMPORT int tb_peek_event(struct tb_event *event, int timeout);

/* Wait for an event forever and fill the 'event' structure with it, when the
 * event is available. Returns the type of the event (one of TB_EVENT_*
 * constants) or -1 if there was an error.
 */
SO_IMPORT int tb_poll_event(struct tb_event *event);

SO_IMPORT void tb_resize(void);

#define TB_OUTPUT_NORMAL    0
#define TB_OUTPUT_256       1
#ifdef WITH_TRUECOLOR
#define TB_OUTPUT_TRUECOLOR 2
#endif

/* Sets the termbox output mode. Termbox has three output options:
 * 1. TB_OUTPUT_NORMAL     => [1..8]
 *    This mode provides 8 different colors:
 *      black, red, green, yellow, blue, magenta, cyan, white
 *    Shortcut: TB_BLACK, TB_RED, ...
 *    Attributes: TB_BOLD, TB_UNDERLINE, TB_REVERSE
 *
 *    Example usage:
 *        tb_change_cell(x, y, '@', TB_BLACK | TB_BOLD, TB_RED);
 *
 * 2. TB_OUTPUT_256        => [0..256]
 *    In this mode you can leverage the 256 terminal mode:
 *    0x00 - 0x07: the 8 colors as in TB_OUTPUT_NORMAL
 *    0x08 - 0x0f: TB_* | TB_BOLD
 *    0x10 - 0xe7: 216 different colors
 *    0xe8 - 0xff: 24 different shades of grey
 *
 *    Example usage:
 *        tb_change_cell(x, y, '@', 184, 240);
 *        tb_change_cell(x, y, '@', 0xb8, 0xf0);
 *
 * 3. TB_OUTPUT_TRUECOLOR  => [0x000000..0xFFFFFF]
 *    This mode supports 24-bit true color. Format is 0xRRGGBB.
 *
 * Execute build/src/demo/output to see its impact on your terminal.
 *
 * If 'mode' is TB_OUTPUT_CURRENT, it returns the current output mode.
 *
 * Default termbox output mode is TB_OUTPUT_NORMAL.
 */
SO_IMPORT int tb_select_output_mode(int mode);

/* Utility utf8 functions. */
#define TB_EOF -1
SO_IMPORT int tb_utf8_char_length(char c);
SO_IMPORT int tb_utf8_char_to_unicode(uint32_t *out, const char *c);
SO_IMPORT int tb_utf8_unicode_to_char(char *out, uint32_t c);
SO_IMPORT int tb_unicode_is_char_wide(uint32_t c);

/* Key constants. See also struct tb_event's key field.
 *
 * These are a safe subset of terminfo keys, which exist on all popular
 * terminals. Termbox uses only them to stay truly portable.
 */
#define TB_KEY_F1               (0xFFFF-0)
#define TB_KEY_F2               (0xFFFF-1)
#define TB_KEY_F3               (0xFFFF-2)
#define TB_KEY_F4               (0xFFFF-3)
#define TB_KEY_F5               (0xFFFF-4)
#define TB_KEY_F6               (0xFFFF-5)
#define TB_KEY_F7               (0xFFFF-6)
#define TB_KEY_F8               (0xFFFF-7)
#define TB_KEY_F9               (0xFFFF-8)
#define TB_KEY_F10              (0xFFFF-9)
#define TB_KEY_F11              (0xFFFF-10)
#define TB_KEY_F12              (0xFFFF-11)
#define TB_KEY_INSERT           (0xFFFF-12)
#define TB_KEY_DELETE           (0xFFFF-13)
#define TB_KEY_HOME             (0xFFFF-14)
#define TB_KEY_END              (0xFFFF-15)
#define TB_KEY_PGUP             (0xFFFF-16)
#define TB_KEY_PGDN             (0xFFFF-17)

// symlinks :)
#define TB_KEY_PAGE_UP          TB_KEY_PGUP
#define TB_KEY_PAGE_DOWN        TB_KEY_PGDN

#define TB_KEY_ARROW_LEFT       (0xFFFF-18)
#define TB_KEY_ARROW_RIGHT      (0xFFFF-19)
#define TB_KEY_ARROW_DOWN       (0xFFFF-20)
#define TB_KEY_ARROW_UP         (0xFFFF-21)

#define TB_KEY_MOUSE_LEFT       (0xFFFF-22)
#define TB_KEY_MOUSE_RIGHT      (0xFFFF-23)
#define TB_KEY_MOUSE_MIDDLE     (0xFFFF-24)
#define TB_KEY_MOUSE_RELEASE    (0xFFFF-25)
#define TB_KEY_MOUSE_WHEEL_UP   (0xFFFF-26)
#define TB_KEY_MOUSE_WHEEL_DOWN (0xFFFF-27)

// #define TB_KEY_ARROW_UP         (0xFFFF-65)
// #define TB_KEY_ARROW_DOWN       (0xFFFF-66)
// #define TB_KEY_ARROW_RIGHT      (0xFFFF-67)
// #define TB_KEY_ARROW_LEFT       (0xFFFF-68)

/* These are all ASCII code points below SPACE character and a BACKSPACE key. */
#define TB_KEY_CTRL_TILDE       0x00
#define TB_KEY_CTRL_2           0x00 /* clash with 'CTRL_TILDE' */
#define TB_KEY_CTRL_A           0x01
#define TB_KEY_CTRL_B           0x02
#define TB_KEY_CTRL_C           0x03
#define TB_KEY_CTRL_D           0x04
#define TB_KEY_CTRL_E           0x05
#define TB_KEY_CTRL_F           0x06
#define TB_KEY_CTRL_G           0x07
#define TB_KEY_BACKSPACE        0x08
#define TB_KEY_CTRL_H           0x08 /* clash with 'CTRL_BACKSPACE' */
#define TB_KEY_TAB              0x09
#define TB_KEY_CTRL_I           0x09 /* clash with 'TAB' */
#define TB_KEY_CTRL_J           0x0A
#define TB_KEY_CTRL_K           0x0B
#define TB_KEY_CTRL_L           0x0C
#define TB_KEY_ENTER            0x0D
#define TB_KEY_CTRL_M           0x0D /* clash with 'ENTER' */
#define TB_KEY_CTRL_N           0x0E
#define TB_KEY_CTRL_O           0x0F
#define TB_KEY_CTRL_P           0x10
#define TB_KEY_CTRL_Q           0x11
#define TB_KEY_CTRL_R           0x12
#define TB_KEY_CTRL_S           0x13
#define TB_KEY_CTRL_T           0x14
#define TB_KEY_CTRL_U           0x15
#define TB_KEY_CTRL_V           0x16
#define TB_KEY_CTRL_W           0x17
#define TB_KEY_CTRL_X           0x18
#define TB_KEY_CTRL_Y           0x19
#define TB_KEY_CTRL_Z           0x1A
#define TB_KEY_ESC              0x1B
#define TB_KEY_CTRL_LSQ_BRACKET 0x1B /* clash with 'ESC' */
#define TB_KEY_CTRL_3           0x1B /* clash with 'ESC' */
#define TB_KEY_CTRL_4           0x1C
#define TB_KEY_CTRL_BACKSLASH   0x1C /* clash with 'CTRL_4' */
#define TB_KEY_CTRL_5           0x1D
#define TB_KEY_CTRL_RSQ_BRACKET 0x1D /* clash with 'CTRL_5' */
#define TB_KEY_CTRL_6           0x1E
#define TB_KEY_CTRL_7           0x1F
#define TB_KEY_CTRL_SLASH       0x1F /* clash with 'CTRL_7' */
#define TB_KEY_CTRL_UNDERSCORE  0x1F /* clash with 'CTRL_7' */
#define TB_KEY_SPACE            0x20
#define TB_KEY_BACKSPACE2       0x7F
#define TB_KEY_CTRL_8           0x7F /* clash with 'BACKSPACE2' */

enum {
	TB_META_SHIFT = 2,
	TB_META_ALT,
	TB_META_ALTSHIFT,
	TB_META_CTRL,
	TB_META_CTRLSHIFT,
	TB_META_ALTCTRL,
	TB_META_ALTCTRLSHIFT,
	TB_META_MOTION  // for mouse events
};

/* These are non-existing ones.
 *
 * #define TB_KEY_CTRL_1 clash with '1'
 * #define TB_KEY_CTRL_9 clash with '9'
 * #define TB_KEY_CTRL_0 clash with '0'
 */

#ifdef __cplusplus
}
#endif
