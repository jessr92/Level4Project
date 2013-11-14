make
./cpu_implementation_no_wrapper > cpu.txt
./gpu_implementation_no_wrapper > gpu.txt
diff cpu.txt gpu.txt
