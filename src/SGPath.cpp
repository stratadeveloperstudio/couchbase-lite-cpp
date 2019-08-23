//
//  SGPath.cpp
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

#include "SGPath.h"
#include <litecore/FilePath.hh>
#include <litecore/Error.hh>
#define DEBUG(...) printf("SGPath: "); printf(__VA_ARGS__)
using namespace litecore;
namespace Strata {
    SGPath::SGPath(const std::string& path){
        constructPath(path);
    }
    void SGPath::constructPath(const std::string& p){
        if(p.empty()){
            path_ = kSGCurrentDir;
        }else if(p.back() != kSGSeparatorChar_){
            path_ = p;
            path_.push_back(kSGSeparatorChar_);
        }else{
            path_ = p;
        }
    }
    bool SGPath::isValidDir(){
        FilePath filePath(path_);
        if(path_ == kSGCurrentDir){
            return true;
        }
        if(!filePath.existsAsDir()){
            return false;
        }
        return true;
    }
    bool SGPath::mkdir(){
        FilePath filePath(path_);
        if(!filePath.exists()){
            try{
                if(!filePath.mkdir()){
                    DEBUG("Error creating %s directory\n", path_.c_str());
                    return false;
                }
            }catch(const litecore::error& err){
                DEBUG("Something went wrong! %s ... Code:%d\n", err.what(), err.code);
                return false;
            }
        }
        return true;
    }
    void SGPath::addChildDir(const std::string& dirname){
        if(dirname.empty()){
            return;
        }
        if(path_.back() != kSGSeparatorChar_){
            path_.push_back(kSGSeparatorChar_);
        }
        path_.append(dirname);
    }
    const std::string &SGPath::getPath() const{
        return path_;
    }
}
