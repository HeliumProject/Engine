#/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ `uname` == "Darwin" ]; then
	$DIR/Utilities/MacOSX/premake4 "$@"
fi

if [ `uname` == "Linux" ]; then
	$DIR/Utilities/Linux/premake4 "$@"
fi