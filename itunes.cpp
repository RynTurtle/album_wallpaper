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
int compare_album_names(std::string spotify, std::string itunes){
    spotify = lowercase(spotify);
    itunes = lowercase(itunes);
    int success_rate = 0; // how many characters are the same 
    
    
    size_t minimum_amount = std::min(spotify.length(), itunes.length()); // finds the least amount of characters of the two words 
    size_t maximum_amount = std::max(spotify.length(), itunes.length());

    for (size_t i = 0; i < maximum_amount; ++i) {
        if (i < minimum_amount){ 
            if (spotify[i] == itunes[i]) {
                success_rate++;
            }
        }else{ // extra words that one sentence doesnt have
            success_rate--;
        }
    }    
    float percentage = static_cast<float>(success_rate) / minimum_amount * 100;
    if (percentage < 0){
        return 0;
    }
    return percentage;
}


nlohmann::json search(nlohmann::json list_of_dicts,std::string key, std::string value){
    std::vector<nlohmann::json> possible;
    for (auto d: list_of_dicts){
        if  (d.contains(key)){
            auto itunes_album = lowercase(d[key]);
            auto spotify_album = lowercase(value);
            // find based by accuracy instead of common replaced words 

            //std::cout << "itunes: "  << itunes_album << "spotify: " << spotify_album <<  "\n" << d << "\n";
            auto percentage= compare_album_names(spotify_album,itunes_album);        
            d["percentage"] = percentage; 
            if (percentage > 50){
                possible.push_back(d);
            }
            
        }
    }

    return possible;
}



    
int Itunes::get_id(std::string artist){
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

nlohmann::json Itunes::get_albums(std::string artist_id){
    std::stringstream url;
    url  << "https://itunes.apple.com/lookup?id=" << artist_id << "&entity=album&sort=recent&limit=200&country=gb";
    /*  first result is info regarding the artist e.g.
    {"amgArtistId":353484,"artistId":2715720,"artistLinkUrl":"https://music.apple.com/gb/artist/kanye-west/2715720?uo=4","artistName":"Kanye West","artistType":"Artist","primaryGenreId":18,"primaryGenreName":"Hip-Hop/Rap","wrapperType":"artist"}
        the rest is the artists albums 


        for some reason specifying album also returns singles, itunes specify what they are e.g. - Single and - EP
        spotify doesnt do this
    */
    nlohmann::json results = get_request(url.str())["results"];

    // remove the first result (its not an album) e.g. {"amgArtistId":2377309,"artistId":479756766,"artistLinkUrl":"https://music.apple.com/gb/artist/the-weeknd/479756766?uo=4","artistName":"The Weeknd","artistType":"Artist","primaryGenreId":15,"primaryGenreName":"R&B/Soul","wrapperType":"artist"}
    if (!results.empty()) {
        results.erase(results.begin());
    }    

    std::cout << url.str() << "\n";
    return results;
}


std::stringstream Itunes::uncompressed(nlohmann::json album_data){
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

std::vector<nlohmann::json> Itunes::find_album(std::string spotify_album_name,nlohmann::json itunes_artist_albums){
    spotify_album_name = lowercase(spotify_album_name);
    std::vector<std::string> spotify_possible_varients;
    std::vector<nlohmann::json> all_results;
    auto potential_result = search(itunes_artist_albums,"collectionName",spotify_album_name);
    if (potential_result.size() > 0){
        for (auto a : potential_result){
            if (all_results.empty()){
                all_results.push_back(a);
            }

            auto  b = all_results.back()["percentage"];
            if (a["percentage"] > b){
                all_results.pop_back(); 
                all_results.push_back(a);
                    
            }                                  
        }   
    }
    return all_results;
}

