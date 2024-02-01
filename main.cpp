#include "header.h"
#include <iostream>
#include <filesystem>


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

    // adding .jpg because the file extension that apple gives can be something other than .jpg
    wallpaper_name = wallpaper_name + ".jpg";
    if (!std::filesystem::exists("./Wallpapers/Temp/" + wallpaper_name) && !std::filesystem::exists("./Wallpapers/Finished/" + wallpaper_name)){
        std::cout << "downloading album art into ./Wallpapers/Temp/" << wallpaper_name << "\n";
        download_url(url,"./Wallpapers/Temp/" + wallpaper_name);
        // download url into temp because its not in finished aswell 

    if (!std::filesystem::exists("./Wallpapers/Finished/" + wallpaper_name)){
        std::cout << "downloading wallpaper using ffmpeg into ./Wallpapers/Finished/" << wallpaper_name << "\n";
        // download ffmpeg wallpaper  
        create_wallpaper(wallpaper_name)
        ;
        // remove file from temp folder 
        
        std::filesystem::remove("./Wallpapers/Temp/" + wallpaper_name);
    }}


    std::cout << wallpaper_name << "\n"; 
}


void most_liked_albums(){ // displays albums with the most amount of likes (how many times they appear in the liked list) maybe show the top 

}



void random_albums(int sleep_amount){
    refresh_access();
    //int i = 0;
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
                auto found = Itunes().find_album(album["name"],albums);
                if (found.size() > 0){
                    auto u = Itunes().uncompressed(found[0]);
                    // change wallpaper 

                    size_t last_slash = u.str().find_last_of('/');
                    std::string wallpaper_name = u.str().substr(last_slash + 1);
                    wallpaper_name = std::to_string(artist_id) + "-" + wallpaper_name;
                    download_wallpaper(u.str(),wallpaper_name);


                    std::cout << album["name"] << " " << found[0]["collectionName"] << " " << album["image"] << " " << u.str() << " "  << "\n";
                }else{
                    std::cout << "couldn't find " << album["name"] << "\n";
                }
            } 
        }
        // sleep
    }
   // std::cout << "found " << i << " albums" << "\n";
}




int main() {
    /*  check if spotify has the right credentials in the file 
        if not then start the process for the user to add in their credentials input() and get auth tokens 

        create wallpaper folders if its not already created 
    */

    //while (true){
    random_albums(1);
    //}
}
