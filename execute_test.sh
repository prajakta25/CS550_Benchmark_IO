#!/bin/bash
echo -e "I_O \t\tBuffer_Size_KB \t\tWrite_Count \t\tRead_Count \t\tTime_Taken_s" > output.txt

#List of I/Os to be called
io=(Posix Mmap Ummap)

#List of buffer size to be called
buf=(1k 8k 64K 1m 8m 16m)

#List of write count to be called
wcnt=(1 2 4 6 8 10)

#List of read count to be called
rcnt=(1 2 4 6 8 10)

for ((i=0; i< ${#io[*]}; i++ ))
do
    for ((j=0; j< ${#buf[*]}; j++ ))
    do
      for ((k=0; k< ${#wcnt[*]}; k++ ))
        do  
            for ((l=0; l< ${#rcnt[*]}; l++ ))
            do
            if [ "${wcnt[$l]}" -ge "${rcnt[$k]}" ]; then
                build/./generator -client "${io[$i]}" -fn test.txt -bs "${buf[$j]}" -wcnt " ${wcnt[$l]}" -rcnt ${rcnt[$k]} >> output.txt
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
