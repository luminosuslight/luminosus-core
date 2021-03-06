#ifdef GL_ES
precision mediump float;
#endif
varying highp vec2 qt_TexCoord0;
uniform lowp float qt_Opacity;
uniform lowp float lineWidth;
uniform lowp float smoothness;
uniform lowp vec4 color;
uniform lowp vec4 backgroundColor;
uniform lowp float value;

// draws a horeshoe shaped ring in backgroundColor and a partial ring starting
// from bottom left in clockwise direction with [value] percent in [color]

void main() {
    vec2 pos = qt_TexCoord0 - vec2(0.5, 0.5);
    float r = distance(pos, vec2(0.0, 0.0));
    float innerBound = smoothstep(0.5 - lineWidth - smoothness, 0.5 - lineWidth, r);
    float outerBound = 1.0 - smoothstep(0.5 - smoothness, 0.5, r);
    float M_PI = 3.1415926535897932384626433832795;
    float angle = mod(atan(pos.y, pos.x) - M_PI * 0.7, M_PI * 2.0);
    float active = step(angle / (2.0 * M_PI), value * 0.8);
    float ringBackground = step(angle / (2.0 * M_PI), 0.8);
    gl_FragColor = innerBound * outerBound * ((active*color) + (ringBackground*(1.0-active)*backgroundColor)) * qt_Opacity;
}
