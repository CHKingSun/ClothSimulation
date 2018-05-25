#version 330 core

struct Texture {
    sampler2D tex;
    bool enable;
};

struct Light{ //Point light
    bool enable;

    float factor;

    vec3 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float kc, kl, kq;
};

uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess;

uniform Texture u_texture;

uniform Light u_light;

in vec3 v_N;
in vec3 v_E;
in vec3 v_mPos;
in vec2 v_texcoord;

out vec4 fragColor;

vec4 calLight(Light l, vec3 N, vec3 E, vec3 m_pos) { //N and E are normalized;
    vec3 l_dir = l.position - m_pos;
    float dis = length(l_dir);
    float attenuation = 1.0 / (l.kc + dis * l.kl + dis * dis * l.kq);
    vec3 L = normalize(l_dir);
    float cosT = max(dot(L, N), 0.0);
    float cosA = 0.0;
    if(cosT != 0.0) cosA = max(dot(N, normalize(L + E)), 0.0);

    vec4 color = vec4(0.0);
    attenuation *= l.factor;
    color += l.ambient * attenuation * u_ambient;
    color += l.diffuse * cosT * attenuation * u_diffuse;
    color += l.specular * pow(cosA, u_shininess) * attenuation * u_specular;
    return color;
}

void main() {

    fragColor = vec4(0.0);

    if(u_light.enable) fragColor = calLight(u_light, v_N, v_E, v_mPos);
    else fragColor = u_ambient;

    if(u_texture.enable) fragColor *= texture(u_texture.tex, v_texcoord);
    else fragColor *= vec4(0.8, 0.8, 0.8, 1.0);
}
