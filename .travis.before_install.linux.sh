# Suppress apt-get confirmation prompts and update
printf "APT::Get::Assume-Yes \"true\";" | sudo tee -a /etc/apt/apt.conf
sudo apt-get update

# Install linux prerequisites
sudo DEBIAN_FRONTEND=noninteractive Dependencies/install-packages-linux.sh

# Update compiler
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5