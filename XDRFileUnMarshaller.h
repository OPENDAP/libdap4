// XDRFileUnMarshaller.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#ifndef I_XDRFileUnMarshaller_h
#define I_XDRFileUnMarshaller_h 1

#include "UnMarshaller.h"
#include "XDRUtils.h"

namespace libdap {

/** @brief unmarshaller that knows how to unmarshall/deserialize dap objects
 * using XDR from a file
 */
class XDRFileUnMarshaller : public UnMarshaller {
private:
    XDR *_source;

    XDRFileUnMarshaller();
    XDRFileUnMarshaller(const XDRFileUnMarshaller &um);
    XDRFileUnMarshaller &operator=(const XDRFileUnMarshaller &);

public:
    /**
     * @brief Builds an XDR unmarshaller that reads from a `FILE*`.
     * @param out Source file stream.
     */
    XDRFileUnMarshaller(FILE *out);
    ~XDRFileUnMarshaller() override;

    /** @copydoc UnMarshaller::get_byte */
    void get_byte(dods_byte &val) override;

    /** @copydoc UnMarshaller::get_int16 */
    void get_int16(dods_int16 &val) override;
    /** @copydoc UnMarshaller::get_int32 */
    void get_int32(dods_int32 &val) override;

    /** @copydoc UnMarshaller::get_float32 */
    void get_float32(dods_float32 &val) override;
    /** @copydoc UnMarshaller::get_float64 */
    void get_float64(dods_float64 &val) override;

    /** @copydoc UnMarshaller::get_uint16 */
    void get_uint16(dods_uint16 &val) override;
    /** @copydoc UnMarshaller::get_uint32 */
    void get_uint32(dods_uint32 &val) override;

    /** @copydoc UnMarshaller::get_str */
    void get_str(string &val) override;
    /** @copydoc UnMarshaller::get_url */
    void get_url(string &val) override;

    /** @copydoc UnMarshaller::get_opaque */
    void get_opaque(char *val, unsigned int len) override;
    /** @copydoc UnMarshaller::get_int */
    void get_int(int &val) override;

    /** @copydoc UnMarshaller::get_vector(char **, unsigned int &, Vector &) */
    void get_vector(char **val, unsigned int &num, Vector &vec) override;
    /** @copydoc UnMarshaller::get_vector(char **, unsigned int &, int, Vector &) */
    void get_vector(char **val, unsigned int &num, int width, Vector &vec) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // I_XDRFileUnMarshaller_h
