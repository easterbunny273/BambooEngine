#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.1415f

/****************************************************************
  ** Nestes class Graphic::ICamera **
  *************************************************************** */
Bamboo::ICamera::ICamera()
{
    m_bInitialized = false;
    m_m4ProjectionMatrix = glm::mat4();

    m_fRotationHorizontal = 160;
    m_fRotationVertical = -45;
    m_v3CameraPosition = glm::vec3(0.0f, 10.0f, 0.0f);
    m_v3MoveVector = glm::vec3(0.0f, 0.0f, 0.0f);


    // register for camera movement events
//    EventManager *pEventManager = MainApp::GetInstance()->GetEventManager();
//    assert (pEventManager != NULL);

//    pEventManager->RegisterEventListener(this, CameraMovementEvent::EventType());
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::SetOrthoProjection(float fLeft,
                                         float fRight,
                                         float fTop,
                                         float fBottom,
                                         float fNear,
                                         float fFar)
{
    assert(!"not implemented yet");
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::SetPerspectiveProjection(float fFieldOfView,
                                               float fAspectRatio,
                                               float fNearPlane,
                                               float fFarPlane)
{
    m_m4ProjectionMatrix = glm::perspective(fFieldOfView, fAspectRatio, fNearPlane, fFarPlane);

    m_bInitialized = true;
}

/****************************************************************
  *************************************************************** */
glm::mat4 Bamboo::ICamera::GetProjectionMatrix() const
{
    assert (m_bInitialized);

    return m_m4ProjectionMatrix;
}

/****************************************************************
  *************************************************************** */
glm::mat4 Bamboo::ICamera::GetViewMatrix() const
{
    return m_m4ViewMatrix;
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::RotateVertical(float fValue)
{
    // lock vertical rotation to ]-90, 90[ because else
    // the view vector changes the direction if it exceeds 90 or -90

    m_fRotationVertical += fValue;

    if (m_fRotationVertical < -89.99f)
        m_fRotationVertical = -89.99f;
    else
    if (m_fRotationVertical > 89.99f)
        m_fRotationVertical = 89.99f;
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::RotateHorizontal(float fValue)
{
    m_fRotationHorizontal += fValue;

    while (m_fRotationHorizontal < 0.0f)
          m_fRotationHorizontal += 360.0f;

    while (m_fRotationHorizontal >= 360.0f)
        m_fRotationHorizontal -= 360.0f;
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::Move(float fFactor)
{
    //Logger::debug() << m_fRotationHorizontal << ":" << m_fRotationVertical << Logger::endl;

    glm::vec3 v3MoveX;
    glm::vec3 v3MoveZ;

    if (m_v3MoveVector.x != 0)
    {
        //if camera should move in x-direction, we must calculate the look-at point
        //on the camera-sphere and use this vector as reference, and move normal to this vector

        v3MoveX.x = sin(((m_fRotationHorizontal+270) / 180.0) * PI);
        v3MoveX.y = 0.0;
        v3MoveX.z = cos(((m_fRotationHorizontal+270) / 180.0) * PI);

        //normalize to project point on our sphere
        v3MoveX /= sqrt(v3MoveX.x*v3MoveX.x + v3MoveX.y*v3MoveX.y + v3MoveX.z*v3MoveX.z);

        //and multiplicate with given x-factor to adjust movement length
        v3MoveX *= m_v3MoveVector.x;
    }

    if (m_v3MoveVector.y != 0)
    {
        //if camera should move in y-direction, we can use the given value directly

        v3MoveX.y = m_v3MoveVector.y;
    }

    if (m_v3MoveVector.z != 0)
    {
        //if camera should move in z-direction, we must calculate the look-at point
        //on the camera-sphere and use this vector as reference, and move along this vector

        v3MoveZ.x = sin((m_fRotationHorizontal / 180.0) * PI);
        v3MoveZ.y = tan((m_fRotationVertical / 180.0) * PI);
        v3MoveZ.z = cos((m_fRotationHorizontal / 180.0) * PI);

        //normalize to project point on our sphere
        v3MoveZ /= sqrt(v3MoveZ.x*v3MoveZ.x + v3MoveZ.y*v3MoveZ.y + v3MoveZ.z*v3MoveZ.z);

        //and multiplicate with given z-factor to adjust movement length
        v3MoveZ *= m_v3MoveVector.z;
    }

    m_v3CameraPosition += (v3MoveX + v3MoveZ) * fFactor;

    glm::vec3 v3LookAt;

    v3LookAt.x = sin(((m_fRotationHorizontal) / 180.0) * PI);
    v3LookAt.y = tan((m_fRotationVertical / 180.0) * PI);
    v3LookAt.z = cos(((m_fRotationHorizontal) / 180.0) * PI);

    m_m4ViewMatrix = glm::lookAt(m_v3CameraPosition, m_v3CameraPosition + v3LookAt, glm::vec3(0,1,0));
}

/****************************************************************
  *************************************************************** */
void Bamboo::ICamera::AddToMoveVector(glm::vec3 vVector)
{
    m_v3MoveVector += vVector;
}

std::shared_ptr<Bamboo::ICamera> Bamboo::PerspectiveCamera::Create(float fFieldOfView,
                                                                  float fAspectRatio,
                                                                  float fNearPlane,
                                                                  float fFarPlane,
                                                                  glm::vec3 vInitialPosition,
                                                                  float fInitialRotationX,
                                                                  float fInitialRotationY)
{
    Bamboo::PerspectiveCamera *pCamera = new Bamboo::PerspectiveCamera();

    pCamera->SetPerspectiveProjection(fFieldOfView, fAspectRatio, fNearPlane, fFarPlane);
    pCamera->m_v3CameraPosition = vInitialPosition;
    pCamera->m_fRotationHorizontal = fInitialRotationX;
    pCamera->m_fRotationVertical = fInitialRotationY;

    return std::shared_ptr<Bamboo::ICamera>(pCamera);
}
