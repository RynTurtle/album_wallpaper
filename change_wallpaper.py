import ctypes
import os 

def set_wallpaper(filename):
    path = f"{os.path.dirname(os.path.realpath(__file__))}\\images\\wallpapers\\{filename}.jpg" 
    ctypes.windll.user32.SystemParametersInfoW(20, 0, path, 0)

