#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp -o $podir/kipiplugin_htmlexport.pot
rm -f rc.cpp
