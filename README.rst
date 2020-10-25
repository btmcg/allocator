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

    git submodule add -- https://github.com/catchorg/Catch2.git third_party/catch2/2.13.2
    cd third_party/catch2/2.13.2
    git checkout v2.13.2

    git clone --branch=v1.5.2 --depth=1 https://github.com/google/benchmark.git gb
    cd gb
    cmake \
        -DBENCHMARK_ENABLE_GTEST_TESTS=BOOL=OFF \
        -DBENCHMARK_ENABLE_LTO:BOOL=ON \
        -DBENCHMARK_ENABLE_TESTING:BOOL=OFF \
        -DCMAKE_BUILD_TYPE:STRING=RELEASE \
        -DCMAKE_CXX_COMPILER:STRING=g++ \
        -DCMAKE_INSTALL_PREFIX:PATH=../../third_party/google-benchmark/gcc-10.2.0/1.5.2 \
        -DGCC_AR:STRING=gcc-ar \
        -DGCC_RANLIB:STRING=gcc-ranlib \
        -S . -B "build"
    cmake --build "build" --config Release --target install --parallel

    # now build with clang
    rm -rf build

    cmake \
        -DBENCHMARK_ENABLE_GTEST_TESTS=BOOL=OFF \
        -DBENCHMARK_ENABLE_LTO:BOOL=ON \
        -DBENCHMARK_ENABLE_TESTING:BOOL=OFF \
        -DBENCHMARK_USE_LIBCXX:BOOL=ON \
        -DCMAKE_BUILD_TYPE:STRING=RELEASE \
        -DCMAKE_CXX_COMPILER:STRING=clang++ \
        -DCMAKE_INSTALL_PREFIX:PATH=../../third_party/google-benchmark/clang-10.0.1/1.5.2 \
        -DLLVMAR_EXECUTABLE:STRING=llvm-ar \
        -DLLVMNM_EXECUTABLE:STRING=llvm-nm \
        -DLLVMRANLIB_EXECUTABLE:STRING=llvm-ranlib \
        -S . -B "build"
    cmake --build "build" --config Release --target install --parallel

    cd ..
    rm -rf gb


Upgrading third_party libraries
-------------------------------

fmt
~~~

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/7.0.3
    cd third_party/fmt/7.0.3
    git checkout 7.0.3

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
