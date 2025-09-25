#pragma once

#include <string>
#include <sstream>
#include <ctime>

namespace web::http {
    enum class StatusCode {

    };

    class HttpResponse {
    public:
        HttpResponse() = default;
        explicit HttpResponse(StatusCode code, const std::string& body);
    private:

    };
}