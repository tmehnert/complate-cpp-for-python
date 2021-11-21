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

#include <complate/core/stringstream.h>
#include <pybind11/pybind11.h>

static const char STRINGSTREAM_DOC_CLASS[] = R"DELIM(
  Stream which stores rendered output as a string.

  Use str() to get the rendered output.
)DELIM";

void registerStringStream(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace complate;

  py::class_<StringStream, Stream>(m, "StringStream")
      .def(py::init<>(), "Construct a StringStream.")
      .def("str", &StringStream::str, "Get the rendered output.")
      .doc() = STRINGSTREAM_DOC_CLASS;
}
