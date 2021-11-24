noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES = cleanup.sh

cleanup.sh: cleanup.sh.in $(top_srcdir)/config.status
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > $(builddir)/cleanup.sh
	chmod +x cleanup.sh

clean-local: cleanup.sh
	sh $(builddir)/cleanup.sh
	rm -rf dods_cache
