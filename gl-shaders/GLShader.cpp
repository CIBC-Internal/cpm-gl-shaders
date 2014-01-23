/// \author James Hughes
/// \date   January 2014

#include <stdexcept>
#include <cstring>
#include <algorithm>
#include "GLShader.hpp"

namespace CPM_GL_SHADERS_NS {

GLuint loadShaderProgram(const std::list<ShaderSource>& shaders)
{
  GLuint program = glCreateProgram();
  GL_CHECK();
  if (0 == program)
  {
    // This usually indicates an invalid context.
    throw std::runtime_error("Unable to create GL program using glCreateProgram.");
    return 0;
  }

  // Vector of compiled shaders alongside a function to delete the program and
  // all shaders.
  std::vector<GLuint> compiledShaders;
  auto deleteShaders = [&]()
  {
    for (auto compShader = compiledShaders.begin(); compShader != compiledShaders.end(); ++compShader)
    {
      glDeleteShader(*compShader);
    }
  };
  auto deleteProgramAndShaders = [&]()
  {
    deleteShaders();
    glDeleteProgram(program);
  };

  // Compile all shaders.
  int idx = 0;
  for (auto it = shaders.begin(); it != shaders.end(); ++it)
  {
    GLuint shader = glCreateShader(it->mShaderType);
    GL_CHECK();
    if (0 == shader)
    {
      deleteProgramAndShaders();
      throw std::runtime_error("Failed to create shader using glCreateShader");
    }

    // Add shader to list now, so it will be removed via any call to
    // deleteProgramAndShaders.
    compiledShaders.push_back(shader);

    // Set the source and compile.
    const char* contents = it->mSources[0];
    GL(glShaderSource(shader, it->mSources.size(), &contents, NULL));
    GL(glCompileShader(shader));

    // Check the compile status.
    GLint compiled;
    GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
    if (!compiled)
    {
      GLint infoLen = 0;

      GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen));
      if (infoLen > 1)
      {
        char* infoLog = new char[infoLen];

        GL(glGetShaderInfoLog(shader, infoLen, NULL, infoLog));
        std::cerr << "Error compiling shader program with index " << idx << ":"
                  << std::endl << infoLog << std::endl;

        delete[] infoLog;
      }

      deleteProgramAndShaders();
      throw std::runtime_error("Failed to compile shader.");
    }

    // Attach the shader to the program
    GL(glAttachShader(program, shader));

    ++idx;
  }

  // Link program.
  GL(glLinkProgram(program));

	// Check the link status 
	GLint linked;
	GL(glGetProgramiv(program, GL_LINK_STATUS, &linked));
	if (!linked)
	{
		GLint infoLen = 0;
		GL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen));

		if (infoLen > 1)
		{
      char* infoLog = new char[infoLen];

			GL(glGetProgramInfoLog(program, infoLen, NULL, infoLog));
      std::cerr << "Error linking program:" << std::endl;
      std::cerr << infoLog << std::endl;

      delete[] infoLog;
		}

    deleteProgramAndShaders();
    throw std::runtime_error("Failed to link shader.");
	}

  // Remove unnecessary compiled shaders.
  deleteShaders();

  return program;
}

std::vector<ShaderAttribute> getProgramAttributes(GLuint program)
{
  // Check the active attributes.
  GLint activeAttributes;
  GL(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttributes));

  std::vector<ShaderAttribute> attributes;
  const int maxAttribNameSize = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH;
  char attributeName[maxAttribNameSize];
  for (int i = 0; i < activeAttributes; i++)
  {
    GLsizei charsWritten = 0;
    GLint attribSize;
    GLenum type;

    GL(glGetActiveAttrib(program, static_cast<GLuint>(i), maxAttribNameSize,
                         &charsWritten, &attribSize, &type, attributeName));

    GLint loc = glGetAttribLocation(program, attributeName);

    attributes.push_back(ShaderAttribute(attributeName, attribSize, type, loc));
  }

  return attributes;
}

void sortAttributesAlphabetically(std::vector<ShaderAttribute>& attribs)
{
  auto comparison = [](const ShaderAttribute& lhs, const ShaderAttribute& rhs)
  {
    return std::strcmp(lhs.nameInCode, rhs.nameInCode) < 0;
  };
  std::sort(attribs.begin(), attribs.end(), comparison);
}

std::vector<ShaderUniform> getProgramUniforms(GLuint program)
{
  GLint activeUniforms;
  GL(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniforms));

  std::vector<ShaderUniform> uniforms;
  const int maxUniformNameSize = GL_ACTIVE_UNIFORM_MAX_LENGTH;
  char uniformName[maxUniformNameSize];
  for (int i = 0; i < activeUniforms; i++)
  {
    GLsizei charsWritten = 0;
    GLint uniformSize;
    GLenum type;

    GL(glGetActiveUniform(program, static_cast<GLuint>(i), maxUniformNameSize,
                          &charsWritten, &uniformSize, &type, uniformName));

    GLint loc = glGetUniformLocation(program, uniformName);

    uniforms.push_back(ShaderUniform(uniformName, uniformSize, type, loc));
  }

  return uniforms;
}



ShaderAttribute::ShaderAttribute(const std::string& name, GLint s, GLenum t, GLint loc) :
    size(s),
    type(t),
    attribLoc(loc),
    normalize(false)
{
  if (name.length() < MaxNameLength - 1)
  {
    std::strcpy(nameInCode, name.c_str());
  }
  else
  {
    const char* msg = "cpm-gl-shader: Attribute name longer than CPM_GLSHADER_MAX_ATTRIB_NAME - 1.";
    std::cerr << msg << std::endl;
    throw std::runtime_error(msg);
  }
}

ShaderUniform::ShaderUniform(const std::string& name, GLint s, GLenum t, GLint loc) :
    size(s),
    type(t),
    uniformLoc(loc)
{
  if (name.length() < MaxNameLength - 1)
  {
    std::strcpy(nameInCode, name.c_str());
  }
  else
  {
    const char* msg = "cpm-gl-shader: Uniform name longer than CPM_GLSHADER_MAX_UNIFORM_NAME - 1.";
    std::cerr << msg << std::endl;
    throw std::runtime_error(msg);
  }
}


} // namespace CPM_GL_SHADER_NS


