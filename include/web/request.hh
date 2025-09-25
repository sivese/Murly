#pragma once

#include <string>
#include <unordered_map>
#include <optional>

namespace web::http {
    enum class Method {
        GET,
        POST,
        PUT,
        DELETE,
        HEAD,
        OPTIONS,
        PATCH,
        TRACE,
        CONNECT,
        UNKNOWN
    };

    enum class Version {
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0,
        HTTP_3_0,
        UNKNOWN
    };

    class HttpRequest {
    public:
        HttpRequest() = default;
        explicit HttpRequest(const std::string& rawRequest);

        bool parse(const std::string& buffer);
        bool isComplete() const;

        Method method() const { return _method; }

        const std::string& uri()    const { return _uri; }
        const std::string& path()   const { return _path; }
        const std::string& query()  const { return _query; }
        
        Version version() const { return _version; }

        const std::unordered_map<std::string, std::string>& headers() const { return _headers; }
        const std::string& body() const { return _body; }

        std::optional<std::string>  getHeader(const std::string& name) const;
        bool                        hasHeader(const std::string& name) const;
        std::size_t                 contentLength() const;

        std::optional<std::string>                   getQueryParam(const std::string& name) const;
        std::unordered_map<std::string, std::string> getQueryParams() const;

        std::string methodToString() const;
        std::string versionToString() const;
        std::string toString() const;

        void reset();
    private:
        Method _method = Method::UNKNOWN;
        std::string _uri;
        std::string _path;
        std::string _query;

        Version _version = Version::UNKNOWN;
        std::unordered_map<std::string, std::string> _headers;
        std::string _body;

        bool _isComplete = false;

        Method      stringToMethod(const std::string& method) const;
        Version     stringToVersion(const std::string& version) const;
        
        std::string toLowerCase(const std::string& str) const;
        std::string toUpperCase(const std::string& str) const;

        void parseRequestLine(const std::string& line);
        void parseHeaderLine(const std::string& line);
        void parseUri(const std::string& uri);
        void parseQuery(const std::string& query);
    };
}