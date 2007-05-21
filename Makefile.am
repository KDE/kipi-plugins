# Do *NOT* indent the content of the ifs, this will break the generated
# Makefile
if compile_SLIDESHOW
SLIDESHOWDIR = slideshow
endif

if compile_KAMERAKLIENT
KAMERAKLIENTDIR = kameraklient
endif

if compile_HTMLEXPORT
HTMLEXPORTDIR = htmlexport
endif

if compile_ACQUIREIMAGES
ACQUIREIMAGESDIR = acquireimages
endif

if compile_RAWCONVERTER
RAWCONVERTERDIR = rawconverter
endif

if compile_IPODEXPORT
IPODEXPORTDIR = ipodexport
endif

if compile_CALENDAR
CALENDARDIR = calendar
endif

SUBDIRS = common printwizard mpegencoder cdarchiving simpleviewerexport \
	      sendimages batchprocessimages wallpaper findimages galleryexport flickrexport \
	      jpeglossless timeadjust gpssync metadataedit imageviewer \
	      $(SLIDESHOWDIR) $(KAMERAKLIENTDIR) $(ACQUIREIMAGESDIR) $(RAWCONVERTERDIR) \
	      $(HTMLEXPORTDIR) $(IPODEXPORTDIR) $(CALENDARDIR)

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

