#pragma once
#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace httpxx
{
    using Headers = std::unordered_map<std::string, std::optional<std::string>>;

    const Headers headers = {
        {"Accept", "*/*"},
        {"Accept-Charset", std::nullopt},
        {"Accept-Encoding", std::nullopt},
        {"Accept-Language", std::nullopt},
        {"Accept-Ranges", std::nullopt},
        {"Age", std::nullopt},
        {"Allow", std::nullopt},
        {"Authorization", std::nullopt},
        {"Cache-Control", std::nullopt},
        {"What May be Stored by Caches", std::nullopt},
        {"Modifications of the Basic Expiration Mechanism", std::nullopt},
        {"Cache Revalidation and Reload Controls", std::nullopt},
        {"No-Transform Directive", std::nullopt},
        {"Cache Control Extensions", std::nullopt},
        {"Connection", std::nullopt},
        {"Content-Encoding", std::nullopt},
        {"Content-Language", std::nullopt},
        {"Content-Length", std::nullopt},
        {"Content-Location", std::nullopt},
        {"Content-MD5", std::nullopt},
        {"Content-Range", std::nullopt},
        {"Content-Type", "text/html"},
        {"Date", std::nullopt},
        {
            "Clockless Origin Server Operation",
            std::nullopt
        }, // This could be adjusted for clarity
        {"ETag", std::nullopt},
        {"Expect", std::nullopt},
        {"Expires", std::nullopt},
        {"From", std::nullopt},
        {"Host", std::nullopt},
        {"If-Match", std::nullopt},
        {"If-Modified-Since", std::nullopt},
        {"If-None-Match", std::nullopt},
        {"If-Range", std::nullopt},
        {"If-Unmodified-Since", std::nullopt},
        {"Last-Modified", std::nullopt},
        {"Location", std::nullopt},
        {"Max-Forwards", std::nullopt},
        {"Pragma", std::nullopt},
        {"Proxy-Authenticate", std::nullopt},
        {"Proxy-Authorization", std::nullopt},
        {"Range", std::nullopt},
        {"Byte Ranges", std::nullopt}, // This may need to be adjusted for accuracy
        {"Range Retrieval Requests", std::nullopt},
        {"Referer", std::nullopt},
        {"Retry-After", std::nullopt},
        {"Server", std::nullopt},
        {"TE", std::nullopt},
        {"Trailer", std::nullopt},
        {"Transfer-Encoding", std::nullopt},
        {"Upgrade", std::nullopt},
        {"User-Agent", std::nullopt},
        {"Vary", std::nullopt},
        {"Via", std::nullopt},
        {"Warning", std::nullopt},
        {"WWW-Authenticate", std::nullopt}
    };

    const Headers response_headers = {
        {"Access-Control-Allow-Origin", std::nullopt},
        {"Connection", std::nullopt},
        {"Content-Encoding", std::nullopt},
        {
            "Content-Length", std::nullopt
        },
        {"Content-Type", std::nullopt},
        {"Date", std::nullopt},
        {"Etag", std::nullopt},
        {"Keep-Alive", std::nullopt},
        {"Last-Modified", std::nullopt},
        {"Server", std::nullopt},
        {"Set-Cookie", std::nullopt},
        {"Transfer-Encoding", std::nullopt},
        {"Vary", std::nullopt},
        {"X-Backend-Server", std::nullopt},
        {"X-Cache-Info", std::nullopt},
        {"X-kuma-revision", std::nullopt},
        {"x-frame-options", std::nullopt}
    };

    inline std::string headersToString(const Headers& headers)
    {
        std::ostringstream oss{};

        for (const auto& [k, v] : headers)
        {
            if (v.has_value())
            {
                oss << k << ": " << v.value() << '\n';
            }
            else
            {
                continue;
            }
        }

        return oss.str();
    }

    inline bool areEmpty(const Headers& hd)
    {
        return std::ranges::all_of(
            hd,
            [](const std::pair<const std::string, const std::optional<std::string>>
            & pair)
            {
                return pair.second == std::nullopt;
            });
    }
} // namespace httpxx

