buildTest:
	gcc -std=c11 -o sampleTest instructorList.o sampleTest.c -Wall -Werror
all:
	# gcc instructorList.o s-talk.c -lpthread printMsg.c -o s-talk
	# gcc s-talk-client.c -o s-client
	gcc instructorList.o test.c -lpthread -o test
clean:
	rm -f test

run3333:
	valgrind ./test 3333 ubuntu 6666

run6666: 
	valgrind ./test 6666 ubuntu 3333

runCat3333:
	cat someTestData.txt ./test 3333 ubuntu 6666

runOutPutTo6666:
	./test 6666 ubuntu 3333 >> outPut.txt


