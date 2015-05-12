// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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


#ifndef MEDIA_TYPES_H_
#define MEDIA_TYPES_H_

#include <string>

static std::string DAS1 = "dods_das";
static std::string DAS2 = "dods-das";

static std::string DDS1 = "dods_dds";
static std::string DDS2 = "dods-dds";

static std::string DAP2_DATA1 = "dods_data";
static std::string DAP2_DATA2 = "dods-data";

static std::string DDX1 = "dods_ddx";
static std::string DDX2 = "dods-ddx";

static std::string DAP2_ERR1 = "dods_error";
static std::string DAP2_ERR2 = "dods-error";

static std::string WEB_ERR1 = "web_error";
static std::string WEB_ERR2 = "web-error";

static std::string DAP4_DMR = "dap4-dmr";
static std::string DAP4_DATA = "dap4-data";
static std::string DAP4_ERR ="dap4-error";

static const std::string DMR_Content_Type = "application/vnd.opendap.dap4.dataset-metadata";
static const std::string DAP4_DATA_Content_Type = "application/vnd.opendap.dap4.data";
// application/vnd.opendap.dap4.data
#endif /* MEDIA_TYPES_H_ */
