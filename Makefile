all: run logs

build/pebble-bezier.pbw: appinfo.json src/*.c
	pebble build

.PHONY: run
run: build/pebble-bezier.pbw
	pebble install --phone 192.168.1.104

.PHONY: logs
logs:
	pebble logs --phone 192.168.1.104

.PHONY: clean
clean:
	pebble clean
