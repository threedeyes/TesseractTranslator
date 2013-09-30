
BINARY := ./build/TesseractTranslator

OBJS := ConfigView.o \
		TesseractTranslator.o \
		TesseractLanguages.o \
		TesseractApp.o \
		shared/TranslatorSettings.o \
		shared/TranslatorWindow.o \
		shared/BaseTranslator.o \
		shared/StreamBuffer.o

OBJDIR := build

RSRCS := ./res/TesseractTranslator.rsrc

OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

CC := g++
LD := $(CC)

LIBS := -lbe -ltranslation -lroot -lstdc++ -ltesseract -lnetwork
CFLAGS := -O3 -I./ -I./src/shared
LDFLAGS := 

.PHONY : clean build install

default : build

build : $(BINARY)
	
$(BINARY) : $(OBJDIR) $(OBJS) $(RSRCS)
	$(LD) $(CFLAGS) $(OBJS) -o $(BINARY) $(LDFLAGS) $(LIBS)
	xres -o $(BINARY) $(RSRCS)
	mimeset -f $(BINARY)

clean:
	rm -rf $(OBJDIR)

build/%.o : src/%.cpp
	@mkdir -p $(OBJDIR)/shared
	$(CC) $(CFLAGS) -c $< -o $@


install:
	mkdir -p /boot/home/config/add-ons/Translators
	cp $(BINARY) /boot/home/config/add-ons/Translators
