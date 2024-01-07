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
    //std::cout << result << "\n";
}

// use refresh token to get access token 
void third(){

}


// playback state 
void playback_state(){

}


//get all liked songs, organise them to unique albums
void get_liked_albums(){

}




int main(){

    // if there isnt any refresh token then 
    first();

    //then get refresh using code in url  
    second("AQBIt-V5NQ_e7aO_n4YAdNjAUg4ON-ZOZxbO4alWjnv65KxWKI9zglhKXmL8K0JNIAvm0l1cKG0uCa3hy1i-Wwj_9FU02MoufkbHxO9OmLVouVqOQAbmoiHs_wPuiFobuNgL6cHxoblSA09N6P6Ybvmmxkm5domYqDaJ7-jgtboGQmel922geZHC54o4ENB1m52_Fx2wmlydzjhW-6oKFpaKcnSAwpSf1b5rlpn6nA");

}



