#include <utils.hh>

#include <sstream>
#include <ostream>

namespace utils
{
  namespace misc
  {
    template <class C, typename S>
    inline Separator<C, S>::Separator(const C& c, const S& s)
    : container_{c}, separator_{s} {}

    template <class C, typename S>
    inline std::ostream& Separator<C, S>::operator()(std::ostream& o) const
    {
      using iter_type = typename C::const_iterator;
      const iter_type begin = container_.begin();
      const iter_type end = container_.end();
      for (auto i = begin; i != end; ++i)
      {
        o << *i;
        if (i != std::prev(end))
          o << separator_;
      }
      return o;
    }

    template <class C, typename S>
    inline Separator<C, S>
    separate(const C& c, const S& s)
    {
      return Separator<C, S>(c, s);
    }

    template <class C, typename S>
    inline std::ostream&
    operator<<(std::ostream& o, const Separator<C, S>& s)
    {
      return s(o);
    }

    template <typename T>
    std::string string_from(T value)
    {
      std::ostringstream ss;
      ss << value;
      return ss.str();
    }
  }
}
