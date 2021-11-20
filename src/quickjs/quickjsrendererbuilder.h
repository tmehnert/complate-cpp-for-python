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

#include <complate/quickjs/quickjsrendererbuilder.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "prototypes.h"

static const char QUICKJS_RENDERER_BUILDER_DOC_CLASS[] = R"DELIM(
  Builder for QuickJsRenderer.

  This class should support constructing a QuickJsRenderer.
)DELIM";

void registerQuickJsRendererBuilder(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace std;
  using namespace complate;

  py::class_<QuickJsRendererBuilder>(m, "QuickJsRendererBuilder")
      .def(py::init<>(),
           "Construct a builder to create a QuickJsRenderer instance.")
      .def("source", py::overload_cast<string>(&QuickJsRendererBuilder::source),
           "Pass the content of your views.js bundle as string.",
           py::arg("sourceStr"))
      .def("source",
           py::overload_cast<QuickJsRendererBuilder::SourceCreator>(
               &QuickJsRendererBuilder::source),
           "Pass a function that return the content of your views.js bundle.",
           py::arg("sourceCreator"))

      .def("bindings",
           py::overload_cast<Object>(&QuickJsRendererBuilder::bindings),
           "Pass your bindings.", py::arg("bindingsObj"))
      .def("bindings",
           py::overload_cast<QuickJsRendererBuilder::BindingsCreator>(
               &QuickJsRendererBuilder::bindings),
           "Pass a function that return your bindings.",
           py::arg("bindingsCreator"))
      .def(
          "prototypes",
          [](QuickJsRendererBuilder &builder, const py::list &types) {
            builder.prototypes(Prototypes::create_prototypes(types));
            return ref(builder);
          },
          "Pass your Python classes, which you want to use in views.")
      .def(
          "prototypes",
          [](QuickJsRendererBuilder &builder, const py::function &creator) {
            builder.prototypes(Prototypes::create_prototypes(creator()));
            return ref(builder);
          },
          "Pass a function that return Python classes, you want to use.")
      .def("unique", &QuickJsRendererBuilder::unique,
           "Build a renderer instance")
      .doc() = QUICKJS_RENDERER_BUILDER_DOC_CLASS;
}
