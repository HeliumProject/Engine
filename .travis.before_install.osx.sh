# Download and extract FBX SDK 2015.1 installer
wget http://images.autodesk.com/adsk/files/fbx20151_fbxsdk_clang_mac.pkg.tgz
tar xvzf fbx20151_fbxsdk_clang_mac.pkg.tgz
rm fbx20151_fbxsdk_clang_mac.pkg.tgz

# Run the installer pkg
sudo installer -allowUntrusted -verboseR -pkg ./fbx20151_fbxsdk_clang_macos.pkg -target /
rm -r fbx20151_fbxsdk_clang_macos.pkg

# Create symlink at expected location
ln -s "/Applications/Autodesk/FBX SDK/2015.1/" $FBX_SDK
