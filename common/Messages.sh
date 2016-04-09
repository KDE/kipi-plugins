#! /bin/sh
$XGETTEXT tips.cpp `find libkipiplugins -name \*.cpp -o -name \*.h -o -name \*.h.cmake` -o $podir/kipiplugins.pot
