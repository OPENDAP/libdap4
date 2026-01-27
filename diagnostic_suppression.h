//
// Created by James Gallagher on 1/26/26.
//

#ifndef LIBDAP_DIAGNOSTIC_SUPPRESSION_H
#define LIBDAP_DIAGNOSTIC_SUPPRESSION_H

// Define a macro for convenience. To keep things simple, use a macro for
// each kind of warning. There should be few uses of this. jhrg 1/26/26
//
// Based on https://nelkinda.com/blog/suppress-warnings-in-gcc-and-clang/

#ifdef __GNUC__

#define A_cast_function_type_mismatch                                                                                  \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wcast-function-type-mismatch\"")

#define A_Pop _Pragma("GCC diagnostic pop")

#else
// Define them empty for other compilers (e.g., MSVC) if needed

#define A_IgnoreUnused
#define A_Pop
<<<<<<< HEAD

=======
>>>>>>> 16b1de8c78711c08eb5ce19acd48412584448df1
#endif

#endif // LIBDAP_DIAGNOSTIC_SUPPRESSION_H
