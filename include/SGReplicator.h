//
//  SGReplicator.h
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

#ifndef SGREPLICATOR_H
#define SGREPLICATOR_H

#include <litecore/c4.h>

#include "SGDatabase.h"
#include "SGReplicatorConfiguration.h"
namespace Strata {
    typedef struct {
        uint64_t completed;// The number of completed changes processed.
        uint64_t total;// The total number of changes to be processed.
        uint64_t document_count;// Number of documents transferred so far.
    } SGReplicatorProgress;

    enum class SGReplicatorReturnStatus {
        kNoError,
        kStillRunning,
        kConfigurationError,
        kInternalError,
        kAboutToStop // Asked to stop the replicator while it's running.
    };

    enum class SGReplicatorInternalStatus {
        kStarting,
        kStarted,
        kStopping,
        kStopped
    };

    std::ostream& operator << (std::ostream& os, const SGReplicatorReturnStatus& return_status);

    /*
     * Warning: This object can be initialized only once in the program life cycle. See constructor for more information.
     *
     * Thread safe is guaranteed on these functions:
     * start(), stop(),
     */
    class SGReplicator {
    public:
        SGReplicator();

        /** SGReplicator.
        * @brief Initial setup the replicator. Warning: This object can be initialized only once in the program life cycle. See constructor for more information.
        * @param replicator_configuration The SGReplicator configuration object.
        */
        SGReplicator(SGReplicatorConfiguration *replicator_configuration);

        virtual ~SGReplicator();

        enum class ActivityLevel {
            kStopped = 0,
            kOffline,
            kConnecting,
            kIdle,
            kBusy
        };

        friend std::ostream& operator << (std::ostream& os, const ActivityLevel& activity_level);

        /** SGReplicator start.
        * @brief Starts a C4replicator. Thread Safe.
        */
        SGReplicatorReturnStatus start();

        /** SGReplicator stop.
        * @brief Stop a running replicator thread. Thread Safe.
        */
        void stop();

        /** SGReplicator restart.
        * @brief Attempts to restart the replicator.
        */
        void restart();

        /** SGReplicator addChangeListener.
        * @brief Adds the callback function to the replicator's onStatusChanged event.
        * @param callback The callback function.
        */
        void addChangeListener(const std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress)> &callback);

        /** SGReplicator addDocumentErrorListener.
        * @brief Adds the callback function to the replicator's onDocumentEnded event. (This can notifiy for error and also for added Doc to local DB)
        * @param callback The callback function.
        */
        void addDocumentEndedListener(
                const std::function<void(bool pushing, std::string doc_id, std::string error_message, bool is_error,
                                         bool error_is_transient)> &callback);

        /** SGReplicator addValidationListener.
        * @brief Adds the callback function to the replicator's validationFunc event. All incoming revisions from SyncGateway will be accepted!
        * @param callback The callback function.
        */
        void addValidationListener(
                const std::function<void(const std::string &doc_id, const std::string &json_body)> &callback);

        /** SGReplicator getReplicatorConfig.
        * @brief Returns the current replicator configuration
        */
        SGReplicatorConfiguration* getReplicatorConfig();
      
    private:
        C4Replicator *c4replicator_{nullptr};
        SGReplicatorConfiguration *replicator_configuration_{nullptr};
        C4ReplicatorParameters replicator_parameters_;
        C4Error c4error_ {};
        std::mutex replicator_lock_;

        bool replicator_can_restart_ = true;

        std::function<void(SGReplicator::ActivityLevel, SGReplicatorProgress progress)> on_status_changed_callback_;
        std::function<void(bool pushing, std::string doc_id, std::string error_message, bool is_error,
                           bool error_is_transient)> on_document_error_callback_;
        std::function<void(const std::string &doc_id, const std::string &json_body)> on_validation_callback_;

        /** SGReplicator setReplicatorType.
        * @brief Set the replicator type to the C4ReplicatorParameters.
        * @param replicator_type The enum replicator type to be used for the replicator.
        */
        void setReplicatorType(SGReplicatorConfiguration::ReplicatorType replicator_type);

        bool isValidSGReplicatorConfiguration();

        /** SGReplicator restart.
        * @brief Attempts to restart the replicator.
        */
        SGReplicatorReturnStatus automatedRestart(const int &delay_seconds);

        // c4repl_stop is async and we need to track it so we don't endup with running another replicator.
        // When Activity status changed to stopped then we can free the replicator.
        SGReplicatorInternalStatus internal_status_ = SGReplicatorInternalStatus::kStopped;



        bool manual_restart_ = false;
    };
}

#endif //SGREPLICATOR_H
