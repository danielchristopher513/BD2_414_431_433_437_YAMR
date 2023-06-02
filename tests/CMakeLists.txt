cmake_minimum_required(VERSION 2.8)

set(LIBS lmr event_core event_pthreads pthread ssh)

add_executable(testing_red testing_red.cpp)
target_link_libraries(testing_red ${LIBS})

add_executable(testing_map testing_map.cpp)
target_link_libraries(testing_map ${LIBS})

add_executable(testing_map_reduce testing_map_reduce.cpp)
target_link_libraries(testing_map_reduce ${LIBS})

