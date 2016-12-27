This is a Swift wrapper for [termbox](https://github.com/nsf/termbox).

Both the original C library and a Swift wrapper are included for Swift Package
Manager uses. You can import `ctermbox` to use the original C API, or import
`Termbox` to use the Swift API.

For a quick start, checkout this [demo][] project, which uses SPM to use Termbox
as a dependency and includes a small TUI program built with it.

You can read the comments in `Sources/Termbox/Termbox.swift` for more
documentations. There are more demo programs in the [C library][] as well.

[demo]: https://github.com/dduan/TerminalPaint
[C library]: https://github.com/nsf/termbox/tree/master/src/demo
