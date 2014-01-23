/// \author James Hughes
/// \date   January 2014

#ifndef IAUNS_GLSHADER_HPP
#define IAUNS_GLSHADER_HPP

// All functions below assume there is a valid OpenGL context active.
#include <vector>
#include <list>
#include <tuple>
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

  /// \param name       Name of the attribute in code.
  /// \param s          Size of the attributes in units of type (t).
  /// \param t          OpenGL type of the attribute.
  /// \param loc        Location of the attribute in the shader (set to 0 if this is
  ///                   VBO attribute list)..
  /// \param normalize  If 1, then this attribute will be normalized between 0-1.
  ///                   Only used if this is a VBO attribute list.
  ShaderAttribute(const std::string& name, GLint s, GLenum t, GLint loc,
                  GLboolean normalize = 0);

  GLint     size;       ///< Size of attribute, in units of 'type'.
  size_t    sizeBytes;  ///< Size of the attribute, in bytes. Calculated in constructor.
  GLenum    type;       ///< GL type.
  GLint     attribLoc;  ///< Location as returned by glGetAttribLocation.
  GLboolean normalize;  ///< Always set to false. This is not data coming from
                        ///< OpenGL, it is only useful in the context of
                        ///< sending attributes to OpenGL. With this, we have
                        ///< all we need to call glVertexAttribPointer.

  // The following variables are calculated for you in the constructor.
  GLenum    baseType;   ///< Base GL type.
  int       numComps;   ///< Number of components in the GL base type.

  char      nameInCode[MaxNameLength];  ///< Name of the attribute in-code.
};

/// Determines if the given attribute array has the attribute with 'name'.
/// Will perform a strcmp on every string (nameInCode).
/// \return -1 if no attribute exists, otherwise this returns the index to
///         the attribute.
int hasAttribute(ShaderAttribute* array, size_t size, const char* name);

/// Collects all shader attributes into a vector of ShaderAttribute.
std::vector<ShaderAttribute> getProgramAttributes(GLuint program);

/// Sorts a vector of shader attributes alphabetically by 'nameInCode'.
void sortAttributesAlphabetically(std::vector<ShaderAttribute>& attribs);

/// Binds all attributes in given ShaderAttribute array.
/// Note: Be sure to set the normalize ShaderAttribute variable appropriately.
void bindAllAttributes(ShaderAttribute* array, size_t size);

/// Unbinds all attributes as bound by bindAllAttributes.
void unbindAllAttributes(ShaderAttribute* array, size_t size);

/// Binds all of \p subset's attributes, using superset as the guide that
/// determines the stride between attributes. Use this when a VBO has more
/// attributes than the shader accepts. This function is less efficient than
/// bindAllAttributes above. If supersetSize == subsetSize, this function
/// will print a warning, then proceed. If it finds unsatsified attributes,
/// an exception will be thrown.
/// Note: Be sure to set the normalize ShaderAttribute variable appropriately.
void bindSubsetAttributes(ShaderAttribute* superset, size_t supersetSize,
                          ShaderAttribute* subset, size_t subsetSize);

/// Unbind all attributes bound by bindSubsetAttributes.
void unbindSubsetAttributes(ShaderAttribute* superset, size_t supersetSize,
                            ShaderAttribute* subset, size_t subsetSize);

/// Minimal structure based on the intersection between shader and VBO.
struct ShaderAttributeApplied
{
  GLint       attribLoc;    ///< Attribute location from the shader.
  GLenum      baseType;     ///< Base OpenGL type of the attribute.
  GLint       numComps;     ///< Number of components of type \p baseType.
  GLboolean   normalize;    ///< Taken from the VBO's attribute list.
  uint32_t    offset;       ///< Calculated offset into VBO's memory.
};

/// Builds a sequence of applied attributes. Use this to set set up a VBO for 
/// rendering with a particular shader. If an error occurs, a runtime exception
/// will be thrown, but the tuple <0,0> will be returned on systems that do not
/// use exceptions.
/// \param superset     The set that specifies the stride of the buffer.
///                     Commonly corresponds to the VBO's attribute list.
/// \param supersetSize The size of \p superset.
/// \param subset       The subset that determines the attribute location.
///                     Commonly corresponds to the shader's attribute list.
/// \param subsetSize   The size of \p subset.
/// \param out          Array that will be modified with the appropriate binding
///                     values.
/// \param outMaxSize   Maximum size of \p out. An exception is thrown if this
///                     is not large enough to hold all attributes.
/// \return The first parameter of the tuple is the resultant size of the
///         modified ShaderAtributeApplied array. The second is the stride of
///         all components combined together.
std::tuple<size_t, size_t> buildPreappliedAttrib(
    ShaderAttribute* superset, size_t supersetSize,
    ShaderAttribute* subset, size_t subsetSize,
    ShaderAttributeApplied* out, size_t outMaxSize);

/// Binds shader attributes based off of the intersection of a superset and
/// subset as calculated prior by buildPreAppliedAttrib. This function is more
/// efficient and cache friendly than bindAllAttributes or bindSubsetAttributes.
/// \param array  \p out from buildPreAppliedAttrib.
/// \param size   Second tuple parameter from buildPreAppliedAttrib.
/// \param stride First tuple parameter from buildPreAppliedAttrib.
void bindPreappliedAttrib(ShaderAttributeApplied* array, size_t size,
                          size_t stride);

/// Unbind all attributes bound in bindPreappliedAttrib.
void unbindPreappliedAttrib(ShaderAttributeApplied* array, size_t size);

/// Generic structure for holding a shader uniform.
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
