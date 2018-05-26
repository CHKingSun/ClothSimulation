#version 330 core

layout(location = 1) in vec3 a_position;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec2 a_texcoord;

uniform vec3 u_mPos;
uniform vec3 u_mScale;
uniform mat3 u_mRotate;
//model_view * vec4(position, 1.0) = u_mPos + (u_mRotate * (u_mScale * position));
//u_NMatrix * normal = u_mRotate * (u_mScale * normal);

uniform vec3 p_eye;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 v_N;
out vec3 v_E;
out vec3 v_mPos;
out vec2 v_texcoord;

void main() {
    vec3 m_pos = u_mPos + (u_mRotate * (u_mScale * a_position));
    gl_Position = u_proj * (u_view * vec4(m_pos, 1.0));
    
    if(a_normal == vec3(0.0f)) v_N = a_normal;
    else {
        v_N = normalize(u_mRotate * (u_mScale * a_normal));
        v_E = normalize(p_eye - m_pos);
        v_mPos = m_pos;
    }
    v_texcoord = a_texcoord;
}
