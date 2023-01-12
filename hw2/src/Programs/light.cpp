#include <iostream>
#include "context.h"
#include "program.h"

bool LightProgram::load() {
  /* TODO#4-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to to VBOs
   * Note:
   *           If you implament BasicProgram properly, You might inherent BasicProgram's load function
   */
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);
  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];
  glGenVertexArrays(num_model, VAO);

  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[3];
    glGenBuffers(3, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  }
  return true;
}

void LightProgram::doMainLoop() {
  /* TODO#4-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass light and material parameters to shader
   *           6. Pass model texture to shaders
   *           7. Draw with glDrawArrays
   * Note:
   *           1. light paramters are providered in context.h
   *           2. material parameter for each object get be found in ctx->objects[i]->material
   */
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];
    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    //ModelNormalMatrix
    const float* mn =
        glm::value_ptr(glm::transpose(glm::inverse(ctx->objects[i]->transformMatrix * model->modelMatrix)));
    GLint mnmatLoc = glGetUniformLocation(programId, "ModelNormalMatrix");
    glUniformMatrix4fv(mnmatLoc, 1, GL_FALSE, mn);

    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
    
    //material=========================================================
    Material material = (ctx->objects[i]->material);
    // ambient
    const float* ma = glm::value_ptr(material.ambient);
    GLint maLoc = glGetUniformLocation(programId, "material.ambient");
    glUniform3fv(maLoc, 1, ma);
    // diffuse
    const float* md = glm::value_ptr(material.diffuse);
    GLint mdLoc = glGetUniformLocation(programId, "material.diffuse");
    glUniform3fv(mdLoc, 1, md);
    // specular
    const float* msp = glm::value_ptr(material.specular);
    GLint mspLoc = glGetUniformLocation(programId, "material.specular");
    glUniform3fv(mspLoc, 1, msp);
    // shininess
    GLfloat msh = material.shininess;
    GLint mshLoc = glGetUniformLocation(programId, "material.shininess");
    glUniform1f(mshLoc, msh);

    //DirectionLight====================================================
    // Enable
    GLint dle = ctx->directionLightEnable;
    GLint dleLoc = glGetUniformLocation(programId, "dl.enable");
    glUniform1i(dleLoc, dle);
    // Direction
    const float* dld = glm::value_ptr(ctx->directionLightDirection);
    GLint dldLoc = glGetUniformLocation(programId, "dl.direction");
    glUniform3fv(dldLoc, 1, dld);
    // Color
    const float* dlc = glm::value_ptr(ctx->directionLightColor);
    GLint dlcLoc = glGetUniformLocation(programId, "dl.lightColor");
    glUniform3fv(dlcLoc, 1, dlc);

    // PointLight====================================================
    // Enable
    GLint ple = ctx->pointLightEnable;
    GLint pleLoc = glGetUniformLocation(programId, "pl.enable");
    glUniform1i(pleLoc, ple);
    // Position
    const float* plp = glm::value_ptr(ctx->pointLightPosition);
    GLint plpLoc = glGetUniformLocation(programId, "pl.position");
    glUniform3fv(plpLoc, 1, plp);
    // Color
    const float* plc = glm::value_ptr(ctx->pointLightColor);
    GLint plcLoc = glGetUniformLocation(programId, "pl.lightColor");
    glUniform3fv(plcLoc, 1, plc);
    // Constant
    GLfloat plcst = ctx->pointLightConstant;
    GLint plcstLoc = glGetUniformLocation(programId, "pl.constant");
    glUniform1f(plcstLoc, plcst);
    // Linear
    GLfloat pll = ctx->pointLightLinear;
    GLint pllLoc = glGetUniformLocation(programId, "pl.linear");
    glUniform1f(pllLoc, pll);
    // Quardratic
    GLfloat plq = ctx->pointLightQuardratic;
    GLint plqLoc = glGetUniformLocation(programId, "pl.quadratic");
    glUniform1f(plqLoc, plq);
    
    
    // Spotlight====================================================
    // Enable
    GLint sle = ctx->spotLightEnable;
    GLint sleLoc = glGetUniformLocation(programId, "sl.enable");
    glUniform1i(sleLoc, sle);
    // Direction
    const float* sld = glm::value_ptr(ctx->spotLightDirection);
    GLint sldLoc = glGetUniformLocation(programId, "sl.direction");
    glUniform3fv(sldLoc, 1, sld);
    // Position
    const float* slp = glm::value_ptr(ctx->spotLightPosition);
    GLint slpLoc = glGetUniformLocation(programId, "sl.position");
    glUniform3fv(slpLoc, 1, slp);
    // Color
    const float* slc = glm::value_ptr(ctx->spotLightColor);
    GLint slcLoc = glGetUniformLocation(programId, "sl.lightColor");
    glUniform3fv(slcLoc, 1, slc);
    // CutOff
    GLfloat slcf = ctx->spotLightCutOff;
    GLint slcfLoc = glGetUniformLocation(programId, "sl.cutOff");
    glUniform1f(slcfLoc, slcf);
    // Constant
    GLfloat slcst = ctx->spotLightConstant;
    GLint slcstLoc = glGetUniformLocation(programId, "sl.constant");
    glUniform1f(slcstLoc, slcst);
    // Linear
    GLfloat sll = ctx->spotLightLinear;
    GLint sllLoc = glGetUniformLocation(programId, "sl.linear");
    glUniform1f(sllLoc, sll);
    // Quardratic
    GLfloat slq = ctx->spotLightQuardratic;
    GLint slqLoc = glGetUniformLocation(programId, "sl.quadratic");
    glUniform1f(slqLoc, slq);

    // 啟用第 0 號 texture
    glActiveTexture(GL_TEXTURE0);
    // 將紋理綁定上 GL_TEXTURE0
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);

    glDrawArrays(model->drawMode, 0, model->numVertex);
  }
  glUseProgram(0);
}
