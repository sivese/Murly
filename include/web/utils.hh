#pragma once

#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

namespace web::http::utils {
    std::string urlEncode(const std::string& url);
    std::string urlDecode(const std::string& url);
    
    // MIME type detection
    std::string getMimeType(const std::string& filename);
    
    // HTTP date formatting
    std::string formatHttpDate(std::time_t time);
    std::time_t parseHttpDate(const std::string& dateStr);
    
    // Header parsing utilities
    std::unordered_map<std::string, std::string> parseQueryString(const std::string& query);
    std::unordered_map<std::string, std::string> parseCookies(const std::string& cookieHeader);
    
    // Content type helpers
    bool isJsonContentType(const std::string& contentType);
    bool isFormContentType(const std::string& contentType);
    bool isMultipartContentType(const std::string& contentType);
}