APP=app
SRCDIR=src
OBJDIR=obj
SUFFIXES=.c .cpp .s
LIBS=m GL glut GLEW freetype

NODEBUGOPTS=-O2
DEBUGOPTS=-ggdb -DDEBUG

NOPROFILEOPTS=
PROFILEOPTS=-pg

$(shell [ -e .opts ] || echo -e 'DEBUG=0\nPROFILE=0' >.opts)

LASTDEBUG := $(shell cat .opts | grep 'DEBUG' | sed -E 's/.*=([01])/\1/')
LASTPROFILE := $(shell cat .opts | grep 'PROFILE' | sed -E 's/.*=([01])/\1/')
DEBUG ?= $(LASTDEBUG)
PROFILE ?= $(LASTPROFILE)

REBUILD:=
ifneq "$(DEBUG)" "$(LASTDEBUG)"
REBUILD := dummy
endif
ifneq "$(PROFILE)" "$(LASTPROFILE)"
REBUILD := dummy
endif

OPTS:=
ifeq "$(DEBUG)" "1"
OPTS += $(DEBUGOPTS)
else
OPTS += $(NODEBUGOPTS)
endif
ifeq "$(PROFILE)" "1"
OPTS += $(PROFILEOPTS)
else
OPTS += $(NOPROFILEOPTS)
endif


SUBDIRS=$(shell find $(SRCDIR) -type d | grep -v include)
INCLUDEDIRS=$(shell find $(SRCDIR) -type d -name include)
OBJSUBDIRS=$(subst $(SRCDIR),$(OBJDIR),$(SUBDIRS))

CC=gcc
CFLAGS=-std=c11 -march=native -pipe $(OPTS) -Wall -Wextra -Wshadow $(foreach inc,$(INCLUDEDIRS), -I$(inc)) -I/usr/include/freetype2
CXX=g++
CXXFLAGS=-std=c++14 -march=native -pipe $(OPTS) -Wall -Wextra -Wshadow $(foreach inc,$(INCLUDEDIRS), -I$(inc)) -I/usr/include/freetype2
LD=g++
LDFLAGS=$(OPTS) $(addprefix -l,$(LIBS))

SRCS=$(foreach subd,$(SUBDIRS),$(foreach suf,$(SUFFIXES),$(wildcard $(subd)/*$(suf))))
HEADERS=$(foreach subd,$(INCLUDEDIRS),$(shell find $(subd) -regex '.*\.h'))
OBJS=$(foreach subd,$(SUBDIRS),$(foreach suf,$(SUFFIXES),$(subst $(SRCDIR),$(OBJDIR),$(subst $(suf),.o,$(wildcard $(subd)/*$(suf))))))


all: $(APP) $(if $(REBUILD),writeopts,)

$(APP): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) $(REBUILD) Makefile | $(OBJSUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) $(REBUILD) Makefile | $(OBJSUBDIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s $(REBUILD) Makefile | $(OBJSUBDIRS)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJSUBDIRS):
	mkdir -p $@

writeopts:
	@echo 'DEBUG=$(DEBUG)' >.opts
	@echo 'PROFILE=$(PROFILE)' >>.opts

analysis:
	PROFILE=1 $(MAKE) all
	./$(APP)
	gprof ./$(APP) >analysis

dummy:

tar: clean
	tar -zcvf ../$(APP).tar.gz .
	@mv ../$(APP).tar.gz $(APP).tar.gz

clean:
	@rm -rf $(OBJDIR) $(APP) gmon.out analysis

.PHONY: all tar clean writeopts analysis dummy
