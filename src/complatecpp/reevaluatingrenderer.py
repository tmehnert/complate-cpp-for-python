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
from .core import Renderer


class ReEvaluatingRenderer(Renderer):
    """
    Renderer which creates a Renderer for every render call.

    This implementation can be used in development to cause
    the complate source bundle to be re-evaluated on every render call.
    So you can edit your JSX-Components and see the change in HTML output
    without having to restart your application.
    """
    def __init__(self, creator):
        # Don't use super()
        Renderer.__init__(self)
        self.creator = creator

    def render(self, view, parameters, stream):
        """Render a view to a Stream

        :param view: Name of the view you want to be rendered.
        :type view: str
        :param parameters: The view Parameters aka 'the Model' which passed to the view.
        :param stream: A stream in which the HTML output will be forwarded.
        """
        self.creator().render(view, parameters, stream)

    def render_tostring(self, view, parameters):
        """Render a view to a String.

        :param view: Name of the view you want to be rendered.
        :type view: str
        :param parameters: The view Parameters aka 'the Model' which passed to the view.
        :return: A string which contains the HTML output.
        """
        return self.creator().render_tostring(view, parameters)
