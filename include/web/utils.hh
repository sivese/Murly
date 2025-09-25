#pragma once

#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

namespace web::http::utils {
    std::string urlEncode(const std::string& str) {
        std::ostringstream encoded;
        encoded << std::hex;
        
        for (char c : str) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded << c;
            } else {
                encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            }
        }
        
        return encoded.str();
    }

    std::string urlDecode(const std::string& str) {
        std::string decoded;
        decoded.reserve(str.length());
        
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%' && i + 2 < str.length()) {
                std::string hex = str.substr(i + 1, 2);
                char c = static_cast<char>(std::stoi(hex, nullptr, 16));
                decoded += c;
                i += 2;
            } else if (str[i] == '+') {
                decoded += ' ';
            } else {
                decoded += str[i];
            }
        }
        
        return decoded;
    }

    std::string getMimeType(const std::string& filename) {
        // Extract file extension
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) {
            return "application/octet-stream";
        }
        
        std::string ext = filename.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Common MIME types
        static const std::unordered_map<std::string, std::string> mimeTypes = {
            // Text
            {"txt", "text/plain"},
            {"html", "text/html"},
            {"htm", "text/html"},
            {"css", "text/css"},
            {"js", "application/javascript"},
            {"json", "application/json"},
            {"xml", "application/xml"},
            {"csv", "text/csv"},
            
            // Images
            {"png", "image/png"},
            {"jpg", "image/jpeg"},
            {"jpeg", "image/jpeg"},
            {"gif", "image/gif"},
            {"svg", "image/svg+xml"},
            {"webp", "image/webp"},
            {"ico", "image/x-icon"},
            
            // Audio/Video
            {"mp3", "audio/mpeg"},
            {"wav", "audio/wav"},
            {"mp4", "video/mp4"},
            {"webm", "video/webm"},
            {"ogg", "audio/ogg"},
            
            // Documents
            {"pdf", "application/pdf"},
            {"doc", "application/msword"},
            {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {"xls", "application/vnd.ms-excel"},
            {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            
            // Archives
            {"zip", "application/zip"},
            {"tar", "application/x-tar"},
            {"gz", "application/gzip"},
            {"rar", "application/x-rar-compressed"},
            
            // Fonts
            {"ttf", "font/ttf"},
            {"woff", "font/woff"},
            {"woff2", "font/woff2"}
        };
        
        auto it = mimeTypes.find(ext);
        return (it != mimeTypes.end()) ? it->second : "application/octet-stream";
    }

    std::string formatHttpDate(std::time_t time) {
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&time), "%a, %d %b %Y %H:%M:%S GMT");
        return oss.str();
    }

    std::time_t parseHttpDate(const std::string& dateStr) {
        std::tm tm = {};
        std::istringstream iss(dateStr);
        
        // Try different HTTP date formats
        iss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
        if (!iss.fail()) {
            return std::mktime(&tm);
        }
        
        iss.clear();
        iss.str(dateStr);
        iss >> std::get_time(&tm, "%A, %d-%b-%y %H:%M:%S GMT");
        if (!iss.fail()) {
            return std::mktime(&tm);
        }
        
        iss.clear();
        iss.str(dateStr);
        iss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
        if (!iss.fail()) {
            return std::mktime(&tm);
        }
        
        return 0; // Failed to parse
    }

    std::unordered_map<std::string, std::string> parseQueryString(const std::string& query) {
        std::unordered_map<std::string, std::string> params;
        
        if (query.empty()) return params;
        
        std::istringstream queryStream(query);
        std::string pair;
        
        while (std::getline(queryStream, pair, '&')) {
            size_t equalPos = pair.find('=');
            if (equalPos != std::string::npos) {
                std::string key = urlDecode(pair.substr(0, equalPos));
                std::string value = urlDecode(pair.substr(equalPos + 1));
                params[key] = value;
            } else {
                params[urlDecode(pair)] = "";
            }
        }
        
        return params;
    }

    std::unordered_map<std::string, std::string> parseCookies(const std::string& cookieHeader) {
        std::unordered_map<std::string, std::string> cookies;
        
        if (cookieHeader.empty()) return cookies;
        
        std::istringstream cookieStream(cookieHeader);
        std::string cookie;
        
        while (std::getline(cookieStream, cookie, ';')) {
            // Trim whitespace
            cookie.erase(0, cookie.find_first_not_of(" \t"));
            cookie.erase(cookie.find_last_not_of(" \t") + 1);
            
            size_t equalPos = cookie.find('=');
            if (equalPos != std::string::npos) {
                std::string name = cookie.substr(0, equalPos);
                std::string value = cookie.substr(equalPos + 1);
                cookies[name] = value;
            }
        }
        
        return cookies;
    }

    bool isJsonContentType(const std::string& contentType) {
        return contentType.find("application/json") != std::string::npos;
    }

    bool isFormContentType(const std::string& contentType) {
        return contentType.find("application/x-www-form-urlencoded") != std::string::npos;
    }

    bool isMultipartContentType(const std::string& contentType) {
        return contentType.find("multipart/form-data") != std::string::npos;
    }
}