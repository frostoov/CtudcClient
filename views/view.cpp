#include "view.hpp"

using trek::net::Response;

using std::string;
using std::logic_error;

View::View(const string& name, const Methods& method, QObject* parent)
    : QObject(parent),
      mMethods(method),
      mName(name) { }

void View::handleReponse(const Response& response) {
    if(response.object != mName)
        throw logic_error("View::handleReponse invalid object name");
    auto& method = mMethods.at(response.method);
    method(response);
}

const string& View::name() const {
    return mName;
}
