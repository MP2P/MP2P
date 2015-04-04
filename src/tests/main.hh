#include <iostream>
#include <forward_list>
#include <queue>
#include <thread>
#include <algorithm>
#include <mutex>
#include "color.hh"

int main(int argc, char *argv[]);
void naive_caller(unsigned nb_messages, unsigned thread_nb);
void classic_caller(unsigned nb_messages, unsigned thread_nb);
