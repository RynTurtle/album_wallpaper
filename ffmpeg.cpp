#include "header.h"
#include <iostream>
#include <filesystem>


/*
create a wallpaper 16:9 resolution, blur the image then add an overlay of the album cover ontop of the background 

ffmpeg -i ./Wallpapers/Temp/2715720-19UMGIM89397.rgb.jpg -i ./Wallpapers/Temp/2715720-19UMGIM89397.rgb.jpg -filter_complex "boxblur=100:100,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0" -q:v 1 ./Wallpapers/Finished/2715720-19UMGIM89397.rgb.jpg
*/


int create_wallpaper(std::string filename){
    std::ostringstream command;
    command << "ffmpeg -i " << "./Wallpapers/Temp/" << filename << " -i " << "./Wallpapers/Temp/" << filename <<  " -filter_complex boxblur=100:100,scale=iw*16/9:ih,overlay=x=(main_w-overlay_w)/2:y=0 -q:v 1 ./Wallpapers/Finished/" << filename;
    std::cout << "Running command: " << command.str() << "\n";

    int result = std::system(command.str().c_str());
    // Check if the command executed successfully
    if (result == 0) {
        std::cout << "FFmpeg command executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing FFmpeg command. Exit code: " << result << std::endl;
    }

    return 0;
}

