/*
    itunes used for getting the uncompressed highest quality image possible, spotify only provides a maximum of 640x640 

*/
#include "header.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

std::string replace_spaces(const std::string& input) {
    std::string result;
    for (char ch : input) {
        if (ch == ' ') {
            result += "%20";
        } else {
            result += ch;
        }
    }

    return result;
}


class Itunes {
    public:
        int get_id(std::string artist){
            // returns artist id
                std::stringstream url;
                url  << "https://itunes.apple.com/search?term=" << artist <<  "&entity=allArtist";
                nlohmann::json results = request(replace_spaces(url.str()))["results"];
                // Iterate through the JSON array
                for (const auto& element : results) {
                    // Access individual values using the keys

                    std::string name = element["artistName"];
                    int artist_id = element["artistId"];
                    
                    if (name == artist){
                        return artist_id;
                    }

                }
            return 1;
        }

        auto get_albums(std::string artist_id){
            std::stringstream url;
            url  << "http://itunes.apple.com/lookup?id=" << artist_id << "&entity=album&sort=recent&limit=200&country=gb";
            /*  first result is info regarding the artist e.g.
            {"amgArtistId":353484,"artistId":2715720,"artistLinkUrl":"https://music.apple.com/gb/artist/kanye-west/2715720?uo=4","artistName":"Kanye West","artistType":"Artist","primaryGenreId":18,"primaryGenreName":"Hip-Hop/Rap","wrapperType":"artist"}
                the rest is the artists albums 


                for some reason specifying album also returns singles, itunes specify what they are e.g. - Single and - EP
                spotify doesnt do this
            */
            nlohmann::json results = request(replace_spaces(url.str()))["results"];
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
};


int main() {
    int artist_id = Itunes().get_id("Kendrick Lamar");
    if (artist_id == 1){
        std::cout << "Couldn't find artist";
    }else{
        std::cout << "Artist Id: " << artist_id;
    } 


    auto albums = Itunes().get_albums(std::to_string(artist_id));
    std::cout << albums << "\n";
    auto u = Itunes().uncompressed(albums[1]).str();
    std::cout << u << "\n";


};
