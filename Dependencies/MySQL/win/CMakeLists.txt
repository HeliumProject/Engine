CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(CONNECTOR_PLATFORM "x64")
  IF(NOT CONNECTOR_PLATFORM_SUFFIX)
    SET(CONNECTOR_PLATFORM_SUFFIX "winx64")
  ENDIF(NOT CONNECTOR_PLATFORM_SUFFIX)
  SET(CONNECTOR_DEFAULT_DEST "ProgramFiles64Folder")
ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(CONNECTOR_PLATFORM "Intel")
  IF(NOT CONNECTOR_PLATFORM_SUFFIX)
    SET(CONNECTOR_PLATFORM_SUFFIX "win32")
  ENDIF(NOT CONNECTOR_PLATFORM_SUFFIX)
  SET(CONNECTOR_DEFAULT_DEST "ProgramFilesFolder")
ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)

INCLUDE("version.cmake")

SET(CONNECTOR_PRODUCT_DEST "MySQL Connector C ${CONNECTOR_PRODUCT_VERSION}")

# Generate GUID
EXECUTE_PROCESS(COMMAND uuidgen
                OUTPUT_VARIABLE CONNECTOR_PKG_ID1)
STRING(STRIP ${CONNECTOR_PKG_ID1} CONNECTOR_PKG_ID1)
EXECUTE_PROCESS(COMMAND uuidgen
                OUTPUT_VARIABLE CONNECTOR_PKG_ID2)
STRING(STRIP ${CONNECTOR_PKG_ID2} CONNECTOR_PKG_ID2)

# check if wix is in current path #
FIND_PATH(CONNECTOR_WIX_DIR candle.exe
        $ENV{WIX_DIR}/bin
        $ENV{ProgramFiles}/wix/bin
        $ENV{ProgramFiles}/Windows Installer */bin)

IF(NOT CONNECTOR_WIX_DIR)
  MESSAGE(ERROR "Wix not found. Please change your environment variable PATH or specify WIX_DIR")
ENDIF(NOT CONNECTOR_WIX_DIR)

IF (CONNECTOR_SIGNED)
  FIND_PROGRAM(HAVE_MD5SUM NAMES md5sum
  PATH ENV PATH)
  IF(NOT HAVE_MD5SUM)
    MESSAGE(ERROR "Can't find md5sum")
  ENDIF(NOT HAVE_MD5SUM)

  FIND_PROGRAM(HAVE_SIGNTOOL NAMES signtool
  PATH ENV PATH)
  IF(NOT HAVE_SIGNTOOL)
    MESSAGE(ERROR "Can't find signtool")
  ENDIF(NOT HAVE_SIGNTOOL)
ENDIF (CONNECTOR_SIGNED)

IF(NOT EXTRA_NAME_SUFFIX)
  SET(EXTRA_NAME_SUFFIX "")
ENDIF(NOT EXTRA_NAME_SUFFIX)

SET(WIXOUT "mysql-connector-c${EXTRA_NAME_SUFFIX}-${CONNECTOR_PRODUCT_VERSION}${CONNECTOR_PRODUCT_LEVEL}-${CONNECTOR_PLATFORM_SUFFIX}")
SET(ProductURN "MSQZC-600-ZZZZ")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mysql-connc-msi-template.xml.in
               ${CMAKE_CURRENT_SOURCE_DIR}/mysql-connc-msi-template.${CONNECTOR_PLATFORM}.xml @ONLY)

MESSAGE(STATUS "${WIXOUT}")

# CMake does not allow for single custom commands to have multiple outputs.
ADD_CUSTOM_COMMAND(OUTPUT mysql-connc-msi-template.${CONNECTOR_PLATFORM}.wixobj
                   COMMAND ${CONNECTOR_WIX_DIR}/candle
                   ARGS mysql-connc-msi-template.${CONNECTOR_PLATFORM}.xml mysql-connc-msi-ui.xml mysql-connc-msi-registration.xml)
ADD_CUSTOM_COMMAND(OUTPUT mysql-connc-msi-ui.wixobj
                   COMMAND ${CONNECTOR_WIX_DIR}/candle
                   ARGS mysql-connc-msi-template.${CONNECTOR_PLATFORM}.xml mysql-connc-msi-ui.xml mysql-connc-msi-registration.xml)
ADD_CUSTOM_COMMAND(OUTPUT mysql-connc-msi-registration.wixobj
                   COMMAND ${CONNECTOR_WIX_DIR}/candle
                   ARGS mysql-connc-msi-template.${CONNECTOR_PLATFORM}.xml mysql-connc-msi-ui.xml mysql-connc-msi-registration.xml)

ADD_CUSTOM_COMMAND(OUTPUT ${WIXOUT}.msi
                   COMMAND ${CONNECTOR_WIX_DIR}/light 
                   ARGS -o ${WIXOUT}.msi mysql-connc-msi-template.${CONNECTOR_PLATFORM}.wixobj mysql-connc-msi-ui.wixobj mysql-connc-msi-registration.wixobj ${CONNECTOR_WIX_DIR}/WixUI.wixlib ${CONNECTOR_WIX_DIR}/wixca.wixlib -loc ${CONNECTOR_WIX_DIR}/WixUI_en-us.wxl
                   DEPENDS mysql-connc-msi-template.${CONNECTOR_PLATFORM}.wixobj mysql-connc-msi-ui.wixobj mysql-connc-msi-registration.wixobj)

ADD_CUSTOM_TARGET(MSI ALL DEPENDS "${WIXOUT}.msi")

IF(CONNECTOR_SIGNED)
  ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.msi.md5"
                     COMMAND signtool sign -a -d "MySQL Connector C ${CONECTOR_PRODUCT_VERSION}" -du "http://www.mysql.com" -t "http://timestamp.verisign.com/scripts/timestamp.dll" ${WIXOUT}.msi
                     COMMAND md5sum ${WIXOUT}.msi > ${WIXOUT}.msi.md5)
  ADD_CUSTOM_TARGET(MSI_SIGNED ALL DEPENDS "${WIXOUT}.msi.md5")
ENDIF(CONNECTOR_SIGNED)
