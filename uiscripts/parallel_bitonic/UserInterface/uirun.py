import os
import time
from subprocess import call
from time import sleep


processors = [2, 4, 8, 16, 32, 64, 128]
Q = range(16, 21)

for n_proc in processors:
    for q in Q:
        filename = "results{0}{1}.txt".format(n_proc, q)
        command = "mpiexec  -n {0} ./mpi-bitonic {1}".format(n_proc, q)
        call(command,shell=True)
        print  filename + "is done"
