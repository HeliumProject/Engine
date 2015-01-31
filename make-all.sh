#/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

$DIR/premake.sh gmake

make $@ config=debug_x64
if [ "$?" -ne "0" ]; then
	echo "Debug failed!"
	exit 1
fi

make $@ config=intermediate_x64
if [ "$?" -ne "0" ]; then
	echo "Intermediate failed!"
	exit 1
fi

make $@ config=profile_x64
if [ "$?" -ne "0" ]; then
	echo "Profile failed!"
	exit 1
fi

make $@ config=release_x64
if [ "$?" -ne "0" ]; then
	echo "Release failed!"
	exit 1
fi
