import requests 
import cv2 as cv2
import numpy as np
import urllib.request
import time

class UnFoundException(Exception):
    "Raised when an album cant be found"
    pass 

class itunes():
    def __init__(self,artist_name,country):
        self.country = country 
        self.artist = artist_name
        self.artist_id = self.get_artist_id(artist_name)
        if self.artist_id == None:
            print(f"artist id not found: {artist_name}")
            raise UnFoundException
        print(f"{artist_name} id:{self.artist_id}")


    def get_uncompressed_image(self,data):
        url = data['artworkUrl100']
        path = url.split("/image/thumb/")[1].replace("/100x100bb.jpg","")
        uncompressed = f"https://a5.mzstatic.com/us/r1000/0/{path}"
        return uncompressed


    def get_artist_id(self,name):
        r = requests.get(f"https://itunes.apple.com/search?term={name}&country={self.country}&entity=allArtist")
        data = r.json()
        if len(data['results']) > 0:
            for r in data['results']:
                if r['artistName'].lower() == name.lower():
                    return r['artistId']
            # if theres no id returned
            return None 
        
    #200 results per page limit ONLY on the lookup api, you can paginate through the search endpoint. This is literally the dumbest shit ever but whatever. 
    def lookup_artist_albums(self): # "album" also contains some singles, spotify does the same 
        print("looking up album")
        r = requests.get(f"http://itunes.apple.com/lookup?id={self.artist_id}&country={self.country}&entity=album&limit=200&sort=recent")
        data = r.json()
        if len(data['results']) > 0:
            return data['results'][1:] # the first item in list is just information 


    def lookup_artist_songs(self): 
        print("checking singles")
        r = requests.get(f"http://itunes.apple.com/lookup?id={self.artist_id}&country={self.country}&entity=song&limit=200&sort=recent")
        data = r.json()
        if len(data['results']) > 0:
            return data['results'][1:] # the first item in list is just information 
        



    def search_song(self,search): 
        r = requests.get(f"https://itunes.apple.com/search?term={search}&limit=200&country={self.country}&entity=song")
        print("searching for the song")
        data = r.json()
        artist_results = []
        if len(data['results']) > 0:
            for result in data['results']:
                if result['artistName'].lower() == self.artist.lower() or self.artist.lower() in result['artistName'].lower() or result['artistName'].lower() in self.artist.lower():
                    artist_results.append(result) 
                
            
            return artist_results