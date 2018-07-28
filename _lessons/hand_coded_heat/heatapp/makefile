# Implicit rule for object files
%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

RM ?= rm -rf

SRC = heat.C args.C ftcs.C upwind15.C crankn.C
OBJ = $(SRC:.C=.o)
EXE = heat

# Linking the final heat app
heat: $(OBJ)
	$(CXX) -o heat $(OBJ) -lm

check:
	@./heat 1>/dev/null 2>&1
	@diff heat_soln_final.curve heat_soln_check.curve; \
	if [[ $$? -eq 0 ]]; then \
	    echo "PASSED"; \
	else \
	    echo "FAILED"; \
	fi

objclean:
	$(RM) $(OBJ)

# alias for objclean
clean: objclean
	$(RM) $(EXE)

dataclean:
	@datafiles=$$(ls heat_*.curve | grep -v heat_soln_check.curve); \
	$(RM) $$datafiles

allclean: clean objclean
