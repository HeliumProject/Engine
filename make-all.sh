#/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd Build

make $@ config=debug
if [ "$?" -ne "0" ]; then
	echo "Debug failed!"
	exit 1
fi

make $@ config=intermediate
if [ "$?" -ne "0" ]; then
	echo "Intermediate failed!"
	exit 1
fi

make $@ config=profile
if [ "$?" -ne "0" ]; then
	echo "Profile failed!"
	exit 1
fi

make $@ config=release
if [ "$?" -ne "0" ]; then
	echo "Release failed!"
	exit 1
fi

popd