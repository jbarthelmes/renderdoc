#include "gl_test.h"

RD_TEST(GL_VAO_DSA, OpenGLGraphicsTest)
{
  static constexpr const char *Description =
      "VAO DSA";

  std::string common = R"EOSHADER(

#version 420 core

#define v2f v2f_block \
{                     \
	vec4 color;       \
}

)EOSHADER";

  std::string vertex = R"EOSHADER(

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec4 aRectSrc;
layout (location = 2) in vec4 aTint;

out v2f OUT;

void main()
{
  vec2 vertices[] = vec2[](
    vec2(+1.0, +1.0),
    vec2(-1.0, +1.0),
    vec2(+1.0, -1.0),
    vec2(-1.0, -1.0)
  );
  vec2 src_p0 = vec2(aRectSrc.x, aRectSrc.y);
  vec2 src_p1 = vec2(aRectSrc.z, aRectSrc.w);
  vec2 src_size = src_p1 - src_p0;
  vec2 src_half_size = src_size / 2.0;
  vec2 src_center = (src_p1 + src_p0) / 2.0;
  vec2 src_pos = vertices[gl_VertexID] * src_half_size + src_center;
  vec2 dst_p0 = vec2(aPosition.x, -aPosition.y) - src_half_size;
  vec2 dst_p1 = dst_p0 + src_size;
  vec2 dst_half_size = (dst_p1 - dst_p0) / 2.0;
  vec2 dst_center = (dst_p1 + dst_p0) / 2.0;
  vec2 cpos = dst_center * 16.0;
  vec2 dst_pos = vertices[gl_VertexID] * dst_half_size + cpos;
  vec2 pos = 2.0 * dst_pos / 100.0;
  float posz = -cpos.y / 100.0 + 0.5;
  gl_Position = vec4(pos.xy, posz, 1.0);
  OUT.color = aTint;
}

)EOSHADER";

  std::string pixel = R"EOSHADER(

in v2f IN;

out vec4 color;

void main()
{
    color = IN.color;
}

)EOSHADER";

  int main()
  {
    glMajor = 4;
    glMinor = 5;
    // initialise, create window, create context, etc
    if(!Init())
      return 3;

    GLuint program = MakeProgram(common + vertex, common + pixel); 

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayBindingDivisor(vao, 0, 1);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 1, 1);
    glVertexArrayBindingDivisor(vao, 1, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0);
    glVertexArrayAttribBinding(vao, 2, 2);
    glVertexArrayBindingDivisor(vao, 2, 1);

    GLuint vbo[3];
    glCreateBuffers(3, vbo);
    glVertexArrayVertexBuffer(vao, 0, vbo[0], 0, 8);
    glVertexArrayVertexBuffer(vao, 1, vbo[1], 0, 16);
    glVertexArrayVertexBuffer(vao, 2, vbo[2], 0, 4);

    while(Running())
    {
      glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
      // pass
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClearDepth(1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glUseProgram(program);

      float pos[2]{};
      float src_rect[4]{0.0f, 0.0f, 16.0f, 16.0f};
      unsigned int tint = 0xFFFFFFFF;

      glNamedBufferData(vbo[0], 8,
                         pos, GL_STATIC_DRAW);
      glNamedBufferData(vbo[1], 16, src_rect,
                         GL_STATIC_DRAW);
      glNamedBufferData(vbo[2], 4, &tint,
                         GL_STATIC_DRAW);
      glBindVertexArray(vao);
      glVertexArrayVertexBuffer(vao, 0, vbo[0], 0, 8);
      glVertexArrayVertexBuffer(vao, 1, vbo[1], 0, 16);
      glVertexArrayVertexBuffer(vao, 2, vbo[2], 0, 4);
      const bool workaround = false;
      if(workaround)
      {
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0);
        glVertexArrayBindingDivisor(vao, 0, 1);
        glVertexArrayBindingDivisor(vao, 1, 1);
        glVertexArrayBindingDivisor(vao, 2, 1);
      }

      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

      Present();
    }

    return 0;
  }
};

REGISTER_TEST();
