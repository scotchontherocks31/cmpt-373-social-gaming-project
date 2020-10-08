//include files
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohman::json;
//the class is suposed to take a JSON file and
//output an object of json library
//not sure it is correct tho

class JSON_to_internal_JSON {
private:
    json data;
public:
    std::string getJson(std::string file_name);
    json processJson(std::string file_name);
};