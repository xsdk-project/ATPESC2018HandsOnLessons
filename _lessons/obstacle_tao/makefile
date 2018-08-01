# Define the MFEM directories here
MFEM_DIR = /projects/ATPESC2018/FASTMath/spark/mfem
MFEM_BUILD_DIR = /projects/ATPESC2018/FASTMath/spark/mfem
CONFIG_MK = $(MFEM_BUILD_DIR)/config/config.mk
CC = $(PETSC_DIR)/$(PETSC_ARCH)/bin/mpic++
-include $(CONFIG_MK)

.SUFFIXES: .o .cpp .mk
.PHONY: all clean

all: obstacle

obstacle:
	$(CC) obstacle.cpp $(MFEM_LIB_FILE) $(CONFIG_MK) $(MFEM_CXX) $(MFEM_FLAGS) $< -o $@ -L/usr/local/lib/gcc/4.9 $(MFEM_LIBS)

clean:
	rm -f *.o *~ obstacle
	rm -rf *.dSYM *.TVD.*breakpoints
