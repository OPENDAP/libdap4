// ServerFunctionsList.h

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: Nathan Potter <npotter@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef I_ServerFunctionsList_h
#define I_ServerFunctionsList_h 1

#include <map>
#include <string>

#include <expr.h>
#include <D4Function.h>

#include <ServerFunction.h>

namespace libdap {

class ServerFunctionsListUnitTest;
class ConstraintEvaluator;

class ServerFunctionsList {
private:
    static ServerFunctionsList * d_instance;
    std::multimap<std::string, ServerFunction *> d_func_list;

    static void initialize_instance();
    static void delete_instance();

    virtual ~ServerFunctionsList();

    friend class ServerFunctionsListUnitTest;

protected:
    ServerFunctionsList() {}

public:
    // Added typedefs to reduce clutter jhrg 3/12/14
    typedef std::multimap<std::string, ServerFunction *>::iterator SFLIter;
    typedef std::multimap<std::string, ServerFunction *>::const_iterator SFLCIter;

    static ServerFunctionsList * TheList();

    virtual void add_function(ServerFunction *func);

    virtual bool find_function(const std::string &name, bool_func *f) const;
    virtual bool find_function(const std::string &name, btp_func  *f) const;
    virtual bool find_function(const std::string &name, proj_func *f) const;
    virtual bool find_function(const std::string &name, D4Function *f) const;

    SFLIter begin();
    SFLIter end();
    ServerFunction *getFunction(SFLIter it);

    virtual void getFunctionNames(std::vector<std::string> *names);
};

}

#endif // I_ServerFunctionsList_h
