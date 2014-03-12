Level4Project
=============

Level 4 Project - Document Filtering using OpenCL

Requirements
============

OpenCL SDK for target device installed.

Parsed collection 'collection.raw' or plain text documents 'TREC_ALL.txt'.

Profile of interesting terms 'profile.bin'.

Bloom filter 'bloomfilter.raw' (optional)

List of binaries
================

cpu_reference_implementation - Single threaded C++ scorer

cpu_reference_implementation_mt - Multi threaded C++ scorer

cpu_implementation_no_wrapper - OpenCL CPU scorer

gpu_implementation_no_wrapper - Accelerator device scorer

opencl_full - OpenCL Parser (all devices)

opencl_full_fork - OpenCL Parser dual device (all devices)

Default options
===============

No bloom filter.

opencl_full_fork runs on CPU and GPU.

opencl_full runs on GPU.

Other binaries run on CPU.

Score 'collection.raw'.

Parse 'TREC_all.txt'.

Compiling the code
================

\> make

Compiles all binaries for the default options.

\> make CPPFLAGS+=-DDEVx (where x = CPU, GPU, or ACC)

Compiles all binaries and, for non-cpu-prefixed binaries, changes the target
device to either CPU, GPU, or ACC.

\> make CPPFLAGS+=-DHTML_PARSE

Compiles all binaries, opencl_full and opencl_full_fork now target the WARC
file containing HTML response headers and bodies to parse and score instead.

\> make CPPFLAGS+=-DBLOOM_FILTER

Compiles all binaries to use the bloom filter. Default bloom filter file is
'bloomfilter.raw'.

\> make CPPFLAGS+=-DBLOOM_FILTER
CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterNew.raw\"'

Compiles all binaries to use the bloom filter. Overrides bloom filter file to
'bloomfilterNew.raw'.

Applies also to COLLECTION_FILE for the parsed terms input file, DOCUMENT_FILE
for the plain text input file, and HTML_FILE for the WARC file.

\> make CPPFLAGS+=-DDEVCPU CPPFLAGS+=-DBLOOM_FILTER
CPPFLAGS+=-DBLOOM_FILTER_FILE='\"bloomfilterAll0.raw\"'

Compiles all devices to use the CPU with the bloomfilterAll0.raw bloom filter.

Running the code
================

All binaries can run without arguments and will load the profile 'profile.bin'.

All binaries can take an optional argument for the profile name.
