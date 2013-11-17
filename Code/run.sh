make
echo "Executing CPU Reference Implementation"
./cpu_reference_implementation > cpuRefNoOptimise.txt
echo "Executing CPU OpenCL Implementation"
./cpu_implementation_no_wrapper > cpu.txt
echo "Executing GPU OpenCL Implementation"
./gpu_implementation_no_wrapper > gpu.txt
echo "---------cpu.txt versus cpuRefNoOptimise.txt---------"
diff cpu.txt cpuRefNoOptimise.txt
echo "---------cpu.txt versus gpu.txt---------"
diff cpu.txt gpu.txt
echo "---------cpu.txt versus cpuRef.txt---------"
diff cpu.txt cpuRef.txt
echo "---------gpu.txt versus cpuRef.txt---------"
diff gpu.txt cpuRef.txt
