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

#include <iostream>
#include <string>

#include "SGDatabase.h"
#include "SGBucketManager.h"

#define DEBUG(...) printf("SGBucketManager: "); printf(__VA_ARGS__)

using namespace std;

namespace Strata {
    SGBucketManager::SGBucketManager() {}

    SGBucketManager::~SGBucketManager() {
        for(auto element : buckets_) {
            delete element.second;
        }
        buckets_.clear();
    }
    
    SGBucketReturnStatus SGBucketManager::createBucket(const std::string &bucket_name, const std::string &bucket_path) {
        SGBucket *buc = new SGBucket(bucket_name, bucket_path);
        buckets_.insert(make_pair(bucket_name, buc));
        return SGBucketReturnStatus::kNoError;
    }

    std::vector<std::string> SGBucketManager::getBuckets() {
        std::vector<std::string> buckets;
        for(auto element : buckets_) {
            buckets.push_back(element.first);
        }
        return buckets;
    }

    SGBucket& SGBucketManager::getBucketByName(const std::string &bucket_name) {
        if(buckets_.find(bucket_name) == buckets_.end()) {
            return nullptr;
        }
        return *buckets_.at(bucket_name);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    SGBucket::SGBucket(const std::string &bucket_name, const std::string &bucket_path) {
        // db_ = std::unique_ptr<SGDatabase>(new SGDatabase(bucket_name, bucket_path));
        db_ = new SGDatabase(bucket_name, bucket_path);
        db_->open();
        SGMutableDocument newDoc(db_, "a");
    }

    SGBucket::~SGBucket() {
    }

    SGBucketReturnStatus SGBucket::createDocument(const std::pair<std::string, std::string> &doc) { cout << "\n[VICTOR] Inside createDocument()\n";

        // db_ = new SGDatabase("my-Db");
        //  db_->open();

        // if(db_ == nullptr) cout << "\nIT IS NULLPTR\n"; else cout << "\nIT IS NOT NULLPTR\n" << endl;
        // db_->close();
// cout << "\n[VICTOR] End of createDocument()\n";
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }
// cout << "\n[VICTOR] End of createDocument()\n";
        // if(!db_->isOpen()) {
        //     cout << "Attempted to create document but bucket " << get<1>(doc) << " is not open.";
        //     return SGBucketReturnStatus::kError;
        // }

        // if(get<0>(doc).empty() || get<1>(doc).empty()) {
        //     cout << "ID and body contents of document may not be empty.";
        //     return SGBucketReturnStatus::kError;
        // }

        // SGMutableDocument newDoc(db_, get<0>(doc));

        // if(newDoc.exist()) {
        //     cout << "A document with ID '" + get<0>(doc) + "' already exists. Modify the ID and try again.";
        //     return SGBucketReturnStatus::kError;
        // }

        // if(!newDoc.setBody(get<1>(doc))) {
        //     cout << "Error setting content of created document. Body must be in JSON format.";
        //     return SGBucketReturnStatus::kError;
        // }

        // if(db_->save(&newDoc) != SGDatabaseReturnStatus::kNoError) {
        //     cout << "Error saving document to database.";
        //     return SGBucketReturnStatus::kError;
        // }

        return SGBucketReturnStatus::kNoError;
    }

}