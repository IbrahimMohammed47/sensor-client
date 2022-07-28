build:client_runner

client_runner:
	g++ -Wall -o build/client_runner.o src/client_runner.cpp

clean:
	rm -rf build/*o

build_tests:
	g++ tests/tests.cpp -o build/tests.o /usr/src/gtest/src/gtest_main.cc /usr/src/gtest/src/gtest-all.cc -I /usr/include -I /usr/src/gtest -L /usr/local/lib -lpthread