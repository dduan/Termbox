This is a Swift wrapper for [termbox](https://github.com/nsf/termbox).

Both the original C library and a Swift wrapper are included for Swift Package
Manager uses. You can import `ctermbox` to use the original C API, or import
`Termbox` to use the Swift API.

For a quick start, checkout the example `paint` (`swift run paint`),
a small TUI program built with termbox.

You can read the comments in `Sources/Termbox/Termbox.swift` for more
documentations. There are more demo programs in the [C library][] as well.

[demo]: https://github.com/dduan/Termbox/blob/main/Examples/paint/main.swift
[C library]: https://github.com/nsf/termbox/tree/master/src/demo
