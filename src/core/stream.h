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
#include <pybind11/pybind11.h>

class PyStream : public complate::Stream {
public:
  void write(const char *str, int len) override {
    PYBIND11_OVERRIDE_PURE(void, Stream, write, str, len);
  }

  void writeln(const char *str, int len) override {
    PYBIND11_OVERRIDE_PURE(void, Stream, writeln, str, len);
  }

  void flush() override { PYBIND11_OVERRIDE_PURE(void, Stream, flush); }
};

static const char STREAM_DOC_CLASS[] = R"DELIM(
  Stream interface used to render output.

  This interface used by `complate::Renderer` to write generated HTML.
  Several implementations included, but when you need your own
  Stream implementation, then subclass this interface. But don't call super
  in your implementation constructor,
  call `Stream.__init__(self, *args, **kwargs)` instead.
)DELIM";

static const char STREAM_DOC_FLUSH[] = R"DELIM(
  Flush the stream.

  When your Stream implementation doesn't support
  or require to `flush()`, then just leave implementation
  empty.
)DELIM";

void registerStream(pybind11::module_ &m) {
  namespace py = pybind11;
  using namespace complate;

  py::class_<Stream, PyStream>(m, "Stream")
      .def(py::init<>(), "Does nothing. Stream is an interface.")
      .def("write", &Stream::write, "Write a string to the stream.",
           py::arg("string"), py::arg("length"))
      .def("writeln", &Stream::writeln,
           "Write a string to the stream followed by a newline.",
           py::arg("string"), py::arg("length"))
      .def("flush", &Stream::flush, STREAM_DOC_FLUSH)
      .doc() = STREAM_DOC_CLASS;
}
