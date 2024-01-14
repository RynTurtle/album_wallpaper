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


nlohmann::json is_in_json(nlohmann::json list_of_dicts,std::string key, std::string value){
    int i = 0;
    for (auto d: list_of_dicts){
        if  (d.contains(key)){
            if (lowercase(d[key]) == lowercase(value)){
                return d;
            }
        }
        i++;
    }
    return false;
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




    /* reasons why it cant find the album:
        1. spotify has added brackets for special anniversary albums 
        2. the single release was on spotify but not on apple
        3. completely different album name formats  e.g. sptotify = Black Panther The Album Music From And Inspired By, apple =  Black Panther: The Album
    */
    nlohmann::json find_album(std::string spotify_album_name,nlohmann::json itunes_artist_albums){
        spotify_album_name = lowercase(spotify_album_name);
        std::vector<std::string> possible_varients;

        possible_varients.insert(possible_varients.end(), {
            spotify_album_name,
            spotify_album_name + " - single",
            spotify_album_name + " - ep" 

        });

        for (std::string album_varients: possible_varients){

            auto is_in = (is_in_json(itunes_artist_albums,"collectionName",album_varients));
            if  (is_in !=  false){
                return is_in;
            }
        }
        std::cout << "couldn't find: " << spotify_album_name << "\n";
        return 0;
    }

};


int main() {


    refresh_access();
    std::vector<std::unordered_map<std::string, std::string>> album = get_unique_albums();
    for (auto album : album) {        
        int artist_id = Itunes().get_id(album["artist"]);
        if (artist_id == 1){
            std::cout << "Couldn't find artist: "  << album["artist"] << "\n";
        }else{
            //std::cout << album["name"] << " " << album["artist"] << " id: " << artist_id << "\n";

            auto albums = Itunes().get_albums(std::to_string(artist_id));
            std::cout << Itunes().find_album(album["name"],albums) << "\n";
        } 
        



    }

    /*
    int artist_id = Itunes().get_id("Kendrick Lamar");
    auto albums = Itunes().get_albums(std::to_string(artist_id));
    Itunes().find_album("Mr. Morale & The Big Steppers",albums);
    //std::cout << u << "\n";
    */
};
