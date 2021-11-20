# User Guide

This guide should help you to get started.

## Table of contents

- [Generating views](#generating-views)
    - [Setup Pipeline](#setup-pipeline)
    - [Create your first JSX view](#create-your-first-jsx-view)
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
