buildTest:
	gcc -std=c11 -o sampleTest instructorList.o sampleTest.c -Wall -Werror
all:
	gcc s-talk.c -o s-talk
	gcc s-talk-client.c -o s-client
clean:
	rm -f s-talk -f s-client