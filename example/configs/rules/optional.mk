make_optional:
 ifneq (,$(filter $(MAKECMDGOALS),debug debug_all))
	@$(eval CFLAGS_OPTIONAL:=$(CFLAGS_DEBUG))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_DEBUG)
 endif
 ifneq (,$(filter $(MAKECMDGOALS),sanitize sanitize_all))
	@$(eval CFLAGS_OPTIONAL:=$(CFLAGS_SANITIZE))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_SANITIZE)
 endif
 ifneq (,$(filter $(MAKECMDGOALS),assembly assembly_all))
	@$(eval CFLAGS_OPTIONAL:=$(CFLAGS_ASSEMBLY))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_ASSEMBLY)
	@$(eval ASMS:=$(OBJS:.o=.S))
	@$(eval OBJS:=)
 endif
 ifneq (,$(filter $(MAKECMDGOALS),debug_assembly debug_assembly_all))
	@$(eval CFLAGS_OPTIONAL:=$(CFLAGS_DEBUG_ASSEMBLY))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_DEBUG_ASSEMBLY)
	@$(eval ASMS:=$(OBJS:.o=.S))
	@$(eval OBJS:=)
 endif
 ifneq (,$(filter $(MAKECMDGOALS),pedantic pedantic_all))
	@$(eval DEFINES+=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_PEDANTIC)
	@$(eval CFLAGS+=$(CFLAGS_PEDANTIC))
 endif
 ifneq (,$(filter $(MAKECMDGOALS),x86 x86_all))
	@$(eval CFLAGS_OPTIONAL+=$(CFLAGS_X86))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_X86)
 endif
 ifneq (,$(filter $(MAKECMDGOALS),debug_x86 debug_x86_all))
	@$(eval CFLAGS_OPTIONAL:=$(CFLAGS_DEBUG_X86))
	@$(eval DEFINES:=$(shell echo $(basename $(NAME)) | tr a-z A-Z)_DEBUG_X86)
 endif

debug_all: pre
debug: multi

sanitize_all: pre
sanitize: multi

assembly_all: pre
assembly: multi

debug_assembly_all: pre
debug_assembly: multi

pedantic_all: pre
pedantic: multi

x86_all: pre
x86: multi

debug_x86_all: pre
debug_x86: multi
