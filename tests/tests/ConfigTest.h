//
// Created by iustus on 12/6/24.
//

#ifndef CONFIGTEST_H
#define CONFIGTEST_H
#include <gtest/gtest.h>

#include "../../lib/core/http_config.hh"
#include "../../lib/core/http_enums.hh"

TEST(ConfigTest, Config_fromFile)
{
    // Use an existing directory
    const auto test_case_config = Config{6969, "/home/iustus/http_test/"};
    const auto config = Config::fromFile("../../config.toml");
    EXPECT_EQ(config, test_case_config);
}

TEST(ConfigTest, ContentTypeGetsDetectedRight) {
    // Test HTML files
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/index.html"),
              httpxx::ContentType::TEXT_HTML)
        << "HTML file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("webpage.htm"),
              httpxx::ContentType::TEXT_HTML)
        << "Alternative HTML file extension detection failed";

    // Test various image types
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/images/pic.jpg"),
              httpxx::ContentType::IMAGE_JPEG)
        << "JPEG file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("image.jpeg"),
              httpxx::ContentType::IMAGE_JPEG)
        << "JPEG alternative file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/assets/logo.png"),
              httpxx::ContentType::IMAGE_PNG)
        << "PNG file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("icon.svg"),
              httpxx::ContentType::IMAGE_SVG)
        << "SVG file detection failed";

    // Test various document types
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/docs/report.pdf"),
              httpxx::ContentType::APPLICATION_PDF)
        << "PDF file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("data.json"),
              httpxx::ContentType::APPLICATION_JSON)
        << "JSON file detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/notes/readme.md"),
              httpxx::ContentType::TEXT_MARKDOWN)
        << "Markdown file detection failed";

    // Test case insensitivity
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/UPPERCASE.PNG"),
              httpxx::ContentType::IMAGE_PNG)
        << "Uppercase file extension detection failed";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/MixedCase.JpEg"),
              httpxx::ContentType::IMAGE_JPEG)
        << "Mixed case file extension detection failed";

    // Test default/unknown types
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/random/file.xyz"),
              httpxx::ContentType::APPLICATION_OCTET_STREAM)
        << "Unknown file type not defaulting correctly";
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/no_extension_file"),
              httpxx::ContentType::APPLICATION_OCTET_STREAM)
        << "No extension file not defaulting correctly";

    // Test full path handling
    EXPECT_EQ(httpxx::getContentTypeFromFilename("/very/long/path/to/script.js", true),
              httpxx::ContentType::TEXT_JAVASCRIPT)
        << "Full path JavaScript file detection failed";
}

// Optional: If you want to test converting to MIME string
TEST(ConfigTest, ContentTypeToStringConversion) {
    EXPECT_EQ(httpxx::contentTypeToString(httpxx::ContentType::TEXT_HTML),
              "text/html")
        << "HTML MIME type string conversion failed";

    EXPECT_EQ(httpxx::contentTypeToString(httpxx::ContentType::IMAGE_PNG),
              "image/png")
        << "PNG MIME type string conversion failed";
}

#endif //CONFIGTEST_H
