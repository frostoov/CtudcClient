#pragma once

#include <json.hpp>

namespace trek {
namespace net {

struct Request {
	using Json       = nlohmann::json;
	using JsonString = Json::string_t;
	using JsonArray  = Json::array_t;

	Request(const JsonString& object,
	        const JsonString& method,
			const JsonArray& inputs = {});
	Request(const std::string& request);
	explicit operator std::string() const;

	JsonString  object;
	JsonString  method;
	JsonArray   inputs;
};

} //net
} //trek
