./runProfileAll.sh
cd experimentRun
mkdir E5462\ Reference
mkdir E5462\ Multi-Threaded\ Reference
mkdir E5462\ OpenCL
mkdir Tesla\ C2075
mv gpu*.txt Tesla\ C2075
mv cpuRefMT*.txt E5462\ Multi-Threaded\ Reference
mv cpuRef*.txt E5462\ Reference
mv cpu*.txt E5462\ OpenCL
cd ..
python summary.py experimentRun
mv experimentRun experimentRunBloomAll0MarkII
scp -r experimentRunBloomAll0MarkII gordonreid@86.14.186.202:~/Level4Project/Code/
