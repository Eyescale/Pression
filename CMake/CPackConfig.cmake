# Copyright (c) 2012 Stefan Eilemann <eile@eyescale.ch>

# Info: http://www.itk.org/Wiki/CMake:Component_Install_With_CPack

mark_as_advanced(PRESSION_PACKAGE_VERSION)

set(CPACK_PACKAGE_VENDOR "www.eyescale.ch")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6, libturbojpeg")

set(CPACK_MACPORTS_CATEGORY devel)
set(CPACK_MACPORTS_DEPENDS Lunchbox)

include(CommonCPack)
include(OSSCPack)
