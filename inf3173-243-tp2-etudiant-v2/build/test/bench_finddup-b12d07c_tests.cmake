add_test( Benchmark /home/jaycee/Downloads/inf3173-243-tp2-etudiant-v2/build/bin/bench_finddup Benchmark  )
set_tests_properties( Benchmark PROPERTIES WORKING_DIRECTORY /home/jaycee/Downloads/inf3173-243-tp2-etudiant-v2/build/test SKIP_RETURN_CODE 4)
set( bench_finddup_TESTS Benchmark)
