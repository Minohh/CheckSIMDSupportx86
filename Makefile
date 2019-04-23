#all : SIMD_Support.asm main.c
	#gcc -c main.c -o main.o
	#yasm -f elf32 -Worphan-labels SIMD_Support.asm -o SIMD_Support.o
	#gcc main.o SIMD_Support.o -o main

C_SIMD: SIMD_Support.c
	gcc -g -o SIMD_Support SIMD_Support.c

.PHONY : clean
clean :
	rm -rf *.o
	rm -rf main
	rm -rf SIMD_Support
