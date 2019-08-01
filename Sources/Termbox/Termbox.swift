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
        case .backspace:       return 0x08
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
        case .arrowLeft:       return 0xffff-18
        case .arrowRight:      return 0xffff-19
        case .arrowDown:       return 0xffff-20
        case .arrowUp:         return 0xffff-21

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
        case 0xffff-18: self = .arrowLeft
        case 0xffff-19: self = .arrowRight
        case 0xffff-20: self = .arrowDown
        case 0xffff-21: self = .arrowUp
        default:
            self = .rawValue(rawValue)
        }
    }

    public static func == (lhs: Key, rhs: Key) -> Bool {
        return lhs.rawValue == rhs.rawValue
    }
}


public typealias AttrSize = UInt16
/// Attributes, it is possible to use multiple attributes by combining them
/// using bitwise OR ('|'). Although, colors cannot be combined. But you can
/// combine attributes and a single color. See also `Cell`'s `foreground` and
/// `background` fields.
public struct Attributes: OptionSet {
    public let rawValue: AttrSize

    public init(rawValue: AttrSize) {
        self.rawValue = rawValue
    }

    public static let `default` = Attributes(rawValue: AttrSize(TB_DEFAULT))
    public static let zero = Attributes(rawValue: 0)

    // colors
    public static let red           = Attributes(rawValue: AttrSize(TB_RED))
    public static let green         = Attributes(rawValue: AttrSize(TB_GREEN))
    public static let yellow        = Attributes(rawValue: AttrSize(TB_YELLOW))
    public static let blue          = Attributes(rawValue: AttrSize(TB_BLUE))
    public static let magenta       = Attributes(rawValue: AttrSize(TB_MAGENTA))
    public static let cyan          = Attributes(rawValue: AttrSize(TB_CYAN))
    public static let lighterGray   = Attributes(rawValue: AttrSize(TB_LIGHTER_GRAY))
    public static let mediumGray    = Attributes(rawValue: AttrSize(TB_MEDIUM_GRAY))
    public static let lightRed      = Attributes(rawValue: AttrSize(TB_LIGHT_RED))
    public static let lightGreen    = Attributes(rawValue: AttrSize(TB_LIGHT_GREEN))
    public static let lightYellow   = Attributes(rawValue: AttrSize(TB_LIGHT_YELLOW))
    public static let lightBlue     = Attributes(rawValue: AttrSize(TB_LIGHT_BLUE))
    public static let lightMagenta  = Attributes(rawValue: AttrSize(TB_LIGHT_MAGENTA))
    public static let lightCyan     = Attributes(rawValue: AttrSize(TB_LIGHT_CYAN))
    public static let white         = Attributes(rawValue: AttrSize(TB_WHITE))
    public static let black         = Attributes(rawValue: AttrSize(TB_BLACK))
    public static let darkestGray   = Attributes(rawValue: AttrSize(TB_DARKEST_GRAY))
    public static let darkerGray    = Attributes(rawValue: AttrSize(TB_DARKER_GRAY))
    public static let darkGray      = Attributes(rawValue: AttrSize(TB_DARK_GRAY))
    public static let lightGray     = Attributes(rawValue: AttrSize(TB_LIGHT_GRAY))
    public static let lightestGray  = Attributes(rawValue: AttrSize(TB_LIGHTEST_GRAY))

    // other
    public static let bold      = Attributes(rawValue: AttrSize(TB_BOLD))
    public static let underline = Attributes(rawValue: AttrSize(TB_UNDERLINE))
    public static let reverse   = Attributes(rawValue: AttrSize(TB_REVERSE))
}

public enum Modifier: UInt8 {
    case none = 0
    case shiftAlt     // 1
    case shift        // 2
    case alt          // 3
    case altShift     // 4
    case ctrl         // 5
    case ctrlShift    // 6
    case altCtrl      // 7
    case altCtrlShift // 8
    case altAlt       // 9
}

public enum Mouse: UInt16 {
    case left = 0xffe9      // 0xffff-22
    case right = 0xffe8     // 0xffff-23
    case middle = 0xffe7    // 0xffff-24
    case release = 0xffe6   // 0xffff-25
    case wheelUp = 0xffe5   // 0xffff-26
    case wheelDown = 0xffe4 // 0xffff-27
}

/// User interaction event.
public enum Event {
    case key(modifier: Modifier, value: Key)
    case character(modifier: Modifier, value: UnicodeScalar)
    case resize(width: UInt16, height: UInt16)
    case mouse(x: UInt16, y: UInt16, event: Mouse)
    case other(UInt8)
    case timeout

    init(_ tbEvent: tb_event) {
        switch tbEvent.type {
        case 1:
            if tbEvent.ch == 0 {
                self = .key(modifier: Modifier(rawValue: tbEvent.meta) ?? .none,
                            value: Key(rawValue: tbEvent.key))
            } else {
                self = .character(modifier: Modifier(rawValue: tbEvent.meta) ?? .none,
                                  value: UnicodeScalar(tbEvent.ch)!)
            }
        case 2:
            self = .resize(width: tbEvent.w, height: tbEvent.h)
        case 3:
            guard let mouse = Mouse(rawValue: tbEvent.key) else {
                self = .other(tbEvent.type)
                return
            }
            self = .mouse(x: tbEvent.x, y: tbEvent.y, event: mouse)
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

public enum OutputMode: Int32 {
    case color256 = 1
    case trueColor = 2
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

        tb_clear_screen()
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
    /// resized after `clear()` or `render()` function calls. Both
    /// dimensions have an unspecified negative value when called before
    /// initialization or after shutdown.
    public static var width: Int32 {
        return tb_width()
    }

    public static var height: Int32 {
        return tb_height()
    }

    /// Clears the internal back buffer using default color.
    public static func clear() {
        tb_clear_buffer()
    }

    /// Should be called after a
    public static func resize() {
        tb_resize()
    }

    /// Clears the internal back buffer using specified color/attributes.
    public static func clear(foreground: Attributes, background: Attributes) {
        tb_set_clear_attributes(foreground.rawValue, background.rawValue)
    }

    /// Synchronizes the internal back buffer with the terminal.
    public static func render() {
        tb_render()
    }

    public static var title: String = "" {
        didSet {
            tb_set_title(title)
        }
    }

    /// Sets the position of the cursor. Upper-left character is (0, 0). If you
    /// pass (-1, -1) to hide the curser. Cursor is hidden by default.
    public static func setCursor(x: Int32, y: Int32) {
        tb_set_cursor(x, y)
    }

    public static func hideCursor() {
        tb_hide_cursor()
    }

    public static func showCursor() {
        tb_show_cursor()
    }

    public static func enableMouse() {
        tb_enable_mouse()
    }

    public static func disableMouse() {
        tb_disable_mouse()
    }

    /// Changes cell's parameters in the internal back buffer at the specified
    /// position.
    public static func putc(x: Int32, y: Int32, char: UnicodeScalar,
        foreground: Attributes = .default, background: Attributes = .default)
    {
        tb_char(x, y, foreground.rawValue, background.rawValue, char.value)
    }

    /// Changes cell's parameters in the internal back buffer at the specified
    /// position.
    public static func puts(x: Int32, y: Int32, string: String,
        foreground: Attributes = .default, background: Attributes = .default)
    {
        let buffer = UnsafeMutablePointer<Int8>(mutating: string)
        tb_string(x, y, foreground.rawValue, background.rawValue, buffer)
    }

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
    public static func peekEvent(timeoutInMilliseconds timeout: Int32) -> Event
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
    public static func pollEvent() -> Event {
        var tbEvent = tb_event()
        tb_poll_event(&tbEvent)
        return Event(tbEvent)
    }

    public static var debug: (String) -> Void = { _ in }
}

