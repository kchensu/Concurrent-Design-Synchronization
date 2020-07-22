buildTest:
	gcc -std=c11 -o sampleTest instructorList.o sampleTest.c -Wall -Werror
all:
	# gcc instructorList.o s-talk.c -lpthread printMsg.c -o s-talk
	# gcc s-talk-client.c -o s-client
	gcc instructorList.o test.c -lpthread -o test
clean:
	rm -f test

# chage names to your specific machines	
# done so we dont haveto type the commands
# numbers just so we know host port
run3333:
	valgrind ./test 4444 kevin-Ubuntu-System 6666

run6666: 
	./test 6666 kevin-Ubuntu-System 4444

runCat3333:
	valgrind cat someTestData.txt | ./test 4444 kevin-Ubuntu-System 6666

runOutPutTo6666:
	valgrind ./test 6666 kevin-Ubuntu-System 4444 >> outPut.txt


