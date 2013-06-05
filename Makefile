tools:
	$(MAKE) -f Tools.make CXX=clang++

runtime:
	$(MAKE) -f Runtime.make CXX=clang++

.PHONY : tools runtime