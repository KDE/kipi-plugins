# Do *NOT* indent the content of the ifs, this will break the generated
# Makefile
if compile_SLIDESHOW
SLIDESHOWDIR = slideshow
endif

if compile_KAMERAKLIENT
KAMERAKLIENTDIR = kameraklient
endif

if compile_HTMLGALLERY
HTMLGALLERYDIR = htmlgallery
endif

if compile_JPEGLOSSLESS
jpeglosslessdir = jpeglossless
endif

if compile_TIMEADJUST
timeadjustdir = timeadjust
endif

if compile_LIBKEXIF
JPEGLOSSLESSDIR = $(jpeglosslessdir)
TIMEADJUSTDIR = $(timeadjustdir)
endif


if compile_ACQUIREIMAGES
ACQUIREIMAGESDIR = acquireimages
endif

if compile_RAWCONVERTER
RAWCONVERTERDIR = rawconverter
endif

SUBDIRS = printwizard calendar mpegencoder cdarchiving imagesgallery simpleviewerexport\
	sendimages batchprocessimages wallpaper findimages galleryexport flickrexport\
	$(SLIDESHOWDIR) $(KAMERAKLIENTDIR) $(JPEGLOSSLESSDIR) \
	$(TIMEADJUSTDIR) $(ACQUIREIMAGESDIR) $(RAWCONVERTERDIR) \
	$(HTMLGALLERYDIR)

rcdir = $(kde_datadir)/kipi
rc_DATA = tips

# i18n translation tips messages
messages: rc.cpp
	$(PREPARETIPS) >> tips.cpp
	LIST=`find . -name \tips.cpp`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/kipiplugins.pot; \
	fi
	rm -f tips.cpp

