import PackageDescription

let package = Package(
    name: "Termbox",
    targets: [
        Target(name: "Termbox", dependencies: ["ctermbox"]),
        Target(name: "ctermbox"),
    ]
)
