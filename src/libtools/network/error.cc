#include <network.hh>

#include <libconfig.h++>

namespace network
{

  std::unordered_map<uint16_t, std::string> Error::errors;

  Error::Error(const ErrorType et)
      : status_{et}
  {
  }

  Error::~Error()
  {
  }

  bool Error::update_conf(const std::string &path)
  {
    libconfig::Config cfg;

    try
    {
      cfg.readFile(path.c_str());
    }
    catch (const libconfig::FileIOException &fioex)
    {
      //std::cerr << "i/o error while reading error file." << std::endl;
      utils::Logger::cerr() << "i/o error while reading error file.";
      return false;
    }
    catch (const libconfig::ParseException &pex)
    {
      //std::cerr << "parse error at " << pex.getFile() << ":" << pex.getLine()
          //<< " - " << pex.getError() << std::endl;
      utils::Logger::cerr() << "parse error at " << pex.getFile() << ":" + std::to_string(pex.getLine())
          << " - " << pex.getError();
      return false;
    }

    //std::cout << "Getting all errors... " << std::flush;
    utils::Logger::cout() << "Getting all errors... ";

    std::string msg{};
    for (uint16_t i = 0; i < UINT16_MAX; ++i)
    {
      msg = std::string("");
      cfg.lookupValue("errors.codes.id-" + std::to_string(i) + ".message", msg);
      if (msg == "")
        continue;
      errors.emplace(i, msg);
    }

    //std::cout << "Done!" << std::endl;
    utils::Logger::cout() << "Done!";
    return true;
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
