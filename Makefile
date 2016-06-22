all:
	$(MAKE) -C common
	$(MAKE) -C quid
	$(MAKE) -C protoc
	$(MAKE) -C scrape
	$(MAKE) -C extract
	$(MAKE) -C pitch
	$(MAKE) -C cynder
	$(MAKE) -C orchestrate
	$(MAKE) -C connect

clean:
	$(MAKE) -C common clean
	$(MAKE) -C quid clean
	$(MAKE) -C protoc clean
	$(MAKE) -C scrape clean
	$(MAKE) -C extract clean
	$(MAKE) -C pitch clean
	$(MAKE) -C cynder clean
	$(MAKE) -C orchestrate clean
	$(MAKE) -C connect clean
