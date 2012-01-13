#include <glm/gtc/matrix_transform.hpp>

//general includes
#include "PC_Logger.h"
#include "ShaderManager.h"
#include "TextureManager.h"

//class specific
#include "RenderNodes/RenderNode_Generic.h"

GeometryData::TextureType tTextureTypes[4] = { GeometryData::TextureNames::ALBEDO,
                                               GeometryData::TextureNames::NORMAL,
                                               GeometryData::TextureNames::SPECULAR,
                                               GeometryData::TextureNames::DISPLACE };

Bamboo::RN_Generic::RN_Generic(std::shared_ptr<GeometryData::GenericObject> spObject)
    : m_pnVertexArrayObjects(NULL),
      m_pnVertexBufferObjects(NULL),
      m_pnIndexBufferObjects(NULL),
      m_pvsAttributeNames(NULL),
      m_pvnAttributeOffsets(NULL),
      m_pnNumIndices(NULL),
      m_pnColorTexture(NULL),
      m_pnNormalTexture(NULL),
      m_pnSpecularTexture(NULL),
      m_pnDisplaceTexture(NULL),
      m_spObject(spObject)
{
    ItlLoadShader();
    ItlPrepareGLBuffers();
    ItlPrepareTextures();
}

Bamboo::RN_Generic::~RN_Generic()
{
    ItlDeleteBuffers();
    ItlDeleteTextures();
}

void Bamboo::RN_Generic::ItlRender()
{
    static int i=0;

    i++;

    glm::mat4 oldTrans1;

    oldTrans1 = glm::rotate(glm::mat4(), i / 100.0f, glm::vec3(1.0f, 0.0f, 0.0f));

   // SetTransformMatrix(oldTrans1);

    TextureManager *pTextureManager = ItlGetGraphicCore()->GetTextureManager();
    assert (pTextureManager != NULL);

    ShaderManager *pShaderManager = ItlGetGraphicCore()->GetShaderManager();
    assert (pShaderManager != NULL);

    GLuint *pTextureArrayToUse[4] = { m_pnColorTexture, m_pnNormalTexture, m_pnSpecularTexture, m_pnDisplaceTexture };

    std::vector<GLuint> vUsedTextures;
    vUsedTextures.reserve(4);


    for (unsigned int nMesh = 0; nMesh < m_nNumMeshes; nMesh++)
    {
        for (int i = 0; i < 4; i++)
        {
            // if we have a specific texture type
            if (m_vmTextureInformations[nMesh].find(tTextureTypes[i]) != m_vmTextureInformations[nMesh].end())
            {
                // look if shader wants the texture
                GLint iLocation = pShaderManager->GetUniform(tTextureTypes[i]);

                if (iLocation != -1)
                {
                    GLuint nTextureID = pTextureArrayToUse[i][nMesh];
                    GLuint iUsedUnit = pTextureManager->UseTexture(nTextureID);

                    glUniform1i(iLocation, iUsedUnit);

                    vUsedTextures.push_back(nTextureID);
                }
            }
        }

        glBindVertexArray(m_pnVertexArrayObjects[nMesh]);
        glBindBuffer(GL_ARRAY_BUFFER, m_pnVertexBufferObjects[nMesh]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pnIndexBufferObjects[nMesh]);

        glDrawElements(GL_TRIANGLES, m_pnNumIndices[nMesh], GL_UNSIGNED_INT, (const GLvoid *) 0 );

        for (unsigned int i=0; i < vUsedTextures.size(); i++)
            pTextureManager->UnuseTexture(vUsedTextures[i]);

        vUsedTextures.clear();
    }
}


void Bamboo::RN_Generic::ItlPreRender()
{

}

void Bamboo::RN_Generic::ItlPostRender()
{

}

void Bamboo::RN_Generic::ItlPreRenderChildren()
{

}

void Bamboo::RN_Generic::ItlPostRenderChildren()
{

}

bool Bamboo::RN_Generic::ItlTestSkipRendering()
{

    return false;
}

void Bamboo::RN_Generic::ItlPrepareGLBuffers()
{
    // delete old buffers, if not NULL
    ItlDeleteBuffers();

    // alloc memory for arrays
    assert (m_pnVertexBufferObjects == NULL);
    assert (m_pnVertexArrayObjects == NULL);
    assert (m_pnIndexBufferObjects == NULL);
    assert (m_pnNumIndices == NULL);
    assert (m_pvsAttributeNames == NULL);
    assert (m_pvnAttributeOffsets == NULL);

    m_nNumMeshes = m_spObject->NumMeshes();

    m_pnVertexArrayObjects = new GLuint[m_nNumMeshes];
    m_pnVertexBufferObjects = new GLuint[m_nNumMeshes];
    m_pnIndexBufferObjects = new GLuint[m_nNumMeshes];
    m_pvsAttributeNames = new std::vector<std::string>[m_nNumMeshes];
    m_pvnAttributeOffsets = new std::vector<unsigned int>[m_nNumMeshes];
    m_pnNumIndices = new unsigned int [m_nNumMeshes];
    m_vmTextureInformations.resize(m_nNumMeshes);

    // generate/allocate opengl ids for buffers

    glGenBuffers(m_nNumMeshes, m_pnVertexBufferObjects);
    glGenBuffers(m_nNumMeshes, m_pnIndexBufferObjects);


    // now, fill the buffers

    for (unsigned int nMesh = 0; nMesh < m_nNumMeshes; nMesh++)
    {
        std::shared_ptr<GeometryData::GenericMesh> spMesh(m_spObject->GetMesh(nMesh));
        assert (spMesh);

       // GeometryData::GenericMesh *spMesh;

        std::vector<GLfloat> vBufferData;
        std::vector<unsigned int> vOffsets;
        std::vector<std::string> vsAttributeNames;

        std::vector<std::pair<std::string, std::vector<float> > > vAttributeList = spMesh->GetAttributeList();

        for (unsigned int i=0; i < vAttributeList.size(); i++)
        {
            vOffsets.push_back(vBufferData.size());
            vsAttributeNames.push_back(vAttributeList[i].first);

            for (unsigned int j=0; j < vAttributeList[i].second.size(); j++)
            {
                vBufferData.push_back(vAttributeList[i].second[j]);
            }
        }

        for (unsigned int i=0; i < 4; i++)
        {
            float * pfTexCoords = spMesh->GetTextureCoords(tTextureTypes[i]);

            if (pfTexCoords != NULL)
            {
                m_vmTextureInformations[nMesh][tTextureTypes[i]] = vBufferData.size();

                for (unsigned int j=0; j < spMesh->NumVertices() * 2; j++)
                {
                    vBufferData.push_back(pfTexCoords[j]);
                }
            }
        }

        m_pvsAttributeNames[nMesh] = vsAttributeNames;
        m_pvnAttributeOffsets[nMesh] = vOffsets;

        glBindBuffer(GL_ARRAY_BUFFER, m_pnVertexBufferObjects[nMesh]);
        glBufferData(GL_ARRAY_BUFFER, vBufferData.size() * sizeof(GLfloat), &(vBufferData[0]), GL_STATIC_DRAW);

        // now fill the indices buffer
        unsigned int *pIndices = spMesh->GetIndices(m_pnNumIndices[nMesh]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pnIndexBufferObjects[nMesh]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pnNumIndices[nMesh] * sizeof(GLuint), pIndices, GL_STATIC_DRAW);
    }

    ItlPrepareVAO();

}

void Bamboo::RN_Generic::ItlDeleteBuffers()
{
    if (m_pnVertexBufferObjects != NULL)
    {
        assert (m_pnIndexBufferObjects != NULL);

        glDeleteBuffers(m_nNumMeshes, m_pnVertexBufferObjects);
        glDeleteBuffers(m_nNumMeshes, m_pnIndexBufferObjects);
        glDeleteVertexArrays(m_nNumMeshes, m_pnVertexArrayObjects);

        delete[] m_pnVertexArrayObjects;
        m_pnVertexArrayObjects = NULL;

        delete[] m_pnVertexBufferObjects;
        m_pnVertexBufferObjects = NULL;

        delete[] m_pnIndexBufferObjects;
        m_pnIndexBufferObjects = NULL;

        delete [] m_pvsAttributeNames;
        m_pvsAttributeNames = NULL;

        delete [] m_pvnAttributeOffsets;
        m_pvnAttributeOffsets = NULL;
    }

}

void Bamboo::RN_Generic::ItlPrepareVAO()
{
    ShaderManager *pShaderManager = ItlGetGraphicCore()->GetShaderManager();
    assert (pShaderManager != NULL);

    pShaderManager->PushActiveShader();
    pShaderManager->ActivateShader("deferred_pass");

    // generate arrays
    glGenVertexArrays(m_nNumMeshes, m_pnVertexArrayObjects);


    for (unsigned int nMesh = 0; nMesh < m_nNumMeshes; nMesh++)
    {
        glBindVertexArray(m_pnVertexArrayObjects[nMesh]);
        glBindBuffer(GL_ARRAY_BUFFER, m_pnVertexBufferObjects[nMesh]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pnIndexBufferObjects[nMesh]);

        // set attrib pointers
        for (unsigned int i=0; i < m_pvsAttributeNames[nMesh].size(); i++)
        {
            std::string sCurrentAttributeName(m_pvsAttributeNames[nMesh][i]);

            const GLint l_current_attribute (ItlGetGraphicCore()->GetShaderManager()->GetAttribute(sCurrentAttributeName));

            unsigned int nOffset = m_pvnAttributeOffsets[nMesh][i];

            if (l_current_attribute != -1)
            {
                glVertexAttribPointer(l_current_attribute, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(nOffset * sizeof(GLfloat)));
                glEnableVertexAttribArray(l_current_attribute);
            }
        }

        // set texcoords pointers
        for (auto iter = m_vmTextureInformations[nMesh].begin(); iter != m_vmTextureInformations[nMesh].end(); iter++)
        {
            std::string sCurrentAttributeName(iter->first);

            sCurrentAttributeName.append("_coords");

            Logger::debug() << sCurrentAttributeName << Logger::endl;

            const GLint l_current_attribute (ItlGetGraphicCore()->GetShaderManager()->GetAttribute(sCurrentAttributeName));

            unsigned int nOffset = iter->second;

            if (l_current_attribute != -1)
            {
                glVertexAttribPointer(l_current_attribute, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(nOffset * sizeof(GLfloat)));
                glEnableVertexAttribArray(l_current_attribute);
            }
        }
    }

    pShaderManager->PopActiveShader();
}

void Bamboo::RN_Generic::ItlLoadShader()
{
    ItlGetGraphicCore()->GetShaderManager()->AddShader("deferred_pass", new Shader("BambooEngine/shaders/deferred_pass.vert", "BambooEngine/shaders/deferred_pass.frag"));
}

void Bamboo::RN_Generic::ItlPrepareTextures()
{
    TextureManager *pTextureManager = ItlGetGraphicCore()->GetTextureManager();
    assert (pTextureManager != NULL);

    assert (m_pnColorTexture == NULL);
    m_pnColorTexture    = new GLuint[m_nNumMeshes];
    m_pnNormalTexture   = new GLuint[m_nNumMeshes];
    m_pnSpecularTexture = new GLuint[m_nNumMeshes];
    m_pnDisplaceTexture = new GLuint[m_nNumMeshes];

    for (unsigned int nMesh = 0; nMesh < m_nNumMeshes; nMesh++)
    {
        std::shared_ptr<GeometryData::GenericMesh> spMesh(m_spObject->GetMesh(nMesh));

        GLuint * psTargetArrays [4] = { &m_pnColorTexture[nMesh], &m_pnNormalTexture[nMesh], &m_pnSpecularTexture[nMesh], &m_pnDisplaceTexture[nMesh] };
        bool bGammaCorrected[4] = { true, false, false, false };

        for (int i=0; i < 4; i++)
        {
            std::string sTexturePath = spMesh->GetTexturePath(tTextureTypes[i]);

            if (sTexturePath.empty() == false)
            {
                std::string sFullTexturePath = std::string("models/") + sTexturePath;
                pTextureManager->LoadTexture(*(psTargetArrays[i]), sFullTexturePath, bGammaCorrected[i]);
            }
        }
    }


}

void Bamboo::RN_Generic::ItlDeleteTextures()
{
    TextureManager *pTextureManager = ItlGetGraphicCore()->GetTextureManager();
    assert (pTextureManager != NULL);

    assert (m_pnColorTexture != NULL);
    delete[] m_pnColorTexture;
    m_pnColorTexture = NULL;

    assert (m_pnNormalTexture != NULL);
    delete[] m_pnNormalTexture;
    m_pnNormalTexture = NULL;

    assert (m_pnSpecularTexture != NULL);
    delete[] m_pnSpecularTexture;
    m_pnSpecularTexture = NULL;

    assert (m_pnDisplaceTexture != NULL);
    delete[] m_pnDisplaceTexture;
    m_pnDisplaceTexture = NULL;
}
