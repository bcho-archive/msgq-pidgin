# command-notification Makefile
# Simo Mattila <simo.h.mattila at gmail dot com>

GTK_PIDGIN_INCLUDES= `pkg-config --cflags gtk+-2.0 pidgin`

GTK_PREFIX=/usr/local
GTK_PREFIX2=/usr

CFLAGS= -O2 -Wall -fpic -g
LDFLAGS= -shared

INCLUDES = \
      -I$(GTK_PREFIX)/include \
      -I$(GTK_PREFIX)/include/gtk-2.0 \
      -I$(GTK_PREFIX)/include/glib-2.0 \
      -I$(GTK_PREFIX)/include/pango-1.0 \
      -I$(GTK_PREFIX)/include/atk-1.0 \
      -I$(GTK_PREFIX)/lib/glib-2.0/include \
      -I$(GTK_PREFIX)/lib/gtk-2.0/include \
      -I$(GTK_PREFIX2)/include \
      -I$(GTK_PREFIX2)/include/gtk-2.0 \
      -I$(GTK_PREFIX2)/include/glib-2.0 \
      -I$(GTK_PREFIX2)/include/pango-1.0 \
      -I$(GTK_PREFIX2)/include/atk-1.0 \
      -I$(GTK_PREFIX2)/lib/glib-2.0/include \
      -I$(GTK_PREFIX2)/lib/gtk-2.0/include \
      $(GTK_PIDGIN_INCLUDES)

PLUGIN=msgq-pidgin

$(PLUGIN).so: $(PLUGIN).c
	$(CC) $(PLUGIN).c $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(PLUGIN).so

install: $(PLUGIN).so message.o
	cp $(PLUGIN).so ~/.purple/plugins/

message.o: message.c
	$(CC) message.c $(CFLAGS) $(INCLUDES) $(LDFLAGS) -c

uninstall:
	rm -f ~/.purple/plugins/$(PLUGIN).so

clean:
	rm -f $(PLUGIN).so
	rm -f *.o
