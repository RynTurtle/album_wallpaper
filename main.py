from spotify import spotify
from itunes import itunes
import time 
import os 
import requests 
import subprocess

from change_wallpaper import set_wallpaper 
s = spotify()

def find_itunes_album_equal(albums,spotify_album_name):
    try:
        for a in albums:
            # tries to find the correct album image  based on the titles
            if (
                a['collectionName'].lower() == spotify_album_name.lower() or
                f"{spotify_album_name.lower()} - single" == a['collectionName'].lower() or # itunes adds "- Single"  and "- EP"  whereas spotify does not 
                f"{spotify_album_name.lower()} - ep" == a['collectionName'].lower()   
                ):
                return a

        for a in albums:
            # sometimes there are bracketed albums have a different word inside e.g. 2011 dark side of the moon remaster has the word version in spotify but not in apple music 
            # however some albums might have different bracketed versions so we will do this last to reduce the chances of getting the wrong version of the album
            # sometimes it has (featuring [artistname]) in brackets aswell on itunes but not on spotify  
            #print(f'"{spotify_album_name.lower().split("(")[0]}" "{a["collectionName"].lower().split("(")[0]}"')
            if (spotify_album_name.lower().split('(')[0] == a['collectionName'].lower().split('(')[0].strip() or 
                spotify_album_name.lower().split('(')[0] == a['collectionName'].lower().split('(')[0].strip() or
                spotify_album_name.lower().split('(')[0] == a['collectionName'].lower().split('(')[0].strip()
                ):
                return a 

            if a['wrapperType'] == "track":
                if spotify_album_name.lower() == a['trackName']:
                    return a 
    except Exception as e:
        print(e)
        
def get_info():
    wallpapers_downloaded = os.listdir("./images/wallpapers/")
    uncompressed_downloaded = os.listdir("./images/uncompressed/")
    compressed_downloaded = os.listdir("./images/compressed/")

    album = s.info()
    if album  != False:
        artists = album['album_info']['artist']
        album_name = album['album_info']['album']
        album_id = album['album_id']
        # download  uncompressed if you havent already 
        if f"{album['album_id']}.jpg" not in wallpapers_downloaded:
            try:
                i = itunes(artists[0]['name'],"gb")
            except:
                return False 

            # attempts to find the itunes album  
            successful_find = find_itunes_album_equal(i.lookup_artist_albums(),album_name)
            if successful_find == None:
                successful_find = find_itunes_album_equal(i.lookup_artist_songs(),album_name)
            if successful_find == None:
                successful_find = find_itunes_album_equal(i.search_song(album_name),album_name)
            if successful_find == None:


                path  = f"./images/compressed/{album_id}.jpg"    
            else:
                path  = f"./images/uncompressed/{album_id}.jpg"    
                uncompressed = i.get_uncompressed_image(successful_find)
                img_data = requests.get(uncompressed).content
                with open(f'./images/uncompressed/{album_id}.jpg', 'wb') as f:
                    print(f"downloading: {album_id}")
                    f.write(img_data)
            #download edited poster using ffmpeg, chooses path based on if higher quality  can be found or not  
            subprocess.run(["ffmpeg", "-i",path, "-i",path,"-y","-filter_complex", f"boxblur=100:100,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0", "-q:v", "1",f"./images/wallpapers/{album_id}.jpg"])
    
        #remove the two copies once the wallpaper is downloaded
        for f in uncompressed_downloaded:
            os.remove(f"./images/uncompressed/{f}")
        for f in compressed_downloaded:
            os.remove(f"./images/compressed/{f}")

    return album 



while True:
    info = get_info()
    if info != False:            
        set_wallpaper(info['album_id'])
    time.sleep(1)

    # get the main screen number, change the main screen to the one with the poster
    # get all the other screens, change their backgrounds to the stretched blurred image 