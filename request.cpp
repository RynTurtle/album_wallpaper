#include "header.h"

#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


nlohmann::json request(std::string url){
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the target URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle received data 
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // Perform the GET request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Get the HTTP response code
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::cout << "HTTP Response Code: " << httpCode << std::endl;
                
            // parse json 
            nlohmann::json json = nlohmann::json::parse(response);
            return json;
        }
            // Cleanup
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL." << std::endl;
        
    }

    // if nothing is returned then give an error 
    throw;
}
