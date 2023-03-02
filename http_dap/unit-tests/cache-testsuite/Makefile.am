
AUTOMAKE_OPTIONS = foreign

noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES =

http_cache_test:
	cp -r @srcdir@/dods_cache_init http_cache_test

# The cleanup.sh script sets the directories here so the HTTPCache tests
# have a consistent environment. It is run by HTTPCacheTest before any
# of the tests are run.
cleanup.sh: cleanup.sh.in 
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > $(builddir)/cleanup.sh
	chmod +x $(builddir)/cleanup.sh

clean-local:
	if test -d http_cache_test; then chmod -R +w http_cache_test; fi
	-rm -rf gc_cache
	-rm -rf purge_cache
	-rm -rf header_cache
	-rm -rf singleton_cache
	-rm -rf http_cache_test
