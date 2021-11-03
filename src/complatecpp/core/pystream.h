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

#include <complate/core/stream.h>

namespace complate {

class PyStream : public Stream {
public:
  void write(const char *str, int len) override {
    PYBIND11_OVERRIDE_PURE(void, Stream, write, str, len);
  }

  void writeln(const char *str, int len) override {
    PYBIND11_OVERRIDE_PURE(void, Stream, writeln, str, len);
  }

  void flush() override { PYBIND11_OVERRIDE_PURE(void, Stream, flush); }
};
} // namespace complate
