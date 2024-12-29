//
// Created by iustus on 12/21/24.
//

#ifndef HTTPOBJECTS_H
#define HTTPOBJECTS_H
#include <gtest/gtest.h>

#include "httpxx/v2/core/response.h"

TEST(httpxx_objects, Request_parse) {
  constexpr auto raw_req = R"(
GET / HTTP/1.1
)";

  const httpxx::v2::Request req{raw_req};
  const httpxx::v2::Request result{httpxx::HttpMethod::GET, "/"};
  ASSERT_EQ(req, result);
}


#endif //HTTPOBJECTS_H