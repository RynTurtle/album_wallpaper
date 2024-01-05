/*
    itunes used for getting the uncompressed highest quality image possible, spotify only provides a maximum of 640x640 

*/

#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


auto request(std::string url){
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the target URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle received data
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // Perform the GET request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Get the HTTP response code
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::cout << "HTTP Response Code: " << httpCode << std::endl;
            
            // parse json 
            nlohmann::json json = nlohmann::json::parse(response);
            return json;
        }
        // Cleanup
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL." << std::endl;
    
    }

    // if nothing is returned then give an error 
    throw;
}

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
