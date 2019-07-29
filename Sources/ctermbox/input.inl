// if s1 starts with s2 returns true, else false
// len is the length of s1
// s2 should be null-terminated
static bool starts_with(const char *s1, int len, const char *s2) {
  int n = 0;
  while (*s2 && n < len) {
    if (*s1++ != *s2++)
      return false;
    n++;
  }
  return *s2 == 0;
}


#ifdef __linux__

#include <time.h>

struct click {
  int type;
  int x;
  int y;
  struct timespec ts;
};

static void get_time(struct timespec * ts) {
  clock_gettime(CLOCK_MONOTONIC, ts);
}

static double get_timediff(struct timespec start) {
  struct timespec end = { 0, 0 };
  get_time(&end);

  return ((double)end.tv_sec + 1.0e-9 * end.tv_nsec) \
    - ((double)start.tv_sec + 1.0e-9 * start.tv_nsec);
}

#else

#include <time.h>

struct click {
  int type;
  int x;
  int y;
  struct timeval ts;
};

static void get_time(struct timeval * ts) {
  gettimeofday(ts, NULL);
}

static double get_timediff(struct timeval start) {
  struct timeval end = { 0, 0 };
  get_time(&end);

  return ((double)end.tv_sec + 1e-6 * end.tv_usec) \
    - ((double)start.tv_sec + 1e-6 * start.tv_usec);
}

#endif

static int click_count = 1;
static struct click last_click = { -1, -1, -1, { 0, 0 } };
static double time_diff;

#define DOUBLE_CLICK_TIME 0.4

static bool is_double_click(int type, int x, int y) {
  int res = false;

  // if we have a recorded last click,
  // and it matches the current's position and type (left/middle/right)
  if (last_click.y != -1 && y == last_click.y && type == last_click.type) {

    // then get the current time and its difference against the last one
    time_diff = get_timediff(last_click.ts);

    // and toggle the flag if it took less than 0.4 secs
    res = time_diff < DOUBLE_CLICK_TIME;
  }

  // store click for next check
  last_click.x = x;
  last_click.y = y;
  last_click.type = type;
  get_time(&last_click.ts);

  return res;
}

static int parse_mouse_event(struct tb_event *event, const char *buf, int len) {

  if (len >= 6 && starts_with(buf, len, "\033[M")) {
    // X10 mouse encoding, the simplest one
    // \033 [ M Cb Cx Cy
    int b = buf[3] - 32;

    switch (b & 3) {
      case 0:
        if ((b & 64) != 0)
          event->key = TB_KEY_MOUSE_WHEEL_UP;
        else
          event->key = TB_KEY_MOUSE_LEFT;
        break;
      case 1:
        if ((b & 64) != 0)
          event->key = TB_KEY_MOUSE_WHEEL_DOWN;
        else
          event->key = TB_KEY_MOUSE_MIDDLE;
        break;
      case 2:
        event->key = TB_KEY_MOUSE_RIGHT;
        break;
      case 3:
        event->key = TB_KEY_MOUSE_RELEASE;
        break;
      default:
        return -6;
    }

    event->type = TB_EVENT_MOUSE; // TB_EVENT_KEY by default
    if ((b & 32) != 0) event->meta |= TB_META_MOTION;

    // the coord is 1,1 for upper left
    event->x = (uint8_t)buf[4] - 1 - 32;
    event->y = (uint8_t)buf[5] - 1 - 32;

    if (event->key > TB_KEY_MOUSE_RELEASE && !(event->meta & TB_META_MOTION)) { // click
      if (is_double_click(event->key, event->x, event->y)) {
        event->h = ++click_count;
      } else {
        event->h = click_count = 1; // not double click. reset count
      }
    }

    return 6;

  } else if (starts_with(buf, len, "\033[<") || starts_with(buf, len, "\033[")) {

    // xterm 1006 extended mode or urxvt 1015 extended mode
    // xterm: \033 [ < Cb ; Cx ; Cy (M or m)
    // urxvt: \033 [ Cb ; Cx ; Cy M
    int i, mi = -1, starti = -1;
    int isM, isU, s1 = -1, s2 = -1;
    int n1 = 0, n2 = 0, n3 = 0;

    for (i = 0; i < len; i++) {

      // We search the first (s1) and the last (s2) ';'
      if (buf[i] == ';') {
        if (s1 == -1) s1 = i;
        s2 = i;
      }

      // We search for the first 'm' or 'M'
      if ((buf[i] == 'm' || buf[i] == 'M') && mi == -1) {
        mi = i;
        break;
      }
    }

    if (mi == -1)
      return 0;

    // whether it's a capital M or not
    isM = (buf[mi] == 'M');

    if (buf[2] == '<') {
      isU = 0;
      starti = 3;
    } else {
      isU = 1;
      starti = 2;
    }

    if (s1 == -1 || s2 == -1 || s1 == s2)
      return 0;

    n1 = strtoul(&buf[starti], NULL, 10);
    n2 = strtoul(&buf[s1 + 1], NULL, 10);
    n3 = strtoul(&buf[s2 + 1], NULL, 10);

    if (isU)
      n1 -= 32;

    switch (n1 & 3) {
      case 0:
        if ((n1&64) != 0) {
          event->key = TB_KEY_MOUSE_WHEEL_UP;
        } else {
          event->key = TB_KEY_MOUSE_LEFT;
        }
        break;
      case 1:
        if ((n1&64) != 0) {
          event->key = TB_KEY_MOUSE_WHEEL_DOWN;
        } else {
          event->key = TB_KEY_MOUSE_MIDDLE;
        }
        break;
      case 2:
        event->key = TB_KEY_MOUSE_RIGHT;
        break;
      case 3:
        event->key = TB_KEY_MOUSE_RELEASE;
        break;
      default:
        return mi + 1;
    }

    if (!isM) // on xterm mouse release is signaled by lowercase m
      event->key = TB_KEY_MOUSE_RELEASE;

    event->type = TB_EVENT_MOUSE;
    if ((n1 & 32) != 0) event->meta |= TB_META_MOTION;

    event->x = (uint8_t)n2 - 1;
    event->y = (uint8_t)n3 - 1;

    if (event->key > TB_KEY_MOUSE_RELEASE && !(event->meta & TB_META_MOTION)) { // click
      if (is_double_click(event->key, event->x, event->y)) {
        event->h = ++click_count;
      } else {
        event->h = click_count = 1; // not double click. reset count
      }
    }

    return mi + 1;
  }

  return 0;
}

static int parse_bracket_esc(struct tb_event *event, const char *seq, int len) {
  int last = seq[len-1];
  int res  = 0;

  if (len == 3) {

    if (last == 'Z') { // shift + tab
      event->meta = TB_META_SHIFT;
      event->key  = TB_KEY_TAB;

// not needed.
//    } else if ('A' <= last && last <= 'H') { // arrow keys linux and xterm
//      event->key = 0xFFFF + (last - 86);

    } else if ('a' <= last && last <= 'd') { // mrxvt shift + left/right or ctrl+shift + up/down
      // TODO: handle ctrl + shift + arrow in mrxvt
      event->meta = TB_META_SHIFT;
      event->key  = 0xFFFF + (last - 118);
    } else {
      res = -1;
    }

//  not needed.
//  } else if (len == 4 && ('A' <= last && last <= 'Z')) { // F1-F5 xterm
//      event->key = last - 54;

  } else if (len > 5) { // xterm shift or control + f1/keys/arrows

    if (last == '~') {

      if (seq[3] == ';') { // xfce shift/ctrl + keys (delete, pageup/down)

        int offset = seq[2] < 53 ? 38 : seq[2] > 54 ? 41 : 37;
        event->key = 0xFFFF - (seq[2] - offset);
        event->meta = seq[4] - 48;

      } else {

        return -1;
/*
        // TODO: check this
        event->key = (seq[4] == ';') ? (int)seq[3] : ((int)seq[2] * 10 + (int)seq[3]);

        // num may be two digit number, so meta key can be at index 4 or 5
        event->meta = '2' <= seq[5] && seq[5] <= '8' ? (int)seq[4]
                    : '2' <= seq[6] && seq[6] <= '8' ? (int)seq[5] : -1;
*/
      }

    } else if ('A' <= last && last <= 'Z') {

      event->meta = seq[4] - 48;

      if (last >= 80) { // f1-f4 xterm

        // event->key = last - 69; // TODO: verify
        event->key = 0xFFFF + (last - 86);
      } else {  // ctrl + arrows urxvt or shift+home/end in xfce4-term
        int offset = last == 70 ? 85 : 86; // handle shift+end offset in xfce4
        event->key = 0xFFFF + (last - offset);
      }

    } else {
      res = -1;
    }

  } else if (last == '^' || last == '@' || last == '$') { // 4 or 5 in length

    if ('1' <= seq[2] && seq[2] <= '8') { // ctrl/shift + keys urxvt

        // 50 ins      // -12
        // 51 del      // -13
        // 53 pageup   // -16
        // 54 pagedown // -17
        // 55 home     // -14
        // 56 end      // -15

        int offset = seq[2] < 53 ? 38 : seq[2] > 54 ? 41 : 37;
        event->key = 0xFFFF - (seq[2] - offset);
        event->meta = last == '@' ? 6 : last >> 4;

    } else {

      int num = len == 5 ? ((int)seq[3] * 10 + (int)seq[4]) : (int)seq[3];

      if (num >= 25) { // ctrl + shift f1-f12 urxvt
        int offset = num == 25 || num == 26 || num == 29 ? 12 : 13;
        event->key = num - offset; // TODO
        event->meta = TB_META_CTRLSHIFT;
      } else {
        event->meta = last == '@' ? 6 : last >> 4;
        event->key  = num; // ALSO TODO
      }

    }

  } else if (last == '~') { // 4 or 5 in length

    if (len == 5) { // shift + f1-f8, linux/urxvt

      int num = (seq[2]-48) * 10 + seq[3]-48; // f9 is 33, and should be 8
      int offset = 25;

      // TODO: mega fix this.

/*
      if (term == 'linux') {
        offset = 15;

        if (num == 25 || num == 26) // offset disparity
          offset -= 1;
        else if (num == 31) // F5
          offset += 1;
      } else {
        offset = 13;

        if (num == 25 || num == 26 || num == 29) // offset disparity
          offset -= 1;
      }
*/
      event->meta = TB_META_SHIFT;
      event->key  = 0xFFFF - (num - offset);

/*
    // TODO: is this actually sent?
    } else if (seq[3] == 3) { // mrxvt shift + insert

      event->meta = TB_META_SHIFT;
      event->key  = TB_KEY_INSERT;
*/

    } else {
      // not needed.
      // event->key = len == 5 ? ((int)seq[3] * 10 + (int)seq[4]) : (int)seq[3];
      res = -1;

    }

  } else {
    res = -1;
  }

  return res;
}

static int parse_esc_seq(struct tb_event *event, const char *seq, int len) {

  if (len == 1) {
    event->key  = TB_KEY_ESC;
    return 1;

  } else if (len == 2) { // alt+char or alt+shift+char, ctrl+alt+char or alt + enter

    event->meta = seq[1] < 27 ? TB_META_ALTCTRL : seq[1] >= 'A' && seq[1] <= 'Z' ? TB_META_ALTSHIFT : TB_META_ALT;

#ifdef __APPLE__
    if (seq[1] == 'b' || seq[1] == 'f') { // alt+left/right
      event->key = seq[1] == 'b' ? TB_KEY_ARROW_LEFT : TB_KEY_ARROW_RIGHT;
      return 1;
    }
#endif

    switch(seq[1]) {
      case 10:
        event->key = TB_KEY_ENTER; break;
      case 127:
        event->key = TB_KEY_BACKSPACE; break;
      default:
            if (seq[1] < 27) { // ctrl+alt+char
              event->ch  = seq[1] + 96;
              event->key = seq[1];
            } else {
            event->ch  = seq[1]; break;
            }
    }

    return 1;
  }

  int i;
  for (i = TB_KEYS_NUM-1; i >= 0; i--) {
    if (starts_with(seq, len, keys[i])) {
      event->ch = 0;
      event->key = 0xFFFF-i;
      return 1; // strlen(keys[i]);
    }
  }

  int last, num;
  switch(seq[1]) {
    case '[':
      if (parse_bracket_esc(event, seq, len) == 0)
        return 1;
      break;

    case 'O':
      if (seq[2] > 96 && seq[2] < 101) { // ctrl + arrows mrxvt/urxvt
        event->meta = TB_META_CTRL;
        event->key  = 0xFFFF + (seq[2] - 118);

      } else if (seq[2] == 49) { // xfce4 arrow keys
        event->key  = 0xFFFF + (seq[len-1] - 80);
        event->meta = TB_META_CTRLSHIFT;

// not needed. parsed previously.
//      } else if (65 <= seq[2] && seq[2] <= 68) { // arrows xterm/mrxvt
//        event->key  = 0xFFFF + (seq[2] - 86);
//        event->meta = 0;

      } else { /* unknown */ }

      return 1;
      break;

    case 27: // double esc, urxvt territory

      last = seq[len-1];
      // TODO: check what happens when issuing CTRL+PAGEDOWN and then ALT+PAGEUP repeatedly

      switch(last) {
        case 'R': // mrxvt alt + f3
          event->meta = TB_META_ALT;
          event->key  = 0xFFFF - 2;
          break;

        case '~': // urxvt alt + key or f1-f12
          event->meta = TB_META_ALT;

          if (len == 6) { // f1-f12
            num = (seq[3]-48) * 10 + seq[4]-48; // f9 is 20
            event->key = 0xFFFF - (num - 12);
          } else { // ins/del/etc
            num = (int)seq[3];
            int offset = num < 53 ? 38 : num > 54 ? 41 : 37;
            event->key = 0xFFFF - (num - offset);
          }

          break;

        case '^':
        case '@': // ctrl + alt + arrows
          event->meta = last == '^' ? TB_META_ALTCTRL : TB_META_ALTCTRLSHIFT;
          event->key  = seq[5];
          break;

        default:

          if ('a' <= last && last <= 'z') { // urxvt ctr/alt arrow or ctr/shift/alt arrow
            event->meta = seq[2] == 91 ? TB_META_ALTSHIFT : seq[4] == 'O' ? TB_META_ALTCTRL : TB_META_ALTCTRLSHIFT;
            event->key  = 0xFFFF + (last - 118);

          } else if ('A' <= last && last <= 'Z') { // urxvt alt + arrow keys
            event->meta = TB_META_ALT;
            event->key  = 0xFFFF + (last - 86);
          } else {
            return -1;
          }

          break;
        }

      break; // case 27

    default:
      printf("Unknown: %d\n", seq[1]);
          return -1;
  }

  return 1;
}

