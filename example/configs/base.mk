.EXPORT_ALL_VARIABLES:

NAME := $(notdir $(CURDIR))
NPWD := $(CURDIR)/$(NAME)

SRCS := $(shell find ./ -name "*.c")
OBJS := $(SRCS:.c=.o)
ASMS := # reversed for 'assembly' and 'assembly_all' rules where ASMS:=$(OBJS:%.o=%.S) and OBJS:= sets to nothing

# all header .h files must be placed in to "includes" folder(or sub-dirs) only.
I_PATHS := ../
ifneq (,$(I_PATHS))
IFLAGS := $(addprefix -I,$(foreach ip,$(I_PATHS),$(abspath $(shell find $(ip) -type d))))
else
$(error "all header .h files must be placed in to "includes" folder(and sub-dirs) only.")
endif

# Compiler settings.
CC     := clang
CFLAGS := -Wall -Wextra -Werror -Wunused -MMD -std=c11

ifeq (,$(shell command -v clang 2> /dev/null))
CC     := gcc
endif

CFLAGS_OPTIMIZE       := -march=native -mtune=native -Ofast -pipe -flto -fpic
CFLAGS_DEBUG          := -g3
CFLAGS_SANITIZE       := $(CFLAGS_DEBUG) -fsanitize=address
CFLAGS_ASSEMBLY       := $(filter-out -flto -fpic,$(CFLAGS_OPTIMIZE)) -S -masm=intel
CFLAGS_DEBUG_ASSEMBLY := $(CFLAGS_DEBUG) -S -masm=intel
CFLAGS_PEDANTIC       := -Wpedantic
CFLAGS_X86            := -m32
CFLAGS_DEBUG_X86      := $(CFLAGS_DEBUG) -m32

CFLAGS_OPTIONAL       := $(CFLAGS_OPTIMIZE)
