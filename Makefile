#-*- mode: makefile -*-
ifndef UFTRACE_HOME
  uftrace_home = $(CURDIR)/..
else
  uftrace_home = $(UFTRACE_HOME)
endif

include $(uftrace_home)/Makefile.include
srcdir = $(CURDIR)

# set objdir to $(O) by default (if any)
ifeq ($(objdir),)
  ifneq ($(O),)
    objdir = $(O)
  else
    objdir = $(CURDIR)
  endif
endif

uname_M := $(shell uname -m 2>/dev/null || echo not)
ARCH ?= $(shell echo $(uname_M) | sed -e s/i.86/i386/ -e s/arm.*/arm/ )
ifeq ($(ARCH),x86_64)
  ifneq ($(findstring m32,$(CFLAGS)),)
    override ARCH := i386
  endif
endif

CFLAGS := -D_GNU_SOURCE -iquote $(uftrace_home) -iquote $(uftrace_home)/arch/$(ARCH) -fPIC
LDFLAGS := -shared -lrt -ldl -pthread -Wl,-z,noexecstack

# Pinpoint stub
TARGET += system-check gen_pb gen_grpc 
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTO_PATH = ./protos
PROTO_SRCS := $(wildcard $(PROTO_PATH)/*.proto)
PROTO_OBJS := $(patsubst $(PROTO_PATH)/%.proto,$(PROTO_PATH)/%.pb.o,$(PROTO_SRCS))
PROTO_C_SRCS := $(patsubst $(PROTO_PATH)/%.proto,$(PROTO_PATH)/%.pb.cc,$(PROTO_SRCS))
PROTO_GRPC_OBJS := $(patsubst $(PROTO_PATH)/%.proto,$(PROTO_PATH)/%.grpc.pb.o,$(PROTO_SRCS))
PROTO_GRPC_C_SRCS := $(patsubst $(PROTO_PATH)/%.proto,$(PROTO_PATH)/%.grpc.pb.cc,$(PROTO_SRCS))

PINPT_PATH = ./pinpoint
PINPT_SRCS := $(wildcard $(PINPT_PATH)/*.cc)
PINPT_OBJS := $(patsubst $(PINPT_PATH)/%.cc,$(PINPT_PATH)/%.o,$(PINPT_SRCS))

vpath %.proto $(PROTO_PATH)

HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = g++
PINPT_CPPFLAGS := `pkg-config --cflags protobuf grpc`
PINPT_CXXFLAGS := -std=c++11 -g -fPIC -I$(PROTO_PATH) -I$(PINPT_PATH)

ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
           -pthread\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
           -pthread\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
endif

ifeq ($(DEBUG), 1)
  CFLAGS += -O0 -g
else
  CFLAGS += -O2 -g
endif

# Plugin stub
TARGET += plugin_skel.so plugin_nodejs.so

PLUGIN_SRCS := $(srcdir)/plugin_skeleton.c $(srcdir)/plugin_nodejs.c
PLUGIN_UTILS_SRCS += $(srcdir)/utils/debug.c $(srcdir)/utils/regs.c
PLUGIN_UTILS_SRCS += $(srcdir)/utils/rbtree.c $(srcdir)/utils/filter.c
PLUGIN_UTILS_SRCS += $(srcdir)/utils/demangle.c $(srcdir)/utils/utils.c
PLUGIN_UTILS_SRCS += $(srcdir)/utils/script.c $(srcdir)/utils/script-python.c
PLUGIN_UTILS_SRCS += $(srcdir)/utils/auto-args.c $(srcdir)/utils/dwarf.c
PLUGIN_UTILS_SRCS += $(wildcard $(srcdir)/utils/symbol*.c)
PLUGIN_UTILS_OBJS := $(patsubst $(srcdir)/utils/%.c,$(objdir)/%.op,$(PLUGIN_UTILS_SRCS))


all: $(TARGET)

# Plugin 
$(PLUGIN_UTILS_OBJS): $(objdir)/%.op: $(uftrace_home)/utils/%.c
	$(QUIET_CC_FPIC)$(CC) $(CFLAGS) -c $< -o $@

plugin_skel.so: $(PLUGIN_SRCS) $(PLUGIN_UTILS_OBJS)
	$(QUIET_CC_FPIC)$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

plugin_nodejs.so: $(PLUGIN_SRCS) $(PLUGIN_UTILS_OBJS) $(PINPT_OBJS) $(PROTO_OBJS) $(PROTO_GRPC_OBJS)
	$(QUIET_CC_FPIC)$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Pinpoint 
gen_pb: $(PROTO_SRCS)
	$(PROTOC) -I $(PROTO_PATH) --cpp_out=$(PROTO_PATH) $^

gen_grpc: $(PROTO_SRCS)
	$(PROTOC) -I $(PROTO_PATH) --grpc_out=$(PROTO_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $^

$(PINPT_OBJS): $(PINPT_PATH)/%.o: $(PINPT_PATH)/%.cc
	$(QUIET_CC_FPIC)$(CXX) -g -fPIC $(PINPT_CPPCFLAGS) $(PINPT_CXXFLAGS) -c -o $@ $<

$(PROTO_OBJS):
	$(foreach src, $(PROTO_C_SRCS), \
	$(CXX) -g $(src) $(LDFLAGS) -fPIC -c -o $(patsubst $(PROTO_PATH)/%.pb.cc,$(PROTO_PATH)/%.pb.o,$(src));)

$(PROTO_GRPC_OBJS):
	$(foreach src, $(PROTO_GRPC_C_SRCS), \
	$(CXX) -g $(src) $(LDFLAGS) -fPIC -c -o $(patsubst $(PROTO_PATH)/%.grpc.pb.cc,$(PROTO_PATH)/%.grpc.pb.o,$(src));)



clean:
	$(Q)$(RM) $(objdir)/*.o $(objdir)/*.op 
	$(Q)$(RM) $(objdir)/plugin_skel.so
	$(Q)$(RM) $(objdir)/plugin_nodejs.so
	$(Q)$(RM) $(objdir)/system_check
	$(Q)$(RM) $(srcdir)/protos/*.cc
	$(Q)$(RM) $(PINPT_PATH)/*.o $(PROTO_PATH)/*.o


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif

.PHONY: all
