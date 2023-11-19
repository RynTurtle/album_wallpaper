import requests 
import json 
import math
import os 
import random 
import time 
from itunes import itunes,UnFoundException
import json 

class spotify():
    def __init__(self):
        with open("keys.json") as f:
            self.data = json.load(f)            
        self.redirect_uri = "http://localhost:8888/callback"
        self.scopes = "user-library-read user-read-playback-state"
        self.headers = {"Authorization": "Bearer " +  self.data['access_token'],"Content-Type": "application/json "}
        authorize_url = f"https://accounts.spotify.com/authorize?client_id={self.data['client_id']}&response_type=code&redirect_uri={self.redirect_uri}&scope={self.scopes.replace(' ','%20')}"
        self.start = time.time()
        self.liked_albums = []
        if self.data['refresh_token'] == "":
            code = input(f"Click here and copy paste your authorization code {authorize_url}:  ")
            self.get_refresh(code)
        
        self.get_access()



    def get_access(self):
        r = requests.post(f"https://accounts.spotify.com/api/token?grant_type=refresh_token&refresh_token={self.data['refresh_token']}&client_id={self.data['client_id']}&client_secret={self.data['client_secret']}",headers={'Content-Type': 'application/x-www-form-urlencoded'})
        if r.status_code == 200:
            print("successfully refreshed access token")
            self.data['access_token'] = r.json()['access_token']
            self.headers = {"Authorization": "Bearer " +  self.data['access_token'],"Content-Type": "application/json "}

            with open("keys.json","w") as f:        
                json.dump(self.data,indent=2,fp=f)

        with open("keys.json") as f:
            self.data = json.load(f)


    def get_refresh(self,code):
        r = requests.post(f"https://accounts.spotify.com/api/token?grant_type=authorization_code&code={code}&redirect_uri={self.redirect_uri}&client_id={self.data['client_id']}&client_secret={self.data['client_secret']}",headers={'Content-Type': 'application/x-www-form-urlencoded'})
        if r.status_code == 200:
            print("successfully got the refresh token")
            self.data['refresh_token'] = r.json()['refresh_token']
            with open("keys.json","w") as f:        
                json.dump(self.data,indent=2,fp=f)



    def album_images(self):  # gets unique album images from your liked songs 
        albums = []
        s = requests.Session()
        s.headers.update(self.headers)
    
        r = s.get(f"https://api.spotify.com/v1/me/tracks?limit=50&offset=0")
        if r.status_code == 200:
            offset = r.json()['total']
            for i in range(math.ceil(offset / 50)): 
                r = s.get(f"https://api.spotify.com/v1/me/tracks?limit=50&offset={i * 50}")
                for items in r.json()['items']:
                    album_image = items['track']['album']['images'][0]['url']
                    album_name = items['track']['album']['name']
                    album_id = items['track']['album']['id']
                    if album_name not in albums:
                        albums.append({"name":album_name,"image":album_image,"id":album_id})     
            return albums

        if r.status_code == 401:
            self.get_access()
        else:
            return False 
        # potential data to show: album information e.g. name, date, album count 

    def album_lookup(self,album_id): # finds tracklist and information about the specified album  (limit to 50 songs in a playlist atm, need to change for huge albums)
        r = requests.get(f"https://api.spotify.com/v1/albums/{album_id}",headers=self.headers)
        if r.status_code == 200:
            data = r.json()
            album_length = 0 
            tracks = []            
            for track in data['tracks']['items']:
                tracks.append({"name":track['name'],"position":track['track_number'],"duration":track['duration_ms']})
                
            for t in tracks:
                album_length += t['duration']
            return {"album_type":data['album_type'],"artist":data['artists'],"record-label":data['label'],"release-date":data['release_date'],"tracks":tracks,"disc_amount":data['tracks']['items'][-1]['disc_number'],"album_duration_ms":album_length}
    
    def playback_state(self):
        r = requests.get("https://api.spotify.com/v1/me/player",headers=self.headers)
        if r.status_code == 200:
            return r.json()
        elif r.status_code == 401:
            self.get_access()
            return {"is_playing":False} 
        else:
            return {"is_playing":False}  
        

    def info(self):
        images_downloaded = os.listdir("./images/compressed/")
        total_info = {}
        playback = self.playback_state()
        if 'item' in playback:
            if playback['item'] == None: 
                return False 
            
        if playback['is_playing'] == True and playback['item']['is_local'] != True: # ignores currently playing local file because i want to display album image which isnt provided
            print("listening to music")
            item = playback['item']
            album = item['album']
            album_name = album['name']
            image = album['images'][0]['url']
            album_id = item['album']['id']
            listening_to = item['name']   
            progress = playback['progress_ms']
            device_type = item['type']

            total_info['currently_playing'] =  {"listening_to":listening_to,"progress_ms":progress,"device_type":device_type}
        else:
            print("not listening to music")
            if self.liked_albums == []:
                self.liked_albums = self.album_images()
            
            album = random.choice(self.liked_albums)
            album_name = album['name']
            image = album['image']
            album_id = album['id']


        lookup_album = self.album_lookup(album_id)
        total_info['album_id'] =  album_id
        total_info['album_info'] = lookup_album 
        total_info['album_info']['album'] = album_name
        total_info['album_info']['image'] = image

        if f"{album_id}.jpg" not in images_downloaded: 
            img_data = requests.get(image).content
            with open(f'./images/compressed/{album_id}.jpg', 'wb') as f:
                print(f"downloading: {album_id}")
                f.write(img_data) 

        return total_info

