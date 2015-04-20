#!/usr/bin/sh


CB_VERSION=rel-3.0.2.xml
CB_RELEASE_GIT=https://github.com/couchbase/manifest.git
TEMP_FOLDER_NAME=arch-couchbase-3.0.3


set -e

cwd=$(pwd)
cd /tmp
rm -rf $TEMP_FOLDER_NAME
mkdir $TEMP_FOLDER_NAME
cd $TEMP_FOLDER_NAME

# Install official dependencies
sudo pacman --noconfirm --needed -Sy\
       python2 \
       python-virtualenv \
       erlang \
       couchdb

virtualenv -p /usr/bin/python2.7 temp-python2
source temp-python2/bin/activate

# Install AUR dependencies

if ! pacman -Qs repo | grep local/repo; then
  wget https://aur.archlinux.org/packages/re/repo/repo.tar.gz
  tar -xvf repo.tar.gz
  cd repo
  makepkg -sic --noconfirm --needed
  cd ..
  rm -rf repo
else
  echo "INFO: repo already installed."
fi


if ! pacman -Qs v8-3.21 | grep v8-3.21; then
  wget https://aur.archlinux.org/packages/v8/v8-3.21/v8-3.21.tar.gz
  tar -xvf v8-3.21.tar.gz
  cd v8-3.21
  makepkg -sic --noconfirm --needed
  cd ..
  rm -rf v8-3.21
else
  echo "INFO: v8-3.21 already installed."
fi
#
## Install couchbase
rm -rf couchbase
mkdir couchbase
cd couchbase
git config --global user.email your@email.addr
git config --global user.name  your_name
repo init -u git://github.com/couchbase/manifest.git -m $CB_VERSION
repo sync
make
#sudo make install
mkdir -p /usr/local
cp -R install/* /usr/local/
ulimit -n 10240

echo '
couchbase-server soft nofile 65536
couchbase-server hard nofile 65536
' | sudo tee -a /etc/security/limits.conf
deactivate

cd ..
rm rf $TEMP_FOLDER_NAME
cd $cwd

set +e
