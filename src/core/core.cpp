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
#include <complate/core/prototypebuilder.h>
#include <complate/core/renderer.h>
#include <complate/core/stringstream.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include "mapper.h"
#include "pystream.h"

namespace py = pybind11;
using namespace std;
using namespace complate;

PYBIND11_MODULE(core, m) {
  m.doc() = "Python bindings for complate-cpp - Core library";

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
      }))
      .def("get_function",
           [](const Value &v) { return v.optional<Function>(); });

  py::class_<Function>(m, "Function")
      .def("apply", [](const Function &function, const Array &args) {
        return Mapper::value_to_python(function.apply(args));
      });

  py::implicitly_convertible<py::none, Value>();
  py::implicitly_convertible<py::bool_, Value>();
  py::implicitly_convertible<py::int_, Value>();
  py::implicitly_convertible<py::float_, Value>();
  py::implicitly_convertible<py::str, Value>();
  py::implicitly_convertible<py::dict, Value>();
  py::implicitly_convertible<py::list, Value>();
  py::implicitly_convertible<py::function, Value>();
  py::implicitly_convertible<py::object, Value>();

  py::class_<Stream, PyStream>(m, "Stream")
      .def(py::init<>())
      .def("write", &Stream::write, py::arg("string"), py::arg("length"))
      .def("writeln", &Stream::writeln, py::arg("string"), py::arg("length"))
      .def("flush", &Stream::flush);

  py::class_<StringStream, Stream>(m, "StringStream")
      .def(py::init<>())
      .def("str", &StringStream::str);

  py::class_<Renderer>(m, "Renderer")
      .def("render",
           py::overload_cast<const string &, const string &, Stream &>(
               &Renderer::render),
           py::arg("view"), py::arg("parameters"), py::arg("stream"))
      .def("render",
           py::overload_cast<const string &, const Object &, Stream &>(
               &Renderer::render),
           py::arg("view"), py::arg("parameters"), py::arg("stream"))
      .def("render_tostring",
           py::overload_cast<const string &, const string &>(
               &Renderer::renderToString),
           py::arg("view"), py::arg("parameters"))
      .def("render_tostring",
           py::overload_cast<const string &, const Object &>(
               &Renderer::renderToString),
           py::arg("view"), py::arg("parameters"));
}
