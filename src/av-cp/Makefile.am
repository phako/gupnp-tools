if UNINSTALLED
shareddir = $(abs_top_builddir)/src
else
shareddir = $(datadir)/gupnp-tools
endif

AM_CFLAGS = $(LIBGUPNP_CFLAGS) $(LIBGLADE_CFLAGS) -I$(top_srcdir) \
	    -DDATA_DIR='"$(shareddir)"' -I../common

bin_PROGRAMS = gupnp-av-cp

gupnp_av_cp_SOURCES = av-cp.c \
		      av-cp-gui.c \
		      av-cp-playlisttreeview.c \
		      av-cp-renderercombo.c \
		      av-cp.h \
		      av-cp-gui.h \
		      av-cp-playlisttreeview.h \
		      av-cp-renderercombo.h

gupnp_av_cp_LDADD = $(LIBGLADE_LIBS) $(LIBGUPNP_LIBS) \
		    ../common/libicons.a
gupnp_av_cp_LDFLAGS = -export-dynamic

MAINTAINERCLEANFILES = Makefile.in