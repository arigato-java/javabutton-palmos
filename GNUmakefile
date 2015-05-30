# -*- coding:shift_jis -*-
.PHONY: clean Resources Code

javabutton.prc: Resources Code
	build-prc javabutton.prc "ジャバ" JaBo src/*.grc rsrc/*.bin

Resources:
	$(MAKE) -C rsrc

Code:
	$(MAKE) -C src

clean:
	$(MAKE) -C rsrc clean
	$(MAKE) -C src clean
	rm -f *.prc

