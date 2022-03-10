test-SwiftPM:
	@swift build -Xswiftc -warnings-as-errors

test-docker:
	@Scripts/docker.sh Termbox 'swift build -Xswiftc -warnings-as-errors' 5.5.3 bionic
