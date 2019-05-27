//
//  SGAuthenticator.h
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

#ifndef SGAUTHENTICATOR_H
#define SGAUTHENTICATOR_H

#include <string>
#include <FleeceImpl.hh>
#include <MutableArray.hh>
#include <MutableDict.hh>

namespace Spyglass {
    class SGAuthenticator {
    public:
        SGAuthenticator() {}

        virtual ~SGAuthenticator() {}

        virtual void authenticate(fleece::Retained<fleece::impl::MutableDict> options) = 0;
    };

    class SGBasicAuthenticator : public SGAuthenticator {
    public:
        SGBasicAuthenticator();

        SGBasicAuthenticator(const std::string &username, const std::string &password);

        virtual ~SGBasicAuthenticator();

        void setUserName(const std::string &username);

        const std::string getUserName() const;

        void setPassword(const std::string &password);

        const std::string getPassword() const;

        /** SGBasicAuthenticator authenticate.
        * @brief Creates a fleece dictionary to set auth type, set username and password.
        * @param options The reference to the mutable fleece dicationary.
        */
        void authenticate(fleece::Retained<fleece::impl::MutableDict> options);

    private:
        std::string username_;
        std::string password_;

    };
}


#endif //SGAUTHENTICATOR_H
