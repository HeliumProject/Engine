#/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ `uname` == "Darwin" ]; then
	OS=osx
fi

if [ `uname` == "Linux" ]; then
	OS=linux
fi

pushd $DIR/Dependencies/premake > /dev/null

if [ ! -f bin/release/premake5 ]; then
	make -f Bootstrap.mak $OS
fi

popd > /dev/null

$DIR/Dependencies/premake/bin/release/premake5 "$@"
