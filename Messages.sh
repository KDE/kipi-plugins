#! /bin/sh
$PREPARETIPS >> tips.cpp
$XGETTEXT tips.cpp -o $podir/kipiplugins.pot
rm -f tips.cpp

