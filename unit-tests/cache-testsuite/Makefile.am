noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES = cleanup.sh

# Removed. This breaks automake 1.16. jhrg 11/23/21
# $(top_srcdir)/config.status

cleanup.sh: cleanup.sh.in 
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > $(builddir)/cleanup.sh
	chmod +x $(builddir)/cleanup.sh

clean-local: cleanup.sh
	sh $(builddir)/cleanup.sh
	rm -rf dods_cache
