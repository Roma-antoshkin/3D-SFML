#version 330 core

uniform vec2 u_resolution;
uniform float u_time;
// uniform sampler2D texture1;
vec3 light = normalize(vec3(1., 1., -0.5));
vec3 cam = vec3(0.f, 0.f, -350.f);

const float MAX_DIST = 10000.f;

struct elips {
    vec3 coord;
    vec3 mas;
    mat3 rot;
    mat3 unrot;
};
uniform elips elp;

struct box {
    vec3 coord;
    vec3 mas;
    mat3 rot;
    mat3 unrot;
};
uniform box bx;

// mat3 rotateMatrix(vec3 r) {
//     mat3(
//         1.f, 0.f, 0.f, 
//         0.f, cos(r.x), -sin(r.x),
//         0.f, sin(r.x), cos(r.x)
//     ) * mat3(
//         cos(r.y), 0.f, sin(r.y),
//         0.f, 1.f, 0.f,
//         -sin(r.y), 0, cos(r.y)
//     ) * return mat3(
//         cos(r.z), -sin(r.z), 0.f,
//         sin(r.z), cos(r.z), 0.f,
//         0.f, 0.f, 1.f
//     );
// }

float mod(vec3 v) {
    return sqrt(dot(v, v));
}

// Пересечение с эллипсоидом
vec4 insecElips(vec3 m, vec3 v, elips elp) {
    m = m - elp.coord;
    m = elp.unrot*m;
    v = elp.unrot*v;

    m /= elp.mas;
    v = normalize(v/elp.mas);
    float h = dot(-m, v);
    float d = mod(m + h*v);
    if (d > 1.) return vec4(0, 0, 0, MAX_DIST + 1.f); 
    float s = sqrt(1 - d*d);
    if (mod(m) <= 1)
        return vec4(normalize(elp.rot*(-(m + (h + s)*v)/elp.mas)), 
                                 mod(v*(h + s)*elp.mas));
    return vec4(normalize(elp.rot*((m + (h - s)*v)/elp.mas)), mod(v*(h - s)*elp.mas));
}

vec4 insecBox(vec3 m, vec3 v, box bx) {
    m = m - bx.coord;
    m = bx.unrot*m;
    v = bx.unrot*v;

    m /= bx.mas;
    v = normalize(v/bx.mas);

    vec3 a = 1.0 / v;
	vec3 n = a * m;
	vec3 k = abs(a);
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec4(0., 0., 0., MAX_DIST + 1.);
	return vec4(bx.rot*(-sign(v) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz)), mod(v*tN*bx.mas));
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

    vec3 v = normalize(vec3(cd.x, cd.y/k, 1));

    vec4 insElp = insecElips(cam, v, elp);
    vec4 insBox = insecBox(cam, v, bx);
    vec4 ins = insElp.w < insBox.w ? insElp : insBox;
    if (ins.w < MAX_DIST) gl_FragColor = vec4(vec3(lightRef(v, ins.xyz)), 1.f);
    else gl_FragColor = vec4(0, 0, 0.1, 1);
}