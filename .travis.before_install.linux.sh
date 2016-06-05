# Suppress apt-get confirmation prompts and update
printf "APT::Get::Assume-Yes \"true\";" | sudo tee -a /etc/apt/apt.conf
sudo apt-get update

# Install linux prerequisites
sudo DEBIAN_FRONTEND=noninteractive Dependencies/install-packages.sh

# Download and extract FBX SDK 2015.1 installer
wget http://images.autodesk.com/adsk/files/fbx20161_2_fbxsdk_linux.tar.gz
tar xvzf fbx20161_2_fbxsdk_linux.tar.gz
rm fbx20161_2_fbxsdk_linux.tar.gz

# Install the FBX SDK to the expected location
mkdir $FBX_SDK
printf "yes\nn" | ./fbx20161_2_fbxsdk_linux $FBX_SDK
rm fbx20161_2_fbxsdk_linux
