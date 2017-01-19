#!/bin/bash

VERSION=`grep VERSION config.h | cut -d\" -f2`

svn copy https://ggmud.svn.sourceforge.net/svnroot/ggmud/trunk https://ggmud.svn.sourceforge.net/svnroot/ggmud/tags/ggmud-$VERSION -m "Tagging version $VERSION"
