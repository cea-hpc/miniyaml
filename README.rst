.. This file is part of MiniYAML
   Copyright (C) 2020 Commissariat a l'energie atomique et aux energies
                      alternatives

   SPDX-License-Identifer: LGPL-3.0-or-later

########
MiniYAML
########

MiniYAML is a library that complements libyaml_. It was created in an effort to
to ease the development of C structures serializers/deserializers for the
RobinHood_ project.

.. _libyaml: https://github.com/yaml/libyaml
.. _RobinHood: https://github.com/cea-hpc/robinhood

Installation
============

Install libyaml_ (either from source of using your favorite package manager).
MiniYAML was developped using ``libyaml >= 0.1.7``; feel free to test it with an
older version.

Download MiniYAML's sources:

.. code:: bash

   git clone https://github.com/cea-hpc/miniyaml.git
   cd miniyaml

Build and install with meson_ and ninja_:

.. code:: bash

   meson builddir
   ninja -C builddir
   sudo ninja -C builddir install

.. _meson: https://mesonbuild.com
.. _ninja: https://ninja-build.org

Documentation
=============

To build the API documentation, use doxygen:

.. code:: bash

   # Generate a template configuration file for doxygen (Doxyfile)
   doxygen -g

   # Change some of the default parameeters:
   doxyconfig() {
       sed -i "s|\($1\s*\)=.*$|\1= ${*:2}|" Doxyfile
   }
   doxyconfig PROJECT_NAME      MiniYAML
   doxyconfig INPUT             include
   doxyconfig EXTRACT_ALL       YES
   doxyconfig EXTRACT_STATIC    YES
   doxyconfig ALIASES           error=@exception

   # Run doxygen
   doxygen

Usage
=====

There is a toy emitter_ and a toy parser_ in the examples_ directory. They
should provide users with a good enough overview of how to use miniyaml.

.. code:: console

   $ ./builddir/examples/emitter
   --- !person
   "name": "test"
   "age": 32
   ...

    $ ./builddir/examples/emitter | ./builddir/examples/parser
    person = {.name = test, .age = 32}

.. _emitter: https://github.com/cea-hpc/miniyaml/tree/master/examples/emitter.c
.. _parser: https://github.com/cea-hpc/miniyaml/tree/master/examples/parser.c
.. _examples: https://github.com/cea-hpc/miniyaml/tree/master/examples
