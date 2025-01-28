#version 330 core

uniform vec2 u_resolution;
uniform float u_time;
// uniform vec3 light;
struct camera {
    vec3 coord;
    mat3 rot;
    mat3 unrot;
};
uniform camera cam;
// uniform sampler2D texture1;
vec3 light = normalize(vec3(1., 1., 1.));
vec3 lightCol = vec3(0.95, 0.92, 0.8);

vec3 skycol = vec3(115./255., 186./255, 255./255);
vec3 shadCol = vec3(0.05, 0.08, 0.1);

const float MAX_DIST = 10000.f;

struct elips {
    vec3 coord;
    vec3 mas;
    mat3 rot;
    mat3 unrot;
    vec4 color;
};
uniform elips elp;

struct box {
    vec3 coord;
    vec3 mas;
    mat3 rot;
    mat3 unrot;
    vec4 color;
};
uniform box bx;
struct plane {
    vec4 coord, color;
};
uniform plane pln;

vec4 insPlane;
vec4 insElp;
vec4 insBox;

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
    float h = abs(dot(-m, v));
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

vec4 insecPlane(vec3 m, vec3 v, plane pl) {
    float r = dot(vec4(m, 1.), pl.coord);
    float d = -r/dot(v, pl.coord.xyz);
    pl.coord.w = d < 0 ? MAX_DIST + 1 : d;
    return pl.coord;
}

float sunning(vec3 v) {
    return pow(max(dot(reflect(v, light), -light), 0.f), 32)*0.5;
}

float lightRef(vec3 v, vec3 n) {
    return pow(max(dot(reflect(v, n), light), 0.f), 6)*0.5 + max(dot(n, light), 0.)*0.5;
}

float shading(vec3 v, vec3 m, vec4 ins) {
    vec3 pos = v*ins.w + m;
    vec4 Pln = insecPlane(pos, light, pln);
    vec4 Elp = insecElips(pos, light, elp);
    vec4 Box = insecBox(pos, light, bx);
    if (ins.w < insPlane.w && Pln.w < MAX_DIST ||
        ins.w < insElp.w && Elp.w < MAX_DIST ||
        ins.w < insBox.w && Box.w < MAX_DIST) return 0.*lightRef(v, ins.xyz);
    return lightRef(v, ins.xyz);
}

vec3 castRay(vec3 v) {
    vec3 col = vec3(1., 1., 1.);
    vec4 ins = vec4(0., 0., 0., MAX_DIST + 1);
    insPlane = insecPlane(cam.coord, v, pln);
    insElp = insecElips(cam.coord, v, elp);
    insBox = insecBox(cam.coord, v, bx);
    if(insElp.w < insBox.w) {
        ins = insElp;
        col = elp.color.rgb;
    }
    else {
        ins = insBox;
        col = bx.color.rgb;
    }
    if (ins.w > insPlane.w) {
        ins = insPlane;
        col = pln.color.rgb;
    }

    float mult = shading(v, cam.coord, ins);
    
    if (ins.w < MAX_DIST) col = col.rgb*(mult*lightCol + shadCol);
    else col = skycol + sunning(v)*lightCol;
    col.x = pow(col.x, 0.45);
    col.y = pow(col.y, 0.45);
    col.z = pow(col.z, 0.45);
    return col;
}

void main() {
    vec2 cd = (gl_FragCoord.xy*2.f / u_resolution) - vec2(1.f, 1.f);
    float k = u_resolution.x / u_resolution.y;

    vec3 v = cam.rot*normalize(vec3(-1., cd.x, cd.y/k));

    gl_FragColor = vec4(vec3(castRay(v)), 1.);
}