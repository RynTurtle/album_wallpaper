from spotify import spotify
from itunes import itunes
import time 
import os 
import requests 
import subprocess

from change_wallpaper import set_wallpaper 
s = spotify()

def find_itunes_album_equal(albums,spotify_album_name):
    for a in albums:
        # tries to find the correct album image  based on the titles

        #print(f"{spotify_album_name} - {a['collectionName']}'")
        if (
            a['collectionName'].lower() == spotify_album_name.lower() or
            f"{spotify_album_name.lower()} - single" == a['collectionName'].lower() or # itunes adds "- Single"  and "- EP"  whereas spotify does not 
            f"{spotify_album_name.lower()} - ep" == a['collectionName'].lower()   
            ):
            return a 


    for a in albums:
        # sometimes there are bracketed albums have a different word inside e.g. 2011 dark side of the moon remaster has the word version in spotify but not in apple music 
        # however some albums might have different bracketed versions so we will do this last to reduce the chances of getting the wrong version of the album.
        if spotify_album_name.lower().split('(')[0] == a['collectionName'].lower().split('(')[0]:  
            return a 

        if a['wrapperType'] == "track":
            if spotify_album_name.lower() == a['trackName']:
                return a 
def get_info():
    album = s.info()
    if album != False:
        artists = album['album_info']['artist']
        album_name = album['album_info']['album']
        album_id = album['album_id']

        # download  uncompressed if you havent already 
        if f"{album_id}.jpg" not in u_images_downloaded: 
            i = itunes(artists[0]['name'],"gb")
            successful_find = find_itunes_album_equal(i.lookup_artist_albums(),album_name)
            if successful_find == None:
                successful_find = find_itunes_album_equal(i.lookup_artist_songs(),album_name)
            if successful_find == None:
                successful_find = find_itunes_album_equal(i.search_song(album_name),album_name)
            if successful_find == None:


                album['path'] = f"./images/compressed/{album_id}.jpg"    
            else:
                album['path'] = f"./images/uncompressed/{album_id}.jpg"    
                uncompressed = i.get_uncompressed_image(successful_find)
                img_data = requests.get(uncompressed).content
                with open(f'./images/uncompressed/{album_id}.jpg', 'wb') as f:
                    print(f"downloading: {album_id}")
                    f.write(img_data)
    return album 



while True:
    u_images_downloaded = os.listdir("./images/uncompressed/")
    w_images_downloaded = os.listdir("./images/wallpapers/")
    info = get_info()
    if info != False:
        if f"{info['album_id']}.jpg" not in w_images_downloaded:
                #download edited poster using ffmpeg 
            subprocess.run(["ffmpeg", "-i",info['path'], "-i",info['path'],"-y","-filter_complex", f"boxblur=50:50,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0", "-q:v", "1",f"./images/wallpapers/{info['album_id']}.jpg"])
                    
                    
                # now you have everything you need (album info, spotify url, itunes url)
        set_wallpaper(info['album_id'])
    time.sleep(1)

    #potential  future 
    # SCREEN #1 album image
    # SCREEN #2 album info
    # BACKGROUND - blurred image of album 
