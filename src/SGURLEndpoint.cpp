//
//  SGURLEndpoint.cpp
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

#include "SGURLEndpoint.h"
#include <fleece/FleeceImpl.hh>
using namespace std;
using namespace fleece;
#define DEBUG(...) printf("SGURLEndpoint: "); printf(__VA_ARGS__)

namespace Strata {
    SGURLEndpoint::SGURLEndpoint() {
        c4address_.hostname = slice();
        c4address_.path = slice();
        c4address_.scheme = slice();
        c4address_.port = 0;
    }

    SGURLEndpoint::~SGURLEndpoint() {}

    SGURLEndpoint::SGURLEndpoint(const std::string &full_url) : SGURLEndpoint() {
        uri = full_url;
    }

    bool SGURLEndpoint::init() {
        C4String dbname;
        // c4address_fromURL won't set proper or complete path
        // ws://localhost:4984/staging
        // schema: ws
        // hostname: localhsot
        // port: 4984
        // path: /
        // Note: staging will be set in dbname. which is the remote DB to replicate from

        if(uri.empty()){
            return false;
        }
        if (!c4address_fromURL(slice(uri), &c4address_, &dbname)) {
            return false;
        }

        DEBUG("c4address_fromURL is valid\n");
        setHost( slice(c4address_.hostname).asString() );
        // HACK
        setPath( slice(dbname).asString() );
        setSchema( slice(c4address_.scheme).asString() );
        setPort(c4address_.port);
        return true;
    }

    const C4Address &SGURLEndpoint::getC4Address() const {
        return c4address_;
    }

    const std::string &SGURLEndpoint::getHost() const {
        return host_;
    }

    void SGURLEndpoint::setHost(const std::string &host) {
        host_ = host;
        c4address_.hostname = slice(host_);
    }

    const std::string &SGURLEndpoint::getSchema() const {
        return schema_;
    }

    void SGURLEndpoint::setSchema(const std::string &schema) {
        schema_ = schema;
        c4address_.scheme = slice(schema_);
    }

    const std::string &SGURLEndpoint::getPath() const {
        return path_;
    }

    void SGURLEndpoint::setPath(const std::string &path) {
        //Warning: Don't set c4address_.path here like other setters!
        // Given a url ws://localhost:4984/staging
        // staging would be the path
        path_ = path;
    }

    const uint16_t &SGURLEndpoint::getPort() const {
        return port_;
    }

    void SGURLEndpoint::setPort(const uint16_t &port) {
        port_ = port;
        c4address_.port = port_;
    }
}