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

    "https://accounts.spotify.com/authorize?client_id={client_id}&response_type=code&redirect_uri=redirect_uri&scope={scopes.replace(' ','%20')}" => gives code for you to use to get the refresh token which you will refresh to get the access token
*/



#include "header.h"
