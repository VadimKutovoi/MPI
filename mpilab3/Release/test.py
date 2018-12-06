import os
import time


path = "C:\\Users\\vkutovoi\\Downloads\\Gitprojects\\MPI_LAB1\\mpilab1\\Release"
os.chdir(path)

print("Enter max process count : ", end="")
iterations = int(input())

print("One dimentional integral benchmarking...")
for i in range(1, iterations + 1):
	os.system("mpiexec -n " + str(i) + " mpilab1.exe -v -a1 0 -b1 10000 -h 0.001 -log")



