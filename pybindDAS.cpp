//
// Created by Kodi Neumiller on 5/5/21.
//
#include <DAS.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(DASbind, m) {
    py::class_<DAS>(m, "DAS")
        //init() is a convenience function that takes the types of a constructor’s
        // parameters as template arguments and wraps the corresponding constructor
        .def(py::init<>())
        //if there are default arguments use py::arg()
        .def("print", static_cast<void (DAS::*)(FILE *out, py::arg(dereference) = false)(&DAS::print), "print das with FILE out")
        .def("print", static_cast<void (DAS::*)(ostream &out, bool dereference = false)(&DAS::print), "print das with ostream");
        //print statement
        .def("__repr__", [](const DAS &a) {
            fprintf(a.out, "Attributes {\n") ;

            &DAS::d_attrs.print(a.out, "    ", a.dereference);

            fprintf(a.out, "}\n") ;
        }
}