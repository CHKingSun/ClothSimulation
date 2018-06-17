#version 330 core

const float EXPSION = 0.00072; //deal with Z fighting

uniform ivec2 size;
uniform vec2 rest_length;
uniform float diag_length;

uniform float ks;
uniform float kd;
uniform float ks_bend;
uniform float kd_bend;

uniform float mass;
uniform vec3 gravity;
uniform float a_resistance;
uniform vec3 f_wind;

uniform float delta_time;

uniform vec3 u_position;
uniform vec3 s_center;
uniform float s_radius;

uniform isamplerBuffer constraints_tbo;
uniform samplerBuffer last_vertices_tbo;
uniform samplerBuffer vertices_tbo;

out vec3 o_last_vertex;
out vec3 o_vertex;
out vec3 o_point;

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

    int i = gl_VertexID / size.x;
    int j = gl_VertexID % size.x;
    switch(index) {
    case 0:
        if(i == 0 || j == 0) return false;
        index = gl_VertexID - size.x - 1;
        r_length = diag_length;
        return true;
    case 2:
        if(i == 0 || j == size.x - 1) return false;
        index = gl_VertexID - size.x + 1;
        r_length = diag_length;
        return true;
    case 5:
        if(i == size.y - 1 || j == 0) return false;
        index = gl_VertexID + size.x - 1;
        r_length = diag_length;
        return true;
    case 7:
        if(i == size.y - 1 || j == size.x - 1) return false;
        index = gl_VertexID + size.x + 1;
        r_length = diag_length;
        return true;

    case 1:
        if(i == 0) return false;
        index = gl_VertexID - size.x;
        r_length = rest_length.y;
        return true;
    case 3:
        if(j == 0) return false;
        index = gl_VertexID - 1;
        r_length = rest_length.x;
        return true;
    case 4:
        if(j == size.x - 1) return false;
        index = gl_VertexID + 1;
        r_length = rest_length.x;
        return true;
    case 6:
        if(i == size.y - 1) return false;
        index = gl_VertexID + size.x;
        r_length = rest_length.y;
        return true;

    case 8:
        if(i < 2) return false;
        index = gl_VertexID - size.x * 2;
        r_length = rest_length.y * 2;
        return true;
    case 9:
        if(j < 2) return false;
        index = gl_VertexID - 2;
        r_length = rest_length.x * 2;
        return true;
    case 10:
        if(j >= size.x - 2) return false;
        index = gl_VertexID + 2;
        r_length = rest_length.x * 2;
        return true;
    case 11:
        if(i >= size.y - 2) return false;
        index = gl_VertexID + size.x * 2;
        r_length = rest_length.y * 2;
        return true;
    }
    return false;
}

void dealCollision() {
    o_vertex += u_position;
    if(distance(o_vertex, s_center) <= s_radius) {
        o_vertex = normalize(o_vertex - s_center) * (s_radius + EXPSION) + s_center;
        o_last_vertex = o_vertex - u_position;
    }
    if(o_vertex.y < EXPSION) {
        o_vertex.y = EXPSION;
    }
    o_vertex -= u_position;
}

vec3 calAirForce(vec3 velocity) {
    if(velocity == vec3(0.f)) return vec3(0.f);

    // return vec3(0.f);
    // return a_resistance * velocity;
    return (a_resistance * length(velocity)) * velocity;
}

void main() {
    //gl_VertexID: save the index of current vertex
    vec3 last_p = texelFetch(last_vertices_tbo, gl_VertexID).xyz;
    vec3 now_p = texelFetch(vertices_tbo, gl_VertexID).xyz;
    bool constraint = texelFetch(constraints_tbo, gl_VertexID / 4)[gl_VertexID % 4] != 0;

    if(constraint) {
        o_last_vertex = now_p;
        o_vertex = now_p;
        o_point = o_vertex;
        return;
    }

    if(delta_time == 0.f) {
        o_last_vertex = last_p;
        o_vertex = now_p;
        o_point = o_vertex;
        return;
    }

    vec3 delta_p = now_p - last_p;
    vec3 acceleration = vec3(0.f);
    if(mass != 0.f) {
        vec3 vel = delta_p / delta_time;
        acceleration = mass * gravity + f_wind + calAirForce(vel);
        for(int i = 0; i < 12; ++i) {
            int index = i;
            float r_length;
            if(getSpringMsg(index, r_length)) {
                vec3 n_last_p = texelFetch(last_vertices_tbo, index).xyz;
                vec3 n_now_p = texelFetch(vertices_tbo, index).xyz;
                vec3 dp = now_p - n_now_p;
                float delta_length = length(dp);
                if(delta_length - r_length <= 0.f) continue;

                vec3 n_vel = (n_now_p - n_last_p) / delta_time;
                if(r_length != diag_length) 
                    acceleration += -(ks * (delta_length - r_length) +
                        kd * dot(dp, vel - n_vel) / delta_length) * normalize(dp);
                else acceleration += -(ks_bend * (delta_length - r_length) +
                        kd_bend * dot(dp, vel - n_vel) / delta_length) * normalize(dp);

            }
        }
        acceleration /= mass;
    }

    o_last_vertex = now_p;
    o_vertex = now_p + delta_p + acceleration * delta_time * delta_time;

    dealCollision();
    o_point = o_vertex;
}