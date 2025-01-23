#version 330 core

uniform vec2 u_resolution;
uniform float u_time;
// uniform sampler2D texture1;
vec3 light = normalize(vec3(sin(u_time), 1, cos(u_time)));
vec3 cam = vec3(0.f, 0.f, -200.f);

const float MAX_DIST = 10000.f;

struct elips {
    vec3 coord;
    vec3 mas;
    vec3 rot;
};
uniform elips elp;

float mod(vec3 v) {
    return sqrt(dot(v, v));
}

vec4 insec(vec3 cam, vec3 vec, elips elp) {
    vec3 m = cam - elp.coord;

    m /= elp.mas;
    vec3 v = normalize(vec/elp.mas);
    float h = dot(-m, v);
    float d = mod(m + h*v);
    if (d > 1.) return vec4(0, 0, 0, MAX_DIST + 1.f); 
    float s = sqrt(1 - d*d);
    if (mod(m) <= 1)
        return vec4(normalize(-(m + (h + s)*v)/elp.mas), 
                                 mod(v*(h + s)*elp.mas));
    return vec4(normalize((m + (h - s)*v)/elp.mas), mod(v*(h - s)*elp.mas));
}

float lightRef(vec3 v, vec3 n) {
    return pow(max(dot(reflect(v, n), light), 0.f), 6)*0.5 + max(dot(n, light), 0.)*0.5;
}

void main() {
    vec2 cd = (gl_FragCoord.xy*2.f / u_resolution) - vec2(1.f, 1.f);
    float k = u_resolution.x / u_resolution.y;

    // elips elp;
    // elp.coord = vec3(0, 0, 0);
    // elp.mas = vec3(100, 100, 100);
    // elp.rot = vec3(0, 0, 0);

    vec3 v = normalize(vec3(cd.x*k, cd.y, 1));

    vec4 ins = insec(cam, v, elp);
    if (ins.w < MAX_DIST) gl_FragColor = vec4(vec3(lightRef(v, ins.xyz)), 1.f);
    else gl_FragColor = vec4(0, 0, 0.1, 1);
}