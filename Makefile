all:
	$(MAKE) -C common
	$(MAKE) -C protoc
	$(MAKE) -C scrape
	$(MAKE) -C extract
	$(MAKE) -C pitch

clean:
	$(MAKE) -C common clean
	$(MAKE) -C protoc clean
	$(MAKE) -C scrape clean
	$(MAKE) -C extract clean
	$(MAKE) -C pitch clean
