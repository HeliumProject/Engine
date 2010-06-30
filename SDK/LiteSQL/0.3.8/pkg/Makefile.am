bin_SCRIPTS = litesql-config

CLEANFILES = $(bin_SCRIPTS) litesql.pc

install-data-local: litesql.pc
	mkdir -p $(DESTDIR)/$(libdir)/pkgconfig && \
	cp -p litesql.pc $(DESTDIR)/$(libdir)/pkgconfig

uninstall-local:
	-rm -f $(DESTDIR)/$(libdir)/pkgconfig/litesql.pc

