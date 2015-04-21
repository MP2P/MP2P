#include <utils.hh>

namespace Database {

  bool Database::Initialize()
  {
    const std::string host = utils::Conf::get_instance().DBhost_get();
  //  const unsigned port = utils::Conf::get_instance().DBport_get();
    const std::string password = utils::Conf::get_instance().DBpassword_get();
    const std::string bucket = utils::Conf::get_instance().DBbucket_get();

    const std::string conf("couchbase://" + host + "/" + bucket);

    client_ = new Couchbase::Client(conf, password);
    status_ = new Couchbase::Status(client_->connect());

    if (!status_->success())
    {
      utils::Logger::cerr() << "Couldn't connect to ' " + conf + "' ("
                               + std::string(*status_) + ").";
      return false;
    }

//    std::string storage_sizes = "{ \"values\": ["
//        "{\"file_size\": 2097152, \"nb_part\": 1 },"
//        "{\"file_size\": 4194304, \"nb_part\": 2 },"
//        "{\"file_size\": 8388608, \"nb_part\": 4 },"
//        "{\"file_size\": 16777216, \"nb_part\": 4 },"
//        "{\"file_size\": 33554432, \"nb_part\": 8 },"
//        "{\"file_size\": 67108864, \"nb_part\": 16 },"
//        "{\"file_size\": 134217728, \"nb_part\": 16 },"
//        "{\"file_size\": 268435456, \"nb_part\": 32 },"
//        "{\"file_size\": 536870912, \"nb_part\": 64 },"
//        "{\"file_size\": 1073741824, \"nb_part\": 64 },"
//        "{\"file_size\": 2147483648, \"nb_part\": 128 },"
//        "{\"file_size\": 4294967296, \"nb_part\": 256 },"
//        "{\"file_size\": 8589934592, \"nb_part\": 256 },"
//        "{\"file_size\": 17179869184, \"nb_part\": 512 },"
//        "{\"file_size\": 34359738368, \"nb_part\": 1024 },"
//        "{\"file_size\": 68719476736, \"nb_part\": 1024 },"
//        "{\"file_size\": 137438953472, \"nb_part\": 2048 },"
//        "{\"file_size\": 274877906944, \"nb_part\": 4096 },"
//        "{\"file_size\": 549755813888, \"nb_part\": 4096 },"
//        "{\"file_size\": 1099511627776, \"nb_part\": 8192 },"
//        "{\"file_size\": 2199023255552, \"nb_part\": 16384 },"
//        "{\"file_size\": 4398046511104, \"nb_part\": 16384 },"
//        "{\"file_size\": 8796093022208, \"nb_part\": 32768 },"
//        "{\"file_size\": 17592186044416, \"nb_part\": 65535 },"
//        "{\"file_size\": 35184372088832, \"nb_part\": 65535 },"
//        "{\"file_size\": 70368744177664, \"nb_part\": 131072 },"
//        "{\"file_size\": 140737488355328, \"nb_part\": 262144 },"
//        "{\"file_size\": 281474976710656, \"nb_part\": 262144 }"
//      "]}";
//    Couchbase::UpsertOperation scmd("storage_sizes", storage_sizes);
//    auto sres = scmd.run(*client_);
//    std::cout << "Got status for store. Cas=" << std::hex << sres.cas() << std::endl;

    return true;
  }

  Database::~Database()
  {
    if (client_ != NULL)
      delete client_;
    if (status_ != NULL)
      delete status_;
  }

  inline Database& Database::get_instance()
  {
    static Database instance;
    return instance;
  }

  bool Database::LoadJsonFile(std::string& path)
  {
    if (path != "")
      return false;
    return false;
  }
}