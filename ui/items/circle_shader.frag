#version 310 es
precision mediump float;

layout(location = 0) in highp vec2 qt_TexCoord0;

layout(std140, binding = 0) uniform Params {
    mat4 qt_Matrix;
    float qt_Opacity;
    lowp float lineWidth;
    lowp float smoothness;
    lowp vec4 color;
    lowp vec4 backgroundColor;
    lowp float value;
};

layout(location = 0) out lowp vec4 fragColor;

// draws a full ring in backgroundColor and a partial ring starting
// from top in clockwise direction with [value] percent in [color]

void main() {
    vec2 pos = qt_TexCoord0 - vec2(0.5, 0.5);
    float r = distance(pos, vec2(0.0, 0.0));
    float innerBound = smoothstep(0.5 - lineWidth - smoothness, 0.5 - lineWidth, r);
    float outerBound = 1.0 - smoothstep(0.5 - smoothness, 0.5, r);
    float M_PI = 3.1415926535897932384626433832795;
    float angle = mod(atan(pos.y, pos.x) + M_PI / 2.0, M_PI * 2.0);
    float isActive = step(angle / (2.0 * M_PI), value);
    fragColor = innerBound * outerBound * ((isActive*color) + ((1.0-isActive)*backgroundColor)) * qt_Opacity;
}
