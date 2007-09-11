#!/bin/sh

# script for pulling together a MacOSX app bundle.

GTKQUARTZ_ROOT=/Users/gabry/projects/gtk

version=`grep VERSION config.h | cut -d' ' -f 3 | sed "s/'//g"`
echo "Version is $version"

# setup directory structure

APPROOT=GGMud.app/Contents
Frameworks=$APPROOT/Frameworks
Resources=$APPROOT/Resources
Shared=$Resources/share
Etc=$Resources/etc

echo "Removing old GGMud.app tree ..."

rm -rf GGMud.app

echo "Building new app directory structure ..."

# only bother to make the longest paths

mkdir -p $APPROOT/MacOS
mkdir -p $APPROOT/Resources
mkdir -p $APPROOT/Resources/bin
mkdir -p $Frameworks/modules
mkdir -p $Shared/templates
mkdir -p $Etc

# edit plist
sed -e "s?@ENV@?$env?g" -e "s?@VERSION@?$version?g" < Info.plist.in > Info.plist

# copy static files

cp Info.plist $APPROOT
cp GGMud.icns  $APPROOT/Resources




# copy executable
echo "Copying GGMud executable ..."
cp ggmud $APPROOT/MacOS/
cp gg_help.txt $APPROOT/MacOS/
cp gg_help.txt $APPROOT/Resources/bin/
# copy everything related to gtk-quartz
echo "Copying GTK-Quartz tree ..."
cp -R $GTKQUARTZ_ROOT/lib/*.dylib $Frameworks/
cp -R $GTKQUARTZ_ROOT/etc/* $Etc
echo "Copying all Pango modules ..."
cp -R $GTKQUARTZ_ROOT/lib/pango/1.6.0/modules/*.so $Frameworks/modules
echo "Copying all GDK Pixbuf loaders ..."
cp -R $GTKQUARTZ_ROOT/lib/gtk-2.0/2.10.0/loaders/*.so $Frameworks/modules
# charset alias file
cp -R $GTKQUARTZ_ROOT/lib/charset.alias $Resources

# generate new Pango module file
cat > pangorc <<EOF 
[Pango]
ModulesPath=$GTKQUARTZ_ROOT/lib/pango/1.6.0/modules
EOF
env PANGO_RC_FILE=pangorc $GTKQUARTZ_ROOT/bin/pango-querymodules | sed "s?$GTKQUARTZ_ROOT/lib/pango/1.6.0/modules/?@executable_path/../Frameworks/modules/?" > $Resources/pango.modules
rm pangorc

# generate a new GDK pixbufs loaders file
sed "s?$GTKQUARTZ_ROOT/lib/gtk-2.0/2.10.0/loaders/?@executable_path/../Frameworks/modules/?" < $GTKQUARTZ_ROOT/etc/gtk-2.0/gdk-pixbuf.loaders > $Resources/gdk-pixbuf.loaders


# this one is special - we will set GTK_PATH to $Frameworks/clearlooks
#cp ../../libs/clearlooks/libclearlooks.dylib $Frameworks
#mkdir -p $Frameworks/clearlooks/engines
#(cd $Frameworks/clearlooks/engines && ln -s ../../libclearlooks.dylib libclearlooks.dylib && ln -s ../../libclearlooks.dylib libclearlooks.so)
    
# go through and recursively remove any .svn dirs in the bundle
for svndir in `find GGMud.app -name .svn -type dir`; do
    rm -rf $svndir
done

# now fix up the executables
echo "Fixing up executable dependency names ..."
#for exe in ggmud ; do
    EXE=$APPROOT/MacOS/ggmud
# it was $exe
    changes=""
    for lib in `otool -L $EXE | egrep "($GTKQUARTZ_ROOT|/local/|libs/)" | awk '{print $1}'` ; do
	base=`basename $lib`
	changes="$changes -change $lib @executable_path/../Frameworks/$base"
    done
    if test "x$changes" != "x" ; then
	install_name_tool $changes $EXE
    fi
#done

echo "Fixing up library names ..."
# now do the same for all the libraries we include
for dylib in $Frameworks/*.dylib $Frameworks/modules/*.so ; do
    # skip symlinks
    if test ! -L $dylib ; then
	
	# change all the dependencies

	changes=""
	for lib in `otool -L $dylib | egrep "($GTKQUARTZ_ROOT|/local/|libs/)" | awk '{print $1}'` ; do
	    base=`basename $lib`
	    changes="$changes -change $lib @executable_path/../Frameworks/$base"
	done

	if test "x$changes" != x ; then
	    if  install_name_tool $changes $dylib ; then
		:
	    else
		exit 1
	    fi
	fi

	# now the change what the library thinks its own name is

	base=`basename $dylib`
	install_name_tool -id @executable_path/../Frameworks/$base $dylib
    fi
done

echo "Done."

