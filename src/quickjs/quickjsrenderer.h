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

#include <complate/quickjs/quickjsrenderer.h>
#include <pybind11/pybind11.h>

static const char QUICKJS_RENDERER_DOC_CLASS[] = R"DELIM(
  Renderer which uses the QuickJS JavaScript Engine to render HTML output.

  You should use the QuickJsRendererBuilder to create an object of this class.
)DELIM";

void registerQuickJsRenderer(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace std;
  using namespace complate;

  py::class_<QuickJsRenderer, Renderer>(m, "QuickJsRenderer")
      .def(py::init<const string &>(),
           "Construct a QuickJsRenderer instance, QuickJsBuilder instead.",
           py::arg("source"))
      .doc() = QUICKJS_RENDERER_DOC_CLASS;
}
