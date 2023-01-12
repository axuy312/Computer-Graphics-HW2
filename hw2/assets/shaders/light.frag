#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {

    vec3 lighting = (material.ambient + material.diffuse + material.specular);

    if (dl.enable == 1)
    {
        vec3 ambient = material.ambient;

        vec3 lightDir = normalize(-dl.direction); // 翻轉方向光源的方向
        float diffFactor = max(dot(lightDir, Normal), 0.0);
        vec3 diffuse = diffFactor * material.diffuse * vec3(texture(ourTexture, TexCoord));
        
        float specularStrength = 0.5f;
        vec3 reflectDir = normalize(reflect(-lightDir, Normal));
        vec3 viewDir = normalize(viewPos - FragPos);
        float specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = specFactor * material.specular * vec3(texture(ourTexture, TexCoord));
        
        lighting = lighting  * (ambient + diffuse + specular) * dl.lightColor;
    }
    if (pl.enable == 1)
    {   
        vec3 ambient = material.ambient;

        vec3 lightDir = normalize(FragPos-pl.position); // 翻轉方向光源的方向
        float diffFactor = max(dot(lightDir, Normal), 0.0);
        vec3 diffuse = diffFactor * material.diffuse * vec3(texture(ourTexture, TexCoord));
        
        float specularStrength = 0.9f;
        vec3 reflectDir = normalize(reflect(-lightDir, Normal));
        vec3 viewDir = normalize(viewPos - FragPos);
        float specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = specFactor * material.specular * vec3(texture(ourTexture, TexCoord));

        float distance = length(pl.position - FragPos); // 在世界坐標系中計算距離
        float attenuation = 1.0f / (pl.constant 
                                    + pl.linear * distance
                                    + pl.quadratic * distance * distance);
        lighting = lighting  * (ambient + diffuse + specular) * attenuation * pl.lightColor;
    }
    if (sl.enable == 1)
    {
        vec3 ambient = material.ambient;

        vec3 lightDir = normalize(-sl.direction); // 翻轉方向光源的方向
        float diffFactor = max(dot(lightDir, Normal), 0.0);
        vec3 diffuse = diffFactor * material.diffuse * vec3(texture(ourTexture, TexCoord));
        
        float specularStrength = 0.9f;
        vec3 reflectDir = normalize(reflect(-lightDir, Normal));
        vec3 viewDir = normalize(viewPos - FragPos);
        float specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = specFactor * material.specular * vec3(texture(ourTexture, TexCoord));
        
        float distance = length(sl.position - FragPos); // 在世界坐標系中計算距離
        float attenuation = 1.0f / (sl.constant 
                                    + sl.linear * distance
                                    + sl.quadratic * distance * distance);

        // 環境光成分
        lightDir = normalize(sl.position - FragPos);
        // 光線與聚光燈夾角餘弦值
        float theta = dot(lightDir,normalize(-sl.direction));
        if(theta > sl.cutOff)    
        {
            // 在聚光燈張角範圍內 計算漫反射光成分 鏡面反射成分
            lighting = lighting  * (ambient + diffuse + specular) * attenuation * sl.lightColor;
        }
        else
        {
            // 不在張角範圍內時只有環境光成分
            lighting = lighting  * (ambient) * attenuation * sl.lightColor;
        }

    }
    color = vec4(vec3(texture(ourTexture, TexCoord)) * lighting, 1.0);

}
