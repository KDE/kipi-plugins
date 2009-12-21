#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp -o $podir/kipiplugin_expoblending.pot
rm -f rc.cpp
