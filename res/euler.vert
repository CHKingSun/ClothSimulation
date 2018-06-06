#version 330 core

const vec3 gravity = vec3(0.f, -9.8f, 0.f);

uniform int size; //cloth scale

uniform float mass; //1.f
uniform float a_resistance; //-0.125f
uniform vec3 f_wind; //vec3(0.f, 0.f, 0.f)

uniform float ks; //15.f
uniform float kd; //0.9f
uniform float rest_length; //1.f
uniform float diag_length; //sqrt(2) * rest_length

uniform float delta_time;

uniform vec3 u_position;
//for collision detection

uniform isamplerBuffer constraints_tbo;
uniform samplerBuffer velocities_tbo;
uniform samplerBuffer vertices_tbo;

layout(location = 0) out vec3 o_velocity;
layout(location = 1) out vec3 o_vertex;

bool getSpringMsg(inout int index, inout float r_length) {
    // p - p - 8 - p - p
    // |   |   |   |   |
    // p - 0 - 1 - 2 - p
    // |   |   |   |   |
    // 9 - 3 - p - 4 - 10
    // |   |   |   |   |
    // p - 5 - 6 - 7 - p
    // |   |   |   |   |
    // p - p - 11 - p - p

    int i = gl_VertexID / size;
    int j = gl_VertexID % size;
    switch(index) {
    // case 0:
    //     return false;
    //     if(i == 0 || j == 0) return false;
    //     index = gl_VertexID - size - 1;
    //     r_length = diag_length;
    //     return true;
    // case 2:
    //     if(i == 0 || j == size - 1) return false;
    //     index = gl_VertexID - size + 1;
    //     r_length = diag_length;
    //     return true;
    // case 5:
    //     if(i == size - 1 || j == 0) return false;
    //     index = gl_VertexID + size - 1;
    //     r_length = diag_length;
    //     return true;
    // case 7:
    //     if(i == size - 1 || j == size - 1) return false;
    //     index = gl_VertexID + size + 1;
    //     r_length = diag_length;
    //     return true;

    case 1:
        if(i == 0) return false;
        index = gl_VertexID - size;
        r_length = rest_length;
        return true;
    case 3:
        if(j == 0) return false;
        index = gl_VertexID - 1;
        r_length = rest_length;
        return true;
    case 4:
        if(j == size - 1) return false;
        index = gl_VertexID + 1;
        r_length = rest_length;
        return true;

    case 6:
        if(i == size - 1) return false;
        index = gl_VertexID + size;
        r_length = rest_length;
        return true;

    case 8:
        if(i < 2) return false;
        index = gl_VertexID - size * 2;
        r_length = rest_length * 2;
        return true;
    case 9:
        if(j < 2) return false;
        index = gl_VertexID - 2;
        r_length = rest_length * 2;
        return true;
    case 10:
        if(j >= size - 2) return false;
        index = gl_VertexID + 2;
        r_length = rest_length * 2;
        return true;
    case 11:
        if(i >= size - 2) return false;
        index = gl_VertexID + size * 2;
        r_length = rest_length * 2;
        return true;
    }
    return false;
}

vec3 calAirForce(vec3 velocity) {
    if(velocity == vec3(0.f)) return vec3(0.f);

    // return vec3(0.f);
    // return a_resistance * velocity;
    return (a_resistance * dot(velocity, velocity)) * normalize(velocity);
}

void main() {
    //gl_VertexID: save the index of current vertex
    vec3 p0 = texelFetch(vertices_tbo, gl_VertexID).xyz;
    vec3 v0 = texelFetch(velocities_tbo, gl_VertexID).xyz;
    bool constraint = texelFetch(constraints_tbo, gl_VertexID / 4)[gl_VertexID % 4] != 0;

    if(constraint) {
        o_vertex = p0;
        o_velocity = vec3(0.f);
        return;
    }

    if(delta_time == 0.f) {
        o_vertex = p0;
        o_velocity = v0;
        return;
    }

    vec3 acceleration = vec3(0.f);
    if(mass != 0.f) {
        acceleration = mass * gravity + f_wind + calAirForce(v0);
        for(int i = 0; i < 12; ++i) {
            int index = i;
            float r_length;
            if(getSpringMsg(index, r_length)) {
                vec3 p1 = texelFetch(vertices_tbo, index).xyz;
                vec3 v1 = texelFetch(velocities_tbo, index).xyz;
                float delta_length = distance(p0, p1) - r_length;
                if(delta_length <= 0.f) continue;
                vec3 deltaP = normalize(p0 - p1);
                acceleration += -(ks * delta_length + kd * dot(deltaP, v0 - v1)) * deltaP;
            }
        }
        acceleration /= mass;
    }

    if(p0.y + u_position.y <= 0.00072f) {
        if(acceleration.y < 0.f) acceleration.y = 0.f;
        if(v0.y < 0.f) v0.y = 0.f;
        p0.y = 0.00072f - u_position.y; 
    }
    o_velocity = v0 + acceleration * delta_time;
    o_vertex = p0 + (o_velocity + v0) * (delta_time / 2.f);
}