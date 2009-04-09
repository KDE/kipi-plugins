#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp -o $podir/kipiplugin_advancedslideshow.pot
rm -f rc.cpp
