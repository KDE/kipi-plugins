#! /bin/sh
$PREPARETIPS >> tips.cpp
$XGETTEXT tips.cpp common/libkipiplugins/*.cpp -o $podir/kipiplugins.pot
rm -f tips.cpp

