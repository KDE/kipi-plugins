#! /bin/sh
$PREPARETIPS >> tips.cpp
$XGETTEXT tips.cpp common/libkipiplugins/*.cpp common/libkipiplugins/*.h common/libkipiplugins/*.h.cmake -o $podir/kipiplugins.pot
rm -f tips.cpp

