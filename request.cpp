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


std::string replace_spaces(const std::string& input) {
    std::string result;
    for (char ch : input) {
        if (ch == ' ') {
            result += "%20";
        } else {
            result += ch;
        }
    }

    return result;
}


nlohmann::json get_request(std::string url, bool header){
    std::stringstream auth_header; 

    CURL* curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the target URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        if (header == true) {
            // Set the headers
            struct curl_slist* headers = nullptr;
            std::string access = "Authorization: Bearer " + get_token("access_token");

            headers = curl_slist_append(headers, access.c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // Set the callback function to handle received data 
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // Perform the GET request
        CURLcode res = curl_easy_perform(curl);
    
        // Get the HTTP response code
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        std::cout << "HTTP Response Code: " << httpCode << std::endl;
        curl_easy_cleanup(curl);
                
        // parse json 
        nlohmann::json json = nlohmann::json::parse(response);
        return json;
        
    } else {
        std::cerr << "Failed to initialize cURL." << std::endl;
        
    }

    // if nothing is returned then give an error 
    throw;
}



nlohmann::json post_request(std::string url,std::string paramaters){
    // Initialize cURL
    CURL* curl = curl_easy_init();
    if (curl) {

        // Set the request type to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Specify the content type
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the request data (grant_type, code, redirect_uri, client_id, client_secret)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, paramaters.c_str());

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "cURL failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::cout << "HTTP Response Code: " << httpCode << std::endl;
            nlohmann::json json = nlohmann::json::parse(response);
            return json;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    throw;

}
