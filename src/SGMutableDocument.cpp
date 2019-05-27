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
#define DEBUG(...) printf("SGMutableDocument: "); printf(__VA_ARGS__)
namespace Spyglass {
    SGMutableDocument::SGMutableDocument(class SGDatabase *database, const std::string &docId) : SGDocument(database, docId) {}

    bool SGMutableDocument::setBody(const std::string &body) {
        try {
            fleece::Retained<fleece::impl::Doc> doc = fleece::impl::Doc::fromJSON(body);
            mutable_dict_ = fleece::impl::MutableDict::newDict(doc->asDict());
            DEBUG("Set body: %s\n", body.c_str());
            return true;

        } catch (fleece::FleeceException) {
            DEBUG("Can't set body of invalid json string: %s\n", body.c_str());
            return false;
        };
    }
}