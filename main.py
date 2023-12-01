from spotify import spotify
from itunes import itunes
import time 
import os 
import requests 
import subprocess
import ctypes
import os 


s = spotify()
class album_wallpaper():
    def __init__(self,itunes,spotify_info):
        self.wallpapers_downloaded = os.listdir("./images/wallpapers/")
        self.uncompressed_downloaded = os.listdir("./images/uncompressed/")
        self.compressed_downloaded = os.listdir("./images/compressed/")
        self.i = itunes
        self.spotify_info = spotify_info
        if self.spotify_info == False:
            print("spotify info is false")
        

        
    def get_itunes_album_equivelant(self,itunes_data):
        spotify_album_name = self.spotify_info['album_info']['album']
        for a in itunes_data:
            # tries to find the correct album image  based on the titles
            if (
                a['collectionName'].lower() == spotify_album_name.lower() or
                f"{spotify_album_name.lower()} - single" == a['collectionName'].lower() or # itunes adds "- Single"  and "- EP"  whereas spotify does not 
                f"{spotify_album_name.lower()} - ep" == a['collectionName'].lower()   
                ):
                return a

        for a in itunes_data:
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
                
    def set_wallpaper(self,filename):
        path = f"{os.path.dirname(os.path.realpath(__file__))}\\images\\wallpapers\\{filename}.jpg" 
        ctypes.windll.user32.SystemParametersInfoW(20, 0, path, 0)


    def wallpaper_setup(self):
        if f"{self.spotify_info['album_id']}.jpg" not in self.wallpapers_downloaded:
            # choose what album image to use 
            if self.lookup_itunes_album() == True:
                path  = f"./images/uncompressed/{self.spotify_info['album_id']}.jpg"    
            else:
                path  = f"./images/compressed/{self.spotify_info['album_id']}.jpg"    

            # download wallpaper
            subprocess.run(["ffmpeg", "-i",path, "-i",path,"-y","-filter_complex", f"boxblur=100:100,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0", "-q:v", "1",f"./images/wallpapers/{self.spotify_info['album_id']}.jpg"])
            #remove the two copies once the wallpaper is downloaded
            for f in self.uncompressed_downloaded:
                os.remove(f"./images/uncompressed/{f}")
            for f in self.compressed_downloaded:
                os.remove(f"./images/compressed/{f}")

        return self.spotify_info['album_id']



    def lookup_itunes_album(self):
        successful_find = self.get_itunes_album_equivelant(self.i.lookup_artist_albums())
        if successful_find == None:
            successful_find = self.get_itunes_album_equivelant(self.i.lookup_artist_songs())
        if successful_find == None:
            successful_find = self.get_itunes_album_equivelant(self.i.search_song(self.spotify_info['album_info']['album']))
        if successful_find == None:
            return False
        else:
            uncompressed = self.i.get_uncompressed_image(successful_find)
            img_data = requests.get(uncompressed).content
            with open(f'./images/uncompressed/{self.spotify_info["album_id"]}.jpg', 'wb') as f:
                print(f"downloading uncompressed: {self.spotify_info['album_id']}")
                f.write(img_data)

            spotify_image = self.spotify_info['album_info']['image'] 
            img_data = requests.get(spotify_image).content
            with open(f'./images/compressed/{self.spotify_info["album_id"]}.jpg', 'wb') as f:
                print(f"downloading compressed: {self.spotify_info['album_id']}")
                f.write(img_data) 
        
            return True



now = time.time()

#set the wallpaper cooldown 
listening_cooldown=5
album_shuffle_cooldown=30

while True:
    playback = s.playback_state()
    if playback['is_playing'] == True and playback['item']['is_local'] != True:
        cooldown = listening_cooldown
    else:
        cooldown = album_shuffle_cooldown


    if time.time() - now > cooldown:
        spotify_info = s.info()
        i = itunes(spotify_info['album_info']['artist'][0]['name'],"gb")
        a = album_wallpaper(i,spotify_info)
        setup_result = a.wallpaper_setup()
        a.set_wallpaper(setup_result)
        now = time.time() 
    time.sleep(1)
