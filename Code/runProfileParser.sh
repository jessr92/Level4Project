make clean
make
mkdir parserResults
./opencl_full > parserResults/opencl_fullEP.txt profileEntertainmentPolitical.bin
./opencl_full > parserResults/opencl_fullEF.txt profileEntertainmentFinancial.bin
./opencl_full > parserResults/opencl_fullEI.txt profileEntertainmentInternational.bin
./opencl_full > parserResults/opencl_fullEW.txt profileEntertainmentWashington.bin
./opencl_full > parserResults/opencl_fullSP.txt profileSportsPolitical.bin
./opencl_full > parserResults/opencl_fullSF.txt profileSportsFinancial.bin
./opencl_full > parserResults/opencl_fullSI.txt profileSportsInternational.bin
./opencl_full > parserResults/opencl_fullSW.txt profileSportsWashington.bin
./opencl_full > parserResults/opencl_fullUP.txt profileUSAPolitical.bin
./opencl_full > parserResults/opencl_fullUF.txt profileUSAFinancial.bin
./opencl_full > parserResults/opencl_fullUI.txt profileUSAInternational.bin
./opencl_full > parserResults/opencl_fullUW.txt profileUSAWashington.bin
python summary.py parserResults
