Allocator
=========

A work-in-progress memory pool. The goal here is to write a seamless
drop-in replacement for ``std::allocator`` so that all standard
containers can be used without modification. For usage, see
``src/example``.


Initial repo set up
-------------------

.. code-block::

    git clone --recursive https://www.gitlab.com/btmcg/allocator.git
    cd allocator

    # set up google-benchmark
    git clone --branch=v1.5.1 https://github.com/google/benchmark.git gb
    cd gb

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-gcc/1.5.1
    make -j install

    # now build with clang
    rm -rf CMakeFiles src/generated

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_USE_LIBCXX=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-clang/1.5.1
    make -j install

    cd ..
    rm -rf gb

Upgrading third_party libraries
-------------------------------

fmt
~~~

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/7.0.0
    cd third_party/fmt/7.0.0
    git checkout 7.0.0

**Remove old submodule**

.. code-block::

    vim .gitmodules
    vim .git/config
    git add .gitmodules
    git rm --cached third_party/fmt/6.2.1
    rm -rf .git/modules/third_party/fmt/6.2.1
    rm -rf third_party/fmt/6.2.1

**Point makefile to new version**

.. code-block::

    vim nrmake/third_party.mk
