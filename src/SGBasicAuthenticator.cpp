//
//  SGBasicAuthenticator.cpp
//
//  Copyright 2014 ON Semiconductor.
//  All rights reserved. This software and/or documentation is licensed by ON Semiconductor under
//  limited terms and conditions. The terms and conditions pertaining to the software and/or documentation are available at
//  http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf (“ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software”).
//  Do not use this software and/or documentation unless you have carefully read and you agree to the limited terms and conditions.
//  By using this software and/or documentation, you agree to the limited terms and conditions.
//
//  Copyright 2019 ON Semiconductor
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "SGAuthenticator.h"
#include <litecore/c4Replicator.h>
using namespace fleece;
using namespace fleece::impl;

namespace Strata {
    SGBasicAuthenticator::SGBasicAuthenticator() {}

    SGBasicAuthenticator::SGBasicAuthenticator(const std::string &username, const std::string &password) {
        setUserName(username);
        setPassword(password);
    }

    SGBasicAuthenticator::~SGBasicAuthenticator() {}

    void SGBasicAuthenticator::setUserName(const std::string &username) {
        username_ = username;
    }

    const std::string SGBasicAuthenticator::getUserName() const {
        return username_;
    }

    void SGBasicAuthenticator::setPassword(const std::string &password) {
        password_ = password;
    }

    const std::string SGBasicAuthenticator::getPassword() const {
        return password_;
    }

    void SGBasicAuthenticator::authenticate(fleece::Retained<fleece::impl::MutableDict> options) {
        fleece::Retained<fleece::impl::MutableDict> auth = fleece::impl::MutableDict::newDict();
        auth->set(slice(kC4ReplicatorAuthType), slice(kC4AuthTypeBasic));
        auth->set(slice(kC4ReplicatorAuthUserName), slice(username_));
        auth->set(slice(kC4ReplicatorAuthPassword), slice(password_));
        options->set(slice(kC4ReplicatorOptionAuthentication), auth);
    }
}
