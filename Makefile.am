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

SUBDIRS = printwizard calendar mpegencoder cdarchiving imagesgallery \
	sendimages acquireimages batchprocessimages \
	timeadjust diroperations wallpaper \
	rawconverter findimages gammacalibration \
	$(SLIDESHOWDIR) $(KAMERAKLIENTDIR) $(JPEGLOSSLESSDIR)

