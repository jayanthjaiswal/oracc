include ../oracc-doc-rules.txt

SUBDIRS = 00tools ns builder developer install manager user ood urn sysadmin
EXTRA_DIST = index.html

xinstall-data-local:
	cp index.html $(DESTDIR)$(prefix)/www/doc
	$(DESTDIR)/$(prefix)/bin/agg-project-list.sh
	chmod -R o+r $(htmldir)
	ORACC=$(DESTDIR)$(prefix) @abs_top_builddir@/doc/00tools/estindex.sh $(DESTDIR)$(prefix)/www/doc
