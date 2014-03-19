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
    make CPPFLAGS+=-DHTML_PARSE
    mkdir -p htmlResultsNoBloom/GPU
    echo "Testing full system without bloom filter on GPU"
    OPENCL_FULL "htmlResultsNoBloom/GPU"
    # No Bloom CPUGPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE
    mkdir -p htmlResultsNoBloom/CPUGPU
    echo "Testing full system without bloom filter on CPU and GPU"
    OPENCL_FULL_FORK "htmlResultsNoBloom/CPUGPU"
    # No Bloom CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsNoBloom/CPU
    echo "Testing full system without bloom filter on CPU"
    OPENCL_FULL "htmlResultsNoBloom/CPU"
    # Bloom GPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p htmlResultsBloom/GPU
    echo "Testing full system with bloom filter on GPU"
    OPENCL_FULL "htmlResultsBloom/GPU"
    # Bloom CPUGPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p htmlResultsBloom/CPUGPU
    echo "Testing full system with bloom filter on CPU and GPU"
    OPENCL_FULL_FORK "htmlResultsBloom/CPUGPU"
    # Bloom CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloom/CPU
    echo "Testing full system with bloom filter on CPU"
    OPENCL_FULL "htmlResultsBloom/CPU"
    # BloomAll0 GPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p htmlResultsBloomAll0/GPU
    echo "Testing full system with bloom filter all zeroes on GPU"
    OPENCL_FULL "htmlResultsBloomAll0/GPU"
    # BloomAll0 CPUGPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p htmlResultsBloomAll0/CPUGPU
    echo "Testing full system with bloom filter all zeroes on CPU and GPU"
    OPENCL_FULL_FORK "htmlResultsBloomAll0/CPUGPU"
    # BloomAll0 CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloomAll0/CPU
    echo "Testing full system with bloom filter all zeroes on CPU"
    OPENCL_FULL "htmlResultsBloomAll0/CPU"
    # BloomAll1 GPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p htmlResultsBloomAll1/GPU
    echo "Testing full system with bloom filter all ones on GPU"
    OPENCL_FULL "htmlResultsBloomAll1/GPU"
    # BloomAll1 CPUGPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p htmlResultsBloomAll1/CPUGPU
    echo "Testing full system with bloom filter all ones on CPU and GPU"
    OPENCL_FULL_FORK "htmlResultsBloomAll1/CPUGPU"
    # BloomAll1 CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloomAll1/CPU
    echo "Testing full system with bloom filter all ones on CPU"
    OPENCL_FULL "htmlResultsBloomAll1/CPU"
fi
# Manipa has the Intel Phi we're testing with
if [ "$HOSTNAME" = "manipa" ]; then
    # No Bloom Phi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC
    mkdir -p htmlResultsNoBloom/Phi
    echo "Testing full system without bloom filter on Phi"
    OPENCL_FULL "htmlResultsNoBloom/Phi"
    # No Bloom PhiPhi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI
    mkdir -p htmlResultsNoBloom/PhiPhi
    echo "Testing full system without bloom filter on Phi and Phi"
    OPENCL_FULL_FORK "htmlResultsNoBloom/PhiPhi"
    # Bloom Phi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p htmlResultsBloom/Phi
    echo "Testing full system with bloom filter on Phi"
    OPENCL_FULL "htmlResultsBloom/Phi"
    # Bloom PhiPhi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"'
    mkdir -p htmlResultsBloom/PhiPhi
    echo "Testing full system with bloom filter on Phi and Phi"
    OPENCL_FULL_FORK "htmlResultsBloom/PhiPhi"
    # BloomAll0 Phi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p htmlResultsBloomAll0/Phi
    echo "Testing full system with bloom filter all zeroes on Phi"
    OPENCL_FULL "htmlResultsBloomAll0/Phi"
    # BloomAll0 PhiPhi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'
    mkdir -p htmlResultsBloomAll0/PhiPhi
    echo "Testing full system with bloom filter all zeroes on Phi and Phi"
    OPENCL_FULL_FORK "htmlResultsBloomAll0/PhiPhi"
    # BloomAll1 Phi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p htmlResultsBloomAll1/Phi
    echo "Testing full system with bloom filter all ones on Phi"
    OPENCL_FULL "htmlResultsBloomAll1/Phi"
    # BloomAll1 PhiPhi
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVACC CPPFLAGS+=-DPHIPHI CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"'
    mkdir -p htmlResultsBloomAll1/PhiPhi
    echo "Testing full system with bloom filter all ones on Phi and Phi"
    OPENCL_FULL_FORK "htmlResultsBloomAll1/PhiPhi"
fi
# Togian has a 64 core CPU
if [ "$HOSTNAME" = "togian.dcs.gla.ac.uk" ]; then
    # No Bloom CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsNoBloom/AMDCPU
    echo "Testing full system without bloom filter on CPU"
    OPENCL_FULL "htmlResultsNoBloom/AMDCPU"
    # Bloom CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilter.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloom/AMDCPU
    echo "Testing full system with bloom filter on CPU"
    OPENCL_FULL "htmlResultsBloom/AMDCPU"
    # BloomAll0 CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloomAll0/AMDCPU
    echo "Testing full system with bloom filter all zeroes on CPU"
    OPENCL_FULL "htmlResultsBloomAll0/AMDCPU"
    # BloomAll1 CPU
    make clean
    make CPPFLAGS+=-DHTML_PARSE CPPFLAGS+=-DBLOOM_FILTER CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll1.raw\"' CPPFLAGS+=-DDEVCPU
    mkdir -p htmlResultsBloomAll1/AMDCPU
    echo "Testing full system with bloom filter all ones on CPU"
    OPENCL_FULL "htmlResultsBloomAll1/AMDCPU"
fi
python summary.py htmlResultsNoBloom
python summary.py htmlResultsBloom
python summary.py htmlResultsBloomAll0
python summary.py htmlResultsBloomAll1
