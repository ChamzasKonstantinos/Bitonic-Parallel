import os
import time
from subprocess import call
from time import sleep
from math import log

processors = [2, 4, 8, 16, 32, 64, 128]
Q = range(16, 21)

for n_proc in processors:
    for q in Q:
        filename = "bsortresults{0}{1}.txt".format(n_proc, q)
        command = "./bitonic {0} {1}".format(q,n_proc)
        call(command,shell=True)
        print  filename + " is done"
