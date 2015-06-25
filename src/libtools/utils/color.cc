#include <utils.hh>

namespace utils
{
  namespace color
  {
    static const std::string WHITE = "\033[0m";
    static const std::string RED = "\033[91m";
    static const std::string GREEN = "\033[92m";
    static const std::string YELLOW = "\033[93m";
    static const std::string BLUE = "\033[94m";
    static const std::string PURPLE = "\033[95m";
    static const std::string CYAN = "\033[96m";

    std::ostream& w(std::ostream& o)
    {
      return o << WHITE;
    }

    std::ostream& r(std::ostream& o)
    {
      return o << RED;
    }

    std::ostream& b(std::ostream& o)
    {
      return o << BLUE;
    }

    std::ostream& c(std::ostream& o)
    {
      return o << CYAN;
    }

    std::ostream& g(std::ostream& o)
    {
      return o << GREEN;
    }

    std::ostream& y(std::ostream& o)
    {
      return o << YELLOW;
    }

    std::ostream& p(std::ostream& o)
    {
      return o << PURPLE;
    }
  }
}
