#include <string>
#include <nlohmann/json.hpp>


std::string replace_spaces(const std::string& input);
nlohmann::json get_request(std::string url);
nlohmann::json post_request(std::string url,std::string paramaters);