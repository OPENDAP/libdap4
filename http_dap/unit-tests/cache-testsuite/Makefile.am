
AUTOMAKE_OPTIONS = foreign

noinst_SCRIPTS = cleanup.sh

EXTRA_DIST = cleanup.sh.in dodsrc dot.index dods_cache_init

DISTCLEANFILES =

# Remove this from DISTCLEANFILES because distclean targets may run in parallel
# with clean-local. Look in the Makefile at the clean targets to see why. jhrg 2/2/22
# cleanup.sh

cleanup.sh: cleanup.sh.in 
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > $(builddir)/cleanup.sh
	chmod +x $(builddir)/cleanup.sh

# Moved the cleanup od 'cleanup.sh' here to avoid issues with parallel
# runs while cleaning. See comment above about DISTCLEANFILES. jhrg 2/2/22
clean-local: cleanup.sh
	sh $(builddir)/cleanup.sh
	-rm -rf $(builddir)/cleanup.sh
	-rm -rf dods_cache
