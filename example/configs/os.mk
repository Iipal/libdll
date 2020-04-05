DEL     := rm -rf
ECHO    := echo
NPROCS  := 1
UNAME_S := $(shell uname -s)

# Linux Specifications:
ifeq ($(UNAME_S),Linux)
# if it's not Debian-based distro needs to add "-e" flag for correct work 'echo' with Escape-sequences
 ifeq (,$(shell grep -Ei 'debian|buntu|mint' /etc/*release))
	ECHO += -e
 endif
NPROCS   := $(shell grep -c ^processor /proc/cpuinfo)
endif

# MacOS Specifications:
ifeq ($(UNAME_S),Darwin)
# Only if you have installed gmake(GNU make) on ~/.brew/bin folder via 'brew install make'.
# Required for MacOS systems whereby default installed 'make --version' <=3.8.1
#  and that doesn't have -O option for correct parallel make output.
 ifneq (,$(wildcard ~/.brew/bin/gmake))
	MAKE := ~/.brew/bin/gmake
 endif
NPROCS   := $(shell sysctl -n hw.ncpu)
endif

# Additional make settings:
MAKE                += --no-print-directory
MAKE_PARALLEL_FLAGS := -j $(NPROCS) -l $(NPROCS)
ifeq (4.0.0,$(firstword $(sort $(shell $(MAKE) --version | awk 'NR==1 {print $3}') 4.0.0)))
MAKE_PARALLEL_FLAGS += -Otarget
endif
