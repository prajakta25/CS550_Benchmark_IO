To compile & execute the code :

Step 1: Changes in CMakeLists to compile the code
- Make the following changes in CMakeLists.txt file
- On line 6 change SET(UMAP_LIB "/home/prajakta/CS550_Benchmark_IO/lib/libumap.so") to SET(UMAP_LIB "/your/path/to/CS550_Benchmark_IO/lib/libumap.so")
- On line 7 change SET(UMMAP_LIB "/home/prajakta/CS550_Benchmark_IO/lib/libumalloc.a") to SET(UMMAP_LIB "/your/path/to/CS550_Benchmark_IO/lib/libumalloc.a")

Step 2: Compile the code
- If build folder exists, delete the folder : 
    $rm -f build
- Then create a build folder: 
    $mkdir build
- Go to the build folder: 
    $cd build
- run cmake : 
    $cmake ../
- run make: 
    $make

Step 3: execute the code
$./generator -client Posix -fn test_file.txt -bs 1m -wcnt 1 -rcnt

To know more about the other options run:
$./generator -h