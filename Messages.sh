#! /bin/sh
(cd common/data && $PREPARETIPS > ../../tips.cpp)
$PREPARETIPS >> tips.cpp
$XGETTEXT tips.cpp `find common/libkipiplugins -name \*.cpp -o -name \*.h -o -name \*.h.cmake` -o $podir/kipiplugins.pot
rm -f tips.cpp
