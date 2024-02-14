#include "header.h"
#include <iostream>
#include <filesystem>
#include <random>
#include <unistd.h> // for sleep()
#include <windows.h>


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

int main() {
    /*  check if spotify has the right credentials in the file 
        if not then start the process for the user to add in their credentials input() and get auth tokens 

        create wallpaper folders if its not already created 
    */

    //while (true){
    random_albums(20);
        //printf("loop \n");
    //}
}
