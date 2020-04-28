all: client server

shared:
	$(MAKE) -C shared

client: shared
	$(MAKE) -C client

server: shared
	$(MAKE) -C server

clean:
	$(MAKE) -C shared clean
	$(MAKE) -C client clean
	$(MAKE) -C server clean

.PHONY: all
.PHONY: shared
.PHONY: client
.PHONY: server
.PHONY: clean
