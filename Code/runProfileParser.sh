# Fara has the CPU and GPU we're testing with
if [ "$HOSTNAME" = "fara.dcs.gla.ac.uk" ]; then
    make clean
    make
    mkdir -p parserResults/GPU
    echo "Testing parser/scorer on GPU"
    ./opencl_full > parserResults/GPU/opencl_fullEP.txt profileEntertainmentPolitical.bin
    ./opencl_full > parserResults/GPU/opencl_fullEF.txt profileEntertainmentFinancial.bin
    ./opencl_full > parserResults/GPU/opencl_fullEI.txt profileEntertainmentInternational.bin
    ./opencl_full > parserResults/GPU/opencl_fullEW.txt profileEntertainmentWashington.bin
    ./opencl_full > parserResults/GPU/opencl_fullSP.txt profileSportsPolitical.bin
    ./opencl_full > parserResults/GPU/opencl_fullSF.txt profileSportsFinancial.bin
    ./opencl_full > parserResults/GPU/opencl_fullSI.txt profileSportsInternational.bin
    ./opencl_full > parserResults/GPU/opencl_fullSW.txt profileSportsWashington.bin
    ./opencl_full > parserResults/GPU/opencl_fullUP.txt profileUSAPolitical.bin
    ./opencl_full > parserResults/GPU/opencl_fullUF.txt profileUSAFinancial.bin
    ./opencl_full > parserResults/GPU/opencl_fullUI.txt profileUSAInternational.bin
    ./opencl_full > parserResults/GPU/opencl_fullUW.txt profileUSAWashington.bin
    mkdir -p parserResults/CPUGPU
    echo "Testing parser/scorer on CPU and GPU"
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkEP.txt profileEntertainmentPolitical.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkEF.txt profileEntertainmentFinancial.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkEI.txt profileEntertainmentInternational.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkEW.txt profileEntertainmentWashington.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkSP.txt profileSportsPolitical.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkSF.txt profileSportsFinancial.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkSI.txt profileSportsInternational.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkSW.txt profileSportsWashington.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkUP.txt profileUSAPolitical.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkUF.txt profileUSAFinancial.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkUI.txt profileUSAInternational.bin
    ./opencl_full_fork > parserResults/CPUGPU/opencl_full_forkUW.txt profileUSAWashington.bin
    mean clean
    make CPPFLAGS+=-DDEVCPU
    mkdir -p parserResults/CPU
    echo "Testing parser/scorer on CPU"
    ./opencl_full > parserResults/CPU/opencl_fullEP.txt profileEntertainmentPolitical.bin
    ./opencl_full > parserResults/CPU/opencl_fullEF.txt profileEntertainmentFinancial.bin
    ./opencl_full > parserResults/CPU/opencl_fullEI.txt profileEntertainmentInternational.bin
    ./opencl_full > parserResults/CPU/opencl_fullEW.txt profileEntertainmentWashington.bin
    ./opencl_full > parserResults/CPU/opencl_fullSP.txt profileSportsPolitical.bin
    ./opencl_full > parserResults/CPU/opencl_fullSF.txt profileSportsFinancial.bin
    ./opencl_full > parserResults/CPU/opencl_fullSI.txt profileSportsInternational.bin
    ./opencl_full > parserResults/CPU/opencl_fullSW.txt profileSportsWashington.bin
    ./opencl_full > parserResults/CPU/opencl_fullUP.txt profileUSAPolitical.bin
    ./opencl_full > parserResults/CPU/opencl_fullUF.txt profileUSAFinancial.bin
    ./opencl_full > parserResults/CPU/opencl_fullUI.txt profileUSAInternational.bin
    ./opencl_full > parserResults/CPU/opencl_fullUW.txt profileUSAWashington.bin
fi
# Manipa has the Intel Phi we're testing with
if [ "$HOSTNAME" = "manipa" ]; then
    mean clean
    make CPPFLAGS+=-DDEVACC
    mkdir -p parserResults/Phi
    echo "Testing parser/scorer on Phi"
    ./opencl_full > parserResults/Phi/opencl_fullEP.txt profileEntertainmentPolitical.bin
    ./opencl_full > parserResults/Phi/opencl_fullEF.txt profileEntertainmentFinancial.bin
    ./opencl_full > parserResults/Phi/opencl_fullEI.txt profileEntertainmentInternational.bin
    ./opencl_full > parserResults/Phi/opencl_fullEW.txt profileEntertainmentWashington.bin
    ./opencl_full > parserResults/Phi/opencl_fullSP.txt profileSportsPolitical.bin
    ./opencl_full > parserResults/Phi/opencl_fullSF.txt profileSportsFinancial.bin
    ./opencl_full > parserResults/Phi/opencl_fullSI.txt profileSportsInternational.bin
    ./opencl_full > parserResults/Phi/opencl_fullSW.txt profileSportsWashington.bin
    ./opencl_full > parserResults/Phi/opencl_fullUP.txt profileUSAPolitical.bin
    ./opencl_full > parserResults/Phi/opencl_fullUF.txt profileUSAFinancial.bin
    ./opencl_full > parserResults/Phi/opencl_fullUI.txt profileUSAInternational.bin
    ./opencl_full > parserResults/Phi/opencl_fullUW.txt profileUSAWashington.bin
fi
python summary.py parserResults
