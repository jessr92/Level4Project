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
cpu=("cpuEF.txt" "cpuEI.txt" "cpuEP.txt" "cpuEW.txt" \
     "cpuSF.txt" "cpuSI.txt" "cpuSP.txt" "cpuSW.txt" \
     "cpuUF.txt" "cpuUI.txt" "cpuUP.txt" "cpuUW.txt")
gpu=("gpuEF.txt" "gpuEI.txt" "gpuEP.txt" "gpuEW.txt" \
     "gpuSF.txt" "gpuSI.txt" "gpuSP.txt" "gpuSW.txt" \
     "gpuUF.txt" "gpuUI.txt" "gpuUP.txt" "gpuUW.txt")
profiles=(profile?*.bin)
for i in "${!profiles[@]}"
do
        echo "./cpu_reference_implementation ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./cpu_reference_implementation "${profiles[$i]}" > "experimentRun/${cpuRef[$i]}"
        echo "./cpu_reference_implementation_mt ${profiles[$i]} > experimentRun/${cpuRefMT[$i]}"
        ./cpu_reference_implementation_mt "${profiles[$i]}" > "experimentRun/${cpuRefMT[$i]}"
        echo "./cpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${cpu[$i]}"
        ./cpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${cpu[$i]}"
        echo "./gpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${gpu[$i]}"
        ./gpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${gpu[$i]}"
done
echo "Finished experiment..."
