

set(PRESSION_PACKAGE_VERSION 1.0)
set(PRESSION_REPO_URL https://github.com/Eyescale/Pression.git)
set(PRESSION_DEPENDS eyescale eyescalePorts LibJpegTurbo OpenMP
            REQUIRED Boost Lunchbox)
set(PRESSION_FORCE_BUILD ${CI_BUILD})

if(CI_BUILD_COMMIT)
  set(PRESSION_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(PRESSION_REPO_TAG master)
endif()
set(PRESSION_FORCE_BUILD ON)
set(PRESSION_SOURCE ${CMAKE_SOURCE_DIR})