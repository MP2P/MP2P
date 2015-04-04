#include "main.hh"

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage:\n"
      << "\ttests <log_type> <nb_threads> <nb_messages>\n"
      << "\nlog_type:\n"
      << "\t0: Naive mutex implementation (the current implementation).\n"
      << "\t1: An Herb Sutter's logger, using a mutex.\n"
      << "\t2: An Herb Sutter's logger, using a lock-free data structure.\n"
      << "\nnb_threads:\n"
      << "\tThe number of threads to start.\n"
      << "\nnb_messages:\n"
      << "\tThe number of messages per thread to print.\n"
      << "\nYou should \"time\" it!" << std::endl;
    return 0;
  }
  unsigned log_type = atoi(argv[1]);
  unsigned nb_threads = atoi(argv[2]);
  unsigned nb_messages = atoi(argv[3]);
  std::forward_list<std::thread> threads;
  switch (log_type)
  {
    case 0:
      for (unsigned i = 0; i < nb_threads; i++)
        threads.emplace_front(std::thread(naive_caller, nb_messages, i));
      std::for_each(threads.begin(), threads.end(), [](std::thread &t)
          {
          t.join();
          });
      break;
    case 1:
      for (unsigned i = 0; i < nb_threads; i++)
        threads.emplace_front(std::thread(classic_caller, nb_messages, i));
      std::for_each(threads.begin(), threads.end(), [](std::thread &t)
          {
          t.join();
          });
      break;
    default:
      //for (unsigned i = 0; i < nb_threads; i++)
        //threads.emplace_front(std::thread(lock_free_caller, nb_messages, i));
      //std::for_each(threads.begin(), threads.end(), [](std::thread &t)
          //{
          //t.join();
          //});
      break;
  }
  return 0;
}
