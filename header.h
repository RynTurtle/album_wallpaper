#include <string>
#include <nlohmann/json.hpp>
#include <variant>

std::string replace_spaces(const std::string& input);
nlohmann::json get_request(std::string url,bool header = false);
nlohmann::json post_request(std::string url,std::string paramaters);
int download_url(std::string url, std::string image_path);

std::string get_token(std::string token_type);
int write_token(std::string token_type, std::string value_wanted);

bool is_in_vec_dics(std::vector<std::unordered_map<std::string, std::string>> list_of_dicts,std::string key, std::string value);
nlohmann::json is_in_json(nlohmann::json list_of_dicts,std::string key, std::string value);

std::vector<std::unordered_map<std::string, std::string>> get_unique_albums();
void refresh_access();

int create_wallpaper(std::string filename);

class Itunes{
    public:
        int get_id(std::string artist);
        nlohmann::json get_albums(std::string artist_id);
        std::stringstream uncompressed(nlohmann::json album_data);
        std::vector<nlohmann::json> find_album(std::string spotify_album_name,nlohmann::json itunes_artist_albums);
};



class Spotify{
    public:
        void authorize_url();
        void get_tokens(std::string code);
        void refresh_access();
        std::vector<std::unordered_map<std::string, std::string>> get_unique_albums();


};