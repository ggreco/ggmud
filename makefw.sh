#!/bin/sh

# script for pulling together a MacOSX app bundle using GTK libraries built as frameworks.

GTKQUARTZ_ROOT=/Users/gabry/Source/gtk-osx-framework/framework

version=`grep VERSION config.h | cut -d' ' -f 3 | sed "s/\"//g"`
echo "Version is $version"

# setup directory structure

APPROOT=GGMud.app/Contents
Frameworks=$APPROOT/Frameworks
Resources=$APPROOT/Resources

echo "Removing old GGMud.app tree ..."

rm -rf GGMud.app

echo "Building new app directory structure ..."

# only bother to make the longest paths

mkdir -p $APPROOT/MacOS
mkdir -p $Resources
mkdir -p $Frameworks

# edit plist
sed -e "s?@ENV@?$env?g" -e "s?@VERSION@?$version?g" < Info.plist.in > Info.plist

# copy static files

cp Info.plist $APPROOT
cp GGMud.icns  $Resources


# copy executable
echo "Copying GGMud executable ..."
cp ggmud $APPROOT/MacOS/
cp gg_help.txt $APPROOT/MacOS/
# copy everything related to gtk-quartz
echo "Copying GTK-Quartz tree ..."
cp -R $GTKQUARTZ_ROOT/Glib.framework $GTKQUARTZ_ROOT/Cairo.framework \
      $GTKQUARTZ_ROOT/Gtk.framework $Frameworks/


echo "Done."

