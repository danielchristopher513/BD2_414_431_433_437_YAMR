cmake_minimum_required(VERSION 2.8)

add_library(lmr netcomm.cpp communication_network.h general.h register.h mapper.h mapper.cpp mapinput.h mapinput.cpp spec.h encoder.h hash.cpp mapreduce.h mapreduce.cpp reducer.h reducer.cpp reduceinput.h reduceinput.cpp netpacket.h)
