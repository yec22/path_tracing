g++-12 -O3 -fopenmp *.cpp -o pt
export GOMP_STACKSIZE=32000
time ./pt
rm -rf pt