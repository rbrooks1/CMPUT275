# Name: Ryan Brooks
# ID: 1530605
# Course: CMPUT 275 winter 2019
# assignment: Weekly exercise 6: STL concepts

std = -std=c++11

all: complex_postfix.cpp student_hash.cpp
	@g++ complex_postfix.cpp -o calc $(std)
	@g++ student_hash.cpp -o student $(std)
student: student_hash.cpp
	@g++ student_hash.cpp -o student $(std)
calc: complex_postfix.cpp
	@g++ complex_postfix -o $(std)
student_hash.o: student_hash.cpp
	@g++ -o student_hash.o -c student_hash.cpp $(std)
complex_postfix.o: complex_postfix.cpp
	@g++ -o complex_postfix.o -c complex_postfix.cpp $(std)
clean:
	@rm -f calc
	@rm -f student
	@rm -f complex_postfix.o
	@rm -f student_hash.o