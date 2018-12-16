import os
import time

print("Enter max process count : ", end="")
iterations = int(input())

print("Benchmarking...")
for i in range(1, iterations + 1):
	os.system("mpiexec -n " + str(i) + " mpilab1.exe -v -a1 0 -b1 10000 -h 0.001 -log")
