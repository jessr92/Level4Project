make clean
make
echo "Executing CPU Reference Implementation"
./cpu_reference_implementation > cpuRef.txt
echo "Sleeping for 1m"
sleep 60
echo "Executing CPU OpenCL Implementation"
./cpu_implementation_no_wrapper > cpu.txt
echo "Sleeping for 1m"
sleep 60
echo "Executing GPU OpenCL Implementation"
./gpu_implementation_no_wrapper > gpu.txt
echo "Sleeping for 1m"
sleep 60
echo "Executing CPU PThread Implementation"
./cpu_reference_implementation_mt > cpuRefMT.txt
echo "---------cpu.txt versus gpu.txt---------"
diff cpu.txt gpu.txt
echo "---------cpu.txt versus cpuRef.txt---------"
diff cpu.txt cpuRef.txt
echo "---------gpu.txt versus cpuRef.txt---------"
diff gpu.txt cpuRef.txt
echo "---------cpu.txt versus cpuRefMT.txt---------"
diff cpu.txt cpuRefMT.txt
