# cd to script location
cd "${0%/*}"

# Download and extract FBX SDK installer
wget http://download.autodesk.com/us/fbx/2018/2018.1.1/fbx20181_1_fbxsdk_clang_mac.pkg.tgz
tar xvzf fbx20181_1_fbxsdk_clang_mac.pkg.tgz
rm fbx20181_1_fbxsdk_clang_mac.pkg.tgz

# Run the installer pkg
sudo installer -allowUntrusted -verboseR -pkg ./fbx20181_1_fbxsdk_clang_macos.pkg -target /
rm -r fbx20181_1_fbxsdk_clang_macos.pkg
