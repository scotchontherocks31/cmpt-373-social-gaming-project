//include files
#include <string.h>
#include <nlohmann/json.hpp>

using json = nlohman::json;
//the class is suposed to take a JSON file and
//output a python like dict of correpsponding strings
//not sure it is correct tho

class JSON_to_internal_JSON {
public:
    class JSON_dict_node {
    public:
        String first;
        String second;
    };
};