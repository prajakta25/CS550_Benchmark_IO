#!/bin/bash
echo -e "I_O \t\tFile_Size_MB\t\tBuffer_size_MB \t\tWrite_Count \t\tRead_Count \t\tTime_Taken_s" > output_ch_23Nov.txt

#List of I/Os to be called
io=(Posix Mmap Ummap)

#List of buffer size to be called
buf=(1m 10m 20m 30m 40m 50m)

#List of write count to be called
wcnt=(1 2 4 6 8 10)

#List of read count to be called
rcnt=(1 2 4 6 8 10)

for ((i=0; i< ${#buf[*]}; i++ ))
do
    for ((j=0; j< ${#wcnt[*]}; j++ ))
    do
      for ((k=0; k<  ${#rcnt[*]}; k++ ))
        do  
            for ((l=0; l< ${#io[*]}; l++ ))
            do
            if [ "${wcnt[$j]}" -ge "${rcnt[$k]}" ]; then
                build/./generator -client "${io[$l]}" -fn test.txt -bs "${buf[$i]}" -wcnt " ${wcnt[$j]}" -rcnt ${rcnt[$k]} >> output_ch_23Nov.txt
                rm test.txt
                if [ -e "mmap_read.txt" ]; then
                   rm mmap_read.txt
                fi
                if [ -e "umalloc_read.txt" ]; then
                   rm umalloc_read.txt
                fi
                
            fi
            done
        done
    done
done
