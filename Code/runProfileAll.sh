echo"Starting..."
make clean
make
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefEP.txt profileEntertainmentPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefEF.txt profileEntertainmentFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefEI.txt profileEntertainmentInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefEW.txt profileEntertainmentWashington.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefSP.txt profileSportsPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefSF.txt profileSportsFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefSI.txt profileSportsInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefSW.txt profileSportsWashington.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefUP.txt profileUSAPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefUF.txt profileUSAFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefUI.txt profileUSAInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation > experimentRun/cpuRefUW.txt profileUSAWashington.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTEP.txt profileEntertainmentPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTEF.txt profileEntertainmentFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTEI.txt profileEntertainmentInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTEW.txt profileEntertainmentWashington.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTSP.txt profileSportsPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTSF.txt profileSportsFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTSI.txt profileSportsInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTSW.txt profileSportsWashington.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTUP.txt profileUSAPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTUF.txt profileUSAFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTUI.txt profileUSAInternational.bin
echo "Sleeping..."
sleep 60
./cpu_reference_implementation_mt > experimentRun/cpuRefMTUW.txt profileUSAWashington.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuEP.txt profileEntertainmentPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuEF.txt profileEntertainmentFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuEI.txt profileEntertainmentInternational.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuEW.txt profileEntertainmentWashington.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuSP.txt profileSportsPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuSF.txt profileSportsFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuSI.txt profileSportsInternational.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuSW.txt profileSportsWashington.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuUP.txt profileUSAPolitical.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuUF.txt profileUSAFinancial.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuUI.txt profileUSAInternational.bin
echo "Sleeping..."
sleep 60
./cpu_implementation_no_wrapper > experimentRun/cpuUW.txt profileUSAWashington.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuEP.txt profileEntertainmentPolitical.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuEF.txt profileEntertainmentFinancial.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuEI.txt profileEntertainmentInternational.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuEW.txt profileEntertainmentWashington.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuSP.txt profileSportsPolitical.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuSF.txt profileSportsFinancial.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuSI.txt profileSportsInternational.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuSW.txt profileSportsWashington.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuUP.txt profileUSAPolitical.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuUF.txt profileUSAFinancial.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuUI.txt profileUSAInternational.bin
echo "Sleeping..."
sleep 60
./gpu_implementation_no_wrapper > experimentRun/gpuUW.txt profileUSAWashington.bin
echo "Sleeping..."
sleep 60
echo "Finished"