#version 330 core
 out vec4 FragColor;

 in vec2 TexCoords;
 in vec3 pos;
 in vec3 normal;

 struct LightStrength{ //各个光的强度  每种光的三个分量应该相同
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
     //点光源的衰减常数
     float linear;
     float quadratic;
 };

 uniform float shininess;
 uniform LightStrength lightStrength;
 uniform sampler2D diffuse1;
 uniform sampler2D specular1;
 uniform sampler2D normal1;
 uniform int diffuseTexNum;
 uniform int specularTexNum;
 uniform int normalTexNum;
 uniform int heightTexNum;
 uniform mat4 model;
 uniform vec3 cameraPos;
 const vec3 lightPos=vec3(4.0f,2.0f,2.0f);
 const vec3 defaultDiffuseSampler=vec3(0.9f, 0.9f, 0.9f);
 const vec3 defaultSpecularSampler=vec3(0.01f,0.01f,0.01f);
 void main()
 {
      //全局光
        vec3 diffuseSampler,specularSampler;
        if(diffuseTexNum>0){
             diffuseSampler=texture(diffuse1,TexCoords).rgb;
        }else{
             diffuseSampler=defaultDiffuseSampler;
        }
        if(specularTexNum>0){
              specularSampler=texture(specular1,TexCoords).rgb;
        }else{
              specularSampler=defaultSpecularSampler;
        }
        vec3 ambient=lightStrength.ambient*diffuseSampler;
        vec3 newLightPos=vec3(model*vec4(lightPos,1.0f));
        vec3 lightDir=normalize(newLightPos-pos);
        //漫反射光
        vec3 norm=normalize(normal);
        float diff=max(dot(lightDir,norm),0.0);
        vec3 diffuse=diff*diffuseSampler;
        //反射高光
        vec3 viewDir=normalize(cameraPos-pos);
        vec3 reflectDir=reflect(-lightDir,norm);
        float spec=pow(max(dot(viewDir,reflectDir),0.0),shininess);
        vec3 specular=lightStrength.specular*spec*specularSampler;

         //计算衰减
         float distance=length(pos-newLightPos);
         float attenuation=1.0/(1.0+lightStrength.linear*distance+lightStrength.quadratic*distance*distance);


        FragColor=vec4((ambient+diffuse+specular)*attenuation,1.0f);
 //       FragColor=vec4((specular)*attenuation,1.0f);
 //       FragColor=vec4(texture(normal1,TexCoords).rgb,1.0f);

 }