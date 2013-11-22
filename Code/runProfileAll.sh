make clean
make
mkdir -p experimentRun
echo "Starting experiment..."
cpuRef=("cpuRefEF.txt" "cpuRefEI.txt" "cpuRefEP.txt" "cpuRefEW.txt" \
        "cpuRefSF.txt" "cpuRefSI.txt" "cpuRefSP.txt" "cpuRefSW.txt" \
        "cpuRefUF.txt" "cpuRefUI.txt" "cpuRefUP.txt" "cpuRefUW.txt")
cpuRefMT=("cpuRefMTEF.txt" "cpuRefMTEI.txt" "cpuRefMTEP.txt" "cpuRefMTEW.txt" \
          "cpuRefMTSF.txt" "cpuRefMTSI.txt" "cpuRefMTSP.txt" "cpuRefMTSW.txt" \
          "cpuRefMTUF.txt" "cpuRefMTUI.txt" "cpuRefMTUP.txt" "cpuRefMTUW.txt")
cpu=("cpuMTEF.txt" "cpuMTEI.txt" "cpuMTEP.txt" "cpuMTEW.txt" \
     "cpuMTSF.txt" "cpuMTSI.txt" "cpuMTSP.txt" "cpuMTSW.txt" \
     "cpuMTUF.txt" "cpuMTUI.txt" "cpuMTUP.txt" "cpuMTUW.txt")
gpu=("gpuMTEF.txt" "gpuMTEI.txt" "gpuMTEP.txt" "gpuMTEW.txt" \
     "gpuMTSF.txt" "gpuMTSI.txt" "gpuMTSP.txt" "gpuMTSW.txt" \
     "gpuMTUF.txt" "gpuMTUI.txt" "gpuMTUP.txt" "gpuMTUW.txt")
profiles=(profile?*.bin)
echo "Sleeping..."
#sleep 60
for i in "${!profiles[@]}"
do
        echo "./cpu_reference_implementation ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./cpu_reference_implementation "${profiles[$i]}" > "experimentRun/${cpuRef[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./cpu_reference_implementation_mt ${profiles[$i]} > experimentRun/${cpuRefMT[$i]}"
        ./cpu_reference_implementation_mt "${profiles[$i]}" > "experimentRun/${cpuRefMT[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./cpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${cpu[$i]}"
        ./cpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${cpu[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./gpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${gpu[$i]}"
        ./gpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${gpu[$i]}"
        echo "Sleeping..."
        sleep 60
done
echo "Finished experiment..."
