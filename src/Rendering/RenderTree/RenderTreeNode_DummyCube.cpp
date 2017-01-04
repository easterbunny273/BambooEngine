#include "RenderTreeNode_DummyCube.h"
#include "../RenderContext.h"
#include <GL/glew.h>

void RenderTreeNode_DummyCube::_preRenderSubNodes(RenderContext &context)
{
	// nothing to do
}

void RenderTreeNode_DummyCube::_postRenderSubNodes(RenderContext &context)
{
	auto time = context.getTimeFromStart();

	glLoadIdentity();
	glRotatef(360.0f / 1000.0f * float(time), 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();
}
