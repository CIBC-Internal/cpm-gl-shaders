/// \author James Hughes
/// \date   January 2014

#ifndef IAUNS_GLSHADER_HPP
#define IAUNS_GLSHADER_HPP

// All functions below assume there is a valid OpenGL context active.
#include <vector>
#include <list>
#include <gl-platform/GLPlatform.hpp>

// 24 is the size of the overhead of an std::string in GCC's standard library
// on a 64 bit system, not including the memory allocated for the string itself.
#ifndef CPM_GLSHADER_MAX_ATTRIB_NAME
#define CPM_GLSHADER_MAX_ATTRIB_NAME 24
#endif

#ifndef CPM_GLSHADER_MAX_UNIFORM_NAME
#define CPM_GLSHADER_MAX_UNIFORM_NAME 24
#endif

namespace CPM_GL_SHADERS_NS {

struct ShaderSource
{
  /// \p source     Sources for the shader. Input directly into 'glShaderSource'.
  /// \p shaderType One of: GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
  ///               GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
  ///               GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
  ShaderSource(const std::vector<const char*>& sources, GLenum shaderType) :
      mSources(sources),
      mShaderType(shaderType)
  {}

  std::vector<const char*>  mSources;
  GLenum                    mShaderType;
};

/// Mimics glCreateProgram. Loads, compiles, and links shaders then returns the
/// OpenGL ID. Returns 0 if an error occured. Uses std::cerr to output any
/// important information regarding errors.
GLuint loadShaderProgram(const std::list<ShaderSource>& shaders);

struct ShaderAttribute
{
  static const int MaxNameLength = (CPM_GLSHADER_MAX_ATTRIB_NAME);

  ShaderAttribute(const std::string& name, GLint s, GLenum t, GLint loc);

  GLint   size;       ///< Size of attribute, in units of 'type'.
  GLenum  type;       ///< GL type.
  GLint   attribLoc;  ///< Location as returned by glGetAttribLocation.
  bool    normalize;  ///< Always set to false. This is not data coming from
                      ///< OpenGL, it is only useful in the context of
                      ///< sending attributes to OpenGL. With this, we have
                      ///< all we need to call glVertexAttribPointer.
  char    nameInCode[MaxNameLength];  ///< Name of the attribute in-code.
};

/// Collects all shader attributes into a vector of ShaderAttribute.
std::vector<ShaderAttribute> getProgramAttributes(GLuint program);

/// Sorts a vector of shader attributes alphabetically by 'nameInCode'.
void sortAttributesAlphabetically(std::vector<ShaderAttribute>& attribs);

/// Binds attributes based on given ShaderAttribute array.
void bindAttributes(ShaderAttribute* array, int length);

struct ShaderUniform
{
  static const int MaxNameLength = (CPM_GLSHADER_MAX_UNIFORM_NAME);

  ShaderUniform(const std::string& name, GLint s, GLenum t, GLint loc);

  GLint       size;         ///< Size of uniform, in units of 'type'.
  GLenum      type;         ///< GL type.
  GLint       uniformLoc;   ///< Location as returned by glGetUniformLocation.
  char        nameInCode[MaxNameLength];  ///< Name of the uniform in-code.
};

/// Collects all shader uniforms into a vector of ShaderUniform.
std::vector<ShaderUniform> getProgramUniforms(GLuint program);

} // namespace CPM_GL_SHADER_NS 

#endif 
