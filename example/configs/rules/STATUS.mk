.PHONY: STATUS_START
STATUS_START:
 ifneq (,$(OBJS))
	@$(ECHO) " | ---------------------- |"
	@$(ECHO) " | making: $(CLR_UNDERLINE)$(NAME)$(CLR_WHITE) ..."
	@$(ECHO) " | ---------------------- |"
 endif

.PHONY: STATUS_END
STATUS_END:
	@$(ECHO) "/ ----------------------- |"
 ifneq (,$(NAME))
  ifneq (,$(OBJS))
	@$(ECHO) "| compiled                : $(NAME) $(MSG_SUCCESS)"
  endif
 endif
 ifneq (,$(CC))
	@$(ECHO) "| compiler                : $(CC)"
 endif
 ifneq (,$(DEFINES))
	@$(ECHO) "| compiler custom defines : $(foreach dfns,$(DEFINES),$(CLR_INVERT)$(dfns)$(CLR_WHITE) )"
 endif
 ifneq (,$(CFLAGS))
	@$(ECHO) "| compiler          flags : $(CFLAGS)"
 endif
 ifneq (,$(CFLAGS_OPTIONAL))
	@$(ECHO) "| compiler optional flags : $(CLR_UNDERLINE)$(CFLAGS_OPTIONAL)$(CLR_WHITE)"
 endif
	@$(ECHO) "\\ ----------------------- |"
