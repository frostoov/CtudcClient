#include "ctudcclient.hpp"

using trek::net::Response;

void CtudcClient::addView(View* view) {
    mViews.emplace(view->name(), view);
}

void CtudcClient::handleReponse(const Response& response) {
    auto& view = *mViews.at(response.object);
    view.handleReponse(response);
}
