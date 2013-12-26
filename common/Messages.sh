#! /bin/sh
(cd data && $PREPARETIPS > ../tips.cpp)
$XGETTEXT tips.cpp `find libkipiplugins -name \*.cpp -o -name \*.h -o -name \*.h.cmake` -o $podir/kipiplugins.pot
rm -f tips.cpp
