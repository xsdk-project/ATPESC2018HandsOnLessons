PTOOL ?= visit
RUNAME ?= heat_results
RM = rm -rf

SRC = heat.C utils.C args.C exact.C ftcs.C upwind15.C crankn.C
OBJ = $(SRC:.C=.o)
EXE = heat

# Implicit rule for object files
%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# Linking the final heat app
heat: $(OBJ)
	$(CXX) -o heat $(OBJ) -lm

check/check_soln_final.curve: $(EXE) checkclean
	@./heat runame=check maxt=50 1>/dev/null 2>&1

check: check/check_soln_final.curve
	@diff check/check_soln_final.curve heat_check.curve; \
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

checkclean:
	$(RM) check

allclean: clean objclean

plot:
	@./tools/run_$(PTOOL).sh $(RUNAME)

foo:
	[[ -d $(RUNAME) ]] || exit 1
	@if [[ "$(PTOOL)" = "visit" ]]; then \
	    which visit; \
	    [[ $? -eq 0 ]] || exit 1;\
	    ./tools/run_visit.sh $(RUNAME)
	elif [[ "$(PTOOL)" == "gnuplot" ]]; then \
	    echo "got gnuplot"; \
	    which gnuplot; \
	    [[ $? -eq 0 ]] || exit 1;\
	elif [[ "$(PTOOL)" == "pyplot" ]]; then \
	    echo "got pyplot"; \
	else \
	    echo "plot tool \"$(PTOOL)\" not recognized"; \
	    exit 1; \
	fi
