#/bin/bash

if [ `uname` == "Darwin" ]; then
Utilities/MacOSX/premake4 "$@"
fi