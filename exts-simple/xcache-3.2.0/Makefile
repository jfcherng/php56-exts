srcdir = /root/software/php/php5/xcache-3.2.0
builddir = /root/software/php/php5/xcache-3.2.0
top_srcdir = /root/software/php/php5/xcache-3.2.0
top_builddir = /root/software/php/php5/xcache-3.2.0
EGREP = /usr/bin/grep -E
SED = /usr/bin/sed
CONFIGURE_COMMAND = './configure' '--with-php-config=/usr/local/php56/bin/php-config'
CONFIGURE_OPTIONS = '--with-php-config=/usr/local/php56/bin/php-config'
SHLIB_SUFFIX_NAME = so
SHLIB_DL_SUFFIX_NAME = so
ZEND_EXT_TYPE = zend_extension
RE2C = exit 0;
AWK = gawk
XCACHE_ENABLE_TEST =
shared_objects_xcache = xcache.lo util/xc_stack.lo util/xc_trace.lo xcache/xc_allocator.lo xcache/xc_allocator_bestfit.lo xcache/xc_compatibility.lo xcache/xc_const_string.lo xcache/xc_extension.lo xcache/xc_ini.lo xcache/xc_mutex.lo xcache/xc_opcode_spec.lo xcache/xc_processor.lo xcache/xc_sandbox.lo xcache/xc_shm.lo xcache/xc_shm_mmap.lo xcache/xc_utils.lo mod_cacher/xc_cacher.lo
PHP_PECL_EXTENSION = xcache
XCACHE_AWK = LANG=C /usr/bin/gawk
M4 = /usr/bin/m4 -B 102400
XCACHE_BACKTICK = '`'
GREP = /usr/bin/grep
SED = /usr/bin/sed
XCACHE_INDENT = /usr/bin/indent -kr --use-tabs --tab-size 4 -sob -nce -l 160
XCACHE_PROC_SOURCES = /root/software/php/php5/xcache-3.2.0/processor/hashtable.m4 /root/software/php/php5/xcache-3.2.0/processor/head.m4 /root/software/php/php5/xcache-3.2.0/processor/main.m4 /root/software/php/php5/xcache-3.2.0/processor/process.m4 /root/software/php/php5/xcache-3.2.0/processor/processor.m4 /root/software/php/php5/xcache-3.2.0/processor/string.m4 /root/software/php/php5/xcache-3.2.0/processor/struct.m4
PHP_MODULES = $(phplibdir)/xcache.la
PHP_ZEND_EX =
all_targets = $(PHP_MODULES) $(PHP_ZEND_EX)
install_targets = install-modules install-headers
prefix = /usr/local/php56
exec_prefix = $(prefix)
libdir = ${exec_prefix}/lib
prefix = /usr/local/php56
phplibdir = /root/software/php/php5/xcache-3.2.0/modules
phpincludedir = /usr/local/php56/include/php
CC = cc
CFLAGS = -g -O2
CFLAGS_CLEAN = $(CFLAGS)
CPP = cc -E
CPPFLAGS = -DHAVE_CONFIG_H
CXX =
CXXFLAGS =
CXXFLAGS_CLEAN = $(CXXFLAGS)
EXTENSION_DIR = /usr/local/php56/lib/php/extensions/no-debug-non-zts-20131226
PHP_EXECUTABLE = /usr/local/php56/bin/php
EXTRA_LDFLAGS =
EXTRA_LIBS =
INCLUDES = -I/usr/local/php56/include/php -I/usr/local/php56/include/php/main -I/usr/local/php56/include/php/TSRM -I/usr/local/php56/include/php/Zend -I/usr/local/php56/include/php/ext -I/usr/local/php56/include/php/ext/date/lib
LFLAGS =
LDFLAGS =
SHARED_LIBTOOL =
LIBTOOL = $(SHELL) $(top_builddir)/libtool
SHELL = /bin/sh
INSTALL_HEADERS =
mkinstalldirs = $(top_srcdir)/build/shtool mkdir -p
INSTALL = $(top_srcdir)/build/shtool install -c
INSTALL_DATA = $(INSTALL) -m 644

DEFS = -DPHP_ATOM_INC -I$(top_builddir)/include -I$(top_builddir)/main -I$(top_srcdir)
COMMON_FLAGS = $(DEFS) $(INCLUDES) $(EXTRA_INCLUDES) $(CPPFLAGS) $(PHP_FRAMEWORKPATH)

all: $(all_targets) 
	@echo
	@echo "Build complete."
	@echo "Don't forget to run 'make test'."
	@echo

build-modules: $(PHP_MODULES) $(PHP_ZEND_EX)

build-binaries: $(PHP_BINARIES)

libphp$(PHP_MAJOR_VERSION).la: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(EXTRA_CFLAGS) -rpath $(phptempdir) $(EXTRA_LDFLAGS) $(LDFLAGS) $(PHP_RPATHS) $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(EXTRA_LIBS) $(ZEND_EXTRA_LIBS) -o $@
	-@$(LIBTOOL) --silent --mode=install cp $@ $(phptempdir)/$@ >/dev/null 2>&1

libs/libphp$(PHP_MAJOR_VERSION).bundle: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
	$(CC) $(MH_BUNDLE_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS) $(PHP_GLOBAL_OBJS:.lo=.o) $(PHP_SAPI_OBJS:.lo=.o) $(PHP_FRAMEWORKS) $(EXTRA_LIBS) $(ZEND_EXTRA_LIBS) -o $@ && cp $@ libs/libphp$(PHP_MAJOR_VERSION).so

install: $(all_targets) $(install_targets)

install-sapi: $(OVERALL_TARGET)
	@echo "Installing PHP SAPI module:       $(PHP_SAPI)"
	-@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	-@if test ! -r $(phptempdir)/libphp$(PHP_MAJOR_VERSION).$(SHLIB_DL_SUFFIX_NAME); then \
		for i in 0.0.0 0.0 0; do \
			if test -r $(phptempdir)/libphp$(PHP_MAJOR_VERSION).$(SHLIB_DL_SUFFIX_NAME).$$i; then \
				$(LN_S) $(phptempdir)/libphp$(PHP_MAJOR_VERSION).$(SHLIB_DL_SUFFIX_NAME).$$i $(phptempdir)/libphp$(PHP_MAJOR_VERSION).$(SHLIB_DL_SUFFIX_NAME); \
				break; \
			fi; \
		done; \
	fi
	@$(INSTALL_IT)

install-binaries: build-binaries $(install_binary_targets)

install-modules: build-modules
	@test -d modules && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(EXTENSION_DIR)
	@echo "Installing shared extensions:     $(INSTALL_ROOT)$(EXTENSION_DIR)/"
	@rm -f modules/*.la >/dev/null 2>&1
	@$(INSTALL) modules/* $(INSTALL_ROOT)$(EXTENSION_DIR)

install-headers:
	-@if test "$(INSTALL_HEADERS)"; then \
		for i in `echo $(INSTALL_HEADERS)`; do \
			i=`$(top_srcdir)/build/shtool path -d $$i`; \
			paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
		done; \
		$(mkinstalldirs) $$paths && \
		echo "Installing header files:           $(INSTALL_ROOT)$(phpincludedir)/" && \
		for i in `echo $(INSTALL_HEADERS)`; do \
			if test "$(PHP_PECL_EXTENSION)"; then \
				src=`echo $$i | $(SED) -e "s#ext/$(PHP_PECL_EXTENSION)/##g"`; \
			else \
				src=$$i; \
			fi; \
			if test -f "$(top_srcdir)/$$src"; then \
				$(INSTALL_DATA) $(top_srcdir)/$$src $(INSTALL_ROOT)$(phpincludedir)/$$i; \
			elif test -f "$(top_builddir)/$$src"; then \
				$(INSTALL_DATA) $(top_builddir)/$$src $(INSTALL_ROOT)$(phpincludedir)/$$i; \
			else \
				(cd $(top_srcdir)/$$src && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
				cd $(top_builddir)/$$src && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
			fi \
		done; \
	fi

PHP_TEST_SETTINGS = -d 'open_basedir=' -d 'output_buffering=0' -d 'memory_limit=-1'
PHP_TEST_SHARED_EXTENSIONS =  ` \
	if test "x$(PHP_MODULES)" != "x"; then \
		for i in $(PHP_MODULES)""; do \
			. $$i; $(top_srcdir)/build/shtool echo -n -- " -d extension=$$dlname"; \
		done; \
	fi; \
	if test "x$(PHP_ZEND_EX)" != "x"; then \
		for i in $(PHP_ZEND_EX)""; do \
			. $$i; $(top_srcdir)/build/shtool echo -n -- " -d $(ZEND_EXT_TYPE)=$(top_builddir)/modules/$$dlname"; \
		done; \
	fi`
PHP_DEPRECATED_DIRECTIVES_REGEX = '^(magic_quotes_(gpc|runtime|sybase)?|(zend_)?extension(_debug)?(_ts)?)[\t\ ]*='

test: all
	@if test ! -z "$(PHP_EXECUTABLE)" && test -x "$(PHP_EXECUTABLE)"; then \
		INI_FILE=`$(PHP_EXECUTABLE) -d 'display_errors=stderr' -r 'echo php_ini_loaded_file();' 2> /dev/null`; \
		if test "$$INI_FILE"; then \
			$(EGREP) -h -v $(PHP_DEPRECATED_DIRECTIVES_REGEX) "$$INI_FILE" > $(top_builddir)/tmp-php.ini; \
		else \
			echo > $(top_builddir)/tmp-php.ini; \
		fi; \
		INI_SCANNED_PATH=`$(PHP_EXECUTABLE) -d 'display_errors=stderr' -r '$$a = explode(",\n", trim(php_ini_scanned_files())); echo $$a[0];' 2> /dev/null`; \
		if test "$$INI_SCANNED_PATH"; then \
			INI_SCANNED_PATH=`$(top_srcdir)/build/shtool path -d $$INI_SCANNED_PATH`; \
			$(EGREP) -h -v $(PHP_DEPRECATED_DIRECTIVES_REGEX) "$$INI_SCANNED_PATH"/*.ini >> $(top_builddir)/tmp-php.ini; \
		fi; \
		TEST_PHP_EXECUTABLE=$(PHP_EXECUTABLE) \
		TEST_PHP_SRCDIR=$(top_srcdir) \
		CC="$(CC)" \
			$(PHP_EXECUTABLE) -n -c $(top_builddir)/tmp-php.ini $(PHP_TEST_SETTINGS) $(top_srcdir)/run-tests.php -n -c $(top_builddir)/tmp-php.ini -d extension_dir=$(top_builddir)/modules/ $(PHP_TEST_SHARED_EXTENSIONS) $(TESTS); \
		TEST_RESULT_EXIT_CODE=$$?; \
		rm $(top_builddir)/tmp-php.ini; \
		exit $$TEST_RESULT_EXIT_CODE; \
	else \
		echo "ERROR: Cannot run tests without CLI sapi."; \
	fi

clean:
	find . -name \*.gcno -o -name \*.gcda | xargs rm -f
	find . -name \*.lo -o -name \*.o | xargs rm -f
	find . -name \*.la -o -name \*.a | xargs rm -f 
	find . -name \*.so | xargs rm -f
	find . -name .libs -a -type d|xargs rm -rf
	rm -f libphp$(PHP_MAJOR_VERSION).la $(SAPI_CLI_PATH) $(SAPI_CGI_PATH) $(SAPI_MILTER_PATH) $(SAPI_LITESPEED_PATH) $(SAPI_FPM_PATH) $(OVERALL_TARGET) modules/* libs/*

distclean: clean
	rm -f Makefile config.cache config.log config.status Makefile.objects Makefile.fragments libtool main/php_config.h main/internal_functions_cli.c main/internal_functions.c stamp-h sapi/apache/libphp$(PHP_MAJOR_VERSION).module sapi/apache_hooks/libphp$(PHP_MAJOR_VERSION).module buildmk.stamp Zend/zend_dtrace_gen.h Zend/zend_dtrace_gen.h.bak Zend/zend_config.h TSRM/tsrm_config.h
	rm -f php5.spec main/build-defs.h scripts/phpize
	rm -f ext/date/lib/timelib_config.h ext/mbstring/oniguruma/config.h ext/mbstring/libmbfl/config.h ext/oci8/oci8_dtrace_gen.h ext/oci8/oci8_dtrace_gen.h.bak
	rm -f scripts/man1/phpize.1 scripts/php-config scripts/man1/php-config.1 sapi/cli/php.1 sapi/cgi/php-cgi.1 ext/phar/phar.1 ext/phar/phar.phar.1
	rm -f sapi/fpm/php-fpm.conf sapi/fpm/init.d.php-fpm sapi/fpm/php-fpm.service sapi/fpm/php-fpm.8 sapi/fpm/status.html
	rm -f ext/iconv/php_have_bsd_iconv.h ext/iconv/php_have_glibc_iconv.h ext/iconv/php_have_ibm_iconv.h ext/iconv/php_have_iconv.h ext/iconv/php_have_libiconv.h ext/iconv/php_iconv_aliased_libiconv.h ext/iconv/php_iconv_supports_errno.h ext/iconv/php_php_iconv_h_path.h ext/iconv/php_php_iconv_impl.h
	rm -f ext/phar/phar.phar ext/phar/phar.php
	if test "$(srcdir)" != "$(builddir)"; then \
	  rm -f ext/phar/phar/phar.inc; \
	fi
	$(EGREP) define'.*include/php' $(top_srcdir)/configure | $(SED) 's/.*>//'|xargs rm -f

.PHONY: all clean install distclean test
.NOEXPORT:
XCACHE_PROC_SRC=/root/software/php/php5/xcache-3.2.0/processor/main.m4
XCACHE_PROC_OUT=./processor.out.c
XCACHE_PROC_C=./xc_processor.c.h
XCACHE_PROC_H=./xc_processor.h
XCACHE_INCLUDES_SRC=/root/software/php/php5/xcache-3.2.0/includes.c
XCACHE_INCLUDES_I=./includes.i
XCACHE_STRUCTINFO_OUT=./structinfo.m4

$(XCACHE_INCLUDES_I):
	$(CC) -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) -E $(XCACHE_INCLUDES_SRC) -o $(XCACHE_INCLUDES_I)

$(XCACHE_STRUCTINFO_OUT): $(XCACHE_INCLUDES_I) /root/software/php/php5/xcache-3.2.0/gen_structinfo.awk
	@echo $(XCACHE_STRUCTINFO_OUT) is optional if XCache test is not enabled, feel free if it awk failed to produce it
	-$(XCACHE_AWK) -f /root/software/php/php5/xcache-3.2.0/gen_structinfo.awk < $(XCACHE_INCLUDES_I) > $(XCACHE_STRUCTINFO_OUT).tmp && mv $(XCACHE_STRUCTINFO_OUT).tmp $(XCACHE_STRUCTINFO_OUT)

$(XCACHE_PROC_OUT): $(XCACHE_PROC_SRC) $(XCACHE_STRUCTINFO_OUT) $(XCACHE_PROC_SOURCES)
	$(M4) -D srcdir=$(XCACHE_BACKTICK)"/root/software/php/php5/xcache-3.2.0'" -D builddir=$(XCACHE_BACKTICK)".'" $(XCACHE_ENABLE_TEST) $(XCACHE_PROC_SRC) > $(XCACHE_PROC_OUT).tmp
	mv $(XCACHE_PROC_OUT).tmp $(XCACHE_PROC_OUT)

$(XCACHE_PROC_H): $(XCACHE_PROC_OUT)
	$(GREP) "export: " $(XCACHE_PROC_OUT) | $(SED) "s/.*export:\(.*\):export.*/\1/g" > $(XCACHE_PROC_H)
	-$(XCACHE_INDENT) < $(XCACHE_PROC_H) > $(XCACHE_PROC_H).tmp && mv $(XCACHE_PROC_H).tmp $(XCACHE_PROC_H)

$(XCACHE_PROC_C): $(XCACHE_PROC_OUT) $(XCACHE_PROC_H)
	cp $(XCACHE_PROC_OUT) $(XCACHE_PROC_C)
	-$(XCACHE_INDENT) < $(XCACHE_PROC_OUT) > $(XCACHE_PROC_C).tmp && mv $(XCACHE_PROC_C).tmp $(XCACHE_PROC_C)

xcachesvnclean: clean
	-svn propget svn:ignore . > .svnignore.tmp 2>/dev/null && mv .svnignore.tmp .svnignore
	cat .svnignore | grep -v devel | grep -v svnignore | grep -v ^Makefile | grep -v ^config.nice | xargs rm -rf

xcachetest: all
	$(SED) "s#\\./\\.libs/#$(top_builddir)/\\.libs/#" < /root/software/php/php5/xcache-3.2.0/xcache-test.ini > $(top_builddir)/tmp-php.ini
	if test -z "$(TESTS)"; then \
		TEST_PHP_SRCDIR=/root/software/php/php5/xcache-3.2.0 /root/software/php/php5/xcache-3.2.0/run-xcachetest $(TEST_ARGS) -c $(top_builddir)/tmp-php.ini; \
	fi
	/root/software/php/php5/xcache-3.2.0/run-xcachetest $(TESTS) $(TEST_ARGS) -c $(top_builddir)/tmp-php.ini
$(XCACHE_INCLUDES_I) includes.lo ./includes.lo: /root/software/php/php5/xcache-3.2.0/mod_cacher/xc_cache.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h
mod_assembler/xc_assembler.lo ./mod_assembler/xc_assembler.lo:
mod_cacher/xc_cacher.lo ./mod_cacher/xc_cacher.lo: /root/software/php/php5/xcache-3.2.0/mod_cacher/xc_cache.h /root/software/php/php5/xcache-3.2.0/mod_cacher/xc_cacher.h /root/software/php/php5/xcache-3.2.0/util/xc_align.h /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/util/xc_vector.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.h /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_sandbox.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h $(XCACHE_PROC_H)
mod_coverager/xc_coverager.lo ./mod_coverager/xc_coverager.lo: /root/software/php/php5/xcache-3.2.0/mod_coverager/xc_coverager.h /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.h /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h
mod_decoder/xc_decoder.lo ./mod_decoder/xc_decoder.lo:
mod_disassembler/xc_disassembler.lo ./mod_disassembler/xc_disassembler.lo: /root/software/php/php5/xcache-3.2.0/mod_disassembler/xc_disassembler.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_sandbox.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h $(XCACHE_PROC_H)
mod_encoder/xc_encoder.lo ./mod_encoder/xc_encoder.lo:
mod_optimizer/xc_optimizer.lo ./mod_optimizer/xc_optimizer.lo: /root/software/php/php5/xcache-3.2.0/mod_optimizer/xc_optimizer.h /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.h /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.h /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h $(XCACHE_PROC_H)
util/xc_stack.lo ./util/xc_stack.lo: /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h
util/xc_trace.lo ./util/xc_trace.lo: /root/software/php/php5/xcache-3.2.0/util/xc_trace.h
xcache.lo ./xcache.lo: /root/software/php/php5/xcache-3.2.0/util/xc_foreachcoresig.h /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.h /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.h /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h $(XCACHE_PROC_H)
xcache/xc_allocator_bestfit.lo ./xcache/xc_allocator_bestfit.lo: /root/software/php/php5/xcache-3.2.0/util/xc_align.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_allocator.lo ./xcache/xc_allocator.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_compatibility.lo ./xcache/xc_compatibility.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h
xcache/xc_const_string.lo ./xcache/xc_const_string.lo: /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php4.x.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.0.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.1.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.2.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.3.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.4.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php5.5.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string_opcodes_php6.x.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_extension.lo ./xcache/xc_extension.lo: /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_ini.lo ./xcache/xc_ini.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.h
xcache/xc_malloc.lo ./xcache/xc_malloc.lo: /root/software/php/php5/xcache-3.2.0/util/xc_align.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_mutex.lo ./xcache/xc_mutex.lo: /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_opcode_spec.lo ./xcache/xc_opcode_spec.lo: /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec_def.h /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_processor.lo ./xcache/xc_processor.lo: $(XCACHE_PROC_C)
xcache/xc_sandbox.lo ./xcache/xc_sandbox.lo: /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_sandbox.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h
xcache/xc_shm.lo ./xcache/xc_shm.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h
xcache/xc_shm_mmap.lo ./xcache/xc_shm_mmap.lo: /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h
xcache/xc_utils.lo ./xcache/xc_utils.lo: /root/software/php/php5/xcache-3.2.0/util/xc_stack.h /root/software/php/php5/xcache-3.2.0/util/xc_trace.h /root/software/php/php5/xcache-3.2.0/xcache_globals.h /root/software/php/php5/xcache-3.2.0/xcache.h /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.h /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.h /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.h /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.h /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.h
xcache.lo: /root/software/php/php5/xcache-3.2.0/xcache.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache.c -o xcache.lo 
util/xc_stack.lo: /root/software/php/php5/xcache-3.2.0/util/xc_stack.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/util/xc_stack.c -o util/xc_stack.lo 
util/xc_trace.lo: /root/software/php/php5/xcache-3.2.0/util/xc_trace.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/util/xc_trace.c -o util/xc_trace.lo 
xcache/xc_allocator.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator.c -o xcache/xc_allocator.lo 
xcache/xc_allocator_bestfit.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator_bestfit.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_allocator_bestfit.c -o xcache/xc_allocator_bestfit.lo 
xcache/xc_compatibility.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_compatibility.c -o xcache/xc_compatibility.lo 
xcache/xc_const_string.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_const_string.c -o xcache/xc_const_string.lo 
xcache/xc_extension.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_extension.c -o xcache/xc_extension.lo 
xcache/xc_ini.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_ini.c -o xcache/xc_ini.lo 
xcache/xc_mutex.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_mutex.c -o xcache/xc_mutex.lo 
xcache/xc_opcode_spec.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_opcode_spec.c -o xcache/xc_opcode_spec.lo 
xcache/xc_processor.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_processor.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_processor.c -o xcache/xc_processor.lo 
xcache/xc_sandbox.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_sandbox.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_sandbox.c -o xcache/xc_sandbox.lo 
xcache/xc_shm.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_shm.c -o xcache/xc_shm.lo 
xcache/xc_shm_mmap.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_shm_mmap.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_shm_mmap.c -o xcache/xc_shm_mmap.lo 
xcache/xc_utils.lo: /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/xcache/xc_utils.c -o xcache/xc_utils.lo 
mod_cacher/xc_cacher.lo: /root/software/php/php5/xcache-3.2.0/mod_cacher/xc_cacher.c
	$(LIBTOOL) --mode=compile $(CC)  -I. -I/root/software/php/php5/xcache-3.2.0 $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)  -c /root/software/php/php5/xcache-3.2.0/mod_cacher/xc_cacher.c -o mod_cacher/xc_cacher.lo 
$(phplibdir)/xcache.la: ./xcache.la
	$(LIBTOOL) --mode=install cp ./xcache.la $(phplibdir)

./xcache.la: $(shared_objects_xcache) $(XCACHE_SHARED_DEPENDENCIES)
	$(LIBTOOL) --mode=link $(CC) $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ -export-dynamic -avoid-version -prefer-pic -module -rpath $(phplibdir) $(EXTRA_LDFLAGS) $(shared_objects_xcache) $(XCACHE_SHARED_LIBADD)

