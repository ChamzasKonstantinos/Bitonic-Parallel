  GNU nano 2.0.9       File: Makefile                     

CXX=mpicc
CXXFLAGS=-O3
TARGET=mpi-bitonic
NPROCS = 8

all:
    	make run
$(TARGET):	$(TARGET).c
        $(CXX) $< -o $(TARGET)

run:    $(TARGET)
        # mpiexec -machinefile mpd.hosts -n $(NPROCS) ./$$
        mpiexec  -n $(NPROCS) ./$(TARGET) 16

clean:
      	rm -f *~ *.out









