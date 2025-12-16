g++ -std=c++20 -O2 -Wall -Wextra bf.cpp -o bf && taskset -c 0 ./bf ./test.bf
