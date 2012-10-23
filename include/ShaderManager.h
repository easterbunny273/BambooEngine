/*
 * header file for ShaderManager class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 10/2010, Project "BambooIsland"
 * 03/2011, Project "Free Pool"
 * 10/2012, Project "BambooGraphics"
 */

#pragma once

#ifndef __BAMBOOGRAPHICS_SHADERMANAGER_H
#define __BAMBOOGRAPHICS_SHADERMANAGER_H

#include <vector>
#include <string>
#include <stack>
#include "common_gl.h"
#include "Graphic.h"

namespace BambooGraphics
{
    //!  A class which handles many Shader classes
    /*!
      This class supports handling of many shader classes. It provides
      methods for nested activating of shaders (push the current Shader, activate another and pop (reactivate) the previous one).

      Furthermore, it provides methods to lock the active shader (all activateShader(...) calls are ignored),
      which can be used for things like enforcing a simple shader program when rendering the shadowmap or reflected objects.
    */

    class ShaderManager
    {
    public:
        /*! \name Public classes */
        //@{
        class TShader
        {
        public:
            /*! \name Constructors / Destructor */
            //@{
            /// Constructor which creates a shader program with a vertex shader and a fragment shader
                TShader(const char *szVertexShaderFilename, const char *szFragmentShaderFilename);

            /// Constructor which creates a shader program with a vertex shader, a geometry shader and a fragment shader
                TShader(const char *szVertexShaderFilename, const char *szFragmentShaderFilename, const char *szGeometryShaderFilename);

            /// Constructor which creates a shader program with a vertex shader, tesselation control shader, tesselation evaluation shader and a fragment shader
                TShader(const char *szVertexShaderFilename, const char *szTesselationControlShaderFilename, const char *szTesselationEvaluationShaderFilename, const char *szFragmentShaderFilename);

            /// Constructor which creates a shader program with a vertex shader, tesselation control shader, tesselation evaluation shader, geometry shader and fragment shader
                TShader(const char *szVertexShaderFilename, const char *szTesselationControlShaderFilename, const char *szTesselationEvaluationShaderFilename, const char *szGeometryShaderFilename, const char *szFragmentShaderFilename);

            /// Destructor
                ~TShader();
            //@}

            /*! \name Access to the shader programm */
            //@{
            /// activates the shader - attention: only one shader can be active. activating a shader deactivates the previous active shader
                void    Activate();

            /// returns the position of a given uniform variable
                GLint   GetUniformLocation(const char *szName);

            /// returns the position of a given attribute variable
                GLint   GetAttribLocation(const char *szName);

            /// returns the id of the shader
                GLuint  GetShaderID() { return m_nShaderId; }
            //@}
        private:
            /*! \name Internal help methods */
            //@{
            /// adds a new shader (vertex/geometry/fragment/tesselation control/tesselation evaluate) from a file to the shader program
            void ItlAddShader(GLenum tShaderType, const char *szFilename);

            /// links all added shader parts together to the shader program
            void ItlLinkShader();
            //@}

            /*! \name Static help methods */
            //@{
            /// loads a (text) file to a string (GLubyte *)
            static bool ItlLoadFileToString(const char* filename, GLubyte** ShaderSource, unsigned long* len);

            /// prints the messages of the glsl compiler
            static void ItlPrintShaderLog(GLuint obj);
            //@}

            /*! \name Private members */
            //@{
            GLuint			m_nShaderId;		    ///< the opengl-intern id of the shader (program)
            std::vector<GLuint>	m_glShaderObjects;	    ///< the opengl-intern ids of the shader parts
            bool			m_bReadyForUse;		    ///< wheter the shader is ready for use

            std::map<std::string, GLint>	m_mCachedUniformLocations;
            std::map<std::string, GLint>	m_mCachedAttributeLocations;
            //@}

        };
        //@}

        /*! \name Constructor / Destructor */
        //@{
        /// Constructor, private because the class is designed as a Singelton
        ShaderManager();

        /// Destructor, destroys all used shaders programs
        ~ShaderManager();
        //@}

        /*! \name Public methods */
        //@{
        /// adds a shader program to the manager, from now on the shader can be activated with activateShader(name),
        /// and the Shader instance is destroyed when the ShaderManager gets destroyed.  \sa activateShader()
            void AddShader(std::string sName, TShader *pShader);

        /// activates the given shader if possible, else the method prints an error message to the Logger. \sa Logger
            bool ActivateShader(std::string sName);

        /// pushs the active shader on the stack
            void PushActiveShader();

        /// pops the top shader from the stack and activates it
            void PopActiveShader();
        //@}

        /*! \name Public attributes */
        //@{
        /// returns the location of an uniform variable within the current active shader program
            GLint GetUniform(std::string sUniformName);

        /// returns the location of an attribute variable within the current active shader program
            GLint GetAttribute(std::string sAttributeName);
        //@}

    private:
        /*! \name Private members */
        //@{
        unsigned int                m_nCurrentActiveShaderProgram;	    ///< The id of the currently active shader

        std::vector<TShader *>		m_vpShaders;			///< the shader instances
        std::vector<std::string>	m_vsShaderNames;		///< the name for the shader instances
        std::stack<unsigned int>	m_vActiveShaderStack;		///< the stack for nested activating, push/pop
        //@}
    };
};

#endif //__BAMBOOGRAPHICS_SHADERMANAGER_H
