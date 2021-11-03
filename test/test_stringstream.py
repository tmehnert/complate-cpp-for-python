# Copyright 2021 Torsten Mehnert
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
import os

from complatecpp import StringStream


def test_write():
    stream = StringStream()
    stream.write(string="7 chars", length=7)
    assert stream.str() == "7 chars"


def test_writeln():
    stream = StringStream()
    stream.writeln(string="7 chars", length=7)
    assert stream.str() == "7 chars" + os.linesep


def test_flush():
    stream = StringStream()
    assert stream.flush is not None
    stream.flush()
