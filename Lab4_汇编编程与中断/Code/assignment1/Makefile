main.out:main.o c_func.o cpp_func.o asm_utils.o
	g++ -o main.out main.o c_func.o cpp_func.o asm_utils.o -m32

main.o:main.cpp
	g++ -o main.o -c main.cpp -m32

c_func.o:c_func.c
	gcc -o c_func.o -c c_func.c -m32

cpp_func.o:cpp_func.cpp
	g++ -o cpp_func.o -c cpp_func.cpp -m32

asm_utils.o:asm_utils.asm
	nasm -o asm_utils.o asm_utils.asm -f elf32
