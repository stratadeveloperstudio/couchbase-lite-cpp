//
//  SGDocument.cpp
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

#include <string>
#include "SGDocument.h"

using fleece::impl::Value;
using namespace std;

#define DEBUG(...) printf("SGDocument: "); printf(__VA_ARGS__)

#ifdef SHOW_DETAILED_DATABASE_MESSAGES
const bool show_detailed_database_messages_ = true;
#else
const bool show_detailed_database_messages_ = false;
#endif

namespace Strata {
    SGDocument::SGDocument() {}

    SGDocument::~SGDocument() {
        c4doc_free(c4document_);
    }

    SGDocument::SGDocument(SGDatabase *database, const std::string &docId) {
        setC4document(database->getDocumentById(docId));
        setId(docId);
        initMutableDict();
    }

    const std::string &SGDocument::getId() const {
        return id_;
    }

    void SGDocument::setId(const std::string &id) {
        id_ = id;
    }

    bool SGDocument::exist() const {
        return c4document_ != nullptr;
    }

    std::string SGDocument::getRevision()
    {
        if (c4document_ == nullptr) {
            return std::string();
        }

        return fleece::slice(c4document_->revID).asString();
    }
    
    const std::string SGDocument::getBody() const {
        return mutable_dict_->asDict()->toJSONString();
    }

    const fleece::impl::Dict *SGDocument::asDict() const {
        return mutable_dict_->asDict();
    }

    void SGDocument::initMutableDict() {
        if(exist()) {
            mutable_dict_ = fleece::impl::MutableDict::newDict(Value::fromData(c4document_->selectedRev.body)->asDict());
            if(show_detailed_database_messages_) {
                DEBUG("Doc Id: %s, body: %s, revision:%s\n", id_.c_str(), getBody().c_str(), fleece::slice(c4document_->selectedRev.revID).asString().c_str());
            }
            return;
        }
        // Init a new mutable dict
        mutable_dict_ = fleece::impl::MutableDict::newDict();
        DEBUG("c4document_ is null\n");
    }

    const fleece::impl::Value *SGDocument::get(const std::string &keyToFind) {
        return mutable_dict_->get(keyToFind);
    }

    bool SGDocument::empty() const {
        return mutable_dict_->empty();
    }

    C4Document *SGDocument::getC4document() const {
        return c4document_;
    }

    void SGDocument::setC4document(C4Document *doc) {
        // TODO: Should we free c4document_ before setting it with new reference.
        c4document_ = doc;
    }
}