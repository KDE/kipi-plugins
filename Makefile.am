# Do *NOT* indent the content of the ifs, this will break the generated
# Makefile
if compile_SLIDESHOW
SLIDESHOWDIR = slideshow
endif

if compile_KAMERAKLIENT
KAMERAKLIENTDIR = kameraklient
endif

if compile_JPEGLOSSLESS
JPEGLOSSLESSDIR = jpeglossless
endif

if compile_TIMEADJUST
TIMEADJUSTDIR = timeadjust
endif

SUBDIRS = printwizard calendar mpegencoder cdarchiving imagesgallery \
	sendimages acquireimages batchprocessimages \
	wallpaper rawconverter findimages \
	$(SLIDESHOWDIR) $(KAMERAKLIENTDIR) $(JPEGLOSSLESSDIR) \
	$(TIMEADJUSTDIR)

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