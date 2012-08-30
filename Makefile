all: config build

config configure:
	@node-waf configure

build:
	@node-waf build

clean:
	-@node-waf clean distclean

.PHONY: config configure build clean
