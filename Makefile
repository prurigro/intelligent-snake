FONT=DroidSans-Bold.ttf

EXE=isnake
CC=gcc
CFLAGS=`sdl-config --cflags --libs` -lSDL_ttf
DIR=bin

#x86_64-w64-mingw32-gcc -o IntelligentSnake-Windows/isnake.exe isnake.c `/usr/x86_64-w64-mingw32/bin/sdl-config --cflags --libs` -lSDL_ttf
WINEXE=$(EXE).exe
MINGW=x86_64-w64-mingw32
MINGWPATH=/usr/$(MINGW)
WINCC=$(MINGW)-$(CC)
WINCFLAGS=-I$(MINGWPATH)/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main -L$(MINGWPATH)/lib -lmingw32 -lSDLmain -lSDL -mwindows -lSDL_ttf
WINDIR=windows-$(DIR)

SRC=$(EXE).c

all: $(EXE)

$(EXE):
	install -d $(DIR)
	$(CC) $(CFLAGS) $(SRC)  $< -o $(DIR)/$@
	cp $(FONT) $(DIR)/

windows:
	install -d $(WINDIR)
	cp $(MINGWPATH)/bin/SDL.dll $(WINDIR)/
	cp $(MINGWPATH)/bin/SDL_ttf.dll $(WINDIR)/
	cp $(MINGWPATH)/bin/libfreetype-6.dll $(WINDIR)/
	cp $(MINGWPATH)/bin/zlib1.dll $(WINDIR)/
	cp $(FONT) $(WINDIR)/
	$(WINCC) -o $(WINDIR)/$(WINEXE) $(SRC) $(WINCFLAGS)

clean:
	rm -rf $(DIR) $(WINDIR)
