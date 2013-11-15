make
./cpu_implementation_no_wrapper > cpu.txt
./gpu_implementation_no_wrapper > gpu.txt
echo "---------cpu.txt versus gpu.txt---------"
diff cpu.txt gpu.txt
echo "---------cpu.txt versus cpuRef.txt---------"
diff cpu.txt cpuRef.txt
echo "---------gpu.txt versus cpuRef.txt---------"
diff gpu.txt cpuRef.txt
