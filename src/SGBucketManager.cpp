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

    SGBucket* SGBucketManager::createBucket(const std::string &bucket_name, const std::string &bucket_path) {
        SGBucket *buc = new SGBucket(bucket_name, bucket_path);
        if(!buc) return nullptr;
        buckets_.insert(make_pair(bucket_name, buc));
        return buc;
    }

    std::vector<std::string> SGBucketManager::getBuckets() {
        std::vector<std::string> buckets;
        for(auto &element : buckets_) {
            buckets.push_back(element.first);
        }
        return buckets;
    }

    SGBucketReturnStatus SGBucketManager::deleteBucket(const std::string &bucket_name) {
        if(!bucketExists(bucket_name)) {
            cout << "\nBucket named \"" << bucket_name << "\" does not exist.";
            return SGBucketReturnStatus::kError;
        }
        delete buckets_.at(bucket_name);
        buckets_.erase(bucket_name);
        return SGBucketReturnStatus::kNoError;
    }

    bool SGBucketManager::bucketExists(const std::string &bucket_name) {
        return buckets_.find(bucket_name) != buckets_.end();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SGBucket::SGBucket(const std::string &bucket_name, const std::string &bucket_path) : bucket_name_(bucket_name) {
        db_ = std::unique_ptr<SGDatabase>(new SGDatabase(bucket_name, bucket_path));
        db_->open();
    }

    SGBucketReturnStatus SGBucket::createDocument(const std::pair<std::string, std::string> &doc) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to create document but bucket " << get<1>(doc) << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        if(get<0>(doc).empty() || get<1>(doc).empty()) {
            cout << "ID and body contents of document may not be empty.";
            return SGBucketReturnStatus::kError;
        }

        SGMutableDocument newDoc(db_.get(), get<0>(doc));

        if(newDoc.exist()) {
            cout << "A document with ID '" + get<0>(doc) + "' already exists. Modify the ID and try again.";
            return SGBucketReturnStatus::kError;
        }

        if(!newDoc.setBody(get<1>(doc))) {
            cout << "Error setting content of created document. Body must be in JSON format.";
            return SGBucketReturnStatus::kError;
        }

        if(db_->save(&newDoc) != SGDatabaseReturnStatus::kNoError) {
            cout << "Error saving document to database.";
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::updateDocument(const std::string &doc_name, const std::string &json_body) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to create document but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        if(doc_name.empty()) {
            cout << "Received empty document ID, cannot edit.";
            return SGBucketReturnStatus::kError;
        }

        SGMutableDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            cout << "Document with ID = '" + doc_name + "' does not exist. Cannot edit.";
            return SGBucketReturnStatus::kError;
        }

        doc.setBody(json_body);
        if(db_->save(&doc) != SGDatabaseReturnStatus::kNoError) {
            cout << "Error saving document to database.";
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::deleteDocument(const std::string &doc_name) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to create document but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        if(doc_name.empty()) {
            cout << "Received empty document ID, cannot delete.";
            return SGBucketReturnStatus::kError;
        }

        SGDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            cout << "Document with ID = '" + doc_name + "' does not exist. Cannot delete.";
            return SGBucketReturnStatus::kError;
        }

        if(db_->deleteDocument(&doc) != SGDatabaseReturnStatus::kNoError) {
            cout << "Error deleting document " + doc_name + ".";
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::getDocumentKeys(std::vector<std::string> &doc_keys) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to create document but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        return db_->getAllDocumentsKey(doc_keys) ? SGBucketReturnStatus::kNoError : SGBucketReturnStatus::kError;
    }

    SGBucketReturnStatus SGBucket::readDocument(const std::string &doc_name, std::string &json_body) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to read document but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        SGDocument doc(db_.get(), doc_name);

        if(!doc.exist()) {
            cout << "Document with ID = '" + doc_name + "' does not exist. Cannot read.";
            return SGBucketReturnStatus::kError;
        }

        json_body = doc.getBody();
        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::readContents(std::unordered_map<std::string, std::string> &contents) {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to read contents but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        std::string json_body;
        std::vector<std::string> keys;
        getDocumentKeys(keys);
        contents.clear();
        for(std::string key : keys) {
            SGDocument doc(db_.get(), key);
            contents.insert(make_pair(key, doc.getBody()));
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::searchByDocumentKey(const std::string &searching, std::vector<std::string> &doc_keys) {
        cout << "\nInside SGBucket::searchByDocumentKey(), given key: " << searching << endl;
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to search for document but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        doc_keys.clear();
        const static string json = "[\"SELECT\", {\"WHAT\": [\"._id\"], \"WHERE\": [\"LIKE\", [\"._id\", \"\"], \"" + searching + "\"]}]";

        C4Error c4error_ {};
        std::unique_ptr<C4Query, decltype(&c4query_free)> query(c4query_new(db_->getC4db(), fleece::slice(json), &c4error_), &c4query_free);

        if(query != nullptr){
            C4QueryOptions options = kC4DefaultQueryOptions;
            std::unique_ptr<C4QueryEnumerator, decltype(&c4queryenum_free)> query_enumerator(c4query_run(query.get(), &options, c4str(nullptr), &c4error_), &c4queryenum_free);

            if(query_enumerator != nullptr){
                while (bool is_next_result_available = c4queryenum_next(query_enumerator.get(), &c4error_)) {
                    if(is_next_result_available){
                        fleece::slice doc_name = FLValue_AsString(FLArrayIterator_GetValueAt(&query_enumerator->columns, 0));
                        doc_keys.push_back(doc_name.asString());
                    }else{
                        DEBUG("C4QueryEnumerator_Next failed to run.\n");
                        return SGBucketReturnStatus::kError;
                    }
                }
            }else{
                DEBUG("C4QueryEnumerator failed to run.\n");
                return SGBucketReturnStatus::kError;
            }
        }else{
            DEBUG("C4Query failed to execute a query.\n");
            return SGBucketReturnStatus::kError;
        }
        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::startReplicator(std::string url,
                                                   std::string rep_type,
                                                   std::string username,
                                                   std::string password,
                                                   std::vector<std::string> channels,
                                                   const std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)> &stat_changed,
                                                   const std::function<void(bool, std::string, std::string, bool, bool)> &document_ended,
                                                   const std::function<void(const std::string, const std::string)> &valid_listener) 
    {
        if(db_ == nullptr) {
            cout << "\nBucket does not exist.\n" << endl;
            return SGBucketReturnStatus::kError;
        }

        if(!db_->isOpen()) {
            cout << "Attempted to read contents but bucket " << bucket_name_ << " is not open.";
            return SGBucketReturnStatus::kError;
        }

        url_endpoint_ = std::unique_ptr<SGURLEndpoint>(new SGURLEndpoint(url));

        if(!url_endpoint_->init()){
            cout << "Invalid URL endpoint.";
            return SGBucketReturnStatus::kError;
        }

        replicator_configuration_ = std::unique_ptr<SGReplicatorConfiguration>(new SGReplicatorConfiguration(db_.get(), url_endpoint_.get()));

        if(!username.empty() && !password.empty()) {
            basic_authenticator_ = std::unique_ptr<SGBasicAuthenticator>(new SGBasicAuthenticator(username,password));
            replicator_configuration_->setAuthenticator(basic_authenticator_.get());
        }

        if(rep_type == "pull") {
            replicator_configuration_->setReplicatorType(SGReplicatorConfiguration::ReplicatorType::kPull);
        } else if(rep_type == "push") {
            replicator_configuration_->setReplicatorType(SGReplicatorConfiguration::ReplicatorType::kPush);
        } else if(rep_type == "pushpull") {
            replicator_configuration_->setReplicatorType(SGReplicatorConfiguration::ReplicatorType::kPushAndPull);
        } else {
            cout << "Unidentified replicator type selected. Use \"push\", \"pull\", or \"pushpull\".";
            return SGBucketReturnStatus::kError;
        }

        replicator_configuration_->setChannels(channels);
        replicator_ = std::unique_ptr<SGReplicator>(new SGReplicator(replicator_configuration_.get()));

        if(stat_changed) replicator_->addChangeListener(stat_changed);

        if(document_ended) replicator_->addDocumentEndedListener(document_ended);

        if(valid_listener) {
            replicator_->addValidationListener(valid_listener);
        } else {
            replicator_->addValidationListener(bind(&SGBucket::onValidate_, this, placeholders::_1, placeholders::_2));
        }

        if(replicator_->start() != SGReplicatorReturnStatus::kNoError) {
            cout << "\nProblem starting the replicator.";
            return SGBucketReturnStatus::kError;
        }

        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::stopReplicator() {
        replicator_->stop();
        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::setChannels(std::vector<std::string> channels) {
        replicator_configuration_->setChannels(channels);
        return SGBucketReturnStatus::kNoError;
    }

    SGBucketReturnStatus SGBucket::restartReplicator() {
        if(replicator_->restart() != SGReplicatorReturnStatus::kNoError) {
            return SGBucketReturnStatus::kNoError;
        } else {
            return SGBucketReturnStatus::kError;
        }
    }
}