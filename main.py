from spotify import spotify
from itunes import itunes
import time 
import os 
import requests 
import subprocess

from change_wallpaper import set_wallpaper 
s = spotify()

def find_itunes_album_equal(albums):
    for a in albums:
        #print(f"{a['collectionName']} - {album_name}")
        # itunes adds "- Single" spotify doesnt on their singles 
        if a['collectionName'].lower() == album_name.lower() or f"{album_name.lower()} - single" == a['collectionName'].lower():
            return a 

        

while True:
    u_images_downloaded = os.listdir("./images/uncompressed/")
    w_images_downloaded = os.listdir("./images/wallpapers/")

    album = s.info()
    if album != False:
        artists = album['album_info']['artist']
        album_name = album['album_info']['album']
        album_id = album['album_id']


        # download  uncompressed if you havent already 
        if f"{album_id}.jpg" not in u_images_downloaded: 
            i = itunes(artists[0]['name'],"gb")
            
            successful_find = find_itunes_album_equal(i.lookup_artist_albums())
            if successful_find == None:
                successful_find = find_itunes_album_equal(i.lookup_artist_songs())
            if successful_find == None:
                path = f"./images/compressed/{album_id}.jpg"    
                print("couldn't find itunes album")
            else:
                path = f"./images/uncompressed/{album_id}.jpg"    
                uncompressed = i.get_uncompressed_image(successful_find)
                img_data = requests.get(uncompressed).content
                with open(f'./images/uncompressed/{album_id}.jpg', 'wb') as f:
                    print(f"downloading: {album_id}")
                    f.write(img_data)
            
        print("yo")
        if f"{album_id}.jpg" not in w_images_downloaded:
            print("a") 
            #download edited poster using ffmpeg 
            #path = f"./images/compressed/{album_id}.jpg"    
            subprocess.run(["ffmpeg", "-i",path, "-i",path,"-y","-filter_complex", f"boxblur=50:50,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0", "-q:v", "1",f"./images/wallpapers/{album_id}.jpg"])
                
                
                # now you have everything you need (album info, spotify url, itunes url)
            set_wallpaper(album_id)




    #potential  future 
    # SCREEN #1 album image
    # SCREEN #2 album info
    # BACKGROUND - blurred image of album 
    time.sleep(1)