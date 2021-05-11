#
#    Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
#    Sergey Manoylo <info@smansoft.com>
#

.PHONY = all init clean bindir objdir
.DEFAULT_GOAL = all

ROOT=.

#SM_FILE_BUILD	= Release
#SM_FILE_BUILD	= Debug

CXX = g++
LD = g++

SSTRING	= sstring_lib-0.0.1.1
SM_LOG	= sm_log_lib-0.0.2.2

ifeq ($(SM_FILE_BUILD),Release)
	CXXFLAGS 	= -O3 -DNDEBUG -DSM_OS_LINUX -DSM_SYNC_LOG -Wall -fmessage-length=0 -std=c++17		# release flags
else
	CXXFLAGS 	= -O0 -g3 -ggdb -DDEBUG -DSM_OS_LINUX -DSM_SYNC_LOG -Wall -fmessage-length=0 -std=c++17	# debug flags
endif

LD_FLAGS  	= '-Wl,-L,$(ROOT)/$(SSTRING),-L$(ROOT)/$(SM_LOG)'

VPATH 		= $(ROOT)/src
OBJDIR 		= $(ROOT)/obj

BIN_OUT := $(ROOT)/_bin

INCLUDES 	= -I$(ROOT)/include -I$(SSTRING)/include -I$(SM_LOG)/include

SRCS 		= sm_main.cpp \
		sm_consumer_factory.cpp \
		sm_consumer_item.cpp \
		sm_exceptions.cpp \
		sm_producer_factory.cpp \
		sm_tools.cpp \
		sm_consumer_fin.cpp \
		sm_consumer_mid.cpp \
		sm_log_tool.cpp \
		sm_producer.cpp \
		sm_producer_item.cpp

OBJS 		= $(SRCS:.cpp=.o)

LIBS = -lpthread -l$(SM_LOG) -l$(SSTRING)

TARGET = sm_producer_consumer

define CXX17
    $(CXX) $(CXXFLAGS) $(DEFINES) -c $< -o $(OBJDIR)/$(@F) $(INCLUDES)
endef

$(TARGET): $(addprefix $(OBJDIR)/, $(OBJS))
	${LD} ${LD_FLAGS} -o $(TARGET) $(addprefix $(OBJDIR)/, $(OBJS)) $(LIBS)

$(OBJDIR)/%.o:%.cpp
	$(CXX17)

objdir :
	mkdir -p $(OBJDIR)

bindir :
	mkdir -p $(BIN_OUT)/bin
	mkdir -p $(BIN_OUT)/share
	mkdir -p $(BIN_OUT)/share/smansoft/doc

sstring_lib:
	$(MAKE) -C $(SSTRING)

sm_log_lib:
	$(MAKE) -C $(SM_LOG)

all: sstring_lib sm_log_lib objdir $(TARGET)

clean:
	rm -f $(addprefix $(OBJDIR)/, $(OBJS)) $(ROOT)/$(TARGET)
	rm -fr $(OBJDIR)
	$(MAKE) -C $(SM_LOG) clean 
	$(MAKE) -C $(SSTRING) clean 

install: bindir
	cp -f $(ROOT)/$(TARGET) ${BIN_OUT}/bin
	cp -f $(ROOT)/CHANGELOG $(BIN_OUT)/share/smansoft/doc
	cp -f $(ROOT)/HELP 	$(BIN_OUT)/share/smansoft/doc
	cp -f $(ROOT)/INSTALL 	$(BIN_OUT)/share/smansoft/doc
	cp -f $(ROOT)/LICENSE 	$(BIN_OUT)/share/smansoft/doc
	cp -f $(ROOT)/README 	$(BIN_OUT)/share/smansoft/doc
