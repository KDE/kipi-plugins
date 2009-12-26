#! /bin/sh
$PREPARETIPS >> tips.cpp
$XGETTEXT tips.cpp `find common/libkipiplugins -name \*.cpp -o -name \*.h` common/libkipiplugins/*.h.cmake -o $podir/kipiplugins.pot
rm -f tips.cpp

