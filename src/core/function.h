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

#include <complate/core/function.h>
#include <pybind11/pybind11.h>

#include "mapper.h"

static const char FUNCTION_DOC_CLASS[] = R"DELIM(
  Function to call C++ Functions from JavaScript.

  You don't need to use this type, because the module will convert functions
  back and forth for you. Just use your function as you would in plain Python.
  This type is only available to Python for testing purposes.
)DELIM";

void registerFunction(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace complate;

  py::class_<Function>(m, "Function")
      .def(
          "apply",
          [](const Function &function, const Array &args) {
            return Mapper::value_to_python(function.apply(args));
          },
          "Call the Callback.")
      .doc() = FUNCTION_DOC_CLASS;
}
