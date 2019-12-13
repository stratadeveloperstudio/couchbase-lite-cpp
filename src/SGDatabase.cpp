//
//  SGDatabase.cpp
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

#include "SGDatabase.h"

#include <fleece/FleeceImpl.hh>
#include <fleece/MutableArray.hh>
#include <fleece/MutableDict.hh>
#include <fleece/Doc.hh>
#include <litecore/c4Document+Fleece.h>

#include "SGDocument.h"
#include "SGUtility.h"
#include "SGPath.h"

using namespace std;
using namespace fleece;
using namespace fleece::impl;

#ifdef SHOW_DATABASE_MESSAGES
 #define DEBUG(...) printf("SGDatabase: "); printf(__VA_ARGS__)
#else
 #define DEBUG(...) //
#endif 

namespace Strata {
    SGDatabase::SGDatabase() {}

    SGDatabase::SGDatabase(const std::string &db_name): SGDatabase(db_name, string())  {}

    SGDatabase::SGDatabase(const std::string &db_name, const std::string &path) {
        setDBName(db_name);
        setDBPath(path);
    }

    SGDatabase::~SGDatabase() {
        close();
    }

    void SGDatabase::setDBName(const std::string &name) {
        db_name_ = name;
    }

    const std::string &SGDatabase::getDBName() const {
        return db_name_;
    }

    void SGDatabase::setDBPath(const std::string &path) {
        db_path_ = path;
    }

    const std::string &SGDatabase::getDBPath() const {
        return db_path_;
    }

    SGDatabaseReturnStatus SGDatabase::open() {

        lock_guard<mutex> lock(db_lock_);
        DEBUG("Calling open\n");

        // Check for empty db name
        if (db_name_.empty()) {
            DEBUG("DB name can't be empty! \n");
            return SGDatabaseReturnStatus::kDBNameError;
        }

        SGPath path(db_path_);

        if( !path.isValidDir() ){
            DEBUG("Not valid path!\n");
            return SGDatabaseReturnStatus::kInvalidDBPath;
        }

        path.addChildDir(kSGDatabasesDirectory_);
        if(!path.mkdir()){
            DEBUG("Failed to create databases dir\n");
            return SGDatabaseReturnStatus::kCreateDBDirectoryError;
        }

        // Configure database attributes
        // This is the default DB configuration taken from the Java bindings
        c4db_config_.flags = kC4DB_Create | kC4DB_AutoCompact;
        c4db_config_.storageEngine = kC4SQLiteStorageEngine;
        c4db_config_.versioning = kC4RevisionTrees;
        c4db_config_.encryptionKey.algorithm = kC4EncryptionNone;

        path.addChildDir(db_name_);
        DEBUG("Full db path: %s\n", path.getPath().c_str());

        c4db_ = c4db_open(slice(path.getPath()), &c4db_config_, &c4error_);

        if(c4db_ == nullptr){
            logC4Error(c4error_);
            DEBUG("Error opening the db: %s\n", path.getPath().c_str());
            return SGDatabaseReturnStatus::kOpenDBError;
        }

        return SGDatabaseReturnStatus::kNoError;
    }

    bool SGDatabase::_isOpen() const {
        return c4db_ != nullptr;
    }

    bool SGDatabase::isOpen() {
        lock_guard<mutex> lock(db_lock_);
        return _isOpen();
    }

    SGDatabaseReturnStatus SGDatabase::close() {
        lock_guard<mutex> lock(db_lock_);
        DEBUG("Calling close\n");

        if( !_isOpen() ){
            DEBUG("Calling close on non opened DB\n");
            return SGDatabaseReturnStatus::kCloseDBError;
        }

        if(!c4db_close(c4db_, &c4error_)){
            logC4Error(c4error_);
            return SGDatabaseReturnStatus::kCloseDBError;
        }

        c4db_free(c4db_);

        // c4db_free Deallocate but won't set c4db_ to nullptr
        c4db_ = nullptr;

        DEBUG("Leaving close\n");
        return SGDatabaseReturnStatus::kNoError;
    }

    C4Database *SGDatabase::getC4db() {
        lock_guard<mutex> lock(db_lock_);
        return c4db_;
    }

    SGDatabaseReturnStatus SGDatabase::_createNewDocument(SGDocument *doc, alloc_slice body) {
        // Document does not exist. Creating a new one
        DEBUG("Creating a new document\n");

        C4RevisionFlags revisionFlags = kRevNew;

        C4Document *newdoc = c4doc_create(c4db_, slice(doc->getId()), body, revisionFlags, &c4error_);

        if(newdoc == nullptr){
            logC4Error(c4error_);
            DEBUG("Could not create new document.");
            return SGDatabaseReturnStatus::kCreateDocumentError;
        }
        doc->setC4document(newdoc);
        return SGDatabaseReturnStatus::kNoError;
    }

    SGDatabaseReturnStatus SGDatabase::_updateDocument(SGDocument *doc, alloc_slice new_body) {
        // Document exist. Make modifications to the body
        DEBUG("document Exist. Working on updating the document: %s\n", doc->getId().c_str());
        string rev_id = slice(doc->c4document_->revID).asString();
        DEBUG("REV id: %s\n", rev_id.c_str());

        C4Document *newdoc = c4doc_update(doc->c4document_, new_body, doc->c4document_->selectedRev.flags, &c4error_);

        if(newdoc == nullptr){
            logC4Error(c4error_);
            DEBUG("Could not update the body of an existing document.\n");
            return SGDatabaseReturnStatus::kUpdatDocumentError;
        }

        doc->setC4document(newdoc);

        return SGDatabaseReturnStatus::kNoError;
    }

    SGDatabaseReturnStatus SGDatabase::save(SGDocument *doc) {
        lock_guard<mutex> lock(db_lock_);
        DEBUG("Calling save\n");

        if(!_isOpen()){
            DEBUG("Calling save() while DB is not open\n");
            return SGDatabaseReturnStatus::kOpenDBError;
        }

        if(doc == nullptr){
            DEBUG("Passing uninitialized/invalid SGDocument to save()\n");
            return SGDatabaseReturnStatus::kInvalidArgumentError;
        }

        SGDatabaseReturnStatus status = SGDatabaseReturnStatus::kNoError;

        // Encode document mutable dictionary to fleece format
        alloc_slice fleece_data;
        try{
            fleece_data = JSONConverter::convertJSON(doc->mutable_dict_->toJSONString());
        }catch (const FleeceException& e){
            DEBUG("Convert body error: %s\n", e.what());
            return SGDatabaseReturnStatus::kInvalidDocBody;
        }

        if(!c4db_beginTransaction(c4db_, &c4error_)){
            logC4Error(c4error_);
            DEBUG("save kBeginTransactionError\n");
            return SGDatabaseReturnStatus::kBeginTransactionError;
        }

        C4Document *c4doc = doc->getC4document();

        if (c4doc == nullptr) {
            status = _createNewDocument(doc, fleece_data);

        } else {
            status = _updateDocument(doc, fleece_data);
        }

        if(!c4db_endTransaction(c4db_, true, &c4error_)){
            logC4Error(c4error_);
            DEBUG("save kEndTransactionError\n");
            return SGDatabaseReturnStatus::kEndTransactionError;
        }

        DEBUG("Leaving save\n");
        return status;
    }

    C4Document *SGDatabase::getDocumentById(const std::string &doc_id) {
        lock_guard<mutex> lock(db_lock_);

        if(!_isOpen() || doc_id.empty()){
            return nullptr;
        }

        C4Document *c4doc;
        DEBUG("START getDocumentById: %s\n", doc_id.c_str());

        if(!c4db_beginTransaction(c4db_, &c4error_)){
            logC4Error(c4error_);
            DEBUG("getDocumentById starting transaction failed on document %s\n", doc_id.c_str());
            return nullptr;
        }

        c4doc = c4doc_get(c4db_, slice(doc_id), true, &c4error_);

        if(!c4db_endTransaction(c4db_, true, &c4error_)){
            logC4Error(c4error_);
            DEBUG("getDocumentById ending transaction failed on document %s\n", doc_id.c_str());
            return nullptr;
        }

        DEBUG("END getDocumentById: %s\n", doc_id.c_str());
        return c4doc;
    }

    SGDatabaseReturnStatus SGDatabase::deleteDocument(SGDocument *doc) {
        lock_guard<mutex> lock(db_lock_);

        if(!_isOpen()){
            DEBUG("Calling deleteDocument() while DB is not open\n");
            return SGDatabaseReturnStatus::kOpenDBError;
        }

        if(doc == nullptr){
            DEBUG("Passing uninitialized/invalid SGDocument to deleteDocument()\n");
            return SGDatabaseReturnStatus::kInvalidArgumentError;
        }

        if(!c4db_beginTransaction(c4db_, &c4error_)){
            logC4Error(c4error_);
            DEBUG("deleteDocument kBeginTransactionError\n");
            return SGDatabaseReturnStatus::kBeginTransactionError;
        }

        DEBUG("START deleteDocument: %s\n", doc->getId().c_str());

        // Try to delete the document
        bool is_deleted = c4db_purgeDoc(c4db_, slice(doc->getId()), &c4error_);

        if(!c4db_endTransaction(c4db_, true, &c4error_)){
            logC4Error(c4error_);
            DEBUG("deleteDocument kEndTransactionError\n");
            return SGDatabaseReturnStatus::kEndTransactionError;
        }

        if (!is_deleted) {
            return SGDatabaseReturnStatus::kDeleteDocumentError;
        }

        DEBUG("Document %s deleted\n", doc->getId().c_str());

        doc->setId(string());
        doc->setC4document(nullptr);

        DEBUG("END deleteDocument: %s\n", doc->getId().c_str());
        return SGDatabaseReturnStatus::kNoError;
    }

    bool SGDatabase::getAllDocumentsKey(std::vector<std::string>& document_keys) {
        lock_guard<mutex> lock(db_lock_);

        if(!_isOpen()){
            DEBUG("Trying to run database query while DB is not open!\n");
            return false;
        }

        const static string json = "[\"SELECT\", {\"WHAT\": [[\"._id\"]]}]";
        std::unique_ptr<C4Query, decltype(&c4query_free)> query(c4query_new(c4db_, slice(json), &c4error_), &c4query_free);

        if(query != nullptr){
            C4QueryOptions options = kC4DefaultQueryOptions;
            std::unique_ptr<C4QueryEnumerator, decltype(&c4queryenum_free)> query_enumerator(c4query_run(query.get(), &options, c4str(nullptr), &c4error_), &c4queryenum_free);

            if(query_enumerator != nullptr){
                while (bool is_next_result_available = c4queryenum_next(query_enumerator.get(), &c4error_)) {

                    if(is_next_result_available){
                        slice doc_name = FLValue_AsString(FLArrayIterator_GetValueAt(&query_enumerator->columns, 0));
                        document_keys.push_back(doc_name.asString());
                    }else{
                        logC4Error(c4error_);
                        DEBUG("c4queryenum_next failed to run.\n");
                        return false;
                    }
                }
            }else{
                logC4Error(c4error_);
                DEBUG("C4QueryEnumerator failed to run.\n");
                return false;
            }
        }else{
            logC4Error(c4error_);
            DEBUG("C4Query failed to execute a query.\n");
            return false;
        }

        return true;
    }

    std::ostream& operator << (std::ostream& os, const SGDatabaseReturnStatus& return_status){
        return os << static_cast<underlying_type<SGDatabaseReturnStatus>::type> (return_status);
    }
}