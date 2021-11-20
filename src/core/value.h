/**
 * Copyright 2021 Torsten Mehnert
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <complate/core/value.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

static const char VALUE_DOC_CLASS[] = R"DELIM(
  Javascript compatible Value.

  The variant of all types which can be made available to the JavaScript
  Engine. This class has many constructors that are implicit,
  just assign your type. You normally don't have to deal with this C++
  native type because the module will convert back and forth from Python from/to
  C++.
)DELIM";

void registerValue(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace std;
  using namespace complate;

  py::class_<Value>(m, "Value")
      .def(py::init([](const py::object &obj) {
        if (py::isinstance<py::none>(obj)) {
          return Value(nullptr);
        }
        if (py::isinstance<py::str>(obj)) {
          return Value(py::cast<string>(obj));
        }
        if (py::isinstance<py::bool_>(obj)) {
          return Value(py::cast<bool>(obj));
        }
        if (py::isinstance<py::int_>(obj)) {
          return Value(py::cast<int64_t>(obj));
        }
        if (py::isinstance<py::float_>(obj)) {
          return Value(py::cast<double>(obj));
        }
        if (py::isinstance<py::list>(obj)) {
          return Value(py::cast<Array>(obj));
        }
        if (py::isinstance<py::dict>(obj)) {
          return Value(py::cast<Object>(obj));
        }
        if (py::isinstance<py::function>(obj)) {
          return Value(Function([obj](const Array &args) -> Value {
            auto tup = Mapper::args_to_tuple(args);
            return obj(*tup).cast<Value>();
          }));
        }
        auto name = obj.attr("__class__").attr("__name__").cast<string>();
        auto pptr = make_shared<py::object>(obj);
        return Value(Proxy(name, pptr));
      }), "Construct a javascript compatible value, called implicitly.")
      .def("get_function",
           [](const Value &v) { return v.optional<Function>(); })
      .doc() = VALUE_DOC_CLASS;

  /* Add implicit conversions, so it feels more like a python native module. */
  py::implicitly_convertible<py::none, Value>();
  py::implicitly_convertible<py::bool_, Value>();
  py::implicitly_convertible<py::int_, Value>();
  py::implicitly_convertible<py::float_, Value>();
  py::implicitly_convertible<py::str, Value>();
  py::implicitly_convertible<py::dict, Value>();
  py::implicitly_convertible<py::list, Value>();
  py::implicitly_convertible<py::function, Value>();
  py::implicitly_convertible<py::object, Value>();
}
