# Do *NOT* indent the content of the ifs, this will break the generated
# Makefile

# Please keep following list sorted alphabetically

if compile_ACQUIREIMAGES
ACQUIREIMAGESDIR = acquireimages
endif

if compile_BATCHPROCESSIMAGES
BATCHPROCESSIMAGESDIR = batchprocessimages
endif

if compile_CALENDAR
CALENDARDIR = calendar
endif

if compile_CDARCHIVING
CDARCHIVINGDIR = cdarchiving
endif

if compile_FINDIMAGES
FINDIMAGESDIR = findimages
endif

if compile_FLICKREXPORT
FLICKREXPORTDIR = flickrexport
endif

if compile_GALLERYEXPORT
GALLERYEXPORTDIR = galleryexport
endif

if compile_GPSSYNC
GPSSYNCDIR = gpssync
endif

if compile_HTMLEXPORT
HTMLEXPORTDIR = htmlexport
endif

if compile_IMAGEVIEWER
IMAGEVIEWERDIR = imageviewer
endif

if compile_IPODEXPORT
IPODEXPORTDIR = ipodexport
endif

if compile_JPEGLOSSLESS
JPEGLOSSLESSDIR = jpeglossless
endif

if compile_KAMERAKLIENT
KAMERAKLIENTDIR = kameraklient
endif

if compile_METADATAEDIT
METADATAEDITDIR = metadataedit
endif

if compile_MPEGENCODER
MPEGENCODERDIR = mpegencoder
endif

if compile_PICASAWEBEXPORT
PICASAWEBEXPORTDIR = picasawebexport
endif

if compile_PRINTWIZARD
PRINTWIZARDDIR = printwizard
endif

if compile_RAWCONVERTER
RAWCONVERTERDIR = rawconverter
endif

if compile_SENDIMAGES
SENDIMAGESDIR = sendimages
endif

if compile_SIMPLEVIEWEREXPORT
SIMPLEVIEWEREXPORTDIR = simpleviewerexport
endif

if compile_SLIDESHOW
SLIDESHOWDIR = slideshow
endif

if compile_TIMEADJUST
TIMEADJUSTDIR = timeadjust
endif

if compile_WALLPAPER
WALLPAPERDIR = wallpaper
endif

SUBDIRS = common \
          $(ACQUIREIMAGESDIR) \
          $(BATCHPROCESSIMAGESDIR) \
          $(CALENDARDIR) \
          $(CDARCHIVINGDIR) \
          $(FINDIMAGESDIR) \
          $(FLICKREXPORTDIR) \
          $(GALLERYEXPORTDIR) \
          $(GPSSYNCDIR) \
          $(HTMLEXPORTDIR) \
          $(IMAGEVIEWERDIR) \
          $(IPODEXPORTDIR) \
          $(JPEGLOSSLESSDIR) \
          $(KAMERAKLIENTDIR) \
          $(METADATAEDITDIR) \
          $(MPEGENCODERDIR) \
          $(PICASAWEBEXPORTDIR) \
          $(PRINTWIZARDDIR) \
          $(RAWCONVERTERDIR) \
          $(SENDIMAGESDIR) \
          $(SIMPLEVIEWEREXPORTDIR) \
          $(SLIDESHOWDIR) \
          $(TIMEADJUSTDIR) \
          $(WALLPAPERDIR)

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

