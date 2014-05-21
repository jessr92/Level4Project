function OPENCL_FULL {
    ./opencl_full > $1/opencl_fullEP.txt profileEntertainmentPolitical.bin
    ./opencl_full > $1/opencl_fullEF.txt profileEntertainmentFinancial.bin
    ./opencl_full > $1/opencl_fullEI.txt profileEntertainmentInternational.bin
    ./opencl_full > $1/opencl_fullEW.txt profileEntertainmentWashington.bin
    ./opencl_full > $1/opencl_fullSP.txt profileSportsPolitical.bin
    ./opencl_full > $1/opencl_fullSF.txt profileSportsFinancial.bin
    ./opencl_full > $1/opencl_fullSI.txt profileSportsInternational.bin
    ./opencl_full > $1/opencl_fullSW.txt profileSportsWashington.bin
    ./opencl_full > $1/opencl_fullUP.txt profileUSAPolitical.bin
    ./opencl_full > $1/opencl_fullUF.txt profileUSAFinancial.bin
    ./opencl_full > $1/opencl_fullUI.txt profileUSAInternational.bin
    ./opencl_full > $1/opencl_fullUW.txt profileUSAWashington.bin
}

function OPENCL_FULL_FORK {
    ./opencl_full_fork > $1/opencl_full_forkEP.txt profileEntertainmentPolitical.bin
    ./opencl_full_fork > $1/opencl_full_forkEF.txt profileEntertainmentFinancial.bin
    ./opencl_full_fork > $1/opencl_full_forkEI.txt profileEntertainmentInternational.bin
    ./opencl_full_fork > $1/opencl_full_forkEW.txt profileEntertainmentWashington.bin
    ./opencl_full_fork > $1/opencl_full_forkSP.txt profileSportsPolitical.bin
    ./opencl_full_fork > $1/opencl_full_forkSF.txt profileSportsFinancial.bin
    ./opencl_full_fork > $1/opencl_full_forkSI.txt profileSportsInternational.bin
    ./opencl_full_fork > $1/opencl_full_forkSW.txt profileSportsWashington.bin
    ./opencl_full_fork > $1/opencl_full_forkUP.txt profileUSAPolitical.bin
    ./opencl_full_fork > $1/opencl_full_forkUF.txt profileUSAFinancial.bin
    ./opencl_full_fork > $1/opencl_full_forkUI.txt profileUSAInternational.bin
    ./opencl_full_fork > $1/opencl_full_forkUW.txt profileUSAWashington.bin
}
# Fara has the CPU and GPU we're testing with
if [ "$HOSTNAME" = "fara.dcs.gla.ac.uk" ]; then
    # No Bloom GPU
    make clean
    make
    mkdir -p parserResultsNoBloom/GPU
    echo "Testing full system without bloom filter on GPU"
    OPENCL_FULL "parserResultsNoBloom/GPU"
    # No Bloom CPUGPU
    make clean
    make
    mkdir -p parserResultsNoBloom/CPUGPU
    echo "Testing full system without bloom filter on CPU and GPU"
    OPENCL_FULL_FORK "parserResultsNoBloom/CPUGPU"
    # No Bloom CPU
    make clean
    make CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsNoBloom/CPU
    echo "Testing full system without bloom filter on CPU"
    OPENCL_FULL "parserResultsNoBloom/CPU"
    # Bloom GPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p parserResultsBloom/GPU
    echo "Testing full system with bloom filter on GPU"
    OPENCL_FULL "parserResultsBloom/GPU"
    # Bloom CPUGPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p parserResultsBloom/CPUGPU
    echo "Testing full system with bloom filter on CPU and GPU"
    OPENCL_FULL_FORK "parserResultsBloom/CPUGPU"
    # Bloom CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloom/CPU
    echo "Testing full system with bloom filter on CPU"
    OPENCL_FULL "parserResultsBloom/CPU"
    # BloomAll0 GPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p parserResultsBloomAll0/GPU
    echo "Testing full system with bloom filter all zeroes on GPU"
    OPENCL_FULL "parserResultsBloomAll0/GPU"
    # BloomAll0 CPUGPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p parserResultsBloomAll0/CPUGPU
    echo "Testing full system with bloom filter all zeroes on CPU and GPU"
    OPENCL_FULL_FORK "parserResultsBloomAll0/CPUGPU"
    # BloomAll0 CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloomAll0/CPU
    echo "Testing full system with bloom filter all zeroes on CPU"
    OPENCL_FULL "parserResultsBloomAll0/CPU"
    # BloomAll1 GPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p parserResultsBloomAll1/GPU
    echo "Testing full system with bloom filter all ones on GPU"
    OPENCL_FULL "parserResultsBloomAll1/GPU"
    # BloomAll1 CPUGPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p parserResultsBloomAll1/CPUGPU
    echo "Testing full system with bloom filter all ones on CPU and GPU"
    OPENCL_FULL_FORK "parserResultsBloomAll1/CPUGPU"
    # BloomAll1 CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloomAll1/CPU
    echo "Testing full system with bloom filter all ones on CPU"
    OPENCL_FULL "parserResultsBloomAll1/CPU"
fi
# Curieuse GTX 590
if [ "$HOSTNAME" = "curieuse" ]; then
    # No Bloom 1x GTX 590
    make clean
    make
    mkdir -p parserResultsNoBloom/HalfGTX590
    echo "Testing full system without bloom filter on one half of 590"
    OPENCL_FULL "parserResultsNoBloom/HalfGTX590"
    # Bloom 1x GTX 590
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p parserResultsBloom/HalfGTX590
    echo "Testing full system with bloom filter on one half of 590"
    OPENCL_FULL "parserResultsBloom/HalfGTX590"
    # BloomAll0 1x GTX 590
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p parserResultsBloomAll0/HalfGTX590
    echo "Testing full system with all 0 bloom filter on one half of 590"
    OPENCL_FULL "parserResultsBloomAll0/HalfGTX590"
    # Bloom 1x GTX 590
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p parserResultsBloomAll1/HalfGTX590
    echo "Testing full system with all 1 bloom filter on one half of 590"
    OPENCL_FULL "parserResultsBloomAll1/HalfGTX590"
    # No Bloom Full GTX 590
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DGPUGPU
    mkdir -p parserResultsNoBloom/FullGTX590
    echo "Testing full system without bloom filter on full 590"
    OPENCL_FULL_FORK "parserResultsNoBloom/FullGTX590"
    # Bloom Full GTX 590
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"' CPPFLAGS+=-DGPUGPU
    mkdir -p parserResultsBloom/FullGTX590
    echo "Testing full system with bloom filter on full 590"
    OPENCL_FULL_FORK "parserResultsBloom/FullGTX590"
    # BloomAll0 Full GTX 590
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"' CPPFLAGS+=-DGPUGPU
    mkdir -p parserResultsBloomAll0/FullGTX590
    echo "Testing full system with all 0 bloom filter on full 590"
    OPENCL_FULL_FORK "parserResultsBloomAll0/FullGTX590"
    # Bloom Full GTX 590
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"' CPPFLAGS+=-DGPUGPU
    mkdir -p parserResultsBloomAll1/FullGTX590
    echo "Testing full system with all 1 bloom filter on full 590"
    OPENCL_FULL_FORK "parserResultsBloomAll1/FullGTX590"
fi
# Manipa has the Intel Phi we're testing with
if [ "$HOSTNAME" = "manipa" ]; then
    # No Bloom Phi
    make clean
    make CPPFLAGS+=-DDEVACC
    mkdir -p parserResultsNoBloom/Phi
    echo "Testing full system without bloom filter on Phi"
    OPENCL_FULL "parserResultsNoBloom/Phi"
    # No Bloom PhiPhi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI
    mkdir -p parserResultsNoBloom/PhiPhi
    echo "Testing full system without bloom filter on Phi and Phi"
    OPENCL_FULL_FORK "parserResultsNoBloom/PhiPhi"
    # Bloom Phi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p parserResultsBloom/Phi
    echo "Testing full system with bloom filter on Phi"
    OPENCL_FULL "parserResultsBloom/Phi"
    # Bloom PhiPhi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p parserResultsBloom/PhiPhi
    echo "Testing full system with bloom filter on Phi and Phi"
    OPENCL_FULL_FORK "parserResultsBloom/PhiPhi"
    # BloomAll0 Phi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p parserResultsBloomAll0/Phi
    echo "Testing full system with bloom filter all zeroes on Phi"
    OPENCL_FULL "parserResultsBloomAll0/Phi"
    # BloomAll0 PhiPhi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p parserResultsBloomAll0/PhiPhi
    echo "Testing full system with bloom filter all zeroes on Phi and Phi"
    OPENCL_FULL_FORK "parserResultsBloomAll0/PhiPhi"
    # BloomAll1 Phi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p parserResultsBloomAll1/Phi
    echo "Testing full system with bloom filter all ones on Phi"
    OPENCL_FULL "parserResultsBloomAll1/Phi"
    # BloomAll1 PhiPhi
    make clean
    make CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p parserResultsBloomAll1/PhiPhi
    echo "Testing full system with bloom filter all ones on Phi and Phi"
    OPENCL_FULL_FORK "parserResultsBloomAll1/PhiPhi"
fi
# Togian has a 64 core CPU
if [ "$HOSTNAME" = "togian.dcs.gla.ac.uk" ]; then
    # No Bloom CPU
    make clean
    make CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsNoBloom/AMDCPU
    echo "Testing full system without bloom filter on CPU"
    OPENCL_FULL "parserResultsNoBloom/AMDCPU"
    # Bloom CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloom/AMDCPU
    echo "Testing full system with bloom filter on CPU"
    OPENCL_FULL "parserResultsBloom/AMDCPU"
    # BloomAll0 CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloomAll0/AMDCPU
    echo "Testing full system with bloom filter all zeroes on CPU"
    OPENCL_FULL "parserResultsBloomAll0/AMDCPU"
    # BloomAll1 CPU
    make clean
    make CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p parserResultsBloomAll1/AMDCPU
    echo "Testing full system with bloom filter all ones on CPU"
    OPENCL_FULL "parserResultsBloomAll1/AMDCPU"
fi
python summary.py parserResultsNoBloom
python summary.py parserResultsBloom
python summary.py parserResultsBloomAll0
python summary.py parserResultsBloomAll1
