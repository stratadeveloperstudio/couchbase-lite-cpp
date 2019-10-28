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
#include "SGURLEndpoint.h"
#include "SGBucketManager.h"
#include "SGReplicatorConfiguration.h"

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

    SGBucket* SGBucketManager::createBucket(const string &bucket_name, const string &bucket_path) {
        SGBucket *buc = new SGBucket(bucket_name, bucket_path);
        if(!buc) return nullptr;
        buckets_.insert(make_pair(bucket_name, buc));
        return buc;
    }

    vector<string> SGBucketManager::getBuckets() {
        vector<string> buckets;
        for(auto &element : buckets_) {
            buckets.push_back(element.first);
        }
        return buckets;
    }

    SGBucketReturnStatus SGBucketManager::deleteBucket(const string &bucket_name) {
        if(!bucketExists(bucket_name)) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }
        delete buckets_.at(bucket_name);
        buckets_.erase(bucket_name);
        return SGBucketReturnStatus::kNoError;
    }

    bool SGBucketManager::bucketExists(const string &bucket_name) {
        return buckets_.find(bucket_name) != buckets_.end();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SGBucket::SGBucket(const string &bucket_name, const string &bucket_path) : bucket_name_(bucket_name) {
        db_ = unique_ptr<SGDatabase>(new SGDatabase(bucket_name, bucket_path));
        db_->open();
    }

    SGBucketReturnStatus SGBucket::createDocument(const pair<string, string> &doc) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to create document but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        if(get<0>(doc).empty() || get<1>(doc).empty()) {
            DEBUG("ID and body contents of document may not be empty.\n");
            return SGBucketReturnStatus::kError;
        }

        SGMutableDocument newDoc(db_.get(), get<0>(doc));

        if(newDoc.exist()) {
            DEBUG("A document with the provided ID already exists. Modify the ID and try again.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!newDoc.setBody(get<1>(doc))) {
            DEBUG("Error setting content of created document. Body must be in JSON format.\n");
            return SGBucketReturnStatus::kError;
        }

        if(db_->save(&newDoc) != SGDatabaseReturnStatus::kNoError) {
            DEBUG("Error saving document to database.\n");
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::updateDocument(const string &doc_name, const string &json_body) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to create document but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        if(doc_name.empty()) {
            DEBUG("Received empty document ID, cannot edit.\n");
            return SGBucketReturnStatus::kError;
        }

        SGMutableDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            DEBUG("Document does not exist. Cannot edit.\n");
            return SGBucketReturnStatus::kError;
        }

        doc.setBody(json_body);
        if(db_->save(&doc) != SGDatabaseReturnStatus::kNoError) {
            DEBUG("Error saving document to database.\n");
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::deleteDocument(const string &doc_name) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to create document but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        if(doc_name.empty()) {
            DEBUG("Received empty document ID, cannot delete.\n");
            return SGBucketReturnStatus::kError;
        }

        SGDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            DEBUG("Document does not exist. Cannot delete.\n");
            return SGBucketReturnStatus::kError;
        }

        if(db_->deleteDocument(&doc) != SGDatabaseReturnStatus::kNoError) {
            DEBUG("Error deleting document.\n");
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::getDocumentKeys(vector<string> &doc_keys) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to create document but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        return db_->getAllDocumentsKey(doc_keys) ? SGBucketReturnStatus::kNoError : SGBucketReturnStatus::kError;
    }

    SGBucketReturnStatus SGBucket::readDocument(const string &doc_name, string &json_body) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to read document but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        SGDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            DEBUG("Document does not exist. Cannot read.\n");
            return SGBucketReturnStatus::kError;
        }

        json_body = doc.getBody();
        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::readContents(unordered_map<string, string> &contents) {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to read contents but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        string json_body;
        vector<string> keys;
        getDocumentKeys(keys);
        contents.clear();
        for(string key : keys) {
            SGDocument doc(db_.get(), key);
            contents.insert(make_pair(key, doc.getBody()));
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::startReplicator(string url,
                                                   SGReplicatorConfiguration::ReplicatorType rep_type,
                                                   string username,
                                                   string password,
                                                   vector<string> channels,
                                                   const function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)> &stat_changed,
                                                   const function<void(bool, string, string, bool, bool)> &document_ended,
                                                   const function<void(const string, const string)> &valid_listener) 
    {
        if(db_ == nullptr) {
            DEBUG("Bucket does not exist.\n");
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            DEBUG("Attempted to read contents but bucket is not open.\n");
            return SGBucketReturnStatus::kError;
        }

        url_endpoint_ = unique_ptr<SGURLEndpoint>(new SGURLEndpoint(url));

        if(!url_endpoint_->init()){
            DEBUG("Invalid URL endpoint.\n");
            return SGBucketReturnStatus::kError;
        }

        replicator_configuration_ = unique_ptr<SGReplicatorConfiguration>(new SGReplicatorConfiguration(db_.get(), url_endpoint_.get()));

        if(!username.empty() && !password.empty()) {
            basic_authenticator_ = unique_ptr<SGBasicAuthenticator>(new SGBasicAuthenticator(username,password));
            replicator_configuration_->setAuthenticator(basic_authenticator_.get());
        }

        replicator_configuration_->setReplicatorType(rep_type);
        replicator_configuration_->setChannels(channels);
        replicator_ = unique_ptr<SGReplicator>(new SGReplicator(replicator_configuration_.get()));
        
        if(stat_changed) replicator_->addChangeListener(stat_changed);
        if(document_ended) replicator_->addDocumentEndedListener(document_ended);

        if(valid_listener) {
            replicator_->addValidationListener(valid_listener);
        } else {
            replicator_->addValidationListener(bind(&SGBucket::onValidate_, this, placeholders::_1, placeholders::_2));
        }

        if(replicator_->start() != SGReplicatorReturnStatus::kNoError) {
            DEBUG("Problems starting the replicator.\n");
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    void SGBucket::stopReplicator() {
        replicator_->stop();
    }

    void SGBucket::setChannels(vector<string> channels) {
        replicator_configuration_->setChannels(channels);
    }

    void SGBucket::restartReplicator() {
        replicator_->restart();
    }
}