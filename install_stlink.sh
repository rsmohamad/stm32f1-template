#!/bin/sh

sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install libusb-1.0.0-dev
git clone https://github.com/texane/stlink.git
cd stlink
sudo cp etc/udev/rules.d/* /etc/udev/rules.d
make release
cd build/Release
sudo make install
sudo ldconfig
sudo udevadm control --reload-rules
sudo udevadm trigger
cd ../../../
rm -rf stlink
