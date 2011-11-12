/*
* source file for Graphic class
* written by: christian moellinger <ch.moellinger@gmail.com>
* 10/2010
*/

#include <string>
#include <sstream>
#include <memory>

//#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common_gl.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Logger.h"
#include "RenderNodes/IRenderNode.h"
#include "RenderNodes/RenderNode_PostEffect.h"
#include "RenderNodes/RenderNode_RenderPass.h"
#include "RenderNodes/RenderNode_Camera.h"
#include "RenderNodes/RenderNode_FBO.h"
#include "RenderNodes/RenderNode_Cube.h"
#include "SceneObjects/ISceneObject.h"
#include "Graphic.h"
#include "Scene.h"
#include "Camera.h"

#include "Events.h"
#include "MainApp.h"

using namespace std;

#define PI 3.14159265f

// initialize static members
Bamboo * Bamboo::s_pInstance = NULL;

/****************************************************************
  *************************************************************** */
Bamboo::Bamboo()
{
    m_pShaderManager = new ShaderManager();
    m_pTextureManager = NULL;

    s_pInstance = this;
}

/****************************************************************
  *************************************************************** */
Bamboo::~Bamboo()
{
    assert (m_pShaderManager != NULL);
    if (m_pShaderManager != NULL)
        delete m_pShaderManager;
    m_pShaderManager = NULL;

    assert(m_pTextureManager != NULL);
    if (m_pTextureManager != NULL)
        delete m_pTextureManager;
    m_pTextureManager = NULL;
}

/****************************************************************
  *************************************************************** */
void Bamboo::Render()
{
    for (auto iter=m_mRenderLoops.begin(); iter != m_mRenderLoops.end(); iter++)
    {
        TItlRenderLoop *pRenderLoop = &(iter->second);

        pRenderLoop->spRenderTarget->ClearBuffers();
        pRenderLoop->spCamera->Move(0.0001f);
        pRenderLoop->spRenderGraph->Render();
        pRenderLoop->spRenderTarget->SwapBuffers();
    }
}


/****************************************************************
  *************************************************************** */
Bamboo::ShaderManager * Bamboo::GetShaderManager()
{
    // TODO: should we use the shader manager as a singelton or as a member of the graphics class?

    return m_pShaderManager;
}

/****************************************************************
  *************************************************************** */
Bamboo::TextureManager * Bamboo::GetTextureManager()
{
    if (m_pTextureManager == NULL)
    {
        m_pTextureManager = new TextureManager();
        m_pTextureManager->Initialize();
    }

    // TODO: should we use the texture manager as a singelton or as a member of the graphics class?
    return m_pTextureManager;
}

/****************************************************************
  *************************************************************** */
int Bamboo::AddRenderLoop(std::shared_ptr<Bamboo::IRenderTarget> spRenderTarget,
                             std::shared_ptr<Bamboo::ICamera> spCamera,
                             std::shared_ptr<Bamboo::Scene> spScene)
{
    static int iID = 0;

    TItlRenderLoop NewLoop;

    NewLoop.spRenderTarget = spRenderTarget;
    NewLoop.spCamera = spCamera;
    NewLoop.spScene = spScene;

    // build the render graph
    ItlBuildRenderGraph(NewLoop);

    m_mRenderLoops[iID++] = NewLoop;
}

/****************************************************************
  *************************************************************** */
void Bamboo::RemoveRenderLoop(int iLoopID)
{
    assert (m_mRenderLoops.find(iLoopID) != m_mRenderLoops.end());

    m_mRenderLoops.erase(iLoopID);
}


/****************************************************************
  *************************************************************** */
void Bamboo::ISceneObject::SetTransformMatrix(glm::mat4 mNewMatrix)
{
    GetRenderNode()->SetTransformMatrix(mNewMatrix);
}

/****************************************************************
  *************************************************************** */
std::shared_ptr<Bamboo::IRenderNode> Bamboo::ISceneObject::GetRenderNode()
{
    if (!m_spRenderNode)
        m_spRenderNode = CreateRenderNode();

    assert(m_spRenderNode);

    return m_spRenderNode;
}

/****************************************************************
  *************************************************************** */
void Bamboo::ItlBuildRenderGraph(Bamboo::TItlRenderLoop &tRenderLoop)
{
    tRenderLoop.spRenderGraph = std::shared_ptr<Bamboo::IRenderNode>(new Bamboo::RN_Camera(tRenderLoop.spCamera.get()));

    // load shader, if not loaded
    GetShaderManager()->AddShader("posteffect1", new Bamboo::Shader("BambooEngine/shaders/posteffect1.vs", "BambooEngine/shaders/posteffect1.fs"));

    static int a=0;

    std::string psSceneColorTextures[] = { "scene_color1", "scene_color2" };
    std::string psSceneDepthTextures[] = { "scene_depth1", "scene_depth2" };

    std::shared_ptr<Bamboo::IRenderNode> spAntiAliasFBO(new Bamboo::RN_FBO(2048, 768*2, psSceneColorTextures[a].c_str(), psSceneDepthTextures[a].c_str()));
    std::shared_ptr<Bamboo::RN_PostEffect> spAntiAliasPostEffect(new Bamboo::RN_PostEffect("posteffect1"));
    spAntiAliasPostEffect->SetTexture("texture1", psSceneColorTextures[a].c_str());
    spAntiAliasPostEffect->SetTexture("texture3",  psSceneDepthTextures[a].c_str());

    a++;

    assert (a <= 2);

    spAntiAliasPostEffect->AddChild(spAntiAliasFBO);

    for (auto iter=tRenderLoop.spScene->m_lSceneObjects.begin(); iter != tRenderLoop.spScene->m_lSceneObjects.end(); iter++)
    {
        std::shared_ptr<Bamboo::ISceneObject> spSceneObject = *iter;
        std::shared_ptr<Bamboo::IRenderNode> spRenderNode = spSceneObject->GetRenderNode();

        spRenderNode->SetGraphicCore(this);

        //spAntiAliasFBO->AddChild(spRenderNode);
        tRenderLoop.spRenderGraph->AddChild(spRenderNode);

    }

   // tRenderLoop.spRenderGraph->AddChild(spAntiAliasPostEffect);
}
