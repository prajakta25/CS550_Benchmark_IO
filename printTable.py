#!/usr/bin/python

from prettytable import from_csv
    
#input file
fin = open("output.txt", "rt")
#output file to write the result to
fout = open("out.txt", "wt")
#for each line in the input file
for line in fin:
	#read replace the string and write to output file
	fout.write(line.replace('\t\t', ','))
#close input and output files
fin.close()
fout.close()

with open("out.txt", "r") as fp: 
    x = from_csv(fp)
    
print(x)