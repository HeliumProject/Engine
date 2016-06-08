# Suppress apt-get confirmation prompts and update
printf "APT::Get::Assume-Yes \"true\";" | sudo tee -a /etc/apt/apt.conf
sudo apt-get update

# Install linux prerequisites
sudo DEBIAN_FRONTEND=noninteractive Dependencies/install-packages.sh

# Download and extract FBX SDK installer
wget http://download.autodesk.com/us/fbx_release_older/2016.1.2/fbx20161_2_fbxsdk_linux.tar.gz
tar xvzf fbx20161_2_fbxsdk_linux.tar.gz
rm fbx20161_2_fbxsdk_linux.tar.gz

# Install the FBX SDK to the expected location
mkdir $FBX_SDK
printf "yes\nn" | ./fbx20161_2_fbxsdk_linux $FBX_SDK
rm fbx20161_2_fbxsdk_linux

# Update compiler
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5