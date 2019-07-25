import ctermbox

public enum Key: Equatable {
    case ctrl2
    case ctrlA
    case ctrlB
    case ctrlC
    case ctrlD
    case ctrlE
    case ctrlF
    case ctrlG
    case backspace
    case tab
    case ctrlJ
    case ctrlK
    case ctrlL
    case enter
    case ctrlN
    case ctrlO
    case ctrlP
    case ctrlQ
    case ctrlR
    case ctrlS
    case ctrlT
    case ctrlU
    case ctrlV
    case ctrlW
    case ctrlX
    case ctrlY
    case ctrlZ
    case esc
    case ctrlBackslash
    case ctrlRightBracket
    case ctrl6
    case ctrlSlash
    case space

    // Key constants. See also struct tb_event's Key field.
    //
    // These are a safe subset of terminfo Keys, which exist on all popular
    // terminals. Termbox uses only them to stay truly portable.
    case f1
    case f2
    case f3
    case f4
    case f5
    case f6
    case f7
    case f8
    case f9
    case f10
    case f11
    case f12
    case insert
    case delete
    case home
    case end
    case pageUp
    case pageDown
    case arrowUp
    case arrowDown
    case arrowLeft
    case arrowRight
    case mouseLeft
    case mouseRight
    case mouseMiddle
    case mouseRelease
    case mouseWheelUp
    case mouseWheelDown
    // all others
    case rawValue(UInt16)

    var rawValue: UInt16 {
        switch self {
        case .ctrl2:           return 0x00
        case .ctrlA:           return 0x01
        case .ctrlB:           return 0x02
        case .ctrlC:           return 0x03
        case .ctrlD:          return 0x04
        case .ctrlE:           return 0x05
        case .ctrlF:           return 0x06
        case .ctrlG:           return 0x07
        case .tab:             return 0x09
        case .ctrlJ:           return 0x0a
        case .ctrlK:           return 0x0b
        case .ctrlL:           return 0x0c
        case .enter:           return 0x0d
        case .ctrlN:           return 0x0e
        case .ctrlO:           return 0x0f
        case .ctrlP:           return 0x10
        case .ctrlQ:           return 0x11
        case .ctrlR:           return 0x12
        case .ctrlS:           return 0x13
        case .ctrlT:           return 0x14
        case .ctrlU:           return 0x15
        case .ctrlV:           return 0x16
        case .ctrlW:           return 0x17
        case .ctrlX:           return 0x18
        case .ctrlY:           return 0x19
        case .ctrlZ:           return 0x1a
        case .esc:             return 0x1b
        case .ctrlBackslash:   return 0x1c
        case .ctrlRightBracket:return 0x1d
        case .ctrl6:           return 0x1e
        case .ctrlSlash:       return 0x1f
        case .space:           return 0x20
        case .backspace:       return 0x7f

        case .f1:              return 0xffff-0
        case .f2:              return 0xffff-1
        case .f3:              return 0xffff-2
        case .f4:              return 0xffff-3
        case .f5:              return 0xffff-4
        case .f6:              return 0xffff-5
        case .f7:              return 0xffff-6
        case .f8:              return 0xffff-7
        case .f9:              return 0xffff-8
        case .f10:             return 0xffff-9
        case .f11:             return 0xffff-10
        case .f12:             return 0xffff-11
        case .insert:          return 0xffff-12
        case .delete:          return 0xffff-13
        case .home:            return 0xffff-14
        case .end:             return 0xffff-15
        case .pageUp:          return 0xffff-16
        case .pageDown:        return 0xffff-17
        case .arrowUp:         return 0xffff-18
        case .arrowDown:       return 0xffff-19
        case .arrowLeft:       return 0xffff-20
        case .arrowRight:      return 0xffff-21
        case .mouseLeft:       return 0xffff-22
        case .mouseRight:      return 0xffff-23
        case .mouseMiddle:     return 0xffff-24
        case .mouseRelease:    return 0xffff-25
        case .mouseWheelUp:    return 0xffff-26
        case .mouseWheelDown:  return 0xffff-27

        case let .rawValue(rawValue):
            return rawValue
        }
    }

    // These are all ASCII code points below SPACE character and a BACKSPACE
    // key.
    public static let ctrlTilde: Key        = .ctrl2
    public static let ctrlH: Key            = .backspace
    public static let ctrlI: Key            = .tab
    public static let ctrlM: Key            = .enter
    public static let ctrl3: Key            = .esc
    public static let ctrlLeftBracket: Key  = .esc
    public static let ctrlDash: Key         = .ctrlSlash

    public init(rawValue: UInt16) {
        switch rawValue {
        case 0x00: self = .ctrl2
        case 0x01: self = .ctrlA
        case 0x02: self = .ctrlB
        case 0x03: self = .ctrlC
        case 0x04: self = .ctrlD
        case 0x05: self = .ctrlE
        case 0x06: self = .ctrlF
        case 0x07: self = .ctrlG
        case 0x09: self = .tab
        case 0x0a: self = .ctrlJ
        case 0x0b: self = .ctrlK
        case 0x0c: self = .ctrlL
        case 0x0d: self = .enter
        case 0x0e: self = .ctrlN
        case 0x0f: self = .ctrlO
        case 0x10: self = .ctrlP
        case 0x11: self = .ctrlQ
        case 0x12: self = .ctrlR
        case 0x13: self = .ctrlS
        case 0x14: self = .ctrlT
        case 0x15: self = .ctrlU
        case 0x16: self = .ctrlV
        case 0x17: self = .ctrlW
        case 0x18: self = .ctrlX
        case 0x19: self = .ctrlY
        case 0x1a: self = .ctrlZ
        case 0x1b: self = .esc
        case 0x1c: self = .ctrlBackslash
        case 0x1d: self = .ctrlRightBracket
        case 0x1e: self = .ctrl6
        case 0x1f: self = .ctrlSlash
        case 0x20: self = .space
        case 0x08: self = .backspace
        case 0x7f: self = .backspace
        case 0xffff-0: self = .f1
        case 0xffff-1: self = .f2
        case 0xffff-2: self = .f3
        case 0xffff-3: self = .f4
        case 0xffff-4: self = .f5
        case 0xffff-5: self = .f6
        case 0xffff-6: self = .f7
        case 0xffff-7: self = .f8
        case 0xffff-8: self = .f9
        case 0xffff-9: self = .f10
        case 0xffff-10: self = .f11
        case 0xffff-11: self = .f12
        case 0xffff-12: self = .insert
        case 0xffff-13: self = .delete
        case 0xffff-14: self = .home
        case 0xffff-15: self = .end
        case 0xffff-16: self = .pageUp
        case 0xffff-17: self = .pageDown
        case 0xffff-18: self = .arrowUp
        case 0xffff-19: self = .arrowDown
        case 0xffff-20: self = .arrowLeft
        case 0xffff-21: self = .arrowRight
        case 0xffff-22: self = .mouseLeft
        case 0xffff-23: self = .mouseRight
        case 0xffff-24: self = .mouseMiddle
        case 0xffff-25: self = .mouseRelease
        case 0xffff-26: self = .mouseWheelUp
        case 0xffff-27: self = .mouseWheelDown
        default:
            self = .rawValue(rawValue)
        }
    }

    public static func == (lhs: Key, rhs: Key) -> Bool {
        return lhs.rawValue == rhs.rawValue
    }
}


/// Attributes, it is possible to use multiple attributes by combining them
/// using bitwise OR ('|'). Although, colors cannot be combined. But you can
/// combine attributes and a single color. See also `Cell`'s `foreground` and
/// `background` fields.
public struct Attributes: OptionSet {
    public let rawValue: UInt16

    public init(rawValue: UInt16) {
        self.rawValue = rawValue
    }

    // colors
    public static let `default` = Attributes(rawValue: 0x00)
    public static let black     = Attributes(rawValue: 0x01)
    public static let red       = Attributes(rawValue: 0x02)
    public static let green     = Attributes(rawValue: 0x03)
    public static let yellow    = Attributes(rawValue: 0x04)
    public static let blue      = Attributes(rawValue: 0x05)
    public static let magenta   = Attributes(rawValue: 0x06)
    public static let cyan      = Attributes(rawValue: 0x07)
    public static let white     = Attributes(rawValue: 0x08)

    // other
    public static let bold      = Attributes(rawValue: 0x0100)
    public static let underline = Attributes(rawValue: 0x0200)
    public static let reverse   = Attributes(rawValue: 0x0400)
}

/// Typealias of `tb_cell` with Swift version of the attributes.
///
/// A cell, single conceptual entity on the terminal screen. The terminal screen
/// is basically a 2d array of cells. It has the following fields:
///  - 'ch' is a unicode character
///  - 'fg' foreground color and attributes
///  - 'bg' background color and attributes
public typealias Cell = tb_cell
public extension Cell {
    /// Creates a cell with a character, foreground and background.
    init(character: UnicodeScalar, foreground: Attributes = .default,
        background: Attributes = .default)
    {
        self.init(ch: character.value, fg: foreground.rawValue, bg: background.rawValue)
    }

    var character: UnicodeScalar {
        get {
            return UnicodeScalar(self.ch)!
        }
        set {
            self.ch = newValue.value
        }
    }

    var foreground: Attributes {
        get {
            return Attributes(rawValue: self.fg)
        }
        set {
            self.fg = newValue.rawValue
        }
    }

    var background: Attributes {
        get {
            return Attributes(rawValue: self.bg)
        }
        set {
            self.bg = newValue.rawValue
        }
    }
}

public enum Modifier: UInt8 {
    case alt         = 0x01
    case mouseMotion = 0x02
}

/// User interaction event.
public enum Event {

    case key(modifier: Modifier?, value: Key)
    case character(modifier: Modifier?, value: UnicodeScalar)
    case resize(width: Int32, height: Int32)
    case mouse(x: Int32, y: Int32)
    case other(UInt8)
    case timeout

    init(_ tbEvent: tb_event) {
        switch tbEvent.type {
        case 1:
            if tbEvent.ch == 0 {
                self = .key(modifier: Modifier(rawValue: tbEvent.mod),
                            value: Key(rawValue: tbEvent.key))
            } else {
                self = .character(modifier: Modifier(rawValue: tbEvent.mod),
                                  value: UnicodeScalar(tbEvent.ch)!)
            }
        case 2:
            self = .resize(width: tbEvent.w, height: tbEvent.h)
        case 3:
            self = .mouse(x: tbEvent.x, y: tbEvent.y)
        default:
            self = .other(tbEvent.type)
        }
    }
}

/// Errors from initialization. All of them are self-explanatory, except
/// the pipe trap error. Termbox uses unix pipes in order to deliver a message
/// from a signal handler (SIGWINCH) to the main event reading loop.
public enum InitializationError: Error {
    case unsupportedTerminal
    case failedToOpenTTY
    case pipeTrapError

    init?(_ code: Int32) {
        switch code {
        case TB_EUNSUPPORTED_TERMINAL:
            self = .unsupportedTerminal
        case TB_EFAILED_TO_OPEN_TTY:
            self = .failedToOpenTTY
        case TB_EPIPE_TRAP_ERROR:
            self = .pipeTrapError
        default:
            return nil
        }
    }
}

public struct InputModes: OptionSet {
    public let rawValue: Int32

    public init(rawValue: Int32) { self.rawValue = rawValue }

    public static let esc     = InputModes(rawValue: TB_INPUT_ESC)
    public static let alt     = InputModes(rawValue: TB_INPUT_ALT)
    public static let mouse   = InputModes(rawValue: TB_INPUT_MOUSE)
}

public enum OutputMode: Int32 {
    case normal    = 1
    case color256  = 2
    case color216  = 3
    case grayscale = 4
}

public struct Termbox {
    /// Initializes the termbox library. This function should be called before
    /// any other functions. Function initialize is same as
    /// `initialize(file: "/dev/tty")`.  After successful initialization, the
    /// library must be finalized using the `shutdown()` function.
    public static func initialize() throws {
        if let error = InitializationError(tb_init()) {
            throw error
        }
    }

    public static func initialize(file: String) throws {
        if let error = InitializationError(tb_init_file(file)) {
            throw error
        }
    }

    public static func initialize(fileDescriptor: Int32) throws {
        if let error = InitializationError(tb_init_fd(fileDescriptor)) {
            throw error
        }
    }

    public static func shutdown() {
        tb_shutdown()
    }

    /// Returns the size of the internal back buffer (which is the same as
    /// terminal's window size in characters). The internal buffer can be
    /// resized after `clear()` or `present()` function calls. Both
    /// dimensions have an unspecified negative value when called before
    /// initialization or after shutdown.
    public static var width: Int32 {
        return Int32(tb_width())
    }

    public static var height: Int32 {
        return Int32(tb_height())
    }

    /// Clears the internal back buffer using default color.
    public static func clear() {
        tb_clear()
    }

    /// Clears the internal back buffer using specified color/attributes.
    public static func clear(withForeground foreground: Attributes,
        background: Attributes)
    {
        tb_set_clear_attributes(foreground.rawValue, background.rawValue)
    }

    /// Synchronizes the internal back buffer with the terminal.
    public static func present() {
        tb_present()
    }

    /// Sets the position of the cursor. Upper-left character is (0, 0). If you
    /// pass (-1, -1) to hide the curser. Cursor is hidden by default.
    public static func setCursor(x: Int32, y: Int32) {
        tb_set_cursor(x, y)
    }

    /// Hide the cursor. Equivalent to `setCursor(x: -1, y: -1)`.
    public static func hideCursor() {
        tb_set_cursor(-1, -1)
    }

    /// Changes cell's parameters in the internal back buffer at the specified
    /// position.
    public static func put(x: Int32, y: Int32, cell: Cell) {
        var cell = cell
        tb_put_cell(x, y, &cell)
    }

    /// Changes cell's parameters in the internal back buffer at the specified
    /// position.
    public static func put(x: Int32, y: Int32, character: UnicodeScalar,
        foreground: Attributes = .default, background: Attributes = .default)
    {
        tb_change_cell(x, y, character.value, foreground.rawValue,
                       background.rawValue)
    }

    /// Changes cell's parameters in the internal back buffer at the specified
    /// position.
    public static func puts(x x0: Int32, y: Int32, string: String,
        foreground: Attributes = .default, background: Attributes = .default)
    {
        var x = x0
        for c in string.unicodeScalars {
            put(x: x, y: y, character: c)
            x += 1
        }
    }

    /// Returns a pointer to internal cell back buffer. You can get its
    /// dimensions using `width` and `height`. The pointer stays
    /// valid as long as no `clear()` and `present()` calls are made. The
    /// buffer is one-dimensional buffer containing lines of cells starting from
    /// the top.
    public static var unsafeCellBuffer: UnsafeMutableBufferPointer<Cell> {
        return UnsafeMutableBufferPointer(start: tb_cell_buffer(),
                                          count: Int(self.width * self.height))
    }

    /// The termbox input mode. Termbox has two input modes:
    /// 1. Esc input mode.  When ESC sequence is in the buffer and it doesn't
    ///    match any known ESC sequence => ESC means `Key.esc`.
    /// 2. Alt input mode.  When ESC sequence is in the buffer and it doesn't
    ///    match any known sequence => ESC enables Modifier.alt modifier for the
    ///    next keyboard event.
    ///
    /// You can also apply `.mouse` via OR operation to either of
    /// the modes (e.g. .esc | .mouse). If none of the main two modes were set,
    /// but the mouse mode was, `.esc` mode is used. If for
    /// some reason you've decided to use (.esc | .alt)
    /// combination, it will behave as if only .esc was selected.
    ///
    ///
    /// Default termbox input mode is `.esc`.
    public static var inputModes: InputModes {
        get {
            return InputModes(rawValue: tb_select_input_mode(0))
        }

        set {
            tb_select_input_mode(newValue.rawValue)
        }
    }

    // Sets the termbox output mode. Termbox has three output options:
    // 1. normal     => [1..8]
    //    This mode provides 8 different colors:
    //      black, red, green, yellow, blue, magenta, cyan, white
    //    Shortcut: TB_BLACK, TB_RED, ...
    //    Attributes: TB_BOLD, TB_UNDERLINE, TB_REVERSE
    //
    //    Example usage:
    //        tb_change_cell(x, y, '@', TB_BLACK | TB_BOLD, TB_RED);
    //
    // 2. color256        => [0..256]
    //    In this mode you can leverage the 256 terminal mode:
    //    0x00 - 0x07: the 8 colors as in TB_OUTPUT_NORMAL
    //    0x08 - 0x0f: TB_* | TB_BOLD
    //    0x10 - 0xe7: 216 different colors
    //    0xe8 - 0xff: 24 different shades of grey
    //
    //    Example usage:
    //        tb_change_cell(x, y, '@', 184, 240);
    //        tb_change_cell(x, y, '@', 0xb8, 0xf0);
    //
    // 2. color        => [0..216]
    //    This mode supports the 3rd range of the 256 mode only.
    //    But you don't need to provide an offset.
    //
    // 3. greyscale  => [0..23]
    //    This mode supports the 4th range of the 256 mode only.
    //    But you dont need to provide an offset.
    //
    // Default termbox output mode is `.normal`.
    public static var outputMode: OutputMode {
        get {
            return OutputMode(rawValue: tb_select_output_mode(0))!
        }

        set {
            tb_select_output_mode(newValue.rawValue)
        }
    }

    /// Wait for an event up to 'timeout' milliseconds and fill the 'event'
    /// structure with it, when the event is available.
    public static func peekEvent(timeoutInMilliseconds timeout: Int32) -> Event?
    {
        var tbEvent = tb_event()
        switch tb_peek_event(&tbEvent, timeout) {
        case 0:
            return .timeout
        default:
            return Event(tbEvent)
        }
    }

    // Wait for an event forever and fill the 'event' structure with it when the
    // event is available.
    public static func pollEvent() -> Event? {
        var tbEvent = tb_event()
        tb_poll_event(&tbEvent)
        return Event(tbEvent)
    }
}

