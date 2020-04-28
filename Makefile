all: client server

shared: config.mk
	$(MAKE) -C shared

client: shared config.mk
	$(MAKE) -C client

server: shared config.mk
	$(MAKE) -C server

%.mk: %.mk.default
	cat $< > $@

clean:
	$(MAKE) -C shared clean
	$(MAKE) -C client clean
	$(MAKE) -C server clean

.PHONY: all
.PHONY: shared
.PHONY: client
.PHONY: server
.PHONY: clean
