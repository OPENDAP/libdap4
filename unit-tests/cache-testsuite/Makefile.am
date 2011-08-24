noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES = cleanup.sh

cleanup.sh: cleanup.sh.in ../../config.status
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > cleanup.sh
	chmod +x cleanup.sh

clean-local: cleanup.sh
	sh ./cleanup.sh
	rm -rf dods_cache
