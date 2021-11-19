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
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <complate/quickjs/quickjsrendererbuilder.h>
#include "prototypes.h"

using namespace std;
using namespace complate;
namespace py = pybind11;

PYBIND11_MODULE(quickjs, m) {
  m.doc() = "Python bindings for complate-cpp - QuickJs renderer";

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
             builder.prototypes(Prototypes::create_prototypes(types));
             return ref(builder);
           })
      .def("prototypes",
           [](QuickJsRendererBuilder &builder, const py::function &creator) {
             builder.prototypes(Prototypes::create_prototypes(creator()));
             return ref(builder);
           })
      .def("unique", &QuickJsRendererBuilder::unique);
}