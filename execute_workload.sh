#!/bin/bash
echo -e "I_O \t\tFile_Size_MB\t\tBuffer_size_MB \t\tWrite_Count \t\tRead_Count \t\tTime_Taken_s" > output_updateHeavy_small.txt

#List of I/Os to be called
io=(Posix Mmap Ummap)

#List of buffer size to be called
buf=(10m 20m 40m 60m)
#buf=(100m 200m 300m)



#Update Heavy Workload
equalCount=(1 5 10)

for ((i=0; i< ${#buf[*]}; i++ ))
do
    for ((j=0; j< ${#equalCount[*]}; j++ ))
    do
        for ((l=0; l< ${#io[*]}; l++ ))
        do
            rm -f test.txt
            build/./generator -client "${io[$l]}" -fn test.txt -bs "${buf[$i]}" -wcnt " ${equalCount[$j]}" -rcnt ${equalCount[$j]} >> output_updateHeavy_small.txt
            
            if [ -e "mmap_read.txt" ]; then
                rm mmap_read.txt
            fi
            if [ -e "umalloc_read.txt" ]; then
                rm umalloc_read.txt
            fi
        done
    done
done


#Read Heavy
echo -e "I_O \t\tFile_Size_MB\t\tBuffer_size_MB \t\tWrite_Count \t\tRead_Count \t\tTime_Taken_s" > output_readHeavy_small.txt


#List of write count to be called
wcnt=(1 2 3)

#List of read count to be called
rcnt=(5 10 15)

for ((i=0; i< ${#buf[*]}; i++ ))
do
    for ((j=0; j< ${#wcnt[*]}; j++ ))
    do
      for ((k=0; k<  ${#rcnt[*]}; k++ ))
        do  
            for ((l=0; l< ${#io[*]}; l++ ))
            do
                rm -f test.txt
                build/./generator -client "${io[$l]}" -fn test.txt -bs "${buf[$i]}" -wcnt " ${wcnt[$j]}" -rcnt ${rcnt[$k]} >> output_readHeavy_small.txt
                if [ -e "mmap_read.txt" ]; then
                   rm mmap_read.txt
                fi
                if [ -e "umalloc_read.txt" ]; then
                   rm umalloc_read.txt
                fi
                
            done
        done
    done
done

rm -f test.txt