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
#include "mapper.h"

using namespace std;
using namespace complate;
namespace py = pybind11;

py::object Mapper::value_to_python(const Value& value) {
  if (value.holds<Null>()) {
    return py::cast(value.exactly<Null>());
  } else if (value.holds<Bool>()) {
    return py::cast(value.exactly<Bool>());
  } else if (value.holds<Number>()) {
    const auto& number = value.exactly<Number>();
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

py::tuple Mapper::args_to_tuple(const Array& args) {
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
