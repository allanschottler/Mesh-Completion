# ------------------------------------------------
# Generic Makefile
#
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = mc

CC       = g++
# compiling flags here
CFLAGS   = -Wall -g -std=c++11

LINKER   = g++ -o
# linking flags here
LFLAGS   = -Wall -lm

#GTKFLAGS = -export-dynamic `pkg-config --cflags --libs gtk+-2.0 gtkglext-1.0 gtkglext-x11-1.0`

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

INCLUDE = -I/usr/local/include -I/home/p/libs/libsgtk_64/include/gtkglext-1.0 -I/home/p/libs/libsgtk_64/lib/gtkglext-1.0/include -I/home/p/libs/libsgtk_64/include/gtk-2.0 -I/home/p/libs/libsgtk_64/lib/gtk-2.0/include -I/home/p/libs/libsgtk_64/include/pango-1.0 -I/home/p/libs/libsgtk_64/include/gio-unix-2.0/ -I/home/p/libs/libsgtk_64/include -I/home/p/libs/libsgtk_64/include/gdk-pixbuf-2.0 -I/home/p/libs/libsgtk_64/include/cairo -I/home/p/libs/libsgtk_64/include/glib-2.0 -I/home/p/libs/libsgtk_64/lib/glib-2.0/include -I/home/p/libs/libsgtk_64/include/pixman-1 -I/home/p/libs/libsgtk_64/include/freetype2 -I/home/p/libs/libsgtk_64/include/libpng15 -I/home/p/libs/libsgtk_64/include/atk-1.0 -I/home/p/libs/libsgtk_64/include/gtk-2.0 -I/home/p/libs/libsgtk_64/lib/gtk-2.0/include -I/home/p/libs/libsgtk_64/include/atk-1.0 -I/home/p/libs/libsgtk_64/include/cairo -I/home/p/libs/libsgtk_64/include/gdk-pixbuf-2.0 -I/home/p/libs/libsgtk_64/include/pango-1.0 -I/home/p/libs/libsgtk_64/include/gio-unix-2.0/ -I/home/p/libs/libsgtk_64/include -I/home/p/libs/libsgtk_64/include/glib-2.0 -I/home/p/libs/libsgtk_64/lib/glib-2.0/include -I/home/p/libs/libsgtk_64/include/pixman-1 -I/home/p/libs/libsgtk_64/include/freetype2 -I/home/p/libs/libsgtk_64/include/libpng15 -I/home/p/libs/OSG3.2/include

#-I/usr/include/gtk-2.0 -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/freetype2 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/#x86_64-linux-gnu/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/libpng12 -I/usr/include/atk-1.0 -I/usr/include/harfbuzz -I/usr/include/gtkglext-1.0 -I/usr/lib/gtkglext-1.0/include


LIBSDIR = -L/usr/local/lib -L/home/p/libs/libsgtk_64/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lXi -lXinerama -lXext -latk-1.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lSM -lICE -lX11 -lgio-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lrt -lglib-2.0 -Wl,--export-dynamic -pthread -L/home/p/libs/libsgtk_64/lib -L/usr/lib64 -lgtkglext-x11-1.0 -lgdkglext-x11-1.0 -lGLU -lGL -lXmu -lXt -lgtk-x11-2.0 -lpangox-1.0 -lgdk-x11-2.0 -lXi -lXinerama -lXext -latk-1.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lSM -lICE -lX11 -lgio-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lrt -lglib-2.0 -L/home/p/libs/OSG3.2/lib/Linux26g4_64 -losg -losgUtil -losgGA -losgDB -losgText -losgViewer -losgSim -lOpenThreads -fopenmp -lOpenMeshCore -lOpenMeshTools

#-lgtkglext-x11-1.0 -lgdkglext-x11-1.0 -lGLU -lGL -lXmu -lXt -lSM -lICE -lgtk-x11-2.0 -lpangox-1.0 -lX11 -lgmodule-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfontconfi#g -lgobject-2.0 -lglib-2.0 -lfreetype -losg -losgUtil -losgGA -losgDB -losgText -losgViewer -losgSim -lOpenThreads -fopenmp



$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(INCLUDE) $(OBJECTS) $(LIBSDIR) 
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

