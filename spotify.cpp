/*
    spotify api used to get liked songs and other info like the songs you are currently listening 

    . follow auth flow (getting access token and refresh using client secret/id)
        refresh
        https://accounts.spotify.com/api/token?grant_type=refresh_token&refresh_token={refresh_token}&client_id={client_id}&client_secret={client_secret}
        
        access
        https://accounts.spotify.com/api/token?grant_type=authorization_code&code={code}&redirect_uri={redirect_uri}&client_id={client_id}&client_secret={client_secret}
    
    
    . get liked albums and order them all 

    . dont really need to get other spotify data due to itunes album giving this 

    https://api.spotify.com/v1/me/player

    https://api.spotify.com/v1/me/tracks?limit=60&offset=0

    scopes: "user-library-read user-read-playback-state"
    headers: {"Authorization": "Bearer {access_token]}","Content-Type": "application/json "}
*/
#include "header.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <variant>  

// get the token wanted from json file
std::string get_token(std::string token_type){
    std::ifstream file("tokens.json");
    nlohmann::json data;
    file >> data;
    file.close();
    std::string token_wanted = data[token_type];
    return token_wanted;
}

// replace value of key from json file 
int write_token(std::string token_type, std::string value_wanted){
    std::ifstream file("tokens.json");
    nlohmann::json data;
    file >> data;
    file.close();

    data[token_type] = value_wanted;


    std::ofstream f("tokens.json");
    f << data.dump(4); // Use dump function to format with indentation
    f.close();
    return 0;
} 


std::string redirect_uri = "http://localhost:8888/callback";

// get CODE= value from giving application permissions, will send you to the spotify site to give permission if you already gave permission it will just go straight to the redirect uri  
void first(){
    std::string scopes = replace_spaces("user-library-read user-read-playback-state");
    std::cout <<  "https://accounts.spotify.com/authorize?client_id=" << get_token("client_id")  << "&response_type=code&redirect_uri=" << redirect_uri << "&scope=" <<  scopes << "\n";
}

// use code that was received after giving permission to app to get refresh token 
void get_tokens(std::string code){
    std::stringstream params;
    params<< "grant_type=authorization_code&code=" << code << "&redirect_uri=" << redirect_uri << "&client_id=" << get_token("client_id") << "&client_secret="<< get_token("client_secret");
    nlohmann::json result =  post_request("https://accounts.spotify.com/api/token",params.str());
    if (result.find("error") == result.end()){ // if error isnt found 
        // {access_token:"",refresh_token:""}
        write_token("access_token",result["access_token"]);
        write_token("refresh_token",result["refresh_token"]);
    }
}

// use refresh token to get new access token 
void refresh_access(){
    std::stringstream params; 
    params << "grant_type=refresh_token&refresh_token=" << get_token("refresh_token") << "&client_id=" << get_token("client_id") << "&client_secret=" << get_token("client_secret");
    nlohmann::json result =  post_request("https://accounts.spotify.com/api/token",params.str());
    if (result.find("error") == result.end()){
        std::cout << result << "\n";           
        write_token("access_token",result["access_token"]);
    }
}


// playback state 
void playback_state(){    
    nlohmann::json results = get_request("https://api.spotify.com/v1/me/player",true);
    std::cout << results << "\n";
}


//get all liked songs
std::vector<nlohmann::json> get_liked_songs(){
    // need to paginate to end of results 
    int page = 5;
    bool next_page = true;
    std::vector<nlohmann::json> data; //[{items:[1,2,3]},{items:[1,2,3]}]
    while (next_page  == true){
        nlohmann::json results = get_request("https://api.spotify.com/v1/me/tracks?limit=50&offset=" + std::to_string(page * 50),true);
        std::cout << "offset: " << results["offset"] << " total: " << results["total"]  << "\n";
        //std::cout << results["items"].back() << "\n"
        if (results["items"].size() > 0){
            data.insert(data.end(), results);
        }

        if (results["offset"] > results["total"]){
            next_page = false; 
        }   
        
        ++page;
    }
    return data;

}

bool is_in_vec_dics(std::vector<std::unordered_map<std::string, std::string>> list_of_dicts,std::string key, std::string value){
    for (auto d: list_of_dicts){
        if (d[key] == value){
            return true;
        }

    }
    return false;
}

//organises them to unique albums
std::vector<std::unordered_map<std::string, std::string>> get_unique_albums(){
    std::vector<std::unordered_map<std::string, std::string>> unique_albums;
    std::vector<nlohmann::json> liked = get_liked_songs();
    for (auto page : liked){
        for (auto data : page["items"]){
            std::string album_name = data["track"]["album"]["name"];
            std::string album_url = data["track"]["album"]["images"][0]["url"];
            std::string artist =  data["track"]["album"]["artists"][0]["name"];
            std::string album_type = data["track"]["album"]["album_type"];
            if (is_in_vec_dics(unique_albums,"name",album_name) == false){
                // name is unique
                std::unordered_map<std::string, std::string> album;
                album["album_type"] = album_type;
                album["name"] = album_name;
                album["image"] = album_url;
                album["artist"] = artist;
                
                unique_albums.push_back(album);
            }

        
        }
    }



    return unique_albums;
} 



/*
couldn't find: a different kind of human ÔÇô step 2
couldn't find: yours truly, angry mob
couldn't find: inside in / inside out
couldn't find: b'day deluxe edition
couldn't find: man on the moon: the end of day (deluxe)
couldn't find: beyonc├ë [platinum edition]
couldn't find: mm...food
couldn't find: straight no chaser (ealbum)
couldn't find: r&g (rhythm & gangsta): the masterpiece
couldn't find: unapologetic (deluxe)
couldn't find: anjunabeats pres. super8 & tab 01
couldn't find: nothing but the beat ultimate
couldn't find: clarity (deluxe)
couldn't find: exodus
couldn't find: rastaman vibration
couldn't find: kaya - deluxe edition
couldn't find: ready to die (the remaster; 2015 remaster)
couldn't find: life after death (2014 remastered edition)
couldn't find: the love movement
couldn't find: r u still down? [remember me]
couldn't find: tha blue carpet treatment
couldn't find: trip at knight (complete edition)
couldn't find: the blueprint (explicit version)
couldn't find: g i r l
couldn't find: strictly 4 my n.i.g.g.a.z...
couldn't find: black panther the album music from and inspired by
couldn't find: man on the moon: the end of day (int'l version)
*/





