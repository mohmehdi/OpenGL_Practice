#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_Cord;
layout(location = 2) in vec3 normal;


out vec2 v_TexCord;
out vec3 v_normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;


void main()
{
	vec3 w_pos = vec4(u_Model * vec4(position, 1.f)).xyz;
	v_TexCord = tex_Cord;
	v_normal = normal;
	gl_Position = u_Projection *u_View *u_Model  * vec4( position ,1.f) ;
};



#shader fragment
#version 330 core
layout(location=0) out vec4 color; 
uniform sampler2D u_Texture;
uniform vec3 u_lightColor ;

uniform vec3 lightDir;
uniform vec3 viewDir;

in vec2 v_TexCord; 
in vec3 v_normal;

void main()
{
vec3 h = normalize(lightDir + viewDir);
float diff =  abs(dot(v_normal, lightDir));
float nh =  dot(v_normal, h);
float spec = pow(nh, 48.0f);
vec4 texColor =texture(u_Texture,v_TexCord);
color =   vec4((texColor.xyz*diff + spec)*u_lightColor  ,1.0f) ; 
};
