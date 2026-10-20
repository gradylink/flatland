#version 330
#extension GL_ARB_separate_shader_objects : require

uniform sampler2D InSampler;

layout(location = 0) in vec2 texCoord;

layout(std140) uniform SamplerInfo {
    vec2 OutSize;
    vec2 InSize;
};

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 sceneColor = texture(InSampler, texCoord);

    float viewportHalfWidth = __FLATLAND_VIEWPORT_HALF_WIDTH__;
    float viewportHalfHeight = __FLATLAND_VIEWPORT_HALF_HEIGHT__;
    float viewportAspect = viewportHalfWidth / viewportHalfHeight;
    float screenAspect = OutSize.x / OutSize.y;

    float halfWidth;
    float halfHeight;
    if (screenAspect >= viewportAspect) {
        halfHeight = viewportHalfHeight;
        halfWidth = halfHeight * screenAspect;
    } else {
        halfWidth = viewportHalfWidth;
        halfHeight = halfWidth / screenAspect;
    }

    vec2 ndc = texCoord * 2.0 - 1.0;
    bool outsideViewport = abs(ndc.x) > viewportHalfWidth / halfWidth || abs(ndc.y) > viewportHalfHeight / halfHeight;

    fragColor = outsideViewport ? vec4(0.0, 0.0, 0.0, 1.0) : sceneColor;
}
