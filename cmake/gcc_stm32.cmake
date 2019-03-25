INCLUDE(CMakeForceCompiler)

SET(STM32_SUPPORTED_FAMILIES F1 F4 CACHE INTERNAL "stm32 supported families")

IF(NOT TOOLCHAIN_PREFIX)
     SET(TOOLCHAIN_PREFIX "/usr")
     MESSAGE(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
ENDIF()

IF(NOT TARGET_TRIPLET)
    SET(TARGET_TRIPLET "arm-none-eabi")
    MESSAGE(STATUS "No TARGET_TRIPLET specified, using default: " ${TARGET_TRIPLET})
ENDIF()

IF(NOT STM32_FAMILY)
    MESSAGE(STATUS "No STM32_FAMILY specified, trying to get it from STM32_CHIP")
    IF(NOT STM32_CHIP)
        SET(STM32_FAMILY "F1" CACHE INTERNAL "stm32 family")
        MESSAGE(STATUS "Neither STM32_FAMILY nor STM32_CHIP specified, using default STM32_FAMILY: ${STM32_FAMILY}")
    ELSE()
        STRING(REGEX REPLACE "^[sS][tT][mM]32(([fF][0-4])|([lL][0-1])|([tT])|([wW])).+$" "\\1" STM32_FAMILY ${STM32_CHIP})
        STRING(TOUPPER ${STM32_FAMILY} STM32_FAMILY)
        MESSAGE(STATUS "Selected STM32 family: ${STM32_FAMILY}")
    ENDIF()
ENDIF()

STRING(TOUPPER ${STM32_FAMILY} STM32_FAMILY)
LIST(FIND STM32_SUPPORTED_FAMILIES ${STM32_FAMILY} FAMILY_INDEX)
IF(FAMILY_INDEX EQUAL -1)
    MESSAGE(FATAL_ERROR "Invalid/unsupported STM32 family: ${STM32_FAMILY}")
ENDIF()

SET(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
SET(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/include)
SET(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/lib)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

CMAKE_FORCE_C_COMPILER(${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-g++ GNU)
SET(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-gcc)

SET(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objcopy CACHE INTERNAL "objcopy tool")
SET(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objdump CACHE INTERNAL "objdump tool")

SET(CMAKE_C_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "c compiler flags debug")
SET(CMAKE_CXX_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "cxx compiler flags debug")
SET(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm compiler flags debug")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "linker flags debug")

SET(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "c compiler flags release")
SET(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "cxx compiler flags release")
SET(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags release")
SET(CMAKE_EXE_LINKER_FLAGS_RELESE "-flto" CACHE INTERNAL "linker flags release")

SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET} ${EXTRA_FIND_PATH})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

FUNCTION(STM32_ADD_HEX_BIN_TARGETS TARGET)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    ELSE()
      SET(FILENAME "${TARGET}")
    ENDIF()
    ADD_CUSTOM_TARGET(${TARGET}.hex DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Oihex ${FILENAME} ${FILENAME}.hex)
    ADD_CUSTOM_TARGET(${TARGET}.bin DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FILENAME}.bin)
ENDFUNCTION()

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/gcc_stm32f1.cmake)

MACRO(STM32_KB_TO_BYTES KB BYTES)
    STRING(REGEX REPLACE "^(.+)[kK]$" "\\1" KILOBYTES ${KB})
    IF(KILOBYTES)
        MATH(EXPR ${BYTES} "${KILOBYTES} * 1024")
    ELSE()
        SET(${BYTES} ${KB})
    ENDIF()
ENDMACRO()

FUNCTION(STM32_SET_FLASH_PARAMS TARGET STM32_FLASH_SIZE STM32_RAM_SIZE STM32_STACK_ADDRESS STM32_MIN_STACK_SIZE STM32_MIN_HEAP_SIZE STM32_FLASH_ORIGIN STM32_RAM_ORIGIN)
    IF(NOT STM32_LINKER_SCRIPT)
        MESSAGE(FATAL_ERROR "No linker script specified. Please specify linker script using STM32_LINKER_SCRIPT variable.")
    ENDIF()
    CONFIGURE_FILE(${STM32_LINKER_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld)
    
    GET_TARGET_PROPERTY(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    IF(TARGET_LD_FLAGS)
        SET(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${TARGET_LD_FLAGS}")
    ELSE()
        SET(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld")
    ENDIF()
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})
ENDFUNCTION()

FUNCTION(STM32_SET_FLASH_PARAMS TARGET FLASH_SIZE RAM_SIZE)
    IF(NOT STM32_LINKER_SCRIPT)
        MESSAGE(FATAL_ERROR "No linker script specified. Please specify linker script using STM32_LINKER_SCRIPT variable.")
    ENDIF()
    
    STM32_KB_TO_BYTES(${FLASH_SIZE} FLASH_SIZE)
    STM32_KB_TO_BYTES(${RAM_SIZE} RAM_SIZE)
    
    IF(NOT STM32_FLASH_ORIGIN)
        SET(STM32_FLASH_ORIGIN "0x08000000")
    ENDIF() 
    
    IF(NOT STM32_RAM_ORIGIN)
        SET(STM32_RAM_ORIGIN "536870912")
    ENDIF()
    
    IF(NOT STM32_STACK_ADDRESS)
        MATH(EXPR STM32_STACK_ADDRESS "${STM32_RAM_ORIGIN} + ${RAM_SIZE}")
    ENDIF()
        
    SET(STM32_FLASH_SIZE ${FLASH_SIZE})
    SET(STM32_RAM_SIZE ${RAM_SIZE})
    
    IF(NOT STM32_MIN_STACK_SIZE)
        SET(STM32_MIN_STACK_SIZE "0x200")
    ENDIF()
    
    IF(NOT STM32_MIN_HEAP_SIZE)
        SET(STM32_MIN_HEAP_SIZE "0")
    ENDIF()
        
    CONFIGURE_FILE(${STM32_LINKER_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld)
    
    GET_TARGET_PROPERTY(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    IF(TARGET_LD_FLAGS)
        SET(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${TARGET_LD_FLAGS}")
    ELSE()
        SET(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld")
    ENDIF()
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})
ENDFUNCTION()

FUNCTION(STM32_SET_TARGET_PROPERTIES TARGET)
    IF(NOT STM32_CHIP_TYPE)
        IF(NOT STM32_CHIP)
            MESSAGE(WARNING "Neither STM32_CHIP_TYPE nor STM32_CHIP selected, you'll have to use STM32_SET_CHIP_DEFINITIONS directly")
        ELSE()
            STM32_GET_CHIP_TYPE(${STM32_CHIP} STM32_CHIP_TYPE)
        ENDIF() 
    ENDIF()
    STM32_SET_CHIP_DEFINITIONS(${TARGET} ${STM32_CHIP_TYPE})
    IF(((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE)) AND (NOT STM32_CHIP))
        MESSAGE(FATAL_ERROR "Cannot get chip parameters. Please specify either STM32_CHIP or STM32_FLASH_SIZE/STM32_RAM_SIZE")
    ENDIF()
    IF((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE))
        STM32_GET_CHIP_PARAMETERS(${STM32_CHIP} STM32_FLASH_SIZE STM32_RAM_SIZE)
        IF((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE))
            MESSAGE(FATAL_ERROR "Unknown chip: ${STM32_CHIP}. Try to use STM32_FLASH_SIZE/STM32_RAM_SIZE directly.")
        ENDIF()
    ENDIF()
    STM32_SET_FLASH_PARAMS(${TARGET} ${STM32_FLASH_SIZE} ${STM32_RAM_SIZE})
    MESSAGE(STATUS "${STM32_CHIP} has ${STM32_FLASH_SIZE}iB of flash memory and ${STM32_RAM_SIZE}iB of RAM")
ENDFUNCTION()

FUNCTION(STM32_SET_HSE_VALUE TARGET STM32_HSE_VALUE)   
    GET_TARGET_PROPERTY(TARGET_DEFS ${TARGET} COMPILE_DEFINITIONS)
    IF(TARGET_DEFS)
        SET(TARGET_DEFS "HSE_VALUE=${STM32_HSE_VALUE};${TARGET_DEFS}")
    ELSE()
        SET(TARGET_DEFS "HSE_VALUE=${STM32_HSE_VALUE}")
    ENDIF()
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_DEFINITIONS "${TARGET_DEFS}")
ENDFUNCTION()

MACRO(STM32_GENERATE_LIBRARIES NAME SOURCES LIBRARIES)
    STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
    FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
        STRING(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
        LIST(APPEND ${LIBRARIES} ${NAME}_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
        ADD_LIBRARY(${NAME}_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER} ${SOURCES})
        STM32_SET_CHIP_DEFINITIONS(${NAME}_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER} ${CHIP_TYPE})
    ENDFOREACH()
ENDMACRO()
