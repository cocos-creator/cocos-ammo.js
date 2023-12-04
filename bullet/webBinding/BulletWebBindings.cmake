
MESSAGE("[Bullet]webBinding/BulletWebBindings.cmake")

SET(WEB_BINDING_SOURCE_DIR ${PROJECT_SOURCE_DIR}/webBinding)

SET(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -ffunction-sections\
 -fdata-sections -Werror -ferror-limit=0 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2\
  -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal\
   -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-noreturn\
    -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-covered-switch-default\
     -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code\
      -Wno-unused-macros -Wno-unused-member-function -Wno-used-but-marked-unused -Wno-weak-template-vtables\
       -Wno-deprecated -Wno-non-virtual-dtor -Wno-invalid-noreturn -Wno-return-type-c-linkage\
        -Wno-reserved-id-macro -Wno-c++98-compat-pedantic -Wno-unused-local-typedef -Wno-old-style-cast\
         -Wno-newline-eof -Wno-unused-private-field -Wno-undefined-reinterpret-cast -Wno-invalid-offsetof\
          -Wno-unused-value -Wno-format-nonliteral  -Wno-undef\
          -Wno-unsafe-buffer-usage\
          -Wno-inconsistent-missing-destructor-override -Wno-suggest-destructor-override\
          -Wno-limited-postlink-optimizations\
          -Wbad-function-cast -Wcast-function-type -Wno-unused-parameter\
          -Wno-extra-semi -Wno-suggest-override -Wno-documentation\
          -Wno-zero-as-null-pointer-constant -Wno-cast-qual -Wno-comma -Wno-reserved-identifier\
          -Wno-shadow -Wno-double-promotion -Wno-extra-semi-stmt -Wno-unused-but-set-variable\
          -Wno-invalid-utf8 -Wno-unused-template -Wno-sign-compare -Wno-misleading-indentation\
          -Wno-unreachable-code-return -Wno-switch-enum\
          -Wno-unreachable-code-break -Wno-js-compiler\
          -fno-threadsafe-statics")

# Build debug info for all configurations
SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -std=c++11 -O0 -g3")
SET(CMAKE_CXX_FLAGS_CHECKED "${CMAKE_CXX_FLAGS_CHECKED} -std=c++11 -g3 -gdwarf-2 -O3")
SET(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -std=c++11 -O3 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -O3 -g0")

#https://emscripten.org/docs/optimizing/Optimizing-Code.html#optimizing-code-size
#https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
if(${BUILD_WASM})
        SET(EMSCRIPTEN_BASE_OPTIONS "--bind -s MODULARIZE=1 -s EXPORT_NAME=Bullet -s ALLOW_MEMORY_GROWTH=1\
         -s WASM=1 -s ENVIRONMENT=web -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s FILESYSTEM=0\
          -s MIN_SAFARI_VERSION=110000 -s DYNAMIC_EXECUTION=0 ")
else()
        SET(EMSCRIPTEN_BASE_OPTIONS "--bind -s MODULARIZE=1 -s EXPORT_NAME=Bullet -s ALLOW_MEMORY_GROWTH=1\
         -s WASM=0 -s ENVIRONMENT=web -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s FILESYSTEM=0\
          -s DYNAMIC_EXECUTION=0 -s SINGLE_FILE=1 ")
endif()

SET(BULLET_WEB_BINDINGS_SOURCE
        ${WEB_BINDING_SOURCE_DIR}/BulletWebBindings.cpp
        )

ADD_EXECUTABLE(BulletWebBindings ${BULLET_WEB_BINDINGS_SOURCE})

SET_TARGET_PROPERTIES(BulletWebBindings PROPERTIES
        LINK_FLAGS "${EMSCRIPTEN_BASE_OPTIONS}"
        )

if(${BUILD_WASM})
	set_target_properties(BulletWebBindings PROPERTIES OUTPUT_NAME "bullet.${CMAKE_BUILD_TYPE}.wasm")
else()
	set_target_properties(BulletWebBindings PROPERTIES OUTPUT_NAME "bullet.${CMAKE_BUILD_TYPE}.asm")
endif()

SET_TARGET_PROPERTIES(BulletWebBindings PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

TARGET_LINK_LIBRARIES(BulletWebBindings
        LinearMath
        BulletCollision
        BulletDynamics
        Extensions
        )

TARGET_INCLUDE_DIRECTORIES(BulletWebBindings
        PUBLIC ${PROJECT_SOURCE_DIR}
        PUBLIC ${PROJECT_SOURCE_DIR}/src
        )

# MESSAGE

MESSAGE(STATUS "BULLET_WEB_BINDINGS_SOURCE: ${BULLET_WEB_BINDINGS_SOURCE}")
MESSAGE(STATUS "BulletCollision: ${BulletCollision}")
MESSAGE(STATUS "BulletDynamics: ${BulletDynamics}")
MESSAGE(STATUS "Extensions: ${Extensions}")