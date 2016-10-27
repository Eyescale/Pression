# Copyright (c) 2016, Stefan.Eilemann@epfl.ch
#
# Change this number when adding tests to force a CMake run: 0

include(InstallFiles)

set(TEST_LIBRARIES PressionData ${Boost_PROGRAM_OPTIONS_LIBRARY})

include(CommonCTest)
install_files(share/Pression/tests FILES ${TEST_FILES} COMPONENT examples)
