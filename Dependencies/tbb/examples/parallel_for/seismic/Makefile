# Copyright 2005-2010 Intel Corporation.  All Rights Reserved.
#
# This file is part of Threading Building Blocks.
#
# Threading Building Blocks is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# Threading Building Blocks is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Threading Building Blocks; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.

# GNU Makefile that builds and runs example.
NAME=seismic
ARGS=- 300

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

include ../../common/gui/Makefile.gmake

all:	release test

resources:
ifeq ($(UI),mac)
	mkdir -p $(APPRES)/English.lproj $(NAME).app/Contents/MacOS
	$(PBXCP) xcode/English.lproj/main.nib $(APPRES)/English.lproj
	$(PBXCP) xcode/Info.plist $(APPRES)
endif

release: ../../common/gui/$(UI)video.cpp SeismicSimulation.cpp resources
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(EXE) $< SeismicSimulation.cpp -ltbb $(LIBS)

debug: ../../common/gui/$(UI)video.cpp SeismicSimulation.cpp resources
	$(CXX) -g -O0 -DTBB_USE_DEBUG $(CXXFLAGS) -o $(EXE) $< SeismicSimulation.cpp -ltbb_debug $(LIBS)

clean:
	$(RM) $(EXE) *.o *.d
ifeq ($(UI),mac)
	rm -rf $(NAME).app
endif

test:
ifeq ($(UI),mac)
	export DYLD_LIBRARY_PATH="$(DYLD_LIBRARY_PATH):$(TBBLIBSPATH)"; ./$(EXE) $(ARGS)
else
	./$(EXE) $(ARGS)
endif
