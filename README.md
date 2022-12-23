```shell
cmake -B build .
make -C build -j$(nproc)
for bin in build/main.bench*; do
	$bin;
done
```

**Note:** You need Google Benchmark [installed](https://github.com/google/benchmark#installation) on the computer.

