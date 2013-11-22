make clean
make
mkdir -p experimentRun
echo "Starting experiment..."
cpuRef=("cpuRefEP.txt" "cpuRefEF.txt" "cpuRefEI.txt" "cpuRefEW.txt" \
         "cpuRefSP.txt" "cpuRefSF.txt" "cpuRefSI.txt" "cpuRefSW.txt" \
         "cpuRefUP.txt" "cpuRefUF.txt" "cpuRefUI.txt" "cpuRefUI.txt")
cpuRefMT=("cpuRefMTEP.txt" "cpuRefMTEF.txt" "cpuRefMTEI.txt" "cpuRefMTEW.txt" \
         "cpuRefMTSP.txt" "cpuRefMTSF.txt" "cpuRefMTSI.txt" "cpuRefMTSW.txt" \
         "cpuRefMTUP.txt" "cpuRefMTUF.txt" "cpuRefMTUI.txt" "cpuRefMTUI.txt")
cpu=("cpuEP.txt" "cpuEF.txt" "cpuEI.txt" "cpuEW.txt" \
         "cpuSP.txt" "cpuSF.txt" "cpuSI.txt" "cpuSW.txt" \
         "cpuUP.txt" "cpuUF.txt" "cpuUI.txt" "cpuUI.txt")
gpu=("gpuEP.txt" "gpuEF.txt" "gpuEI.txt" "gpuEW.txt" \
         "gpuSP.txt" "gpuSF.txt" "gpuSI.txt" "gpuSW.txt" \
         "gpuUP.txt" "gpuUF.txt" "gpuUI.txt" "gpuUI.txt")
profiles=(profile*.bin)
echo "Sleeping..."
sleep 60
for i in "${!profiles[@]}"
do
    if [ "${profiles[$i]}" != "profile.bin" ];
    then
        echo "./cpu_reference_implementation ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./cpu_reference_implementation "${profiles[$i]}" > "experimentRun/${cpuRef[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./cpu_reference_implementation_mt ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./cpu_reference_implementation_mt "${profiles[$i]}" > "experimentRun/${cpuRefMT[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./cpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./cpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${cpu[$i]}"
        echo "Sleeping..."
        sleep 60
        echo "./gpu_implementation_no_wrapper ${profiles[$i]} > experimentRun/${cpuRef[$i]}"
        ./gpu_implementation_no_wrapper "${profiles[$i]}" > "experimentRun/${gpu[$i]}"
        echo "Sleeping..."
        sleep 60
    fi
done
echo "Finished experiment..."
