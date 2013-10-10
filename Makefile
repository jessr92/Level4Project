all:
	$(MAKE) -C Code
	$(MAKE) -C Dissertation

clean:
	$(MAKE) -C Code clean
	$(MAKE) -C Dissertation clean
