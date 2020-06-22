include $(top_srcdir)/build/modmake.rulesam
MODNAME=mod_samespace.speech
LOCAL_OBJS=say.pb.o say.grpc.pb.o Grpc_client.o
if HAVE_SAMESPACE

mod_LTLIBRARIES = mod_samespace_speech.la
mod_samespace_la_SOURCES  = mod_samespace_speech.c
mod_samespace_la_CFLAGS   = $(AM_CFLAGS)
mod_samespace_la_LIBADD   = $(switch_builddir)/libfreeswitch.la
mod_samespace_la_LDFLAGS  = -avoid-version -module -no-undefined -shared

else
install: error
all: error
error:
	$(error You must install libcurl to build mod_samespace)
endif

