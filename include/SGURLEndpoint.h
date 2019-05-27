//
//  SGURLEndpoint.h
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

#ifndef SGURLENDPOINT_H
#define SGURLENDPOINT_H

#include <string>

#include <c4.h>

namespace Spyglass {
    class SGURLEndpoint {

    public:
        SGURLEndpoint();

        /** SGURLEndpoint.
        * @brief Sets uri.
        * @param full_url sync-gateway url. i.e "ws://localhost:4984/staging"
        */
        SGURLEndpoint(const std::string &full_url);

        virtual ~SGURLEndpoint();

        /** SGURLEndpoint init.
        * @brief Parse uri to small parts and set member variables. True on success, otherwise false.
        */
        bool init();

        const std::string &getHost() const;

        void setHost(const std::string &host);

        const std::string &getSchema() const;

        void setSchema(const std::string &schema);

        const std::string &getPath() const;

        void setPath(const std::string &path);

        const uint16_t &getPort() const;

        void setPort(const uint16_t &port);

        const C4Address &getC4Address() const;

    private:
        std::string uri;
        std::string host_;
        std::string schema_;
        std::string path_;
        uint16_t port_ {0};

        C4Address c4address_;
    };
}

#endif //SGURLENDPOINT_H