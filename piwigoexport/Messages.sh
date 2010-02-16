#!/bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/kipiplugin_piwigoexport.pot
rm -f rc.cpp
