cmake_minimum_required(VERSION 3.16)
project(PairFlow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ROOT package
find_package(ROOT REQUIRED COMPONENTS RIO Hist Tree RooFit RooFitCore)

# Include ROOT configuration
include(${ROOT_USE_FILE})

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/macro)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

# Link libraries
set(LINK_LIBS ${ROOT_LIBRARIES} Minuit)

# Kit executables
add_executable(TreeClone kit/TreeClone.cpp)
target_link_libraries(TreeClone ${LINK_LIBS})

add_executable(hadd_center kit/hadd_center.cpp)
target_link_libraries(hadd_center ${LINK_LIBS})

add_executable(RMerge kit/RMerge.cpp)
target_link_libraries(RMerge ${LINK_LIBS})

add_executable(BSMerge kit/BSMerge.cpp)
target_link_libraries(BSMerge ${LINK_LIBS})

add_executable(HistExtract kit/HistExtract.cpp)
target_link_libraries(HistExtract ${LINK_LIBS})

add_executable(CheckRootFile kit/CheckRootFile.cpp)
target_link_libraries(CheckRootFile ${LINK_LIBS})

# Macro executables
add_executable(NumContribCalibrationTest macro/NumContribCalibrationTest.cpp)
target_link_libraries(NumContribCalibrationTest ${LINK_LIBS})

add_executable(SE_PR macro/SE_PR.cpp)
target_link_libraries(SE_PR ${LINK_LIBS})

add_executable(ME_PR macro/ME_PR.cpp)
target_link_libraries(ME_PR ${LINK_LIBS})

add_executable(SE_PR_thn macro/SE_PR_thn.cpp)
target_link_libraries(SE_PR_thn ${LINK_LIBS})

add_executable(ME_PR_thn macro/ME_PR_thn.cpp)
target_link_libraries(ME_PR_thn ${LINK_LIBS})

add_executable(SE_RR macro/SE_RR.cpp)
target_link_libraries(SE_RR ${LINK_LIBS})

add_executable(ME_RR macro/ME_RR.cpp)
target_link_libraries(ME_RR ${LINK_LIBS})

add_executable(plot_PR macro/plot_PR.cpp)
target_link_libraries(plot_PR ${LINK_LIBS})

add_executable(RFunc_PR macro/RFunc_PR.cpp)
target_link_libraries(RFunc_PR ${LINK_LIBS})

add_executable(NJpsiCandidatePerEvent macro/NJpsiCandidatePerEvent.cpp)
target_link_libraries(NJpsiCandidatePerEvent ${LINK_LIBS})
