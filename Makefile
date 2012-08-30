all: config build

config configure:
	@node-waf configure

build:
	@node-waf build
	@cp build/Release/*.node ./

test:
	@cd test $(foreach t,$(subst test/,,$(wildcard test/*.js)),&& nodejs $(t))

clean:
	-@node-waf clean distclean
	@rm -f test/*.tc*
	@rm -f *.node

.PHONY: config configure build test clean
