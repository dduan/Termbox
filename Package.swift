// swift-tools-version:4.1
import PackageDescription

let package = Package(
    name: "Termbox",
    products: [
        .library(name: "Termbox", targets: ["Termbox"]),
        .library(name: "ctermbox", targets: ["ctermbox"]),
    ],
    targets: [
        .target(name: "Termbox", dependencies: ["ctermbox"]),
        .target(name: "ctermbox"),
    ]
)
