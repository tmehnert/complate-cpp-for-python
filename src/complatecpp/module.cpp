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
#include <complate/quickjs/quickjsrendererbuilder.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "core/pystream.h"

namespace py = pybind11;
using namespace std;
using namespace complate;

py::object value_to_python(const Value &value) {
  if (value.holds<Null>()) {
    return py::cast(value.exactly<Null>());
  } else if (value.holds<Bool>()) {
    return py::cast(value.exactly<Bool>());
  } else if (value.holds<Number>()) {
    const auto &number = value.exactly<Number>();
    if (number.holds<int32_t>()) {
      return py::cast(number.exactly<int32_t>());
    } else if (number.holds<uint32_t>()) {
      return py::cast(number.exactly<uint32_t>());
    } else if (number.holds<int64_t>()) {
      return py::cast(number.exactly<int64_t>());
    } else if (number.holds<double>()) {
      return py::cast(number.exactly<double>());
    }
  } else if (value.holds<String>()) {
    return py::cast(value.exactly<String>().get<string>());
  }

  return py::none();
}

py::tuple args_to_tuple(const Array &args) {
  if (args.empty()) {
    return py::make_tuple();
  } else if (args.size() == 1) {
    return py::make_tuple(value_to_python(args.at(0)));
  } else if (args.size() == 2) {
    return py::make_tuple(value_to_python(args.at(0)),
                          value_to_python(args.at(1)));
  } else if (args.size() == 3) {
    return py::make_tuple(value_to_python(args.at(0)),
                          value_to_python(args.at(1)),
                          value_to_python(args.at(2)));
  } else if (args.size() == 4) {
    return py::make_tuple(
        value_to_python(args.at(0)), value_to_python(args.at(1)),
        value_to_python(args.at(2)), value_to_python(args.at(3)));
  } else if (args.size() == 5) {
    return py::make_tuple(
        value_to_python(args.at(0)), value_to_python(args.at(1)),
        value_to_python(args.at(2)), value_to_python(args.at(3)),
        value_to_python(args.at(4)));
  } else if (args.size() == 6) {
    return py::make_tuple(
        value_to_python(args.at(0)), value_to_python(args.at(1)),
        value_to_python(args.at(2)), value_to_python(args.at(3)),
        value_to_python(args.at(4)), value_to_python(args.at(5)));
  } else if (args.size() == 7) {
    return py::make_tuple(
        value_to_python(args.at(0)), value_to_python(args.at(1)),
        value_to_python(args.at(2)), value_to_python(args.at(3)),
        value_to_python(args.at(4)), value_to_python(args.at(5)),
        value_to_python(args.at(6)));
  } else if (args.size() == 8) {
    return py::make_tuple(
        value_to_python(args.at(0)), value_to_python(args.at(1)),
        value_to_python(args.at(2)), value_to_python(args.at(3)),
        value_to_python(args.at(4)), value_to_python(args.at(5)),
        value_to_python(args.at(6)), value_to_python(args.at(7)));
  } else {
    throw invalid_argument("More then 8 arguments are not supported");
  }
}

vector<Prototype> create_prototypes(const py::list &types) {
  vector<Prototype> prototypes;
  for (auto type : types) {
    Prototype prototype(type.attr("__name__").cast<string>());

    auto dir = py::module::import("builtins").attr("dir");
    auto callable = py::module::import("builtins").attr("callable");
    for (auto e : dir(type)) {
      auto name = e.cast<string>();
      if (name.find("__") == 0) {
        continue;
      }

      if (callable(type.attr(name.c_str())).cast<bool>()) {
        Method method(name, [name](void *p, const Array &args) {
          auto obj = static_cast<py::object *>(p);
          auto tup = args_to_tuple(args);
          return obj->attr(name.c_str())(*tup).cast<Value>();
        });

        prototype.addMethod(method);
      } else {
        Property prop(
            name,
            [name](void *p) {
              auto obj = static_cast<py::object *>(p);
              return obj->attr(name.c_str()).cast<Value>();
            },
            [name](void *p, const Value &value) {
              auto obj = static_cast<py::object *>(p);
              obj->attr(name.c_str()) = value_to_python(value);
            });
        prototype.addProperty(prop);
      }
    }
    prototypes.push_back(prototype);
  }

  return prototypes;
}

PYBIND11_MODULE(complatecpp, m) {
  m.doc() = "Python bindings for complate-cpp";

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
            auto tup = args_to_tuple(args);
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
        return value_to_python(function.apply(args));
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

  py::class_<QuickJsRenderer, Renderer>(m, "QuickJsRenderer")
      .def(py::init<const std::string &>(), py::arg("source"));

  py::class_<QuickJsRendererBuilder>(m, "QuickJsRendererBuilder")
      .def(py::init<>())
      .def("source", py::overload_cast<string>(&QuickJsRendererBuilder::source),
           py::arg("sourceStr"))
      .def("source",
           py::overload_cast<QuickJsRendererBuilder::SourceCreator>(
               &QuickJsRendererBuilder::source),
           py::arg("sourceCreator"))

      .def("bindings",
           py::overload_cast<Object>(&QuickJsRendererBuilder::bindings),
           py::arg("bindingsObj"))
      .def("bindings",
           py::overload_cast<QuickJsRendererBuilder::BindingsCreator>(
               &QuickJsRendererBuilder::bindings),
           py::arg("bindingsCreator"))
      .def("prototypes",
           [](QuickJsRendererBuilder &builder, const py::list &types) {
             builder.prototypes(create_prototypes(types));
             return ref(builder);
           })
      .def("prototypes",
           [](QuickJsRendererBuilder &builder, const py::function &creator) {
             builder.prototypes(create_prototypes(creator()));
             return ref(builder);
           })
      .def("unique", &QuickJsRendererBuilder::unique);
}
