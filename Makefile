
CXX=mpicc
CXXFLAGS=-O3
TARGET=mpi-bitonic
NPROCS = 8

all:
    	make run
$(TARGET):	$(TARGET).c
        $(CXX) $< -o $(TARGET)

run:    $(TARGET)
        mpiexec  -n $(NPROCS) ./$(TARGET) 16

clean:
      	rm -f *~ $(TARGET)









