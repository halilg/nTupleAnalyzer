#include <string>
#include <vector>
#include <json/json.h>

class cfgreader {
  public:
    cfgreader (std::string);
    std::vector<std::string> datasets;
    std::vector<std::string> categories;
    std::string json_path;
    Json::Value root;
};
