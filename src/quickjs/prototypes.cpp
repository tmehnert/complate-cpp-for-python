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
#include "prototypes.h"

#include <pybind11/functional.h>

#include "mapper.h"

using namespace std;
using namespace complate;
namespace py = pybind11;

vector<Prototype> Prototypes::create_prototypes(const py::list &types) {
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
          auto tup = Mapper::args_to_tuple(args);
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
              obj->attr(name.c_str()) = Mapper::value_to_python(value);
            });
        prototype.addProperty(prop);
      }
    }
    prototypes.push_back(prototype);
  }

  return prototypes;
}
