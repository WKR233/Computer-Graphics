set(pro Rock)

add_executable(${pro}
        gl_env.h
        main.cpp
        texture_image.h
        "learnopengl/camera.h"
        "learnopengl/shader.h"
        "learnopengl/mesh.h"
        "learnopengl/model.h"
)

target_link_libraries(${pro} PRIVATE assimp::assimp glew_s glm stb glfw tinyobjloader)
target_include_directories(${pro} PRIVATE
        ../third_party/glew/include
        ${CMAKE_CURRENT_BINARY_DIR})

target_compile_features(${pro} PRIVATE cxx_std_11)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)