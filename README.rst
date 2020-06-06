allocator
=========


initial repo set up
-------------------

.. code-block::

    git clone --recursive https://www.gitlab.com/btmcg/allocator.git
    cd allocator

    # set up catch
    git submodule add -- https://github.com/catchorg/Catch2.git third_party/catch2/2.12.1
    cd third_party/catch2/2.12.1
    git checkout -b v2.12.1

    # set up fmt
    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/6.2.0
    cd third_party/fmt/6.2.1
    git checkout -b 6.2.1

    # set up google-benchmark
    git clone https://github.com/google/benchmark.git gb
    cd gb
    git checkout -b 1.5.0

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-gcc/1.5.0
    make install -j

    # now build with clang
    rm -rf CMakeFiles src/generated

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_USE_LIBCXX=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-clang/1.5.0
    make install -j

    cd ..
    rm -rf gb
