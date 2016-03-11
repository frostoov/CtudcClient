#pragma once

#include <json.hpp>

namespace trek {
namespace net {

struct Response {
	using Json       = nlohmann::json;
	using JsonString = Json::string_t;
	using JsonArray  = Json::array_t;
	using JsonBool   = Json::boolean_t;
	Response(const JsonString& object,
	         const JsonString& method,
			 const JsonArray&  outputs = {},
			 JsonBool status = true);
	Response(const std::string& response);
	explicit operator std::string() const;

	JsonString  object;
	JsonString  method;
	JsonArray   outputs;
	JsonBool    status;
};

} //net
} //trek
