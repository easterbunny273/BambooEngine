/*
 * fragment shader for parallax lighting
 *
 *
 * written by:
 * christian moellinger <ch.moellinger@gmail.com>
 * 11/2010
 */

#version 330 core

layout (location = 0) out vec4 out_Albedo;
layout (location = 1) out vec4 out_Normal;
layout (location = 2) out vec4 out_Specular;

in vec3 my_Texcoord;
in vec3 my_LightDir;
in vec3 my_ObjPosition;
in vec3 my_EyeDir;
in vec3 my_Normal;
in vec4 my_ScreenPosition;
in vec3 my_Color;

uniform sampler2D color_texture;

void main(void)
{
   //out_Color = vec4(texture(color_texture, my_Texcoord.xy).rgb, 1.0);
   out_Albedo = vec4(my_Color, 1.0);
   out_Normal = vec4(my_Normal, 1.0);
   out_Specular = vec4(my_Normal, 1.0);

}
