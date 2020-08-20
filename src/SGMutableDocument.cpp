/**
******************************************************************************
* @file SGMutableDocument .CPP
* @author Luay Alshawi
* $Rev: 1 $
* $Date:
* @brief Add mutability functionality to SGDocument
******************************************************************************
* @copyright Copyright 2018 On Semiconductor
*/
#include "SGMutableDocument.h"
#include "SGLoggingCategories.h"

namespace Strata {
    SGMutableDocument::SGMutableDocument(class SGDatabase *database, const std::string &docId) : SGDocument(database, docId) {}

    bool SGMutableDocument::setBody(const std::string &body) {
        try {
            alloc_slice_ = fleece::impl::JSONConverter::convertJSON(body);
            if(!alloc_slice_){
                qC4Warning(logDomainSGMutableDocument, "Tried to convert invalid json to fleece data: %s", body.c_str());
                return false;
            }
            mutable_dict_ = fleece::impl::MutableDict::newDict( fleece::impl::Value::fromData(alloc_slice_)->asDict() );
            qC4Debug(logDomainSGMutableDocument, "Set body: %s", mutable_dict_->toJSONString().c_str());
            return true;
        } catch (const fleece::FleeceException& e) {
            qC4Critical(logDomainSGMutableDocument, "Fleece error when parsing json to fleece: body:%s - what: %s", body.c_str(), e.what());
            return false;
        };
    }
}
