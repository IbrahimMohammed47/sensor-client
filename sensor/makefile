build:sensor_runner
sensor_runner:
	g++ -Wall -o build/sensor_runner.o src/sensor_runner.cpp

clean:
	rm -rf build/*o

build_tests:
	g++ tests/tests.cpp -o build/tests.o /usr/src/gtest/src/gtest_main.cc /usr/src/gtest/src/gtest-all.cc -I /usr/include -I /usr/src/gtest -L /usr/local/lib -lpthread