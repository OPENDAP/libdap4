
AUTOMAKE_OPTIONS = foreign

noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES =

cleanup.sh: cleanup.sh.in 
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > $(builddir)/cleanup.sh
	chmod +x $(builddir)/cleanup.sh

clean-local:
	if test -d dods_cache; then chmod -R +w dods_cache; fi
	-rm -rf dods_cache
	-rm -rf dods_cache
	-rm -rf gc_cache
	-rm -rf purge_cache
	-rm -rf header_cache
	-rm -rf singleton_cache
	-rm -rf http_cache_test
	-rm -rf http_mp_cache
	-rm -rf http_mt_cache
