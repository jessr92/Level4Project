make clean
make
mkdir parserResults
mkdir parserResults/CPU
./opencl_full_cpu > parserResults/CPU/opencl_fullEP.txt profileEntertainmentPolitical.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullEF.txt profileEntertainmentFinancial.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullEI.txt profileEntertainmentInternational.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullEW.txt profileEntertainmentWashington.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullSP.txt profileSportsPolitical.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullSF.txt profileSportsFinancial.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullSI.txt profileSportsInternational.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullSW.txt profileSportsWashington.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullUP.txt profileUSAPolitical.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullUF.txt profileUSAFinancial.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullUI.txt profileUSAInternational.bin
./opencl_full_cpu > parserResults/CPU/opencl_fullUW.txt profileUSAWashington.bin
python summary.py parserResults
