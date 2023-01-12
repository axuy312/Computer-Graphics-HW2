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

        vec3 lightDir = normalize(-dl.direction); // ½���V��������V
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

        vec3 lightDir = normalize(FragPos-pl.position); // ½���V��������V
        float diffFactor = max(dot(lightDir, Normal), 0.0);
        vec3 diffuse = diffFactor * material.diffuse * vec3(texture(ourTexture, TexCoord));
        
        float specularStrength = 0.9f;
        vec3 reflectDir = normalize(reflect(-lightDir, Normal));
        vec3 viewDir = normalize(viewPos - FragPos);
        float specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = specFactor * material.specular * vec3(texture(ourTexture, TexCoord));

        float distance = length(pl.position - FragPos); // �b�@�ɧ��Шt���p��Z��
        float attenuation = 1.0f / (pl.constant 
                                    + pl.linear * distance
                                    + pl.quadratic * distance * distance);
        lighting = lighting  * (ambient + diffuse + specular) * attenuation * pl.lightColor;
    }
    if (sl.enable == 1)
    {
        vec3 ambient = material.ambient;

        vec3 lightDir = normalize(-sl.direction); // ½���V��������V
        float diffFactor = max(dot(lightDir, Normal), 0.0);
        vec3 diffuse = diffFactor * material.diffuse * vec3(texture(ourTexture, TexCoord));
        
        float specularStrength = 0.9f;
        vec3 reflectDir = normalize(reflect(-lightDir, Normal));
        vec3 viewDir = normalize(viewPos - FragPos);
        float specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = specFactor * material.specular * vec3(texture(ourTexture, TexCoord));
        
        float distance = length(sl.position - FragPos); // �b�@�ɧ��Шt���p��Z��
        float attenuation = 1.0f / (sl.constant 
                                    + sl.linear * distance
                                    + sl.quadratic * distance * distance);

        // ���ҥ�����
        lightDir = normalize(sl.position - FragPos);
        // ���u�P�E���O�����l����
        float theta = dot(lightDir,normalize(-sl.direction));
        if(theta > sl.cutOff)    
        {
            // �b�E���O�i���d�� �p�⺩�Ϯg������ �譱�Ϯg����
            lighting = lighting  * (ambient + diffuse + specular) * attenuation * sl.lightColor;
        }
        else
        {
            // ���b�i���d�򤺮ɥu�����ҥ�����
            lighting = lighting  * (ambient) * attenuation * sl.lightColor;
        }

    }
    color = vec4(vec3(texture(ourTexture, TexCoord)) * lighting, 1.0);

}
