EXTRA_DIST = cdalbums.ico ShellExecute.bat index.htm

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/kipi/data
	$(mkinstalldirs) $(kde_datadir)/kipi/data/autorun
	$(INSTALL_DATA) $(srcdir)/index.htm $(kde_datadir)/kipi/data/index.htm
	$(INSTALL_DATA) $(srcdir)/cdalbums.ico $(kde_datadir)/kipi/data/autorun/cdalbums.ico
	$(INSTALL_DATA) $(srcdir)/ShellExecute.bat $(kde_datadir)/kipi/data/autorun/ShellExecute.bat

uninstall-local:
	-rm -f $(kde_datadir)/kipi/data/index.htm
	-rm -f $(kde_datadir)/kipi/data/autorun/cdalbums.ico
	-rm -f $(kde_datadir)/kipi/data/autorun/ShellExecute.bat

