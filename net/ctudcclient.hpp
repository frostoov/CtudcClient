#pragma once

#include "ctudcconn.hpp"
#include <trek/net/response.hpp>
#include "views/view.hpp"

#include <unordered_map>

class CtudcClient {
    using Views = std::unordered_map<std::string, View*>;
public:
    void addView(View* view);
    void handleReponse(const trek::net::Response& response);
private:
    Views mViews;
};
