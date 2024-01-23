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

    /* reasons why it cant find the album:
        15th Anniversary Deluxe - specific year anniversary deluxe, not sure how to battle this one 

    */
    nlohmann::json find_album(std::string spotify_album_name,nlohmann::json itunes_artist_albums){
        spotify_album_name = lowercase(spotify_album_name);
        std::vector<std::string> possible_varients;

        possible_varients.insert(possible_varients.end(), {
            spotify_album_name,
            spotify_album_name + " - single",
            spotify_album_name + " - ep",
            removeBrackets(spotify_album_name), // need to remove the last space which gets added from when you remove the brackets
            removeBrackets(spotify_album_name) + " (deluxe version)",
            removeBrackets(spotify_album_name) + " (deluxe Edition)",
            removeBrackets(spotify_album_name) + " (deluxe)",
            removeBrackets(spotify_album_name) + " (The Remaster)",

        });

        for (std::string album_varients: possible_varients){
            //std::cout << album_varients << "\n";
            auto is_in = (is_in_json(itunes_artist_albums,"collectionName",album_varients));
            if  (is_in !=  false){
                return is_in;
            }
        }
        std::cout << "couldn't find: " << spotify_album_name << "\n";
        //std::cout << itunes_artist_albums << "\n";

        return 0;
    }

};


int main() {


    refresh_access();
    std::vector<std::unordered_map<std::string, std::string>> a = get_unique_albums();
    for (auto album : a) {    

        if (album["album_type"] == "album"){
            int artist_id = Itunes().get_id(album["artist"]);
            if (artist_id == 1){
                std::cout << "Couldn't find artist: "  << album["artist"] << "\n";
            }else{

                auto albums = Itunes().get_albums(std::to_string(artist_id));
                
                Itunes().find_album(album["name"],albums);
            } 
        }else{
            std::cout << album["album_type"] << "\n";
        }


    }

    /*
    before - 28 

 a different kind of human ÔÇô step 2
couldn't find: b'day deluxe edition
couldn't find: r&g (rhythm & gangsta): the masterpiece
couldn't find: anjunabeats pres. super8 & tab 01
couldn't find: one more love
couldn't find: nothing but the beat ultimate
couldn't find: natty dread
couldn't find: kaya - deluxe edition
couldn't find: confrontation
couldn't find: life after death (2014 remastered edition)
couldn't find: tha blue carpet treatment
couldn't find: trilogy
couldn't find: kamikaze
couldn't find: g i r l
couldn't find: the documentary
couldn't find: strictly 4 my n.i.g.g.a.z...
couldn't find: the boy who flew to the moon (vol. 1)
couldn't find: black panther the album music from and inspired by

    after -  18
    */
};
