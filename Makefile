test-SwiftPM:
	@swift build -Xswiftc -warnings-as-errors

test-docker:
	@Scripts/run-tests-linux-docker.sh
