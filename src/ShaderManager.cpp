#include <string>
#include <algorithm>

#include "ShaderManager.h"
#include "BambooLib/include/Logger.h"

using namespace std;
using namespace BambooLib;

namespace BambooGraphics
{
using namespace std;
using namespace BambooLib;

int last_active;

//#define CHECK_FOR_GLERROR

bool GraphicsCore::ShaderManager::TShader::ItlLoadFileToString(const char* szFilename, GLubyte** pszShaderSource, unsigned long* nLength)
{
    ifstream file;
    file.open(szFilename, ios::in); // opens as ASCII!

    if(!file || !file.good())
    {
        Logger::fatal() << "Could not read shader source code from " << szFilename << Logger::endl;

    return false;
    }

    //unsigned long pos=file.tellg();
    file.seekg(0,ios::end);
    *nLength = file.tellg();
    file.seekg(ios::beg);

    if (*nLength==0)
    {
        Logger::fatal() << "Could not read shader source code from " << szFilename << " (File was empty)" << Logger::endl;
    return false;
    }

    *pszShaderSource = (GLubyte*) new GLubyte[(*nLength)+1];

    if (*pszShaderSource == 0)
    {
        Logger::fatal() << "Could not reserve memory for shader source code (reading " << szFilename << ")" << Logger::endl;
    return false;
    }

    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value...
    (*pszShaderSource)[*nLength] = 0;

    unsigned int nPos=0;
    while (file.good())
    {
       (*pszShaderSource)[nPos] = file.get();       // get character from file.
       if (!file.eof())
        nPos++;
    }

    (*pszShaderSource)[nPos] = 0;  // 0-terminate it at the correct position

    file.close();

    return true;
}

void GraphicsCore::ShaderManager::TShader::ItlPrintShaderLog(GLuint obj)
{
    int iLogLength = 0;
    int iMaximalLength=0;

    if(glIsShader(obj))
            glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&iMaximalLength);
    else
            glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&iMaximalLength);

    char *szInfoLog = new char[iMaximalLength];

    if (glIsShader(obj))
            glGetShaderInfoLog(obj, iMaximalLength, &iLogLength, szInfoLog);
    else
            glGetProgramInfoLog(obj, iMaximalLength, &iLogLength, szInfoLog);

    if (iLogLength > 1)
        Logger::error() << "Shader messages: \n" << szInfoLog << Logger::endl;

    delete[] szInfoLog;
}


void GraphicsCore::ShaderManager::TShader::ItlAddShader(GLenum tShaderType, const char *szFilename)
{
    //Logger::debug() << "Loading shader source: " << szFilename << Logger::endl;

    GLubyte *szShaderLog = 0;
    unsigned long int nShaderLength;

    bool source_ok = false;

    source_ok = ItlLoadFileToString(szFilename, &szShaderLog, &nShaderLength);

    assert (source_ok);

    GLuint nShaderID = glCreateShader(tShaderType);

    glShaderSource(nShaderID, 1,  (const GLchar **) &szShaderLog, (const GLint *) &nShaderLength);

    glCompileShader(nShaderID);

    //lets see if an error happened
    GLint iCompileStatus;

    //check vertex shader object
    glGetShaderiv(nShaderID, GL_COMPILE_STATUS, &iCompileStatus);
    if (!iCompileStatus)
    {
        ItlPrintShaderLog(nShaderID);
    Logger::fatal() << "Could not compile shader " << szFilename << Logger::endl;
    }
    else
    {
    Logger::debug() << "Successfully compiled shader " << szFilename << Logger::endl;
        m_glShaderObjects.push_back(nShaderID);
    }

    if (szShaderLog != 0)
        delete[] szShaderLog;
}

void GraphicsCore::ShaderManager::TShader::ItlLinkShader()
{
    GLint iProgrammID = glCreateProgram();

    for (unsigned int i=0; i < m_glShaderObjects.size(); i++)
        glAttachShader(iProgrammID, m_glShaderObjects.at(i));

    glLinkProgram(iProgrammID);

    GLint iLinkStatus;
    glGetProgramiv(iProgrammID, GL_LINK_STATUS, &iLinkStatus);

    if (!iLinkStatus)
    {
       ItlPrintShaderLog(iProgrammID);
       Logger::fatal() << "Could not link shader program" << Logger::endl;
    }
    else
    {
            m_nShaderId = iProgrammID;
            m_bReadyForUse = true;

        Logger::debug() << "Successfully linked shader program" << Logger::endl;
    }
}

GraphicsCore::ShaderManager::TShader::TShader(const char *szVertexShaderFilename,
                       const char *szTesselationControlShaderFilename,
                       const char *szTesselationEvaluationShaderFilename,
                       const char *szGeometryShaderFilename,
                       const char *szFragmentShaderFilename)
{
    ItlAddShader(GL_VERTEX_SHADER, szVertexShaderFilename);
    ItlAddShader(GL_TESS_CONTROL_SHADER, szTesselationControlShaderFilename);
    ItlAddShader(GL_TESS_EVALUATION_SHADER, szTesselationEvaluationShaderFilename);
    ItlAddShader(GL_GEOMETRY_SHADER, szGeometryShaderFilename);
    ItlAddShader(GL_FRAGMENT_SHADER, szFragmentShaderFilename);
    ItlLinkShader();
}

GraphicsCore::ShaderManager::TShader::TShader(const char *szVertexShaderFilename,
                       const char *szTesselationControlShaderFilename,
                       const char *szTesselationEvaluationShaderFilename,
                       const char *szFragmentShaderFilename)
{
    ItlAddShader(GL_VERTEX_SHADER, szVertexShaderFilename);
    ItlAddShader(GL_TESS_CONTROL_SHADER, szTesselationControlShaderFilename);
    ItlAddShader(GL_TESS_EVALUATION_SHADER, szTesselationEvaluationShaderFilename);
    ItlAddShader(GL_FRAGMENT_SHADER, szFragmentShaderFilename);
    ItlLinkShader();
}

GraphicsCore::ShaderManager::TShader::TShader(const char *szVertexShaderFilename,
               const char *szFragmentShaderFilename)
    : m_bReadyForUse(false)
{
    ItlAddShader(GL_VERTEX_SHADER, szVertexShaderFilename);
    ItlAddShader(GL_FRAGMENT_SHADER, szFragmentShaderFilename);
    ItlLinkShader();
}

GraphicsCore::ShaderManager::TShader::TShader(const char *szVertexShaderFilename,
               const char *szGeometryShaderFilename,
               const char *szFragmentShaderFilename)
    : m_bReadyForUse(false)
{
    ItlAddShader(GL_VERTEX_SHADER, szVertexShaderFilename);
    ItlAddShader(GL_GEOMETRY_SHADER, szGeometryShaderFilename);
    ItlAddShader(GL_FRAGMENT_SHADER, szFragmentShaderFilename);
    ItlLinkShader();
}


GraphicsCore::ShaderManager::TShader::~TShader()
{
    //delete shader program
    glDeleteProgram(m_nShaderId);

    //delete shader parts
    for_each(m_glShaderObjects.begin(), m_glShaderObjects.end(), [](GLuint shader) { glDeleteShader(shader); });
}

void GraphicsCore::ShaderManager::TShader::Activate()
{
    //activate shader if it is ready for use, else log an error message

    if (m_bReadyForUse)
    {
#ifdef CHECK_FOR_GLERROR
    glGetError();
#endif

    glUseProgram(m_nShaderId);

#ifdef CHECK_FOR_GLERROR
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
        Logger::error() << "glGetError(): " << TranslateGLerror(error) << Logger::endl;
#endif

    last_active = m_nShaderId;
    }
    else
    Logger::error() << "Could not activate shader because it is not ready for use" << Logger::endl;
}

GLint GraphicsCore::ShaderManager::TShader::GetUniformLocation(const char *szName)
{
    std::string sName(szName);

    if ((GLuint) last_active != m_nShaderId)
    Logger::error() << "While setting an uniform of an shader, the shader must be active!" << Logger::endl;

    std::map<std::string, GLint>::iterator iter = m_mCachedUniformLocations.find(sName);

    GLint iLocation;

    if (iter == m_mCachedUniformLocations.end())
    {
    iLocation = glGetUniformLocation(m_nShaderId, szName);
        m_mCachedUniformLocations[sName] = iLocation;
    }
    else
    {
        iLocation = iter->second;
    }

    return iLocation;
}

GLint GraphicsCore::ShaderManager::TShader::GetAttribLocation(const char *szName)
{
    if (static_cast<GLuint>(last_active) != m_nShaderId)
    Logger::error() << "While setting an attribute of an shader, the shader must be active!" << Logger::endl;

    std::string sName(szName);

    std::map<std::string, GLint>::iterator iter = m_mCachedAttributeLocations.find(sName);

    GLint iLocation;

    if (iter == m_mCachedAttributeLocations.end())
    {
    iLocation = glGetAttribLocation(m_nShaderId, szName);
        m_mCachedAttributeLocations[sName] = iLocation;
    }
    else
    {
        iLocation = iter->second;
    }

    return iLocation;
}

GraphicsCore::ShaderManager::ShaderManager()
{
    //nothing to do so far
    m_nCurrentActiveShaderProgram = 0;
}

GraphicsCore::ShaderManager::~ShaderManager()
{
    // destroy all used Shader instances
    // C++11 construct with lambda function :)
    for_each(m_vpShaders.begin(), m_vpShaders.end(), [](TShader *pShader) { delete pShader; });
}

void GraphicsCore::ShaderManager::AddShader(std::string sName, TShader *pShader)
{
    m_vpShaders.push_back(pShader);
    m_vsShaderNames.push_back(sName);
}

bool GraphicsCore::ShaderManager::ActivateShader(std::string sName)
{
    bool bSuccess = false;

    for (unsigned int a=0; a < m_vpShaders.size(); a++)
    {
    if (m_vsShaderNames[a].compare(sName)==0)
    {
            if (m_nCurrentActiveShaderProgram != a)
            {
                m_vpShaders[a]->Activate();
                m_nCurrentActiveShaderProgram = a;
            }

            bSuccess = true;
            break;
    }
    }

    if (bSuccess == false)
    Logger::error() << "Could not activate shader with internal name \"" << sName  << "\", because it is not registered in ShaderManager" << Logger::endl;

    return bSuccess;
}

GLint GraphicsCore::ShaderManager::GetAttribute(std::string sAttributeName)
{
    return m_vpShaders[m_nCurrentActiveShaderProgram]->GetAttribLocation(sAttributeName.data());
}

GLint GraphicsCore::ShaderManager::GetUniform(std::string sUniformName)
{
    return m_vpShaders[m_nCurrentActiveShaderProgram]->GetUniformLocation(sUniformName.data());
}

void GraphicsCore::ShaderManager::PushActiveShader()
{
    m_vActiveShaderStack.push(m_nCurrentActiveShaderProgram);
}

void GraphicsCore::ShaderManager::PopActiveShader()
{
    unsigned int nOldActiveShader = m_nCurrentActiveShaderProgram;

    m_nCurrentActiveShaderProgram = m_vActiveShaderStack.top();

    m_vActiveShaderStack.pop();

    if (m_nCurrentActiveShaderProgram != nOldActiveShader)
        m_vpShaders[m_nCurrentActiveShaderProgram]->Activate();
}

}
