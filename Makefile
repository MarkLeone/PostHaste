
all:
	$(MAKE) -C src

.PHONY: tests
tests:
	$(MAKE) tests -C src

clean:
	$(MAKE) clean -C src
