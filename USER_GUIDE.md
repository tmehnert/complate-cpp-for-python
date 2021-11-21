# User Guide

This guide should help you to get started.

## Table of contents

- [Generating views](#generating-views)
    - [Setup Pipeline](#setup-pipeline)
    - [Create your first JSX view](#create-your-first-jsx-view)
- [Instantiate a renderer](#instantiate-a-renderer)
    - [Using the QuickJsRendererBuilder](#using-the-quickjsrendererbuilder)
    - [Global bindings for your views](#global-bindings-for-your-views)
    - [Prototypes for your own classes](#prototypes-for-your-own-classes)
- [Rendering HTML](#rendering-html)
    - [Render to string](#render-to-string)
    - [Render to stream](#render-to-stream)
    - [Using JSON as view parameters](#using-json-as-view-parameters)
    - [Exception handling](#exception-handling)
    - [More realistic JSX for the examples above](#more-realistic-jsx-for-the-examples-above)
- [Appendix JSX](#appendix-jsx)
    - [Reusable components](#reusable-components)
    - [UI logic on the server](#ui-logic-on-the-server)
    - [UI logic on the client](#ui-logic-on-the-client)

## Generating views

Your views need be packaged into a single **views.js** file, which have to be delivered alongside your application. The
renderer will load this file when it becomes initialized. For the next steps you must have **npm** installed on your
development machine or ci system. Let's assume you have an empty project and get through the steps.

### Setup Pipeline

In this section you will create your pipeline in order be able to create views. We will
use [faucet-pipeline](https://www.faucet-pipeline.org/) as our asset pipeline in this example.

#### Initialize your package.json

```shell
npm init
# This package contains the rendering code.
npm install complate-stream
# These packages needed to transform your views during compilation time.
npm install --save-dev \
  @babel/plugin-transform-react-jsx \
  faucet-pipeline-esnext \
  faucet-pipeline-js
```

#### Adding a faucet.config.js file with following content to root directory

```js
"use strict";

// The directory in which you would like to place your JSX views.
let templateDir = "./views";
module.exports = {
    watchDirs: [templateDir],
    js: [{
        source: templateDir + "/index.js",
        // The path where your bundle should be generted.
        target: "./views.js",
        exports: "render",
        esnext: {
            exclude: ["complate-stream"]
        },
        jsx: {pragma: "createElement"}
    }]
};
```

#### Adding a views/index.js file with following content

```js
import Renderer from "complate-stream";

// Here your views will be registered. We will add your first view later.
const renderer = new Renderer({
    doctype: '<!DOCTYPE html>'
});

// This will export the render function which will be called from Python.
export default function render(view, params, stream) {
    renderer.renderView(view, params, stream)
}
```

#### Add some useful scripts to your package.json to run the pipeline

```json
{
  "scripts": {
    "start": "npm run compile -- --watch",
    "compile": "faucet --compact"
  }
}
```

#### Testing your pipeline

```shell
# Run following, which compile your views once and terminate.
npm run compile
# Or run following, which will compile continuously as you edit your views.
npm start
# Stop "npm start" by pressing STRG+C.
# Congratulations you have configured your view pipeline properly.
```

### Create your first JSX view

In this section you will create a JSX view and register it to the engine. After adding the files, please
run `npm run compile` to check if it works.

#### Adding a views/greeting.jsx file with following content

```jsx
// This line have to be included in every view.
import {createElement} from 'complate-stream';

// Parameter "person" is a view parameter, which will be passed from Python.
export default function Greeting({person}) {
    return <html>
    <head>
        <meta charSet="UTF-8"/>
    </head>
    <body>
    <h1>Hello {person.name}</h1>
    </body>
    </html>
}
```

#### Register your view by adding this to views/index.js

```js
import Greeting from "./greeting"

// Place after instantiate the Renderer.
// Use the name "Greeting", when refering this view from Python.
renderer.registerView(Greeting)
```

## Instantiate a renderer

Now we go inside the Python code of your application and set up the renderer.

### Using the QuickJsRendererBuilder

For easy setup, use the Builder.

`````python
from complatecpp import QuickJsRendererBuilder

renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .unique()
`````

### Global bindings for your views

When instantiate a renderer you can pass a dict, which holds global variables that can be accessed from every view. The
bindings can contain every type that complate supports and will be accessible as JavaScript globals. Just pass
everything you want to use in a global context into the binding.

`````python
from complatecpp import QuickJsRendererBuilder

bindings = {
    "application": {
        "name": "Example"
    }
}

renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .bindings(bindings) \
    .unique()
`````

### Prototypes for your own classes

When you want to make your Python classes available in the JavaScript engine, you have to register a prototype for your
class. After passed to the renderer, you can simply use them in the global bindings dict or in the view parameters dict.
Your classes have to use *slots* or define *properties* in order to make the introspection to work.

`````python
from complatecpp import QuickJsRendererBuilder


class PersonUsingSlots:
    """ Using slots in your class """
    __slots__ = ["name"]

    def __init__(self, name):
        self.name = name


class PersonUsingProps:
    """ Define props in your class """

    def __init__(self, name):
        self._name = name

    @property
    def name(self): return self._name


# Both of the techniques will work with complate
renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .prototypes([PersonUsingSlots, PersonUsingProps]) \
    .unique()
`````

## Rendering HTML

Now all is prepared, we can use the renderer, passing the view name and parameters to it and doing somewhat with the
HTML output. In this section you also see what happens when an error in JSX occurs and how to handle them.

### Render to string

This is the easiest way is to render your HTML. Just call the renderer and get your html back.

````python
from complatecpp import QuickJsRendererBuilder

# Assume you have a Python class called Person.
renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .prototypes([Person]) \
    .unique()

parameters = {
    "person": Person(name="John Doe"),
    "links": {
        "support": "https://example.org/support",
        "homepage": "https://example.org"
    }
}

# You can use render_tostring to get the rendered content.
html = renderer.render_tostring("Greeting", parameters)
````

### Render to stream

You can achieve **progressive rendering** by using a Stream. The difference is that instead the renderer return the
HTML, it's continuously written to the stream while complate generates it. You might have to implement the Stream
interface in order to forward HTML written to your webservers output channel. Very simple implementations of Stream are
included (
StringStream).

````python
from complatecpp import QuickJsRendererBuilder, StringStream

# Assume you have a Python class called Person.
renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .prototypes([Person]) \
    .unique()

parameters = {
    "person": Person(name="John Doe"),
    "links": {
        "support": "https://example.org/support",
        "homepage": "https://example.org"
    }
}

# When you want progressive rendering you have to provide your own Stream implementation.
stream = StringStream()

# You can use render_tostring to get the rendered content.
renderer.render("Greeting", parameters, stream)

# This Stream implementation just stores a string, which is returned here.
html = stream.str()
````

### Using JSON as view parameters

The renderers also accept a JSON string as view parameters. Bindings can't be declared as JSON, but it's no problem to
use JSON as view parameters and declare bindings the native way.

`````python
from complatecpp import QuickJsRendererBuilder

renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .unique()

parameters = """
    {
      "person": {
        "name": "John Doe"
      },
      "links": {
        "support": "https://example.org/support",
        "homepage": "https:// example.org"
      }
    }
    """

# This would produce exactly the same output as the examples above.
html = renderer.render_tostring("Greeting", parameters)
`````

### Exception handling

A renderer will throw a **RuntimeError**, when an error occurs. This usually happens if there is some error in JSX like
accessing an undefined object.

````python
from complatecpp import QuickJsRendererBuilder

renderer = QuickJsRendererBuilder() \
    .source("<content-of-your-views.js>") \
    .unique()

parameters = {}

try:
    html = renderer.render_tostring("Greeting", parameters)
except RuntimeError as e:
    print(e)
````

### More realistic JSX for the examples above

This is a slightly more realistic example of the "Greeting" view. It should act as a preview of what's possible with
JSX. Some idioms will be documented in the [Appendix JSX](#appendix-jsx) section of this guide, but there are many
better JSX guides on the web.

```jsx
import {createElement} from 'complate-stream';
// Reusable JSX components imported from other files
import Layout from './components/layout';
import Link from './components/link';

// JSX components feel like native HTML when used. I love it!
export default function Greeting({person, links}) {
    return <Layout title="Greeting | Example">
        <h1>Hello {person.forename} {person.lastname}</h1>
        <p>
            Click <Link href={links.support} target="_blank"><b>here</b></Link> to get some help.
        </p>
    </Layout>
}
```

## Appendix JSX

This appendix can only be a preview of what is possible with JSX. I recommend you to read some better documentation
about JSX and [Web components](https://www.webcomponents.org/). You should start
at [complate.org](https://complate.org/).

### Reusable components

In order to make your components reusable, start by putting them in a separate file to use it across your application or
in a component library like [complate-fractal](https://github.com/complate/complate-fractal) to use it across multiple
applications and languages like C++, Java, JavaScript or Ruby. Let's start with a separate file. Create a folder
**views/components** and place a file **link.jsx** in it. It's used in the example shown
at [More realistic JSX for the examples above](#more-realistic-jsx-for-the-examples-above). Because of the **children**,
which is the content between the opening and closing tag of Link, this is might be as composable as html itself.

```jsx
<Link href="https://example.org">Everything, <b>including tags</b>, can be placed here</Link>
```

```jsx
import {createElement} from 'complate-stream';

export default function Link({href, target, classNames}, ...children) {
    return <a href={href} target={target} class={classNames}>
        {{children}}
    </a>
}
```

### UI logic on the server

Here is an example how to put some UI logic inside your components. Usually set some additional CSS classes or
conditionally render elements. Let's make an example TodoList, where a list of Todos have a property **what**, which
describes what to do, and can be **veryLate**, which require additional attention.

```jsx
import {createElement} from 'complate-stream';
import Layout from './components/layout';

export default function TodoList({todos}) {
    return <Layout title="My todos">
        <h1>My Todos</h1>
        <section>
            {(todos.length >= 0) ? todos.map(Todo) : <p>All work done!!!</p>}
        </section>
    </Layout>
}

function Todo(todo) {
    return <div class={`todo ${todo.veryLate && 'todo--very-late'}`}>
        {todo.what}
    </div>
}
```

### UI logic on the client

A typical idiom to put some logic on client side is to make use of [Web components](https://www.webcomponents.org/) to
create a custom html element which triggers your JavaScript code on client side. In this example a custom element
**\<to-do\>** is used. For getting this to execute some Code, you would have to define this element and its behaviour in
our clientside JavaScript assets.

```jsx
function Todo(todo) {
    return <to-do class={`todo ${todo.veryLate && 'todo--very-late'}`}>
        {todo.what}
    </to-do>
}
```
