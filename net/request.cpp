#include "request.hpp"

namespace trek {
namespace net {

using nlohmann::json;

Request::Request(const JsonString& object,
                 const JsonString& method,
                 const JsonArray& inputs)
	: object(object),
	  method(method),
	  inputs(inputs) { }

Request::Request(const std::string& request) {
	auto jsonRequest = json::parse(request);
	object = jsonRequest.at("object").get<JsonString>();
	method = jsonRequest.at("method").get<JsonString>();
	inputs = jsonRequest.at("inputs").get<JsonArray>();
}

Request::operator std::string() const {
	return json{
		{"object", object},
		{"method", method},
		{"inputs", inputs},
	} .dump();
}

} //net
} //trek
