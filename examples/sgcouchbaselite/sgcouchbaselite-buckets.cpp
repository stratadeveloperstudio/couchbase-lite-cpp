//
//  sgcouchbaselite.cpp
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
#include <thread>

#include "SGFleece.h"
#include "SGCouchBaseLite.h"

using namespace fleece;
using namespace fleece::impl;
using namespace Strata;

#define DEBUG(...) printf("TEST SGLiteCore: "); printf(__VA_ARGS__)

const char* activity_level_string[] = {"Stopped","Offline","Connecting","Idle", "Busy" };
void onStatusChanged(SGReplicator::ActivityLevel level, SGReplicatorProgress progress){
    float progress_percentage = 0.0;
    if(progress.total > 0){
        progress_percentage = (progress.completed / progress.total) *100;
    }
    DEBUG("Replicator Activity Level: %s %f (%d/%d) \n", activity_level_string[(unsigned int)level], progress_percentage, progress.completed, progress.total);
}
void onDocumentEnded(bool pushing, std::string doc_id, std::string error_message, bool is_error,bool transient){
    DEBUG("onDocumentError: pushing: %d, Doc Id: %s, is error: %d, error message: %s, transient:%d\n", pushing, doc_id.c_str(), is_error, error_message.c_str(), transient);
}

class MiniHCS{
public:
    MiniHCS(SGBucket *b){
        b_ = b;
    };
    virtual~MiniHCS(){};
    void onValidate(const std::string& doc_id, const std::string& json_body) {
        printf("MiniHCS: New incoming revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), json_body.c_str() );
        SGDocument document(b_, doc_id);
        printf("MiniHCS: existing revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), document.getBody().c_str() );
    }

private:
    SGBucket *b_;
};

int main() {

    // Create a bucket manager
    SGBucketManager bucket_mgr_1;

    // Inside the bucket_manager, create a bucket called "Bucket One"
    SGBucket *bucket_1 = bucket_mgr_1.createBucket("Bucket One");

    std::cout << "\nCreated bucket \"Bucket One\"." << std::endl;

    // Verify that the created bucket exists
    std::cout << "\nDoes bucket \"Bucket One\" exist? " << (bucket_mgr_1.bucketExists("Bucket One") ? "Yes" : "No") << std::endl; // Exists
    std::cout << "\nDoes bucket \"Bucket Two\" exist? " << (bucket_mgr_1.bucketExists("Bucket Two") ? "Yes" : "No") << std::endl; // Does not exist

    // Get std::vector<std::string>> with all bucket names
    std::vector<std::string> all_buckets = bucket_mgr_1.getBuckets();

    // Print all existing bucket names
    for(std::string str : all_buckets) std::cout << "\nBucket name: " << str << std::endl;

    // Delete "Bucket One"
    bucket_mgr_1.deleteBucket("Bucket One"); // Exists

    // Delete "Bucket Two"
    bucket_mgr_1.deleteBucket("Bucket Two"); // Does not exist

    // Verify that the deleted buckets do not exist
    std::cout << "\nDoes bucket \"Bucket One\" exist? " << (bucket_mgr_1.bucketExists("Bucket One") ? "Yes" : "No") << std::endl; // Does not exist (was deleted)
    std::cout << "\nDoes bucket \"Bucket Two\" exist? " << (bucket_mgr_1.bucketExists("Bucket Two") ? "Yes" : "No") << std::endl; // Does not exist

    // Inside the bucket_manager, create a bucket called "Documents"
    SGBucket *docs_bucket = bucket_mgr_1.createBucket("Documents");

    // Verify that the created bucket exists
    std::cout << "\nDoes bucket \"Documents\" exist? " << (bucket_mgr_1.bucketExists("Documents") ? "Yes" : "No") << std::endl; // Exists

    // Add document "doc-1" to "Documents" bucket
    std::string id = "doc-1";
    std::string body = "{\"age\":100,\"myobj\":{\"myarray\":[1,2,3,4],\"mykey\":\"myvalue\"}}";
    if(docs_bucket->createDocument(std::make_pair(id, body)) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully created document." << std::endl;
    }

    // Add document "doc-2" to "Documents" bucket
    if(docs_bucket->createDocument(std::make_pair("doc-2", "{\"name\":\"Nimbus\"}")) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully created document." << std::endl;
    }

    // Update document "doc-2"
    if(docs_bucket->updateDocument("doc-2", "{\"name\":\"Nimbus\", \"job\":\"Engineer\"}") == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully updated document." << std::endl;
    }

    // Search bucket for documents with ID containing "doc"
    std::vector<std::string> doc_ids;
    if(docs_bucket->searchByDocumentID("doc", doc_ids) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nThe documents with keys containing \"doc\" are:";
        for(std::string str : doc_ids) std::cout << "\nDocument ID: " << str << std::endl;
    }

    // Read document "doc-2"
    std::string doc_body;
    if(docs_bucket->readDocument("doc-2", doc_body) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully read document. Body: " << doc_body << "." << std::endl;
    }

    // Delete document "doc-2"
    if(docs_bucket->deleteDocument("doc-2") == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully deleted document." << std::endl;
    }

    // Get all document IDs in this bucket
    doc_ids.clear();
    if(docs_bucket->getDocumentKeys(doc_ids) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully got all document keys: ";
        for(std::string str : doc_ids) std::cout << "\nDocument ID: " << str << std::endl;
    }

    // Get all contents in this bucket
    std::unordered_map<std::string, std::string> doc_contents;
    if(docs_bucket->readContents(doc_contents) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully got all contents: ";
        for(auto pair : doc_contents) std::cout << "\nDocument ID: " << pair.first << ", body: " << pair.second << std::endl;
    }

    // Below Query API

    SGBucket *query = bucket_mgr_1.createBucket("Query Example 1");

    std::string DocWithChannelOne = R"foo(
    {
        "name": "Nimbus",
        "channels": "chan1"
    }
    )foo";

    std::string DocWithChannelsOneAndTwo = R"foo(
    {
        "name": "Hall",
        "channels": ["chan1", "chan2"]
    }
    )foo";
   
    query->createDocument(std::make_pair("Document With Channel One", DocWithChannelOne));
    query->createDocument(std::make_pair("Document With Channels One And Two", DocWithChannelsOneAndTwo));

    // Which documents contain "chan1" in the "channels" field?
    std::string query_data = "{\"field\":\"channels\",\"pattern\":\"chan1\"}";
    std::vector<std::string> vec = {};

    if(query->searchByDocumentField(query_data, vec) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nThe documents with keys containing " + query_data + " are: ";
        for(std::string str : vec) std::cout << "\"" << str << "\" "; std::cout << "\n" << std::endl;
        // Displays "Document With Channel One" and "Document With Channels One And Two"
    }

    // Which documents contain "chan2" in the "channels" field?
    query_data = "{\"field\":\"channels\",\"pattern\":\"chan2\"}";
    vec.clear();

    if(query->searchByDocumentField(query_data, vec) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nThe documents with keys containing " + query_data + " are: ";
        for(std::string str : vec) std::cout << "\"" << str << "\" "; std::cout << "\n" << std::endl;
        // Displays "Document With Channels One And Two"
    }

    // Below Replicator API

    // Basic call
    SGBucketManager rep_bucket_mgr;
    SGBucket *rep_bucket_1 = rep_bucket_mgr.createBucket("Replicator Demo 1");
    if(rep_bucket_1->startReplicator("ws://localhost:4984/db") == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully started replicator. (1)" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Call with URL, replicator type, username, password, channel vector
    SGBucket *rep_bucket_2 = rep_bucket_mgr.createBucket("Replicator Demo 2");
    std::string username = "";
    std::string password = "";
    std::vector<std::string> channels = {"chan1"};
    if(rep_bucket_2->startReplicator("ws://localhost:4984/db", SGReplicatorConfiguration::ReplicatorType::kPull, username, password, channels) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully started replicator. (2)" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // Change channels and restart replicator for rep_bucket_2
    channels = {"chan2"};
    rep_bucket_2->setChannels(channels);
    rep_bucket_2->restartReplicator();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Call with all three listeners
    SGBucket *rep_bucket_3 = rep_bucket_mgr.createBucket("Replicator Demo 3");
    MiniHCS miniHCS(rep_bucket_3);
    if(rep_bucket_3->startReplicator("ws://localhost:4984/db", SGReplicatorConfiguration::ReplicatorType::kPushAndPull, "", "", std::vector<std::string> (), onStatusChanged, onDocumentEnded, bind(&MiniHCS::onValidate, &miniHCS, std::placeholders::_1, std::placeholders::_2)) == SGBucketReturnStatus::kNoError) {
        std::cout << "\nSuccessfully started replicator. (3)" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    rep_bucket_1->stopReplicator();
    rep_bucket_2->stopReplicator();
    rep_bucket_3->stopReplicator();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "\n\n\n\n" << std::endl;
    return 0;
}
