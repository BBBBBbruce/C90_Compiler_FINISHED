Language Processor Coursework

By Szymon Kobus & Zhiyuan Zhang


Manual:

Build program:
make bin/c_compiler

Run translator:
bin/c_compiler --translate [source-file.c] -o [dest-file.py]

Similarly, run compiler:
bin/c_compiler -S [source-file.c] -o [dest-file.s]

The test suite provide a automated system to test only compiler.
running command:
./c_compiler_testsuite.sh
Inside the script file, change the $input_dir to different path for different tests cases, default to "test_deliverable/test_cases"
