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
from complatecpp import QuickJsRendererBuilder, ReEvaluatingRenderer

creator_called = 0


def test_creator(views_creator, bindings_creator, prototypes_creator, todolist_html, todolist_parameters):
    global creator_called

    creator = QuickJsRendererBuilder() \
        .source(views_creator) \
        .bindings(bindings_creator) \
        .prototypes(prototypes_creator) \
        .creator()

    def spy():
        global creator_called
        creator_called += 1
        return creator()

    renderer = ReEvaluatingRenderer(spy)

    html = renderer.render_tostring("TodoList", todolist_parameters)
    assert html == todolist_html
    assert creator_called == 1

    html = renderer.render_tostring("TodoList", todolist_parameters)
    assert html == todolist_html
    assert creator_called == 2
