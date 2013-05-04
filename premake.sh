#/bin/bash

if [ `uname` == "Darwin" ]; then
	Utilities/MacOSX/premake4 "$@"
fi

if [ `uname` == "Linux" ]; then
	Utilities/Linux/premake4 "$@"
fi