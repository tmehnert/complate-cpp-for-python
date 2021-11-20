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

#include <complate/core/renderer.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

static const char RENDERER_DOC_CLASS[] = R"DELIM(
  Renderer interface to get HTML output from a view and it's parameters.

  Several implementations included in this core library to alter a other
  specific Renderer's behaviour.
  For actual Renderer implementations that really cause HTML output to be
  generated look at the implementations for JavaScript Engines like QuickJS.
)DELIM";

void registerRenderer(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace std;
  using namespace complate;

  py::class_<Renderer>(m, "Renderer")
      .def("render",
           py::overload_cast<const string &, const string &, Stream &>(
               &Renderer::render),
           "Render a view to a Stream using a JSON string as parameters.",
           py::arg("view"), py::arg("parameters"), py::arg("stream"))
      .def("render",
           py::overload_cast<const string &, const Object &, Stream &>(
               &Renderer::render),
           "Render a view to a Stream using a dict as parameters.",
           py::arg("view"), py::arg("parameters"), py::arg("stream"))
      .def("render_tostring",
           py::overload_cast<const string &, const string &>(
               &Renderer::renderToString),
           "Render a view to a Stream using a JSON string as parameters.",
           py::arg("view"), py::arg("parameters"))
      .def("render_tostring",
           py::overload_cast<const string &, const Object &>(
               &Renderer::renderToString),
           "Render a view to a String using a dict as parameters.",
           py::arg("view"), py::arg("parameters"))
      .doc() = RENDERER_DOC_CLASS;
}
