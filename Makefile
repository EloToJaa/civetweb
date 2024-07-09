#
# Copyright (c) 2018, 2022 CivetWeb Developers
# Copyright (c) 2013 No Face Press, LLC
# License http://opensource.org/licenses/mit-license.php MIT License
#

#This makefile is used to test the other Makefiles


PROG = rest_server
SRC = main.c

TOP_CJSON = pkg/cJSON
CJSON_LIB = $(TOP_CJSON)/cJSON.c $(TOP_CJSON)/cJSON_Utils.c

TOP_CIVETWEB = pkg/civetweb
CIVETWEB_LIB = libcivetweb.a

CFLAGS = -I$(TOP_CIVETWEB)/include -I$(TOP_CJSON) $(COPT) -DNO_FILES -DMG_EXPERIMENTAL_INTERFACES
LIBS = -lpthread

include $(TOP_CIVETWEB)/resources/Makefile.in-os

ifeq ($(TARGET_OS),LINUX)
	LIBS += -ldl
endif

all: $(PROG)

$(PROG): $(CIVETWEB_LIB) $(CJSON_LIB) $(SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(SRC) $(CJSON_LIB) $(CIVETWEB_LIB) $(LIBS)

$(CIVETWEB_LIB):
	$(MAKE) -C $(TOP_CIVETWEB) WITH_IPV6=1 WITH_WEBSOCKET=1 COPT='-DNO_SSL -DMG_EXPERIMENTAL_INTERFACES' clean lib
	cp $(TOP_CIVETWEB)/$(CIVETWEB_LIB) .

clean:
	rm -f $(CIVETWEB_LIB) $(PROG)

.PHONY: all clean

