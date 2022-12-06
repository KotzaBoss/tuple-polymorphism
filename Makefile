ORDER_MAGN := 7	# 1 2 3 4 5 6 ... # For order-of-magnitude-number of objects to create
BENCH := true

CFLAGS := -std=c++20 -O2 -Wall -Wextra -Wpedantic -Wconversion

usage:
	@echo "Usage:"
	@echo -e "\tmake [all|bench|perf_all|perf_virtual|perf_non_virtual|clean] "

all: bench perf

bench:
	@for i in $(ORDER_MAGN); do	\
		echo "Compiling and perfing for ORDER_MAGNitude=$$i";	\
		g++ $(CFLAGS) -DMAX_THINGS=$$((10**$$i)) -DBENCH main.cpp -lbenchmark -o main_bench.out;	\
		perf stat -d ./main_bench.out;	\
	done

perf: perf_virtual perf_non_virtual

perf_virtual:
	@for i in $(ORDER_MAGN); do	\
		echo "Compiling and perfing for ORDER_MAGNitude=$$i";	\
		time g++ $(CFLAGS) -DMAX_THINGS=$$((10**$$i)) -DVIRTUAL main.cpp -o main_virtual.out;	\
		perf stat -d ./main_virtual.out;	\
	done

perf_non_virtual:
	@for i in $(ORDER_MAGN); do	\
		echo "Compiling and perfing for ORDER_MAGNitude=$$i";	\
		time g++ $(CFLAGS) -DMAX_THINGS=$$((10**$$i)) -DNON_VIRTUAL main.cpp -o main_non_virtual.out;	\
		perf stat -d ./main_non_virtual.out;	\
	done

clean:
	rm -f *.out

