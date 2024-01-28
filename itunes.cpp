/*
    itunes used for getting the uncompressed highest quality image possible, spotify only provides a maximum of 640x640 

*/
#include "header.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>


std::string lowercase(std::string s){
    std::transform(s.begin(), s.end(), s.begin(),[](unsigned char c) { return std::tolower(c); });
    return s;
}



// comparing the second sentance to the first one (spotify to apples)
// only downside to this is that if there is multiple versions of an album with different covers and you want a specific spotify album cover theres a chance you dont get the "edition" you want
int compare_album_names(std::string spotify, std::string itunes){
        spotify = lowercase(spotify);
        itunes = lowercase(itunes);
        int success_rate = 0; // how many characters are the same 
        
        
        size_t minimum_amount = std::max(spotify.length(), itunes.length());

        for (size_t i = 0; i < minimum_amount; ++i) {
            if (spotify[i] == itunes[i]) {
                success_rate++;
            }
            
        }
        float percentage = static_cast<float>(success_rate) / minimum_amount * 100;
        return percentage;
}


nlohmann::json search(nlohmann::json list_of_dicts,std::string key, std::string value){
    int i = 0;
    std::vector<nlohmann::json> possible;
    
    for (auto d: list_of_dicts){
        if  (d.contains(key)){
            auto itunes_album = lowercase(d[key]); 
            auto spotify_album = lowercase(value);
            // find based by accuracy instead of common replaced words 
            auto percentage= compare_album_names(spotify_album,itunes_album);
            
        
            d["percentage"] = percentage; 
            possible.push_back(d);
        }
        i++;
    }


    return possible;
}



class Itunes {
    public:
        int get_id(std::string artist){
            // returns artist id
                std::stringstream url;
                url  << "https://itunes.apple.com/search?term=" << artist <<  "&entity=allArtist";
                nlohmann::json results = get_request(url.str())["results"];
                // Iterate through the JSON array
                for (const auto& element : results) {
                    // Access individual values using the keys
                    std::string name = element["artistName"];
                    int artist_id = element["artistId"];
                    if (lowercase(name) == lowercase(artist)){
                        return artist_id;
                    }

                }

            return 1;
        }

        auto get_albums(std::string artist_id){
            std::stringstream url;
            url  << "https://itunes.apple.com/lookup?id=" << artist_id << "&entity=album&sort=recent&limit=200&country=gb";
            /*  first result is info regarding the artist e.g.
            {"amgArtistId":353484,"artistId":2715720,"artistLinkUrl":"https://music.apple.com/gb/artist/kanye-west/2715720?uo=4","artistName":"Kanye West","artistType":"Artist","primaryGenreId":18,"primaryGenreName":"Hip-Hop/Rap","wrapperType":"artist"}
                the rest is the artists albums 


                for some reason specifying album also returns singles, itunes specify what they are e.g. - Single and - EP
                spotify doesnt do this
            */
            nlohmann::json results = get_request(url.str())["results"];
            return results;
        }


        std::stringstream uncompressed(nlohmann::json album_data){
            // explanation of how it works https://bendodson.com/projects/itunes-artwork-finder/ https://github.com/bendodson/itunes-artwork-finder
            std::string original_url = album_data["artworkUrl100"];
            std::string no_size_url = original_url.replace(original_url.find("/100x100bb.jpg"),14,"");

            std::string url_to_remove = "https://is1-ssl.mzstatic.com/image/thumb/";
            std::string path = no_size_url.replace(no_size_url.find(url_to_remove),url_to_remove.size(),"");
            // left with image/thumb/Music126/v4/3e/95/fc/3e95fcb4-62de-ab55-66b6-24377eb56c15/cover.jpg
            // now i need to add the path to the new url 
            std::stringstream uncompressed_url;
            uncompressed_url << "https://a5.mzstatic.com/us/r1000/0/" << path;
            return uncompressed_url;
        }


    std::string removeBrackets(const std::string& input) {
        std::string result;
        bool insideBracket = false;

        for (char c : input) {
            if (c == '(') {
                insideBracket = true;
            } else if (c == ')') {
                insideBracket = false;
            } else if (!insideBracket) {
                if (c == '['){c = '(';};
                if (c == ']'){c = ')';}; 
                result += c;
            }
        }

        if (result.back() ==  *" "){
            // remove any spaces 
            result.pop_back();
            //std::cout << "removed space on string:" << result << "!"  << "\n";
        }

        return result;
    }


    std::vector<nlohmann::json> find_album(std::string spotify_album_name,nlohmann::json itunes_artist_albums){
        spotify_album_name = lowercase(spotify_album_name);
        std::vector<std::string> spotify_possible_varients;
    
        std::vector<nlohmann::json> all_results; 
//        for (std::string spotify_album_varients: spotify_possible_varients){
            //std::cout << album_varients << "\n";
            auto potential_result = (search(itunes_artist_albums,"collectionName",spotify_album_name));
            if (potential_result.size() > 0){
                for (auto a : potential_result){
                    if (all_results.empty()){
                        all_results.push_back(a);
                    }


                    auto  b = all_results.back()["percentage"];
                    //std::cout << " " << a["percentage"] << " " <<  b << " " << "\n";
                    if (a["percentage"] > b){
                        all_results.pop_back(); 
                        all_results.push_back(a);
                         
                    }                                  
                }   
            }
        //std::cout << all_results << "\n";
        return all_results;
    }

};


int main() {
    refresh_access();
    std::vector<std::unordered_map<std::string, std::string>> a = get_unique_albums();
    for (auto album : a) {    
        // the reason why i'm only searching for albums is because sometimes spotify and apple might have different releases for singles, sometimes spotify might classify something as a single whilst apple hasnt got it as a single 
        if (album["album_type"] == "album"){ 
            //std::cout << album["name"] << "\n";

            int artist_id = Itunes().get_id(album["artist"]);
            if (artist_id == 1){
                std::cout << "Couldn't find artist: "  << album["artist"] << "\n";
            }else{

                auto albums = Itunes().get_albums(std::to_string(artist_id));
                
                auto find = Itunes().find_album(album["name"],albums);
                if (find.size() > 0){
                    //std::cout << find.size() << "\n"; 
                    auto u = Itunes().uncompressed(find[0]);
                    std::cout << album["name"] << " " << find[0]["collectionName"] << " " << album["image"] << " " << u.str() << " "  << "\n";
                }else{
                    std::cout << "couldn't find " << album["name"] << "\n";

                }
            } 
        }else{
            std::cout << album["album_type"] << "\n";
        }


    }
}