"use strict";

function render(view, parameters, stream) {
    stream.writeln("")
    stream.writeln(`View: ${view}`)
    stream.write("Parameters: ")
    stream.writeln(JSON.stringify(parameters))
    stream.flush()
}