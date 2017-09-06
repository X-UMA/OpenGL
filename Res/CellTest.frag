#version 410

layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;

uniform sampler2D colorSampler;

//ライトデータ(点光源)
struct PointLight{
	//座標(ワールド座標)
	vec4 position;
	//明るさ
	vec4 color;
};

//ライトの数
const int maxLightCount = 4;

//ライティングパラメータ
layout(std140) uniform LightData{
	//環境光
	vec4 ambientColor;
	//ライトのリスト
	PointLight light[maxLightCount];
}lightData;

out vec4 fragColor;
void main() {
	vec3 lightColor = lightData.ambientColor.rgb;
	for(int i = 0; i < maxLightCount; i++) {
		vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
		float lightPower = 1.0 / dot(lightVector,lightVector);
		float cosTheta = clamp(dot(vec3(0, 0, 1), normalize(lightVector)), 0, 1);
		lightColor += lightData.light[i].color.rgb * cosTheta * lightPower;
	}
	fragColor = inColor * texture(colorSampler,inTexCoord);
	fragColor.rgb *= lightColor;
}