#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>

namespace httpxx
{
    enum class HttpMethod
    {
        GET = 0,
        POST,
        PUT,
        DELETE,
        http_method_count,
    };

    inline std::ostream& operator<<(std::ostream& out, const HttpMethod method)
    {
        switch (method)
        {
        case HttpMethod::GET:
            out << "GET";
            break;
        case HttpMethod::POST:
            out << "POST";
            break;
        case HttpMethod::PUT:
            out << "PUT";
            break;
        case HttpMethod::DELETE:
            out << "DELETE";
            break;
        case HttpMethod::http_method_count:
            break;
        }

        return out;
    }

    inline std::string operator+(const HttpMethod& method)
    {
        std::string str{};

        switch (method)
        {
        case HttpMethod::GET:
            str = "GET";
            break;
        case HttpMethod::POST:
            str = "POST";
            break;
        case HttpMethod::PUT:
            str = "PUT";
            break;
        case HttpMethod::DELETE:
            str = "DELETE";
            break;
        case HttpMethod::http_method_count:
            break;
        }

        return str;
    }

    inline HttpMethod stringToHttpMethod(const std::string& method)
    {
        static const std::unordered_map<std::string, HttpMethod> strToHM{
            {"GET", HttpMethod::GET},
            {"POST", HttpMethod::POST},
            {"PUT", HttpMethod::PUT},
            {"DELETE", HttpMethod::DELETE}

        };

        assert(strToHM.find(method) != strToHM.end());

        return strToHM.at(method);
    }

    enum class StatusCodes
    {
        /*1xx Informational*/
        CONTINUE = 100,
        PRTCL_SWITCH = 101,

        /*2xx Successful*/
        OK = 200,
        CREATED = 200,
        ACCEPTED = 202,
        NON_AUTH_INFO = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,

        /*3xx Redirection*/
        MULTIPLE_CHOICE = 300,
        MOVED_PERMANETLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        USE_PROXY = 305,
        TEMP_REDIRECT = 307,

        /*4xx Client Error*/
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTH_REQUIRED = 407,
        REQUEST_TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        LENGTH_REQUIRED = 411,
        PRECONDITION_FAILED = 412,
        REQ_ENTITY_TOO_LARGE = 413,
        REQ_URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        REQ_RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED = 417,

        /*5xx Server Error*/
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505
    };

    inline std::string operator+(const StatusCodes& method)
    {
        std::string str{};

        switch (method)
        {
        case StatusCodes::CONTINUE:
            str = "CONTINUE";
            break;
        case StatusCodes::PRTCL_SWITCH:
            str = "PRTCL_SWITCH";
            break;
        case StatusCodes::OK:
            str = "OK";
            break;
        case StatusCodes::ACCEPTED:
            str = "ACCEPTED";
            break;
        case StatusCodes::NON_AUTH_INFO:
            str = "NON_AUTH_INFO";
            break;
        case StatusCodes::NO_CONTENT:
            str = "NO_CONTENT";
            break;
        case StatusCodes::RESET_CONTENT:
            str = "RESET_CONTENT";
            break;
        case StatusCodes::PARTIAL_CONTENT:
            str = "PARTIAL_CONTENT";
            break;
        case StatusCodes::MULTIPLE_CHOICE:
            str = "MULTIPLE_CHOICE";
            break;
        case StatusCodes::MOVED_PERMANETLY:
            str = "MOVED_PERMANETLY";
            break;
        case StatusCodes::FOUND:
            str = "FOUND";
            break;
        case StatusCodes::SEE_OTHER:
            str = "SEE_OTHER";
            break;
        case StatusCodes::NOT_MODIFIED:
            str = "NOT_MODIFIED";
            break;
        case StatusCodes::USE_PROXY:
            str = "USE_PROXY";
            break;
        case StatusCodes::TEMP_REDIRECT:
            str = "TEMP_REDIRECT";
            break;
        case StatusCodes::UNAUTHORIZED:
            str = "UNAUTHORIZED";
            break;
        case StatusCodes::PAYMENT_REQUIRED:
            str = "PAYMENT_REQUIRED";
            break;
        case StatusCodes::FORBIDDEN:
            str = "FORBIDDEN";
            break;
        case StatusCodes::NOT_FOUND:
            str = "NOT_FOUND";
            break;
        case StatusCodes::METHOD_NOT_ALLOWED:
            str = "METHOD_NOT_ALLOWED";
            break;
        case StatusCodes::NOT_ACCEPTABLE:
            str = "NOT_ACCEPTABLE";
            break;
        case StatusCodes::PROXY_AUTH_REQUIRED:
            str = "PROXY_AUTH_REQUIRED";
            break;
        case StatusCodes::REQUEST_TIMEOUT:
            str = "REQUEST_TIMEOUT";
            break;
        case StatusCodes::CONFLICT:
            str = "CONFLICT";
            break;
        case StatusCodes::GONE:
            str = "GONE";
            break;
        case StatusCodes::LENGTH_REQUIRED:
            str = "LENGTH_REQUIRED";
            break;
        case StatusCodes::PRECONDITION_FAILED:
            str = "PRECONDITION_FAILED";
            break;
        case StatusCodes::REQ_ENTITY_TOO_LARGE:
            str = "REQ_ENTITY_TOO_LARGE";
            break;
        case StatusCodes::REQ_URI_TOO_LONG:
            str = "REQ_URI_TOO_LONG";
            break;
        case StatusCodes::UNSUPPORTED_MEDIA_TYPE:
            str = "UNSUPPORTED_MEDIA_TYPE";
            break;
        case StatusCodes::REQ_RANGE_NOT_SATISFIABLE:
            str = "REQ_RANGE_NOT_SATISFIABLE";
            break;
        case StatusCodes::EXPECTATION_FAILED:
            str = "EXPECTATION_FAILED";
            break;
        case StatusCodes::INTERNAL_SERVER_ERROR:
            str = "INTERNAL_SERVER_ERROR";
            break;
        case StatusCodes::NOT_IMPLEMENTED:
            str = "NOT_IMPLEMENTED";
            break;
        case StatusCodes::BAD_GATEWAY:
            str = "BAD_GATEWAY";
            break;
        case StatusCodes::SERVICE_UNAVAILABLE:
            str = "SERVICE_UNAVAILABLE";
            break;
        case StatusCodes::GATEWAY_TIMEOUT:
            str = "GATEWAY_TIMEOUT";
            break;
        case StatusCodes::HTTP_VERSION_NOT_SUPPORTED:
            str = "HTTP_VERSION_NOT_SUPPORTED";
            break;
        }

        return str;
    }


    enum class ContentType
    {
        // Text Types
        TEXT_PLAIN,
        TEXT_HTML,
        TEXT_CSS,
        TEXT_JAVASCRIPT,
        TEXT_CSV,
        TEXT_XML,
        TEXT_MARKDOWN,

        // Application Types
        APPLICATION_JSON,
        APPLICATION_XML,
        APPLICATION_PDF,
        APPLICATION_OCTET_STREAM,
        APPLICATION_FORM_URLENCODED,
        APPLICATION_GRAPHQL,
        APPLICATION_WASM,

        // Multipart Types
        MULTIPART_FORM_DATA,
        MULTIPART_MIXED,
        MULTIPART_ALTERNATIVE,
        MULTIPART_RELATED,

        // Image Types
        IMAGE_JPEG,
        IMAGE_PNG,
        IMAGE_GIF,
        IMAGE_WEBP,
        IMAGE_SVG,
        IMAGE_TIFF,
        IMAGE_BMP,

        // Audio Types
        AUDIO_MPEG,
        AUDIO_WAV,
        AUDIO_FLAC,
        AUDIO_OGG,
        AUDIO_WEBM,
        AUDIO_MP4,

        // Video Types
        VIDEO_MP4,
        VIDEO_WEBM,
        VIDEO_AVI,
        VIDEO_QUICKTIME,
        VIDEO_MKV,

        // Font Types
        FONT_WOFF,
        FONT_WOFF2,
        FONT_TTF,
        FONT_OTF,
        FONT_EOT,

        // Archive Types
        APPLICATION_ZIP,
        APPLICATION_RAR,
        APPLICATION_GZIP,
        APPLICATION_TAR,
        APPLICATION_7Z,

        // Office Document Types
        APPLICATION_WORD,
        APPLICATION_WORD_XML,
        APPLICATION_EXCEL,
        APPLICATION_EXCEL_XML,
        APPLICATION_POWERPOINT,
        APPLICATION_POWERPOINT_XML,

        // Other Specialized Types
        APPLICATION_XHTML,
        APPLICATION_RSS,
        APPLICATION_ATOM,
        TEXT_EVENT_STREAM,

        UNKNOWN
    };

    // Helper function to convert enum to string
    inline std::string contentTypeToString(ContentType type)
    {
        switch (type)
        {
        // Text Types
        case ContentType::TEXT_PLAIN: return "text/plain";
        case ContentType::TEXT_HTML: return "text/html";
        case ContentType::TEXT_CSS: return "text/css";
        case ContentType::TEXT_JAVASCRIPT: return "text/javascript";
        case ContentType::TEXT_CSV: return "text/csv";
        case ContentType::TEXT_XML: return "text/xml";
        case ContentType::TEXT_MARKDOWN: return "text/markdown";

        // Application Types
        case ContentType::APPLICATION_JSON: return "application/json";
        case ContentType::APPLICATION_XML: return "application/xml";
        case ContentType::APPLICATION_PDF: return "application/pdf";
        case ContentType::APPLICATION_OCTET_STREAM: return "application/octet-stream";
        case ContentType::APPLICATION_FORM_URLENCODED: return "application/x-www-form-urlencoded";
        case ContentType::APPLICATION_GRAPHQL: return "application/graphql";
        case ContentType::APPLICATION_WASM: return "application/wasm";

        // Multipart Types
        case ContentType::MULTIPART_FORM_DATA: return "multipart/form-data";
        case ContentType::MULTIPART_MIXED: return "multipart/mixed";
        case ContentType::MULTIPART_ALTERNATIVE: return "multipart/alternative";
        case ContentType::MULTIPART_RELATED: return "multipart/related";

        // Image Types
        case ContentType::IMAGE_JPEG: return "image/jpeg";
        case ContentType::IMAGE_PNG: return "image/png";
        case ContentType::IMAGE_GIF: return "image/gif";
        case ContentType::IMAGE_WEBP: return "image/webp";
        case ContentType::IMAGE_SVG: return "image/svg+xml";
        case ContentType::IMAGE_TIFF: return "image/tiff";
        case ContentType::IMAGE_BMP: return "image/bmp";

        // Audio Types
        case ContentType::AUDIO_MPEG: return "audio/mpeg";
        case ContentType::AUDIO_WAV: return "audio/wav";
        case ContentType::AUDIO_FLAC: return "audio/flac";
        case ContentType::AUDIO_OGG: return "audio/ogg";
        case ContentType::AUDIO_WEBM: return "audio/webm";
        case ContentType::AUDIO_MP4: return "audio/mp4";

        // Video Types
        case ContentType::VIDEO_MP4: return "video/mp4";
        case ContentType::VIDEO_WEBM: return "video/webm";
        case ContentType::VIDEO_AVI: return "video/x-msvideo";
        case ContentType::VIDEO_QUICKTIME: return "video/quicktime";
        case ContentType::VIDEO_MKV: return "video/x-matroska";

        // Font Types
        case ContentType::FONT_WOFF: return "font/woff";
        case ContentType::FONT_WOFF2: return "font/woff2";
        case ContentType::FONT_TTF: return "font/ttf";
        case ContentType::FONT_OTF: return "font/otf";
        case ContentType::FONT_EOT: return "application/vnd.ms-fontobject";

        // Archive Types
        case ContentType::APPLICATION_ZIP: return "application/zip";
        case ContentType::APPLICATION_RAR: return "application/x-rar-compressed";
        case ContentType::APPLICATION_GZIP: return "application/gzip";
        case ContentType::APPLICATION_TAR: return "application/x-tar";
        case ContentType::APPLICATION_7Z: return "application/x-7z-compressed";

        // Office Document Types
        case ContentType::APPLICATION_WORD: return "application/msword";
        case ContentType::APPLICATION_WORD_XML: return
                "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        case ContentType::APPLICATION_EXCEL: return "application/vnd.ms-excel";
        case ContentType::APPLICATION_EXCEL_XML: return
                "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        case ContentType::APPLICATION_POWERPOINT: return "application/vnd.ms-powerpoint";
        case ContentType::APPLICATION_POWERPOINT_XML: return
                "application/vnd.openxmlformats-officedocument.presentationml.presentation";

        // Other Specialized Types
        case ContentType::APPLICATION_XHTML: return "application/xhtml+xml";
        case ContentType::APPLICATION_RSS: return "application/rss+xml";
        case ContentType::APPLICATION_ATOM: return "application/atom+xml";
        case ContentType::TEXT_EVENT_STREAM: return "text/event-stream";

        case ContentType::UNKNOWN:
        default:
            return "application/octet-stream";
        }
    }

    // Helper function to convert string to enum
    inline ContentType stringToContentType(const std::string& contentTypeStr)
    {
        if (contentTypeStr == "text/plain") return ContentType::TEXT_PLAIN;
        if (contentTypeStr == "text/html") return ContentType::TEXT_HTML;
        if (contentTypeStr == "text/css") return ContentType::TEXT_CSS;
        if (contentTypeStr == "text/javascript") return ContentType::TEXT_JAVASCRIPT;
        if (contentTypeStr == "text/csv") return ContentType::TEXT_CSV;
        if (contentTypeStr == "text/xml") return ContentType::TEXT_XML;
        if (contentTypeStr == "text/markdown") return ContentType::TEXT_MARKDOWN;

        if (contentTypeStr == "application/json") return ContentType::APPLICATION_JSON;
        if (contentTypeStr == "application/xml") return ContentType::APPLICATION_XML;
        if (contentTypeStr == "application/pdf") return ContentType::APPLICATION_PDF;
        if (contentTypeStr == "application/octet-stream") return ContentType::APPLICATION_OCTET_STREAM;
        if (contentTypeStr == "application/x-www-form-urlencoded") return ContentType::APPLICATION_FORM_URLENCODED;
        if (contentTypeStr == "application/graphql") return ContentType::APPLICATION_GRAPHQL;
        if (contentTypeStr == "application/wasm") return ContentType::APPLICATION_WASM;

        if (contentTypeStr == "multipart/form-data") return ContentType::MULTIPART_FORM_DATA;
        if (contentTypeStr == "multipart/mixed") return ContentType::MULTIPART_MIXED;
        if (contentTypeStr == "multipart/alternative") return ContentType::MULTIPART_ALTERNATIVE;
        if (contentTypeStr == "multipart/related") return ContentType::MULTIPART_RELATED;

        if (contentTypeStr == "image/jpeg") return ContentType::IMAGE_JPEG;
        if (contentTypeStr == "image/png") return ContentType::IMAGE_PNG;
        if (contentTypeStr == "image/gif") return ContentType::IMAGE_GIF;
        if (contentTypeStr == "image/webp") return ContentType::IMAGE_WEBP;
        if (contentTypeStr == "image/svg+xml") return ContentType::IMAGE_SVG;
        if (contentTypeStr == "image/tiff") return ContentType::IMAGE_TIFF;
        if (contentTypeStr == "image/bmp") return ContentType::IMAGE_BMP;

        if (contentTypeStr == "audio/mpeg") return ContentType::AUDIO_MPEG;
        if (contentTypeStr == "audio/wav") return ContentType::AUDIO_WAV;
        if (contentTypeStr == "audio/flac") return ContentType::AUDIO_FLAC;
        if (contentTypeStr == "audio/ogg") return ContentType::AUDIO_OGG;
        if (contentTypeStr == "audio/webm") return ContentType::AUDIO_WEBM;
        if (contentTypeStr == "audio/mp4") return ContentType::AUDIO_MP4;

        if (contentTypeStr == "video/mp4") return ContentType::VIDEO_MP4;
        if (contentTypeStr == "video/webm") return ContentType::VIDEO_WEBM;
        if (contentTypeStr == "video/x-msvideo") return ContentType::VIDEO_AVI;
        if (contentTypeStr == "video/quicktime") return ContentType::VIDEO_QUICKTIME;
        if (contentTypeStr == "video/x-matroska") return ContentType::VIDEO_MKV;

        if (contentTypeStr == "font/woff") return ContentType::FONT_WOFF;
        if (contentTypeStr == "font/woff2") return ContentType::FONT_WOFF2;
        if (contentTypeStr == "font/ttf") return ContentType::FONT_TTF;
        if (contentTypeStr == "font/otf") return ContentType::FONT_OTF;
        if (contentTypeStr == "application/vnd.ms-fontobject") return ContentType::FONT_EOT;

        if (contentTypeStr == "application/zip") return ContentType::APPLICATION_ZIP;
        if (contentTypeStr == "application/x-rar-compressed") return ContentType::APPLICATION_RAR;
        if (contentTypeStr == "application/gzip") return ContentType::APPLICATION_GZIP;
        if (contentTypeStr == "application/x-tar") return ContentType::APPLICATION_TAR;
        if (contentTypeStr == "application/x-7z-compressed") return ContentType::APPLICATION_7Z;

        if (contentTypeStr == "application/msword") return ContentType::APPLICATION_WORD;
        if (contentTypeStr == "application/vnd.openxmlformats-officedocument.wordprocessingml.document")
            return
                ContentType::APPLICATION_WORD_XML;
        if (contentTypeStr == "application/vnd.ms-excel") return ContentType::APPLICATION_EXCEL;
        if (contentTypeStr == "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")
            return
                ContentType::APPLICATION_EXCEL_XML;
        if (contentTypeStr == "application/vnd.ms-powerpoint") return ContentType::APPLICATION_POWERPOINT;
        if (contentTypeStr == "application/vnd.openxmlformats-officedocument.presentationml.presentation")
            return
                ContentType::APPLICATION_POWERPOINT_XML;

        if (contentTypeStr == "application/xhtml+xml") return ContentType::APPLICATION_XHTML;
        if (contentTypeStr == "application/rss+xml") return ContentType::APPLICATION_RSS;
        if (contentTypeStr == "application/atom+xml") return ContentType::APPLICATION_ATOM;
        if (contentTypeStr == "text/event-stream") return ContentType::TEXT_EVENT_STREAM;

        return ContentType::UNKNOWN;
    }

    // Add this function to the existing ContentType enum class file

    // Function to get ContentType from filename
    inline ContentType getContentTypeFromFilename(const std::string& filename)
    {
        // Find the file extension (everything after the last '.')
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos || dotPos == filename.length() - 1)
        {
            return ContentType::APPLICATION_OCTET_STREAM; // Default to binary if no extension
        }

        // Extract the extension and convert to lowercase for case-insensitive matching
        std::string extension = filename.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        // Map file extensions to content types
        if (extension == "txt") return ContentType::TEXT_PLAIN;
        if (extension == "html" || extension == "htm") return ContentType::TEXT_HTML;
        if (extension == "css") return ContentType::TEXT_CSS;
        if (extension == "js") return ContentType::TEXT_JAVASCRIPT;
        if (extension == "json") return ContentType::APPLICATION_JSON;
        if (extension == "xml") return ContentType::APPLICATION_XML;
        if (extension == "pdf") return ContentType::APPLICATION_PDF;
        if (extension == "csv") return ContentType::TEXT_CSV;
        if (extension == "md") return ContentType::TEXT_MARKDOWN;

        // Image types
        if (extension == "jpg" || extension == "jpeg") return ContentType::IMAGE_JPEG;
        if (extension == "png") return ContentType::IMAGE_PNG;
        if (extension == "gif") return ContentType::IMAGE_GIF;
        if (extension == "webp") return ContentType::IMAGE_WEBP;
        if (extension == "svg") return ContentType::IMAGE_SVG;
        if (extension == "tiff" || extension == "tif") return ContentType::IMAGE_TIFF;
        if (extension == "bmp") return ContentType::IMAGE_BMP;

        // Audio types
        if (extension == "mp3") return ContentType::AUDIO_MPEG;
        if (extension == "wav") return ContentType::AUDIO_WAV;
        if (extension == "flac") return ContentType::AUDIO_FLAC;
        if (extension == "ogg") return ContentType::AUDIO_OGG;
        if (extension == "webm") return ContentType::AUDIO_WEBM;
        if (extension == "m4a") return ContentType::AUDIO_MP4;

        // Video types
        if (extension == "mp4") return ContentType::VIDEO_MP4;
        if (extension == "avi") return ContentType::VIDEO_AVI;
        if (extension == "mov") return ContentType::VIDEO_QUICKTIME;
        if (extension == "mkv") return ContentType::VIDEO_MKV;

        // Font types
        if (extension == "woff") return ContentType::FONT_WOFF;
        if (extension == "woff2") return ContentType::FONT_WOFF2;
        if (extension == "ttf") return ContentType::FONT_TTF;
        if (extension == "otf") return ContentType::FONT_OTF;
        if (extension == "eot") return ContentType::FONT_EOT;

        // Archive types
        if (extension == "zip") return ContentType::APPLICATION_ZIP;
        if (extension == "rar") return ContentType::APPLICATION_RAR;
        if (extension == "gz" || extension == "gzip") return ContentType::APPLICATION_GZIP;
        if (extension == "tar") return ContentType::APPLICATION_TAR;
        if (extension == "7z") return ContentType::APPLICATION_7Z;

        // Office document types
        if (extension == "doc") return ContentType::APPLICATION_WORD;
        if (extension == "docx") return ContentType::APPLICATION_WORD_XML;
        if (extension == "xls") return ContentType::APPLICATION_EXCEL;
        if (extension == "xlsx") return ContentType::APPLICATION_EXCEL_XML;
        if (extension == "ppt") return ContentType::APPLICATION_POWERPOINT;
        if (extension == "pptx") return ContentType::APPLICATION_POWERPOINT_XML;

        // Additional types
        if (extension == "xhtml") return ContentType::APPLICATION_XHTML;
        if (extension == "wasm") return ContentType::APPLICATION_WASM;

        // Default to unknown/binary if no match
        return ContentType::APPLICATION_OCTET_STREAM;
    }

    // Optional: Overloaded version that works with file paths
    inline ContentType getContentTypeFromFilename(const std::string& filepath, bool)
    {
        // Extract filename from full path
        size_t lastSlashPos = filepath.find_last_of("/\\");
        std::string filename = (lastSlashPos == std::string::npos)
                                   ? filepath
                                   : filepath.substr(lastSlashPos + 1);

        return getContentTypeFromFilename(filename);
    }

    inline bool isTextFile(const ContentType type)
    {
        switch (type)
        {
        case ContentType::TEXT_PLAIN:
        case ContentType::TEXT_HTML:
        case ContentType::TEXT_CSS:
        case ContentType::TEXT_JAVASCRIPT:
        case ContentType::TEXT_CSV:
        case ContentType::TEXT_XML:
        case ContentType::TEXT_MARKDOWN:
        case ContentType::APPLICATION_JSON:
        case ContentType::APPLICATION_XML:
        case ContentType::APPLICATION_FORM_URLENCODED:
        case ContentType::APPLICATION_GRAPHQL:
        case ContentType::APPLICATION_XHTML:
        case ContentType::APPLICATION_RSS:
        case ContentType::APPLICATION_ATOM:
        case ContentType::TEXT_EVENT_STREAM:
            return true;
        default:
            return false;
        }
    }
} // namespace httpxx
