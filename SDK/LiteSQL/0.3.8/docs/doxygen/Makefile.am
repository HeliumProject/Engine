DOXYGEN_EXTRA_SOURCES = changes.txt contributors.txt cost.txt footer.html future.txt implementation.txt license.txt main.txt mysql_notes.txt usage_cursor.txt usage_database_create_drop.txt usage_defining_database.txt usage_selecting_persistents.txt usage_upcasting.txt usage_update_delete_persistents.txt usage_using_database.txt usage_using_relations.txt
DOXYGEN_OUTPUT_SUBDIRS = @DOXYGEN_OUTPUT_SUBDIRS@
LITESQL_SHORTNAME = @PACKAGE_TARNAME@-@PACKAGE_VERSION@

EXTRA_DIST = $(DOXYGEN_EXTRA_SOURCES) $(DOXYGEN_OUTPUT_SUBDIRS)

if HAVE_DOXYGEN

DOXYGEN = @DOXYGEN@
BUILT_SOURCES = dummy0
CLEANFILES = dummy0

dummy0: doxygen.conf
	$(DOXYGEN) doxygen.conf && touch dummy0

dist-hook:
	find $(distdir)/$(DOXYGEN_OUTPUT_SUBDIRS) -name \.svn
	rm -rf `find $(distdir)/$(DOXYGEN_OUTPUT_SUBDIRS) -name .svn`

install-data-local: dummy0
	mkdir -p $(DESTDIR)$(datadir)/doc/$(LITESQL_SHORTNAME) && \
	for d in $(DOXYGEN_OUTPUT_SUBDIRS) ;\
	do \
	    cp -pr $$d $(DESTDIR)$(datadir)/doc/$(LITESQL_SHORTNAME) ;\
	done

uninstall-local:
	-rm -rf $(DESTDIR)$(datadir)/doc/$(LITESQL_SHORTNAME)

clean-local:
	-for d in $(DOXYGEN_OUTPUT_SUBDIRS) ;\
	do \
	    rm -rf $$d ;\
	done

endif

