# Copyright (c) 2012 Stefan Eilemann <eile@eyescale.ch>

# Info: http://www.itk.org/Wiki/CMake:Component_Install_With_CPack

mark_as_advanced(PRESSION_PACKAGE_VERSION)

set(CPACK_PACKAGE_VENDOR "www.eyescale.ch")
set(CPACK_PACKAGE_CONTACT "Stefan Eilemann <eile@eyescale.ch>")
set(CPACK_PACKAGE_LICENSE "LGPL")
#set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/doc/RelNotes.md)
#set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/doc/RelNotes.md)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libstdc++6, libturbojpeg")

set(CPACK_MACPORTS_CATEGORY devel)
set(CPACK_MACPORTS_DEPENDS Lunchbox)

include(CommonCPack)
include(OSSCPack)
