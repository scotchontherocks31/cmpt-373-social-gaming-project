#include "JSON_to_internal_JSON.h"

std::string JSON_to_internal_JSON::getJson(std::string file_name) {
    std::string file_contents;
    //open file
    ifstream in;
    in.open(file_name);
    //save JSON as a string
    std::string file_contents { istreambuf_iterator<char>(in), istreambuf_iterator<char>() }; //hint found at: https://www.reddit.com/r/learnprogramming/comments/3qotqr/how_can_i_read_an_entire_text_file_into_a_string/
    //return string
    return file_contents;
}

json JSON_to_internal_JSON::processJson(std::string file_name) {
    // call getJson func
    std::string file_data = getJson(file_name);
    //convert string to json object
    auto jsn = json::parse(file_data);
    //return the result
    return jsn;
}