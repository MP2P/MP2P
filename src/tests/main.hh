#include <iostream>
#include <forward_list>
#include <thread>
#include <algorithm>
#include <mutex>
#include "color.hh"

int main(int argc, char *argv[]);
void naive_printer(std::ostream &out, std::mutex &wmutex, const std::string &msg);
void naive_caller(unsigned nb_messages, unsigned thread_nb);
