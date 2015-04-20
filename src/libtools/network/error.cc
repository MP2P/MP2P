#include <network.hh>

#include <libconfig.h++>

namespace network
{

  std::unordered_map<uint16_t, std::string> Error::errors;

  Error::Error(const ErrorType et)
      : status_{et}
  {
  }

  bool Error::initialize(const std::string& path)
  {
    libconfig::Config cfg;

    try
    {
      cfg.readFile(path.c_str());
    }
    catch (const libconfig::FileIOException& fioex)
    {
      utils::Logger::cerr() << "i/o error while reading error file.";
      return false;
    }
    catch (const libconfig::ParseException& pex)
    {
      utils::Logger::cerr() << "parse error at " + std::string(pex.getFile()) + ":" + std::to_string(pex.getLine())
          << " - " << pex.getError();
      return false;
    }

    utils::Logger::cout() << "Getting all errors... ";

    std::string msg;
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

  Error& Error::operator=(Error& e)
  {
    status_ = e.status_get();
    stream_.str(e.stream_get().str());
    return *this;
  }

  Error& Error::operator=(ErrorType e)
  {
    status_ = e;
    return *this;
  }

  // Put the parameter in stream_
  template<typename T>
  Error& Error::operator<<(const T& t)
  {
    stream_ << t;
    return *this;
  }

  Error& Error::operator<<(std::ostream& (*f)(std::ostream&))
  {
    stream_ << f;
    return *this;
  }

  std::ostream& operator<<(std::ostream& o, const Error& e)
  {
    return o << e.stream_get().str();
  }
}
