//
//  SGDocument.h
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

#ifndef SGDOCUMENT_H
#define SGDOCUMENT_H

#include <string>
#include "SGDatabase.h"
#include <litecore/c4Document+Fleece.h>
#include <fleece/FleeceImpl.hh>
#include <fleece/MutableArray.hh>
#include <fleece/MutableDict.hh>

namespace Spyglass {
    // Forward declaration is required due to the circular include for SGDatabase<->SGDocument.
    class SGDatabase;

    class SGDocument {
    public:
        SGDocument();

        virtual ~SGDocument();

        SGDocument(SGDatabase *database, const std::string &docId);

        C4Document *getC4document() const;

        const std::string &getId() const;

        void setId(const std::string &id);

        /** SGDocument getBody.
        * @brief Stringify fleece object (mutable_dict_) to string json format.
        */
        const std::string getBody() const;

        /** SGDocument asDict.
        * @brief Return the internal mutable_dict_ as fleece Dict object.
        */
        const fleece::impl::Dict *asDict() const;

        /** SGDocument empty.
        * @brief check if MutableDict is empty.
        */
        bool empty() const;

        /** SGDocument get.
        * @brief MutableDict wrapper to access document data.
        * @param keyToFind The reference to the key.
        */
        const fleece::impl::Value *get(const std::string &keyToFind);

        /** SGDocument exist.
        * @brief Check if the document exist in the DB.
        */
        bool exist() const;

    private:
        C4Database *c4db_{nullptr};
        C4Document *c4document_{nullptr};
        // Document ID
        std::string id_;

        void setC4document(C4Document *);

        friend SGDatabase;
    protected:

        /** SGDocument initMutableDict.
        * @brief Loads the document's body and set it to mutable_dict_, if the document exist. Otherwise init mutable_dict_
        */
        void initMutableDict();

        fleece::Retained<fleece::impl::MutableDict> mutable_dict_;
    };
}

#endif //SGDOCUMENT_H
