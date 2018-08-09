include ./Makefile.inc

COMMAND = $(ARRU)

CXXFLAGS_EXTERN = $(CXXFLAGS_SO) -DMYSQLPP_MYSQL_HEADERS_BURIED
#LINKFLAGS_EXTERN = $(LINKFLAGS_SO)

INCLUDE =
LIBRARY =

TARGET   = libtinymysql.a
TARGET_R = libtinymysql.ra

SRC_FILES = $(wildcard *.cpp)

include ./Makefile.rules

install :
	mkdir -p /usr/local/include/libtinymysql
	cp *.h /usr/local/include/libtinymysql/
	cp *.a /usr/local/lib/
	cp *.ra /usr/local/lib/