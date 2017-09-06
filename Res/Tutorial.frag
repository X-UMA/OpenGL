#version 410

layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;
layout(location=3) in vec3 inWorldNormal;

uniform sampler2D colorSampler;

//���C�g�f�[�^(�_����)
struct PointLight{
	//���W(���[���h���W)
	vec4 position;
	//���邳
	vec4 color;
};

//���C�g�̐�
const int maxLightCount = 4;

//���C�e�B���O�p�����[�^
layout(std140) uniform LightData{
	//����
	vec4 ambientColor;
	//���C�g�̃��X�g
	PointLight light[maxLightCount];
}lightData;

out vec4 fragColor;
void main() {
	vec3 lightColor = lightData.ambientColor.rgb;
	for(int i = 0; i < maxLightCount; i++) {
		vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
		float lightPower = 1.0 / dot(lightVector,lightVector);
		float cosTheta = clamp(dot(inWorldNormal, normalize(lightVector)), 0, 1);
		lightColor += lightData.light[i].color.rgb * cosTheta * lightPower;
	}

	fragColor = vec4((inColor * texture(colorSampler,inTexCoord)).rgb, 1);
	fragColor.rgb *= lightColor;
	//fragColor.rgb = vec3(inTexCoord.r,inTexCoord.g,0);
	//fragColor = texture(colorSampler,inTexCoord);
}