| Swift 5.3 |
|-|
|[![Amazon Linux 2](https://github.com/dduan/Termbox/workflows/Amazon%20Linux%202/badge.svg)](https://github.com/dduan/Termbox/actions?query=workflow%3A%22Amazon+Linux+2%22)|
|[![CentOS 8](https://github.com/dduan/Termbox/workflows/CentOS%208/badge.svg)](https://github.com/dduan/Termbox/actions?query=workflow%3A%22CentOS+8%22)|
|[![macOS 11.15](https://github.com/dduan/Termbox/workflows/macOS%2011.15/badge.svg)](https://github.com/dduan/Termbox/actions?query=workflow%3A%22macOS+11.15%22)|
|[![Ubuntu Bionic](https://github.com/dduan/Termbox/workflows/Ubuntu%20Bionic/badge.svg)](https://github.com/dduan/Termbox/actions?query=workflow%3A%22Ubuntu+Bionic%22)|
|[![Ubuntu Focal](https://github.com/dduan/Termbox/workflows/Ubuntu%20Focal/badge.svg)](https://github.com/dduan/Termbox/actions?query=workflow%3A%22Ubuntu+Focal%22)|


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
