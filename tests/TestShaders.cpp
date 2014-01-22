/// \author James Hughes
/// \date   November 2013

#include <batch-testing/GlobalGTestEnv.hpp>
#include <batch-testing/ContextTestFixture.hpp>

#include <gl-shaders/GLShader.hpp>
#include <file-util/FileUtil.hpp>

using namespace CPM_BATCH_TESTING_NS;

TEST_F(ContextTestFixture, TestBasicRendering)
{
  // Since we are using ContextTestFixture, a context has already been created
  // for us by the time that we get here.

  // Position data only.
  std::vector<float> vboData = 
  {
    -1.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f
  };
  std::vector<std::string> attribNames = {"aPos"};

  // 16-bit iboData
  std::vector<uint16_t> iboData =
  {
    0, 1, 2, 3
  };

  std::string vertexShader   = CPM_FILE_UTIL_NS::readFile("shaders/Color.vsh");
  std::string fragmentShader = CPM_FILE_UTIL_NS::readFile("shaders/Color.fsh");

  // Compile and link vertex and fragment shader into a program.
  CPM_GL_SHADERS_NS::loadShaderProgram(
      {
        CPM_GL_SHADERS_NS::ShaderSource({vertexShader.c_str()}, GL_VERTEX_SHADER),
        CPM_GL_SHADERS_NS::ShaderSource({fragmentShader.c_str()}, GL_FRAGMENT_SHADER),
      });
  

}


