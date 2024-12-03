#!/usr/bin/env bash

## First version of a Pebble development environment setup script
## It has all the required commands, but it might fail at certain steps...

set -euxo pipefail

sudo apt update && sudo apt upgrade -y
sudo apt-get install software-properties-common git

sudo add-apt-repository universe
sudo apt update 

sudo apt install python2 -y

curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py

sudo python2 get-pip.py

sudo apt install wget python-dev libsdl1.2debian libfdt1 libpixman-1-0 npm gcc -y

cd ~
mkdir ~/pebble-dev
cd ~/pebble-dev

wget https://rebble-sdk.s3-us-west-2.amazonaws.com/pebble-sdk-4.6-rc2-linux64.tar.bz2
tar -jxf pebble-sdk-4.6-rc2-linux64.tar.bz2

cd ~/pebble-dev/pebble-sdk-4.6-rc2-linux64

python2 -m pip install virtualenv --user
python2 -m virtualenv .env

source .env/bin/activate

pip install -r requirements.txt

deactivate

echo 'export PATH=~/pebble-dev/pebble-sdk-4.6-rc2-linux64/bin:$PATH' >> ~/.bashrc 

set +x

echo 'Restart your terminal session'