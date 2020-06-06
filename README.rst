allocator
=========


initial repo set up
-------------------

.. code-block::

    git clone --recursive https://www.gitlab.com/btmcg/allocator.git
    cd allocator

    # set up google-benchmark
    git clone --branch=v1.5.0 https://github.com/google/benchmark.git gb
    cd gb

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-gcc/1.5.0
    make -j install

    # now build with clang
    rm -rf CMakeFiles src/generated

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_USE_LIBCXX=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-clang/1.5.0
    make -j install

    cd ..
    rm -rf gb
