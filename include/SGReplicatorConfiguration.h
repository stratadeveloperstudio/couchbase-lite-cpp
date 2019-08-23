//
//  SGReplicatorConfiguration.h
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

#ifndef SGREPLICATORCONFIGURATION_H
#define SGREPLICATORCONFIGURATION_H

#include <iostream>
#include "SGDatabase.h"
#include "SGURLEndpoint.h"
#include "SGAuthenticator.h"
namespace Strata {
    class SGReplicatorConfiguration {
    public:
        SGReplicatorConfiguration();

        /** SGReplicatorConfiguration.
        * @brief Sets private members.
        * @param db The reference to the SGDatabase
        * @param url_endpoint The reference to the SGURLEndpoint object.
        */
        SGReplicatorConfiguration(SGDatabase *db, SGURLEndpoint *url_endpoint);

        virtual ~SGReplicatorConfiguration();

        enum class ReplicatorType {
            kPushAndPull = 0,
            kPush,
            kPull
        };

        friend std::ostream& operator << (std::ostream& os, const ReplicatorType& rep_type);

        SGDatabase *getDatabase() const;

        void setDatabase(SGDatabase *database);

        const SGURLEndpoint *getUrlEndpoint() const;

        void setUrlEndpoint_(SGURLEndpoint *url_endpoint);

        ReplicatorType getReplicatorType() const;

        void setReplicatorType(ReplicatorType replicator_type);

        void setAuthenticator(SGAuthenticator *authenticator);

        const SGAuthenticator *getAuthenticator() const;

        void setChannels(const std::vector<std::string> &channels);

        /** SGReplicatorConfiguration effectiveOptions.
        * @brief Initialize and build the options for the replicator
        */
        fleece::Retained<fleece::impl::MutableDict> effectiveOptions();

        /** SGReplicatorConfiguration isValid.
        * @brief Validate database_ and url_endpoint_ references.
        */
        bool isValid() const;

    private:
        SGDatabase *database_{nullptr};
        SGAuthenticator *authenticator_{nullptr};

        SGURLEndpoint *url_endpoint_{nullptr};

        ReplicatorType replicator_type_;

        std::vector<std::string> channels_;

        //Holds all extra configuration for the replicator
        fleece::Retained<fleece::impl::MutableDict> options_;

        // Options for the replicator progress level
        const int kNotifyOnEveryDocumentChange = 1;
        const int kNotifyOnEveryAttachmentChange = 2;
    };
}

#endif //SGREPLICATORCONFIGURATION_H
