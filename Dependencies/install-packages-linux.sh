# install packages from apt
apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev libgtk2.0-dev

# cd to script location
cd "${0%/*}"

# Download and extract FBX SDK installer
wget http://download.autodesk.com/us/fbx_release_older/2016.1.2/fbx20161_2_fbxsdk_linux.tar.gz
tar xvzf fbx20161_2_fbxsdk_linux.tar.gz
rm fbx20161_2_fbxsdk_linux.tar.gz

# Install the FBX SDK to the expected location
mkdir fbx
printf "yes\nn" | ./fbx20161_2_fbxsdk_linux fbx
rm fbx20161_2_fbxsdk_linux