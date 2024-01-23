#include <string>
#include <nlohmann/json.hpp>
#include <variant>

std::string replace_spaces(const std::string& input);
nlohmann::json get_request(std::string url,bool header = false);
nlohmann::json post_request(std::string url,std::string paramaters);
std::string get_token(std::string token_type);
bool is_in_vec_dics(std::vector<std::unordered_map<std::string, std::string>> list_of_dicts,std::string key, std::string value);
nlohmann::json is_in_json(nlohmann::json list_of_dicts,std::string key, std::string value);

std::vector<std::unordered_map<std::string, std::string>> get_unique_albums();
void refresh_access();