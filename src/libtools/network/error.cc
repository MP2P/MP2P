#include <network.hh>

namespace network
{
  Error::Error(const ErrorType et)
      : status_{et}
  {
  }

  Error::~Error()
  {
  }

  Error::ErrorType Error::status_get()
  {
    return status_;
  }

  const std::ostringstream &Error::stream_get() const
  {
    return stream_;
  }

  Error &Error::operator=(Error &e)
  {
    status_ = e.status_get();
    stream_.str(e.stream_get().str());
    return *this;
  }

  Error &Error::operator=(ErrorType e)
  {
    status_ = e;
    return *this;
  }

  // Put the parameter in stream_
  template<typename T>
  Error &Error::operator<<(const T &t)
  {
    stream_ << t;
    return *this;
  }

  Error &Error::operator<<(std::ostream &(*f)(std::ostream &))
  {
    stream_ << f;
    return *this;
  }

  std::ostream &operator<<(std::ostream &o, const Error &e)
  {
    return o << e.stream_get().str();
  }
}
