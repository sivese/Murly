#include "web/utils.hh"

using namespace web::http::utils;

std::string urlEncode(const std::string& url) {
    std::ostringstream encoded;
    //What the hex?
    encoded << std::hex;

    for(auto c : url) {
        if(std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return encoded.str();
}

std::string urlDecode(const std::string& url) {
    std::string decoded;
    decoded.reserve(url.length());

    for(size_t i = 0; i < url.length(); ++i) {
        if(url[i] == '%' && i + 2 < url.length()) {
            auto hex = url.substr(i + 1, 2);
            auto c = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded += c;
            i += 2;
        }
        else if(url[i] == '+') {
            decoded += ' ';
        } else {
            decoded += url[i];
        }
    }

    return decoded;
}