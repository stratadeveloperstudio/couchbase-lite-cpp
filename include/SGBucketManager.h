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

#ifndef SGBUCKETMANAGER_H
#define SGBUCKETMANAGER_H

#include <string>

#include "SGReplicator.h"
#include <SGDocument.h>
#include <SGMutableDocument.h>

// #include <fleece/FleeceImpl.hh>
// #include <fleece/MutableArray.hh>
// #include <fleece/MutableDict.hh>

namespace Strata {

    // Forward declaration is required due to the circular include for SGBucket<->SGDocument.
    class SGDocument;

    enum class SGBucketReturnStatus {
        kError,
        kNoError
    };

    class SGBucket {
    public:
        SGBucket(const std::string &bucket_name, const std::string &bucket_path = std::string());

        /* Standard CRUD operations */

        SGBucketReturnStatus createDocument(const std::pair<std::string, std::string> &doc);

        SGBucketReturnStatus updateDocument(const std::string &doc_name, const std::string &json_body);

        SGBucketReturnStatus readDocument(const std::string &doc_name, std::string &json_body);

        SGBucketReturnStatus deleteDocument(const std::string &doc_name);

        /* Additional database operations */

        SGBucketReturnStatus getDocumentKeys(std::vector<std::string> &doc_keys);

        SGBucketReturnStatus readContents(std::unordered_map<std::string, std::string> &contents);

        /* Replicator API */

        // SGBucketReturnStatus startReplicator(std::string url,
        //                                      std::string rep_type,
        //                                      std::string username,
        //                                      std::string password,
        //                                      std::vector<std::string> channels,
        //                                      const std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)> &status_changed,
        //                                      const std::function<void(bool, std::string, std::string, bool, bool)> &document_ended,
        //                                      const std::function<void(const std::string, const std::string)> &valid_listener);

        SGBucketReturnStatus startReplicator(std::string url,
                                                   std::string rep_type,
                                                   std::string username,
                                                   std::string password,
                                                   std::vector<std::string> channels,
                                                   const std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)> &stat_changed = std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)>(),
                                                   const std::function<void(bool, std::string, std::string, bool, bool)> &document_ended = std::function<void(bool, std::string, std::string, bool, bool)>(),
                                                   const std::function<void(const std::string, const std::string)> &valid_listener = std::function<void(const std::string, const std::string)>());

        SGBucketReturnStatus stopReplicator();    

    private:
        std::string bucket_name_;
        
        std::unique_ptr<SGDatabase> db_;

        std::unique_ptr<SGURLEndpoint> url_endpoint_ = nullptr;

        std::unique_ptr<SGReplicatorConfiguration> replicator_configuration_ = nullptr;

        std::unique_ptr<SGBasicAuthenticator> basic_authenticator_ = nullptr;

        std::unique_ptr<SGReplicator> replicator_ = nullptr;

        void onValidate_(const std::string& doc_id, const std::string& json_body) {
            printf("MiniHCS: New incoming revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), json_body.c_str() );
            SGDocument document(db_.get(), doc_id);
            printf("MiniHCS: existing revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), document.getBody().c_str() );
        }

        friend SGDocument;
    };

    class SGBucketManager {
    public:
        SGBucketManager();

        ~SGBucketManager();

        std::vector<std::string> getBuckets();

        bool bucketExists(const std::string &bucket_name);

        SGBucket* createBucket(const std::string &bucket_name, const std::string &bucket_path = std::string());

        SGBucketReturnStatus deleteBucket(const std::string &bucket_name);

    private:
        std::unordered_map<std::string, SGBucket*> buckets_;
    };
}

#endif //SGBUCKETMANAGER_H
