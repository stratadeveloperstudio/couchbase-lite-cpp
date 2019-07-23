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
using namespace std;
using namespace fleece;
using namespace fleece::impl;
using namespace std::placeholders;
using namespace Spyglass;

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

// Mini object to show how to bind functions from another object
class MiniHCS{
public:
    MiniHCS(SGDatabase *db){
        db_ = db;
    };
    virtual~MiniHCS(){};
    void onValidate(const std::string& doc_id, const std::string& json_body) {
        printf("MiniHCS: New incoming revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), json_body.c_str() );
        SGDocument document(db_, doc_id);
        printf("MiniHCS: existing revision: Doc Id: %s, Doc body: %s\n", doc_id.c_str(), document.getBody().c_str() );
    }

private:
    SGDatabase *db_;
};

int main(){


    //SGDatabase sgDatabase("db2", "/Users/zbgd3f/");
    // Default db location will be current location location
    SGDatabase sgDatabase("db2");

    DEBUG("Database will be stored in: %s\n", sgDatabase.getDBPath().c_str());

    if (!sgDatabase.isOpen()) {
        DEBUG("Db is not open yet\n");
    }

    if (sgDatabase.open() != SGDatabaseReturnStatus::kNoError) {
        DEBUG("Can't open DB!\n");
        return 1;
    }

    if (sgDatabase.isOpen()) {
        DEBUG("DB is open using isOpen API\n");
    } else {
        DEBUG("DB is not open, exiting!\n");
        return 1;
    }

    vector<string> document_keys;
    if(!sgDatabase.getAllDocumentsKey(document_keys)){
        DEBUG("Failed to run getAllDocumentsKey()\n");
        return 1;
    }

    // Printing the list of documents key from the local DB.
    for(std::vector <string>::iterator iter = document_keys.begin(); iter != document_keys.end(); iter++)
    {
        DEBUG("Document Key: %s\n", (*iter).c_str());
    }

    SGMutableDocument newdoc(&sgDatabase, "custom_doc");

    // This is not a valid json.
    if( !newdoc.setBody("fdfd") ){
        DEBUG("The body is not set! fdfd is not a valid json\n");
    }

    // This is a valid json.
    std::string json_data = R"foo({"age":100,"myobj":{"myarray":[1,2,3,4],"mykey":"myvalue"},"name":"luay"})foo";
    if( newdoc.setBody(json_data) ){
        DEBUG("json_data is a valid json\n");
    }

    if(sgDatabase.save(&newdoc) != SGDatabaseReturnStatus::kNoError){
        DEBUG("Could not save a doc after using setBody()\n");
        return 1;
    }

    DEBUG("%s\n", newdoc.getBody().c_str());

    if(json_data.compare(newdoc.getBody()) != 0){
        DEBUG("Doc body does not match the original json string used to set the body\n");
        return 1;
    }
    
    SGMutableDocument usbPDDocument(&sgDatabase, "usb-pd-document");


    DEBUG("document Id: %s, body: %s\n", usbPDDocument.getId().c_str(), usbPDDocument.getBody().c_str());

    usbPDDocument.set("number", 30);
    usbPDDocument.set("name", "hello"_sl);


    string name_key = "name";

    const Value *name_value = usbPDDocument.get(name_key);
    if(name_value){

        if(name_value->type() == kString){

            string name_string = name_value->toString().asString();

            DEBUG("name:%s\n", name_string.c_str());
        }else{
            DEBUG("name_value is not a string!\n");
        }

    }else{
        DEBUG("There is no such key called: %s\n", name_key.c_str());
    }

    sgDatabase.save(&usbPDDocument);

    string whatever_key = "game";

    const Value *whatever_value_key = usbPDDocument.get(whatever_key);
    if(whatever_value_key){

        if(whatever_value_key->type() == kNumber){
            usbPDDocument.set(whatever_key, usbPDDocument.get(whatever_key)->asInt() + 1);
        }else{
            DEBUG("Warning: No such key:%s exist\n",whatever_key.c_str());
        }
    }else{
        usbPDDocument.set(whatever_key, 0);
    }

    sgDatabase.save(&usbPDDocument);

    DEBUG("Document Body after save: %s\n", usbPDDocument.getBody().c_str());


    // Bellow Replicator API
    string my_url = "ws://localhost:4984/staging";
    SGURLEndpoint url_endpoint(my_url);

    if(url_endpoint.init()){
        DEBUG("url_endpoint is valid \n");
    }else{
        DEBUG("Invalid url_endpoint\n");
        return 1;
    }

    DEBUG("host %s, \n", url_endpoint.getHost().c_str());
    DEBUG("schema %s, \n", url_endpoint.getSchema().c_str());
    DEBUG("getPath %s, \n", url_endpoint.getPath().c_str());

    SGReplicatorConfiguration replicator_configuration(&sgDatabase, &url_endpoint);

    SGBasicAuthenticator basic_authenticator("username","password");

    replicator_configuration.setAuthenticator(&basic_authenticator);

    replicator_configuration.setReplicatorType(SGReplicatorConfiguration::ReplicatorType::kPushAndPull);

    vector<string> channels = {"channel1", "random_channel_name"};
    replicator_configuration.setChannels(channels);

    SGReplicator replicator(&replicator_configuration);


    replicator.addChangeListener(onStatusChanged);
    replicator.addDocumentEndedListener(onDocumentEnded);

    MiniHCS miniHCS(&sgDatabase);
    replicator.addValidationListener( bind(&MiniHCS::onValidate, &miniHCS, _1, _2) );


    if(replicator.start() != SGReplicatorReturnStatus::kNoError){
        DEBUG("Could not start the replicator!\n");
        return 1;
    }

    DEBUG("About to stop the replicator thread\n");
    this_thread::sleep_for(chrono::milliseconds(1000));

    replicator.stop();
    channels = {"random_channel_name"};
    replicator_configuration.setChannels(channels);
    this_thread::sleep_for(chrono::milliseconds(5000));

    replicator.start();

    this_thread::sleep_for(chrono::milliseconds(5000));


    DEBUG("bye\n");


    return 0;
}
