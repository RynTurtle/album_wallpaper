#include "header.h"
#include <iostream>
#include <filesystem>
#include <random>
#include <unistd.h> // for sleep()
#include <windows.h>
#include <fstream>
#include <nlohmann/json.hpp>


// todo - will need to add a search for singles/compilation/ep function, not sure if i still want this feature but its here if i change my mind  
void current_albums(){

} 

void change_wallpaper(std::string filename){


}


void download_wallpaper(std::string url,std::string wallpaper_name){
    /*  
    Wallpapers
        => Temp
            Spotify
                filename.jpg
            Itunes 
                filename.jpg
        
        =>  Finished - after added then remove temp files
            filename.jpg

    */
    if (!std::filesystem::exists("./Wallpapers")){
        std::filesystem::create_directory("./Wallpapers");

    }

    if (!std::filesystem::exists("./Wallpapers/Temp")){
        std::filesystem::create_directory("./Wallpapers/Temp");

    }

    if (!std::filesystem::exists("./Wallpapers/Finished")){
        std::filesystem::create_directory("./Wallpapers/Finished");        
    }
    if (!std::filesystem::exists("./Wallpapers/Temp/" + wallpaper_name) && !std::filesystem::exists("./Wallpapers/Finished/" + wallpaper_name)){
        std::cout << "downloading album art into ./Wallpapers/Temp/" << wallpaper_name << "\n";
        download_url(url,"./Wallpapers/Temp/" + wallpaper_name);
        // download url into temp because its not in finished aswell 

    if (!std::filesystem::exists("./Wallpapers/Finished/" + wallpaper_name)){
        std::cout << "downloading wallpaper using ffmpeg into ./Wallpapers/Finished/" << wallpaper_name << "\n";
        // download ffmpeg wallpaper  
        create_wallpaper(wallpaper_name);


        // remove file from temp folder 
        for (const auto& entry : std::filesystem::directory_iterator("./Wallpapers/Temp/")) {
            std::filesystem::remove(entry.path());
        }

    }}
}



void most_liked_albums(){ // displays albums with the most amount of likes (how many times they appear in the liked list) maybe show the top 

}



bool find_and_download(std::unordered_map<std::string, std::string> s_album, std::string wallpaper_name){
    int artist_id = Itunes().get_id(s_album["artist"]);
    if (artist_id == 1){
        std::cout << "Couldn't find artist: "  << s_album["artist"] << "\n";  
        return false;
    }else{
        std::cout << "getting album: " << "\n";
        auto albums = Itunes().get_albums(std::to_string(artist_id));

        std::cout << "searching for album: " << "\n";
        auto found = Itunes().find_album(s_album["name"],albums);
        if (found.size() > 0){
            auto u = Itunes().uncompressed(found[0]);
            download_wallpaper(u.str(),wallpaper_name);
            std::cout << s_album["name"] << " " << found[0]["collectionName"] << " " << s_album["image"] << " " << u.str() <<  " " <<found[0]["percentage"]<< "% certain"  << "\n";
            return true;
        }else{
            std::cout << "couldn't find " << s_album["name"] << "\n";
            return false;
        }
    }
    
}



void random_albums(int sleep_amount){
    refresh_access();
    std::vector<std::unordered_map<std::string, std::string>> spotify_albums = get_unique_albums();
    // Shuffle the vector
    std::random_device rd; // random seed
    std::mt19937 g(rd()); // uses seed to shuffle vector
    std::shuffle(spotify_albums.begin(), spotify_albums.end(), g);

    for (auto s_album:spotify_albums){
        if (s_album["album_type"] == "album"){
            std::string wallpaper_name = s_album["artist_id"] + "-" + s_album["album_id"] + ".jpg";
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::wstring wide_wallpaper_name = converter.from_bytes(wallpaper_name);
            std::filesystem::path absolute_path = std::filesystem::absolute("Wallpapers/Finished/");
            std::wstring wide_p = absolute_path.wstring();
            std::wstring path = wide_p + wide_wallpaper_name;
            
            bool success = true;
            if (!std::filesystem::exists(path)){
                success = find_and_download(s_album,wallpaper_name);
            }
            
            if (success == true){
                // change wallpaper 
                SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void*)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
                std::wcout << "changing wallpaper -> " <<  path << "\n";
                sleep(sleep_amount); 
            }
        }
    }

}


void check_tokens_and_verify_authentication(){
    //  check if tokens.json exists, if it isnt then create it and write "access_token" "client_id" "client_secret" "refresh_token" with empty fields 
    if (!std::filesystem::exists("./tokens.json")){
        std::cout << "tokens file doesnt exist, creating file \n"; 
        // Create JSON object
        nlohmann::json root;
        root["access_token"] = "";
        root["client_id"] = "";
        root["client_secret"] = "";
        root["refresh_token"] = "";

        // Write JSON to file
        std::ofstream file("./tokens.json");
        if (file.is_open()) {
            file << std::setw(4) << root << std::endl;
            file.close();
            std::cout << "JSON file created successfully.\n";
        } else {
            std::cerr << "Unable to create JSON file.\n";
        }
    }

    //  prompt user to enter client id and client secret 
    if (get_token("client_id") == ""){
        std::cout<< "You haven't got the required fields client_id, find this in the spotify  dashboard after creating an app https://developer.spotify.com/dashboard \n";
        std::string client_id;        
        std::cout << "enter your spotify client id \n";
        std::cin >> client_id;
        write_token("client_id",client_id);

    }

    if (get_token("client_secret") == ""){
        std::cout<< "You haven't got the required fields client_secret, find this in the spotify  dashboard after creating an app https://developer.spotify.com/dashboard \n";
        std::string client_secret;
        std::cout << "enter your spotify client secret  \n";
        std::cin >> client_secret;
        write_token("client_secret",client_secret);
    }


    //  prompt user to open the spotify authentication site and paste the authorization code 
    if (get_token("access_token") == "" && get_token("refresh_token") == ""){
        std::string authorization_code;
        std::cout << "Authorize the application to access spotify permisisons for the album wallpaper to work, when redirected copy paste the code from the url after code= \n ";
        // prints link to authorize spotify application
        Spotify().authorize_url();
        std::cin >> authorization_code;
        Spotify().get_tokens(authorization_code); 
    }

}


int main() {
    check_tokens_and_verify_authentication();
    
    while (true){
        random_albums(20);
    }
}
