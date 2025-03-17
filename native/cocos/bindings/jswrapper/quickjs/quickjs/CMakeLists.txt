set(QUICKJS_SOURCE 
    ${CMAKE_CURRENT_LIST_DIR}/cutils.c
    ${CMAKE_CURRENT_LIST_DIR}/cutils.h
    ${CMAKE_CURRENT_LIST_DIR}/libbf.c
    ${CMAKE_CURRENT_LIST_DIR}/libbf.h
    ${CMAKE_CURRENT_LIST_DIR}/libregexp-opcode.h
    ${CMAKE_CURRENT_LIST_DIR}/libregexp.c
    ${CMAKE_CURRENT_LIST_DIR}/libregexp.h
    ${CMAKE_CURRENT_LIST_DIR}/libunicode-table.h
    ${CMAKE_CURRENT_LIST_DIR}/libunicode.c
    ${CMAKE_CURRENT_LIST_DIR}/libunicode.h
    ${CMAKE_CURRENT_LIST_DIR}/list.h
    ${CMAKE_CURRENT_LIST_DIR}/quickjs-atom.h
    ${CMAKE_CURRENT_LIST_DIR}/quickjs-libc.c
    ${CMAKE_CURRENT_LIST_DIR}/quickjs-libc.h
    ${CMAKE_CURRENT_LIST_DIR}/quickjs-opcode.h
    ${CMAKE_CURRENT_LIST_DIR}/quickjs.c
    ${CMAKE_CURRENT_LIST_DIR}/quickjs.h
)

set(QUICKJS_DEFINES
    CONFIG_BIGNUM=1
    CONFIG_VERSION="2021-03-27"
)

if(USE_SE_QUICKJS)
    if(NOT USE_MODULES)
        list(APPEND CC_EXTERNAL_SOURCES ${QUICKJS_SOURCE})
        list(APPEND CC_EXTERNAL_INCLUDES ${CMAKE_CURRENT_LIST_DIR})
        list(APPEND CC_EXTERNAL_PRIVATE_DEFINITIONS ${QUICKJS_DEFINES})
    else()
        add_library(quickjs ${QUICKJS_SOURCE})

        target_include_directories(quickjs PUBLIC 
            ${CMAKE_CURRENT_LIST_DIR}
        )

        set_property(TARGET quickjs
            APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS ${QUICKJS_DEFINES}
        )

        set(se_libs_name quickjs)
    endif()
endif()