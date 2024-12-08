#include "http_handlers.hh"
#include "http_headers.hh"
#include "http_objects.hh"
#include <cstring>
#include <filesystem>
#include <sstream>
#include <unistd.h>

#include "http_builders.hh"
#include "http_config.hh"

namespace httpxx
{
    inline float extractHttpVersion(const std::string& http_version)
    {
        std::string r{};
        float res{};

        for (const auto& c : http_version)
        {
            if (std::isdigit(c) || c == '.')
            {
                r += c;
            }
        }

        try
        {
            res = std::stof(r);
        }
        catch (std::exception& e)
        {
            // TODO: answer with a server error.
        }

        return res;
    }

    inline std::vector<std::string> split(const std::string& str,
                                          const std::string& delimiter)
    {
        auto s = str;
        std::vector<std::string> tokens;
        size_t pos = 0;
        while ((pos = s.find(delimiter)) != std::string::npos)
        {
            std::string token = s.substr(0, pos);
            token.erase(token.begin(),
                        std::ranges::find_if(token, [](const unsigned char ch)
                        {
                            return !std::isspace(ch);
                        }));
            tokens.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        tokens.push_back(s);

        return tokens;
    }

    namespace handlers
    {
        inline Request createRequest(const char* _request_string)
        {
            RequestLine rl{};

            std::istringstream iss(_request_string);
            std::string line{};
            std::getline(iss, line);
            assert(line.find("HTTP") != std::string::npos);

            std::istringstream intrss(line);
            std::vector<std::string> headers;
            while (std::getline(intrss, line, ' '))
            {
                headers.push_back(line);
            }
            assert(headers.size() == 3);

            rl.method = stringToHttpMethod(headers.at(0));
            auto uri = headers.at(1);
            rl.uri = uri;
            rl.http_version = extractHttpVersion(headers.at(2));

            Headers hd{httpxx::headers};
            while (std::getline(iss, line))
            {
                // Trim whitespace and check for empty line or just CR/LF
                if (line.empty() || line == "\r")
                {
                    break;
                }

                std::istringstream hss(line);

                auto hv = split(line, ":");
                const auto& head = hv.at(0);
                auto value = hv.at(1);
                assert(not value.empty());

                if (hd.contains(head))
                {
                    hd.at(head) = std::make_optional(value);
                }
                else
                {
                    continue;
                }
            }

            // for (const auto& [k, v]: hd) {
            //
            //     if (v.has_value()) {
            //
            //     std::clog << k << ":" << v.value() << "\n";
            //     }
            //
            // }

            std::getline(iss, line);
            Body body{line};

            auto rq = Request{rl, hd, std::make_optional(body)};
            return rq;
        }

        void response_write(const Response& response, const int client_fd)
        {
            write(client_fd, response.toString().c_str(),
                  std::strlen(response.toString().c_str()));
        }

        auto serve_file(const std::filesystem::path& path) -> Response
        {
            // Check if file exists
            if (!std::filesystem::exists(path))
            {
                return http::ResponseBuilder{}
                .setStatusCode(StatusCodes::NOT_FOUND)
                .setContentType(ContentType::TEXT_HTML)
                .setBody("<h1>404 - File Not Found</h1>")
                .build();
            }

            // Attempt to read the file
            try
            {
                // Open file in binary mode to read all content as raw bytes
                std::ifstream file(path, std::ios::binary | std::ios::in);
                if (!file)
                {
                    throw std::ios_base::failure("Failed to open file");
                }

                // Read the file content into a vector of bytes
                std::ostringstream sout;
                std::copy(std::istreambuf_iterator<char>(file),
                          std::istreambuf_iterator<char>(),
                          std::ostreambuf_iterator<char>(sout));
                // Get content type from the file's extension or logic
                ContentType content_type = getContentTypeFromFilename(path);

                // Create and return the HTTP response
                return http::ResponseBuilder{}
                .setContentType(content_type) // Using the enum value
                .setStatusCode(StatusCodes::OK)
                .setHeader("Content-Length", std::to_string(sout.str().size()))
                .setBody(sout.str()) // Use the binary content as the body
                .build();
            }
            catch (const std::exception& e)
            {
                // Handle failure in file reading
                return http::ResponseBuilder{}
                .setStatusCode(StatusCodes::INTERNAL_SERVER_ERROR)
                .setContentType(ContentType::TEXT_HTML)
                .setBody("<h1>500 - Internal Server Error</h1>")
                .build();
            }
        }


    void handle_request(const httpxx::Router& router, const Config& config, int client_fd,
                        const char* buffer)
    {
        const auto req = createRequest(buffer);\
        Response res{};
        if (req.requestsFile())
        {
            std::clog << "FIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIILE " << req.request_line.uri
                << '\n';
            std::clog << std::format("{}{}", config._www_path, req.request_line.uri) << '\n';
            res = serve_file(std::format("{}{}", config._www_path, req.request_line.uri));
        }
        else
        {
            res = router.get_handler_fn(req.request_line.uri)(client_fd, req);
        }
        response_write(res, client_fd);
        close(client_fd);
    }
} // namespace handlers
} // namespace httpxx
