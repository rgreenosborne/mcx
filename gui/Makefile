VERSION = 0.7

UNAME_O = $(shell uname -o)
ifeq ($(UNAME_O), Msys)
  OS := win
  WX := ~/opt/wxWidgets-2.9.3
  MCX_RES := mcx.res
  DBGVIEW := Dbgview
  EXEEXT := .exe
  ZIP := /c/cygwin/bin/zip.exe
  WXLDFLAGS = -static
  MAKENSIS = "/c/Program Files (x86)/NSIS/makensis.exe"
else ifeq ($(UNAME_O), GNU/Linux)
  OS := linux
  WX := /usr
  EXTRA_CXXFLAGS := -fPIC
  ZIP := zip
else
  $(error "target $(UNAME_O)?")
endif

WXCONFIG := $(WX)/bin/wx-config
CXXFLAGS = $(shell $(WXCONFIG) --cxxflags) $(EXTRA_CXXFLAGS) -g -DVERSION=\"$(VERSION)\"
WXLIBS := $(shell $(WXCONFIG) --libs --debug=yes)
TARGET := $(OS)
WXCXX = $(shell $(WXCONFIG) --cxx)
WXLD = $(shell $(WXCONFIG) --ld)

all: mcx$(EXEEXT)

clean:
	 rm -f mcx$(EXEEXT) *.o mcx.res *.zip mcx_setup.exe

fb:
	wxformbuilder MCXx.fbp

mcxgui.o: mcxgui.cpp
	$(WXCXX) -c $(CXXFLAGS) -o $@ $<

main.o: main.cpp
	$(WXCXX) -c $(CXXFLAGS) -o $@ $<

crc16.o: crc16.cpp
	$(WXCXX) -c $(CXXFLAGS) -o $@ $<

mcxcomm.o: mcxcomm.cpp
	$(WXCXX) -c $(CXXFLAGS) -o $@ $<

mcxcomm_$(TARGET).o: mcxcomm_$(TARGET).cpp
	$(WXCXX) -c $(CXXFLAGS) -o $@ $<

mcx.res: mcx.rc mcx.exe.manifest
	windres --input $< --output $@ --output-format=coff

mcx$(EXEEXT): main.o mcxgui.o crc16.o mcxcomm.o mcxcomm_$(TARGET).o $(MCX_RES)
	$(WXLD) $@ $(WXLDFLAGS) $^ $(WXLIBS)

run: mcx$(EXEEXT)
	LD_LIBRARY_PATH=$(WX)/lib ./mcx$(EXEEXT)

libgcc_s_dw2-1.dll: /mingw/bin/libgcc_s_dw2-1.dll
	cp $^ $@

ifeq ($(OS), win)

mcx_setup.exe: mcx.nsi mcx$(EXEEXT) LICENSE.txt dso.mcx lunar.mcx planetary.mcx solar.mcx
	strip mcx$(EXEEXT)
	$(MAKENSIS) $<

mcx_setup-$(VERSION).exe: mcx_setup.exe
	cp $^ $@

endif

ifeq ($(OS), linux)

mcx-$(VERSION).tar.gz: mcx LICENSE.txt dso.mcx lunar.mcx planetary.mcx solar.mcx
	mkdir -p xxtmp/mcx-$(VERSION)
	cp $^ xxtmp/mcx-$(VERSION)
	strip xxtmp/mcx-$(VERSION)/mcx
	tar cfz mcx-$(VERSION).tar.gz -C xxtmp mcx-$(VERSION)
	rm -rf xxtmp

endif

zip: mcx$(EXEEXT) dso.mcx lunar.mcx planetary.mcx solar.mcx
	strip mcx$(EXEEXT)
	rm -f mcx-$(VERSION).zip
	$(ZIP) mcx-$(VERSION).zip $^

upload: mcx_setup-$(VERSION).exe
	scp ChangeLog mcx_setup-$(VERSION).exe $(DEST)
