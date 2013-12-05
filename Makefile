all:
	$(MAKE) -C Code
	$(MAKE) -C Dissertation
	$(MAKE) -C Report

clean:
	$(MAKE) -C Code clean
	$(MAKE) -C Dissertation clean
	$(MAKE) -C Report clean
