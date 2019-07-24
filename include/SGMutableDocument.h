//
//  SGMutableDocument.h
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

#ifndef SGMUTABLEDOCUMENT_H
#define SGMUTABLEDOCUMENT_H

#include "SGDocument.h"
namespace Spyglass {
    class SGMutableDocument : public SGDocument {
    public:
        SGMutableDocument(SGDatabase *database, const std::string &docId);

        template<typename T>
        void set(const std::string &key, T value) { mutable_dict_->set(key, value); }

        fleece::impl::MutableArray *getMutableArray(fleece::slice key) { return mutable_dict_->getMutableArray(key); }

        fleece::impl::MutableDict *getMutableDict(fleece::slice key) { return mutable_dict_->getMutableDict(key); }

        /** SGMutableDocument setBody.
        * @brief Given a string in json format this will convert it to fleece mutable dictionary. true on success, false otherwise
        * @param body The reference to the string json format.
        */
        bool setBody(const std::string &body);

    private:
        fleece::alloc_slice alloc_slice_;
    };
}
#endif //SGMUTABLEDOCUMENT_H
