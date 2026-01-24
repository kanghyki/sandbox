#include "engine/render/opengl/GlPresenter.h"

#include <iostream>

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

namespace {
unsigned int CompileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512] = {};
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        std::cerr << "Shader compile failed: " << info_log << "\n";
    }
    return shader;
}

unsigned int CreateProgram() {
    const char* vertex_source = "#version 330 core\n"
                                "in vec2 a_pos;\n"
                                "in vec2 a_uv;\n"
                                "out vec2 v_uv;\n"
                                "void main() {\n"
                                "  v_uv = a_uv;\n"
                                "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
                                "}\n";

    const char* fragment_source = "#version 330 core\n"
                                  "in vec2 v_uv;\n"
                                  "out vec4 frag_color;\n"
                                  "uniform sampler2D u_tex;\n"
                                  "void main() {\n"
                                  "  frag_color = texture(u_tex, v_uv);\n"
                                  "}\n";

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex_source);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment_source);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glBindAttribLocation(program, 0, "a_pos");
    glBindAttribLocation(program, 1, "a_uv");
    glLinkProgram(program);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512] = {};
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        std::cerr << "Program link failed: " << info_log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
} // namespace

bool GlPresenter::Init() {
    if (initialized_) {
        return true;
    }

    program_ = CreateProgram();
    if (!program_) {
        return false;
    }

    const float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 1.0f,
    };
    const unsigned short indices[] = {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(program_);
    int sampler = glGetUniformLocation(program_, "u_tex");
    glUniform1i(sampler, 0);
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    initialized_ = true;
    return true;
}

void GlPresenter::Shutdown() {
    if (!initialized_) {
        return;
    }
    if (texture_) {
        glDeleteTextures(1, &texture_);
    }
    if (ebo_) {
        glDeleteBuffers(1, &ebo_);
    }
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (program_) {
        glDeleteProgram(program_);
    }

    texture_ = 0;
    ebo_ = 0;
    vbo_ = 0;
    vao_ = 0;
    program_ = 0;
    initialized_ = false;
}

void GlPresenter::Resize(int width, int height) {
    if (!initialized_) {
        return;
    }
    if (width <= 0 || height <= 0) {
        return;
    }

    if (width != tex_width_ || height != tex_height_) {
        tex_width_ = width;
        tex_height_ = height;
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width_, tex_height_, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glViewport(0, 0, tex_width_, tex_height_);
}

bool GlPresenter::Upload(const IRenderer& renderer) {
    if (!initialized_) {
        return false;
    }

    if (renderer.Width() <= 0 || renderer.Height() <= 0) {
        return false;
    }

    if (renderer.Width() != tex_width_ || renderer.Height() != tex_height_) {
        Resize(renderer.Width(), renderer.Height());
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, renderer.Width(), renderer.Height(), GL_RGBA,
                    GL_UNSIGNED_BYTE, renderer.Pixels());
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void GlPresenter::DrawFullscreen() {
    if (!initialized_) {
        return;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
    glUseProgram(0);
}

void GlPresenter::Present(const IRenderer& renderer) {
    if (!Upload(renderer)) {
        return;
    }
    DrawFullscreen();
}
