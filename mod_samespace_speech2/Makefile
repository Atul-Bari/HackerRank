switch_builddir=/usr/local/src/freeswitch
switch_srcdir=/usr/local/src/freeswitch

build=x86_64-pc-linux-gnu
host=x86_64-pc-linux-gnu
target=$(host)

prefix=/opt/samespace/media/freeswitch
exec_prefix=/opt/samespace/media/freeswitch
libdir=/opt/samespace/media/freeswitch/lib
libexecdir=${exec_prefix}/libexec
includedir=/opt/samespace/media/freeswitch/include/freeswitch
modulesdir=/opt/samespace/media/freeswitch/lib/freeswitch/mod
runtimedir=/opt/samespace/media/freeswitch/var/run/freeswitch
logfiledir=/opt/samespace/media/freeswitch/var/log/freeswitch
confdir = /opt/samespace/media/freeswitch/conf/freeswitch
sysconfdir=/opt/samespace/media/freeswitch/conf
localstatedir=/opt/samespace/media/freeswitch/var/lib/freeswitch
grammardir=/opt/samespace/media/freeswitch/share/freeswitch/grammar

CC=gcc
CXX=g++
AWK=gawk
SHELL=/bin/bash
INSTALL=/usr/bin/install -c
GETLIB=cd /usr/local/src/freeswitch/libs && /bin/bash /usr/local/src/freeswitch/build/getlib.sh
LIBTOOL=$(SHELL) $(switch_builddir)/libtool
AR=ar
LTINSTALL=$(LIBTOOL) --quiet --mode=install $(INSTALL) 
LTUNINSTALL=$(LIBTOOL) --mode=uninstall rm -f 
CCLD = $(CC)
CXXLD = $(CXX)

OPENSSL_LIBS=-lssl -lcrypto
OPENSSL_CFLAGS= -DHAVE_OPENSSL

LIBS=$(switch_builddir)/libfreeswitch.la
DEFS=-DHAVE_CONFIG_H

DYLD_LIBRARY_PATH=/opt/samespace/media/freeswitch/lib:$DYLD_LIBRARY_PATH
LD_LIBRARY_PATH=/opt/samespace/media/freeswitch/lib:$LD_LIBRARY_PATH
OSARCH=`uname -s`
LIBTOOL_LIB_EXTEN = la
SOLINK = -shared -Xlinker -x

CFLAGS=-g -O2
CXXFLAGS=-g -O2
LDFLAGS= -lm  $(OUR_LDFLAGS)

#LOCAL_OBJS= say.pb.o say.grpc.pb.o Grpc_client.o

ALL_CFLAGS   = $(LOCAL_CFLAGS) $(MOD_CFLAGS) -I.  -I/usr/local/src/freeswitch/src/include -I/usr/local/src/freeswitch/src/include -I/usr/local/src/freeswitch/libs/libteletone/src -fPIC -ffast-math -lgreet1 -Wno-unused-result -Wno-misleading-indentation -fvisibility=hidden -DSWITCH_API_VISIBILITY=1 -DCJSON_API_VISIBILITY=1 -DHAVE_VISIBILITY=1 -g -ggdb -DHAVE_OPENSSL -g -O2  -Wall -std=c99 -pedantic -Wdeclaration-after-statement -lc -lspeechMod2 -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions  -Wl,--hash-style=both -O0  -D_GNU_SOURCE $(OUR_CFLAGS)

ALL_CXXFLAGS = $(LOCAL_CFLAGS) $(MOD_CFLAGS) -I/usr/local/src/freeswitch/src/include -I/usr/local/src/freeswitch/src/include -I/usr/local/src/freeswitch/libs/libteletone/src -fPIC -ffast-math -fvisibility=hidden -DSWITCH_API_VISIBILITY=1 -DCJSON_API_VISIBILITY=1 -DHAVE_VISIBILITY=1 -g -O2 -D_GNU_SOURCE $(OUR_CFLAGS)

DEFAULT_VARS:= CFLAGS="$(CFLAGS)" CPPFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" CC="$(CC)" CXX="$(CXX)"
DEFAULT_ARGS:= --build=$(build) --host=$(host) --target=$(target) \
	--prefix="$(prefix)" --exec_prefix="$(exec_prefix)" --libdir="$(libdir)" \
	--disable-shared --with-pic

COMPILE = $(CC) $(ALL_CFLAGS) $(DEFS)
LTCOMPILE = $(LIBTOOL) --mode=compile --tag=CC $(COMPILE)
LINK = $(LIBTOOL) --mode=link --tag=CC $(CCLD) $(ALL_CFLAGS) $(LDFLAGS) -shared -module -avoid-version -rpath $(modulesdir) -o $@

CXXCOMPILE = $(CXX) $(ALL_CXXFLAGS) $(DEFS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile --tag=CXX $(CXXCOMPILE)
CXXLINK = $(LIBTOOL) --mode=link --tag=CXX $(CXXLD) $(ALL_CXXFLAGS) $(LDFLAGS) -shared -module -avoid-version -rpath $(modulesdir) -o $@

CSOURCEFILE=$(MODNAME).c
CXXSOURCEFILE=$(MODNAME).cpp
TOUCH_TARGET=if test -f "$@";then touch "$@";fi;

#MAKE_OPTS = `if test -z "$(VERBOSE)" ; then $(MAKE) --help | grep silent | grep "\-s" 2>&1 >/dev/null && echo -s;fi`
MAKE_OPTS = `test -n "$(VERBOSE)" || echo -s`
RECURSE_MODNAME=`pwd | sed -e 's|^.*/||'`
RECURSE_SOURCEFILE=`if test -f "$$modname.cpp" -o  -f "$(MODDIR)/$$modname.cpp"; then echo "$$modname.cpp"; else echo "$$modname.c"; fi`
RECURSE_OUR_CFLAGS=` $(LOCAL_INSERT_CFLAGS) `
RECURSE_OUR_LDFLAGS=` $(LOCAL_INSERT_LDFLAGS) `
RECURSE_MODDIR=`if test -z $(MODDIR); then pwd | sed -e 's|$(switch_builddir)|$(switch_srcdir)|'; else echo $(MODDIR); fi`
RECURSE_MAKE=+modname="$(RECURSE_MODNAME)" ; \
		sourcefile="$(RECURSE_SOURCEFILE)" ; \
		osarch=`uname -s` ; \
		ourcflags="$(RECURSE_OUR_CFLAGS)" ; \
		ourldflags="$(RECURSE_OUR_LDFLAGS)" ; \
		insertobjs="`$(LOCAL_INSERT_OBJS)`" ; \
		moddir="$(RECURSE_MODDIR)" ; \
		$(MAKE) $(MAKE_OPTS) MODNAME="$$modname" SOURCEFILE="$$sourcefile" MODDIR="$$moddir" \
			OUR_CFLAGS="$$ourcflags" OUR_LDFLAGS="$$ourldflags" OUR_OBJS="$$insertobjs"

all: Makefile
	@$(RECURSE_MAKE) all-modules || exit 1
depend: Makefile
	@$(RECURSE_MAKE) depend-modules || exit 1
clean: Makefile
	@$(RECURSE_MAKE) clean-modules || exit 1
install: Makefile
	@$(RECURSE_MAKE) install-modules || exit 1
uninstall: Makefile
	@$(RECURSE_MAKE) uninstall-modules || exit 1
distclean: Makefile
	@$(RECURSE_MAKE) distclean-modules || exit 1
extraclean: Makefile
	@$(RECURSE_MAKE) extraclean-modules || exit 1

all-modules: local_depend $(MODNAME).$(LIBTOOL_LIB_EXTEN) local_all Makefile
depend-modules: local_depend
clean-modules: local_clean mod_clean
install-modules: all-modules depend_install local_install mod_install
uninstall-modules: local_uninstall mod_uninstall
distclean-modules: clean-modules local_distclean
extraclean-modules: distclean local_extraclean

Makefile:
	@if test ! -f $@; then \
	  if ! pwd | grep $(switch_builddir)/src/mod ; then rulesdir=$(switch_builddir) ; else rulesdir=../../../.. ; fi ; \
	  echo BASE=$$rulesdir > Makefile ; \
	  echo 'include $$(BASE)/build/modmake.rules' >> Makefile ; \
	fi ;

LINK_OUTPUT_REDIR=> .libs/$(MODNAME).log || error="yes" ; \
			if test -n "$(VERBOSE)" -o "$$error" = "yes"; then \
			  cat .libs/$(MODNAME).log ; \
			fi ;\
			if test "$$error" = "yes"; then \
			  exit 1 ;\
			 fi 

$(MODNAME).$(LIBTOOL_LIB_EXTEN): $(LIBS) $(LOCAL_LIBADD) $(LOCAL_OBJS) $(OUR_OBJS) $(MODDIR)/$(SOURCEFILE) $(MODNAME).lo 
	@echo Creating $@...
	@test -d .libs || mkdir .libs
	@error="";\
	if test -f "$(CSOURCEFILE)" -o -f "$(MODDIR)/$(CSOURCEFILE)"; then \
		$(LINK) $(SOLINK) $(MODNAME).lo $(LIBS) $(LOCAL_LDFLAGS) $(LOCAL_OBJS) $(OUR_OBJS) $(LOCAL_LIBADD) $(LOCAL_LDFLAGS_POST) $(LINK_OUTPUT_REDIR) ;\
	else \
		$(CXXLINK) $(SOLINK) $(MODNAME).lo $(LIBS) $(LOCAL_LDFLAGS) $(LOCAL_OBJS) $(OUR_OBJS) $(LOCAL_LIBADD) $(LOCAL_LDFLAGS_POST) $(LINK_OUTPUT_REDIR) ;\
	fi;

mod_clean:
	@rm -fr *.$(LIBTOOL_LIB_EXTEN) *.o *.lo *~ .libs $(LOCAL_OBJS)

mod_install: $(DESTDIR)$(modulesdir)/$(MODNAME).$(LIBTOOL_LIB_EXTEN)

$(DESTDIR)$(modulesdir)/$(MODNAME).$(LIBTOOL_LIB_EXTEN):$(MODNAME).$(LIBTOOL_LIB_EXTEN)
	@echo installing $<
	@$(LTINSTALL) $(MODNAME).$(LIBTOOL_LIB_EXTEN) $(DESTDIR)$(modulesdir) >/dev/null

mod_uninstall:
	@$(LTUNINSTALL) $(DESTDIR)$(modulesdir)/$(MODNAME).$(LIBTOOL_LIB_EXTEN)

# define these in your makefile if you wish
local_all local_depend local_clean depend_install local_install local_distclean local_extraclean local_uninstall:

#
# Implicit rules for creating outputs from input files
#
.SUFFIXES:
.SUFFIXES: .c .lo .o .cpp

.c.o:
	@echo Compiling $<...
	@if test ! -z $(VERBOSE) ; then echo $(COMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi
	@$(COMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< || exit 1

.c.lo:
	@echo Compiling $<...
	@if test ! -z $(VERBOSE) ; then echo $(LTCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi
	@$(LTCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< || exit 1

.cpp.o:
	@echo Compiling $<...
	@if test ! -z $(VERBOSE) ; then echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi
	@$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< || exit 1

.cpp.lo:
	@echo Compiling $<...
	@if test ! -z $(VERBOSE) ; then echo $(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi
	@$(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< || exit 1

$(MODNAME).o: $(MODDIR)/$(SOURCEFILE) \
  $(switch_srcdir)/src/include/switch.h \
  $(switch_builddir)/src/include/switch_am_config.h \
  $(switch_srcdir)/src/include/switch_types.h $(switch_srcdir)/src/include/switch_apr.h \
  $(switch_srcdir)/src/include/switch_core_db.h $(switch_srcdir)/src/include/switch_regex.h \
  $(switch_srcdir)/src/include/switch_core.h $(switch_srcdir)/src/include/switch_loadable_module.h \
  $(switch_srcdir)/src/include/switch_console.h $(switch_srcdir)/src/include/switch_utils.h \
  $(switch_srcdir)/src/include/switch_caller.h $(switch_srcdir)/src/include/switch_config.h \
  $(switch_srcdir)/src/include/switch_frame.h $(switch_srcdir)/src/include/switch_module_interfaces.h \
  $(switch_srcdir)/src/include/switch_channel.h $(switch_srcdir)/src/include/switch_buffer.h \
  $(switch_srcdir)/src/include/switch_event.h $(switch_srcdir)/src/include/switch_resample.h \
  $(switch_srcdir)/src/include/switch_ivr.h $(switch_srcdir)/src/include/switch_rtp.h \
  $(switch_srcdir)/src/include/switch_stun.h $(switch_srcdir)/src/include/switch_log.h \
  $(switch_srcdir)/src/include/switch_xml.h
	@echo Compiling $<...
	if test -f "$(CSOURCEFILE)" -o -f "$(MODDIR)/$(CSOURCEFILE)"; then \
		if test ! -z $(VERBOSE) ; then echo $(COMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi ;\
		$(COMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; \
	else \
		if test ! -z $(VERBOSE) ; then echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi ;\
		$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; \
	fi;

$(MODNAME).lo: $(MODDIR)/$(SOURCEFILE) \
  $(switch_srcdir)/src/include/switch.h \
  $(switch_builddir)/src/include/switch_am_config.h \
  $(switch_srcdir)/src/include/switch_types.h $(switch_srcdir)/src/include/switch_apr.h \
  $(switch_srcdir)/src/include/switch_core_db.h $(switch_srcdir)/src/include/switch_regex.h \
  $(switch_srcdir)/src/include/switch_core.h $(switch_srcdir)/src/include/switch_loadable_module.h \
  $(switch_srcdir)/src/include/switch_console.h $(switch_srcdir)/src/include/switch_utils.h \
  $(switch_srcdir)/src/include/switch_caller.h $(switch_srcdir)/src/include/switch_config.h \
  $(switch_srcdir)/src/include/switch_frame.h $(switch_srcdir)/src/include/switch_module_interfaces.h \
  $(switch_srcdir)/src/include/switch_channel.h $(switch_srcdir)/src/include/switch_buffer.h \
  $(switch_srcdir)/src/include/switch_event.h $(switch_srcdir)/src/include/switch_resample.h \
  $(switch_srcdir)/src/include/switch_ivr.h $(switch_srcdir)/src/include/switch_rtp.h \
  $(switch_srcdir)/src/include/switch_stun.h $(switch_srcdir)/src/include/switch_log.h \
  $(switch_srcdir)/src/include/switch_xml.h
	@echo Compiling $<...
	if test -f "$(CSOURCEFILE)" -o -f "$(MODDIR)/$(CSOURCEFILE)"; then \
		if test ! -z $(VERBOSE) ; then echo $(LTCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi ;\
		$(LTCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; \
	else \
		if test ! -z $(VERBOSE) ; then echo $(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; fi ;\
		$(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(MODDIR)/'`$< ; \
	fi;

$(switch_srcdir)/src/include/switch.h:

$(switch_builddir)/src/include/switch_am_config.h: $(switch_srcdir)/src/include/switch_am_config.h.in

$(switch_srcdir)/src/include/switch_types.h:

$(switch_srcdir)/src/include/switch_apr.h:

$(switch_srcdir)/src/include/switch_core_db.h:

$(switch_srcdir)/src/include/switch_regex.h:

$(switch_srcdir)/src/include/switch_core.h:

$(switch_srcdir)/src/include/switch_loadable_module.h:

$(switch_srcdir)/src/include/switch_console.h:

$(switch_srcdir)/src/include/switch_utils.h:

$(switch_srcdir)/src/include/switch_caller.h:

$(switch_srcdir)/src/include/switch_config.h:

$(switch_srcdir)/src/include/switch_frame.h:

$(switch_srcdir)/src/include/switch_module_interfaces.h:

$(switch_srcdir)/src/include/switch_channel.h:

$(switch_srcdir)/src/include/switch_buffer.h:

$(switch_srcdir)/src/include/switch_event.h:

$(switch_srcdir)/src/include/switch_resample.h:

$(switch_srcdir)/src/include/switch_ivr.h:

$(switch_srcdir)/src/include/switch_rtp.h:

$(switch_srcdir)/src/include/switch_stun.h:

$(switch_srcdir)/src/include/switch_log.h:

$(switch_srcdir)/src/include/switch_xml.h:


.PHONY: all clean depend install distclean extraclean \
	all-modules clean-modules depend-modules install-modules distclean-modules extraclean-modules \
	local_all local_clean local_depend local_install local_distclean local_extraclean \
	mod_clean mod_install mod_uninstall depend_install

