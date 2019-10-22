//
//  SGReplicator.cpp
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

#include <litecore/CivetWebSocket.hh>

#include "SGReplicator.h"
#include "SGUtility.h"

using namespace std;
using namespace fleece;
using namespace fleece::impl;
#define DEBUG(...) printf("SGReplicator: "); printf(__VA_ARGS__)

namespace Strata {
    SGReplicator::SGReplicator() {
        replicator_parameters_.callbackContext = this;
        replicator_parameters_.push = kC4Disabled;
        replicator_parameters_.pull = kC4Disabled;
        replicator_parameters_.optionsDictFleece = slice();
        replicator_parameters_.pushFilter = nullptr;
        replicator_parameters_.validationFunc = nullptr;
        replicator_parameters_.onStatusChanged = nullptr;
        replicator_parameters_.onDocumentEnded = nullptr;
        replicator_parameters_.onBlobProgress = nullptr;
        replicator_parameters_.socketFactory = nullptr;
        // To support multiple/separate replications to multiple sync-gateway/databases at the same time we need to provide our websocket implementation.
        RegisterC4CivetWebSocketFactory();
    }

    SGReplicator::~SGReplicator() {
        stop();
    }

    SGReplicator::SGReplicator(SGReplicatorConfiguration *replicator_configuration): SGReplicator() {
        replicator_configuration_ = replicator_configuration;
        setReplicatorType(replicator_configuration_->getReplicatorType());
    }

    void SGReplicator::stop() {
        lock_guard<mutex> lock(replicator_lock_);
        if(c4replicator_ != nullptr && internal_status_ != SGReplicatorInternalStatus::kStopping && internal_status_ != Strata::SGReplicatorInternalStatus::kStopped){
            internal_status_ = Strata::SGReplicatorInternalStatus::kStopping;
            
            c4repl_stop(c4replicator_);
            c4repl_free(c4replicator_);

            internal_status_ = Strata::SGReplicatorInternalStatus::kStopped;
            c4replicator_ = nullptr;
        }
    }

    SGReplicatorReturnStatus SGReplicator::start() {
        lock_guard<mutex> lock(replicator_lock_);

        if(internal_status_ == Strata::SGReplicatorInternalStatus::kStopping) {
            return SGReplicatorReturnStatus::kAboutToStop;
        }

        if(c4replicator_ != nullptr){
            return SGReplicatorReturnStatus::kStillRunning;
        }

        if (!isValidSGReplicatorConfiguration()) {
            return SGReplicatorReturnStatus::kConfigurationError;
        }

        internal_status_ = Strata::SGReplicatorInternalStatus::kStarting;

        Encoder encoder;
        encoder.writeValue(replicator_configuration_->effectiveOptions());
        alloc_slice replicator_options = encoder.finish();
        replicator_parameters_.optionsDictFleece = replicator_options;

        // Callback function for outgoing revision event
        // This is used for log purposes!
        replicator_parameters_.pushFilter = [](C4String docID, C4RevisionFlags ref, FLDict body, void *context) {
            DEBUG("pushFilter\n");
            alloc_slice fleece_body = FLValue_ToJSON((FLValue) body);
            DEBUG("Doc ID: %s, pushing body json:%s\n", slice(docID).asString().c_str(),
                  fleece_body.asString().c_str());
            return true;
        };

        if(on_status_changed_callback_ == nullptr){
            addChangeListener([](SGReplicator::ActivityLevel, SGReplicatorProgress progress){
                // placeholder to make sure replicator_parameters_.onStatusChanged has a callback.
                // The onStatusChanged needs to run regardless if addChangeListener listener used by the application or not.
            });
        }

        c4replicator_ = c4repl_new(replicator_configuration_->getDatabase()->getC4db(),
                                   replicator_configuration_->getUrlEndpoint()->getC4Address(),
                                   slice(replicator_configuration_->getUrlEndpoint()->getPath()),
                                   nullptr,
                                   replicator_parameters_,
                                   &c4error_
        );

        if(c4replicator_ == nullptr){
            logC4Error(c4error_);
            DEBUG("Replication failed.\n");
            return SGReplicatorReturnStatus::kInternalError;
        }

        internal_status_ = Strata::SGReplicatorInternalStatus::kStarted;
        return SGReplicatorReturnStatus::kNoError;
    }

    void SGReplicator::setReplicatorType(SGReplicatorConfiguration::ReplicatorType replicator_type) {

        switch (replicator_type) {
            case SGReplicatorConfiguration::ReplicatorType::kPushAndPull:
                replicator_parameters_.push = kC4Continuous;
                replicator_parameters_.pull = kC4Continuous;
                break;
            case SGReplicatorConfiguration::ReplicatorType::kPush:
                replicator_parameters_.push = kC4Continuous;
                replicator_parameters_.pull = kC4Disabled;
                break;
            case SGReplicatorConfiguration::ReplicatorType::kPull:
                replicator_parameters_.push = kC4Disabled;
                replicator_parameters_.pull = kC4Continuous;
                break;
            default:
                DEBUG("No replicator type has been provided.");
                break;
        }
    }

    bool SGReplicator::isValidSGReplicatorConfiguration() {
        return  replicator_configuration_ != nullptr && replicator_configuration_->isValid();
    }

    void SGReplicator::addChangeListener(
            const std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress progress)> &callback) {
        on_status_changed_callback_ = callback;
        replicator_parameters_.onStatusChanged = [](C4Replicator *replicator, C4ReplicatorStatus replicator_status,
                                                    void *context) {
            DEBUG("onStatusChanged: %d\n", replicator_status.level);
            SGReplicatorProgress progress;
            progress.total = replicator_status.progress.unitsTotal;
            progress.completed = replicator_status.progress.unitsCompleted;
            progress.document_count = replicator_status.progress.documentCount;
            ((SGReplicator *) context)->on_status_changed_callback_((SGReplicator::ActivityLevel) replicator_status.level, progress);

            SGReplicator *ref = ((SGReplicator *) context);
            if(ref && replicator) {
                if(replicator_status.level == kC4Stopped) {
                    {
                        lock_guard<mutex> lock(ref->replicator_lock_);
                        c4repl_free(ref->c4replicator_);
                        ref->c4replicator_ = nullptr;
                        ref->internal_status_ = Strata::SGReplicatorInternalStatus::kStopped;
                    }

                    // Error code == 0 means no errors were found
                    // In that case, do not restart since stopping was intentional
                    if(replicator_status.error.code != 0) {
                        ref->start();
                    }
                }
                else {
                    ref->internal_status_ = Strata::SGReplicatorInternalStatus::kStarted;
                }
            }
        };
    }

    SGReplicatorReturnStatus SGReplicator::restart() {
        if(internal_status_ != Strata::SGReplicatorInternalStatus::kStopped) {
            this->stop();
        }

        return this->start();
    }

    void SGReplicator::addDocumentEndedListener(
            const std::function<void(bool pushing, std::string doc_id, std::string error_message, bool is_error,
                                     bool error_is_transient)> &callback) {
        on_document_error_callback_ = callback;
        replicator_parameters_.onDocumentEnded = [](C4Replicator *C4NONNULL,
                                                    bool pushing,
                                                    C4HeapString docID,
                                                    C4HeapString revID,
                                                    C4RevisionFlags flags,
                                                    C4Error error,
                                                    bool errorIsTransient,
                                                    void *context) {

            alloc_slice error_message = c4error_getDescription(error);
            ((SGReplicator *) context)->on_document_error_callback_(pushing, slice(docID).asString(),
                                                                    error_message.asString(), error.code > 0,
                                                                    errorIsTransient);
        };
    }

    void SGReplicator::addValidationListener(
            const std::function<void(const std::string &doc_id, const std::string &json_body)> &callback) {
        on_validation_callback_ = callback;
        DEBUG("addValidationListener\n");
        replicator_parameters_.validationFunc = [](C4String docID, C4RevisionFlags ref, FLDict body, void *context) {
            DEBUG("validationFunc\n");

            alloc_slice fleece_json_string = FLValue_ToJSON((FLValue) body);
            ((SGReplicator *) context)->on_validation_callback_(slice(docID).asString(), fleece_json_string.asString());

            // Accept All documents
            return true;
        };
    }

    std::ostream& operator << (std::ostream& os, const SGReplicatorReturnStatus& return_status){
        return os << static_cast<underlying_type<SGReplicatorReturnStatus>::type> (return_status);
    }

    std::ostream& operator << (std::ostream& os, const SGReplicator::ActivityLevel& activity_level){
        return os << static_cast<underlying_type<SGReplicator::ActivityLevel>::type> (activity_level);
    }
}