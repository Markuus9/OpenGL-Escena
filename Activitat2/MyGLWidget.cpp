#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0): QOpenGLWidget(parent), program(NULL)
{
  srand (time(NULL));
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL ()
{
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersModels();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
  ini_variables();
  setTrees(5);
}

void MyGLWidget::ini_variables(){
	makeCurrent();
	// Distancia de desplasament de Luke
	movimentLuke = 1;
	// Valor de la rotació de Luke
	rotacioLuke = 0;
	// Multiplicador rotació camara 
	factorAngleX = 1; 
	factorAngleY = 0.5;
	// Valor inicial d'arbres
	numtrees = 5;
	// Rotació inicial dels Arbres (Dial i +/-)
	angleArbre = 0;
	// Posició inicial Luke
	PosicioLuke = glm::vec3(0,0,0);
	// Cambiar l'estat de l'interficie
	emit setDegrees(angleArbre);
	emit numTrees(numtrees);
	// Calcul inicial de la posició dels arbres/rotació/escala
	float movimiento = 4.5;
	if(escalaArbres==nullptr){
		escalaArbres = new glm::vec3[numtrees];
		rotacioArbres = new float[numtrees];
		posicioArbres = new glm::vec3[numtrees];
		for (int i = 0; i<numtrees; i++){
			escalaArbres[i] = glm::vec3(glm::linearRand(0.08f, 0.13f));
			rotacioArbres[i] = glm::linearRand(0.00f, 360.0f);
			posicioArbres[i] = glm::vec3(glm::linearRand(-movimiento, movimiento),0,glm::linearRand(-movimiento, movimiento));
		}
	}
	update();
}

void MyGLWidget::iniEscena ()
{
    centreEscena = glm::vec3(0,0,0);
    glm::vec3 punt_max(5.0f, 2.0f, 5.0f);
    glm::vec3 punt_min(-5.0f, 0.0f, -5.0f);

    // Calculem el centre de l'escena
    glm::vec3 centro = (punt_max + punt_min) / 2.0f;

    // Calculamos el radi:
    radiEscena = glm::length(punt_max - centro);;
    d=2*radiEscena;

    ortho = false;
}

void MyGLWidget::iniCamera() {

    angleY = 0;
    angleX = -(M_PI/4.0)*(180/M_PI);
    ortho = false;

    fov = M_PI/2.0f;
    znear = 0.1f;
    zfar  = 200;

    viewTransform();
    projectTransform();
}

void MyGLWidget::viewTransformOrtho (){
	glm::vec3 UP = glm::vec3(0,0,-1);
	glm::vec3 VRP = centreEscena;
	glm::vec3 OBS = glm::vec3(centreEscena.x,d,centreEscena.z);
	glm::mat4 View(1.0f);
  View = glm::lookAt(OBS,VRP,UP);
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::viewTransform ()
{
    // Matriu de posició i orientació de l'observador
    glm::mat4 View(1.0f);
    vrp = centreEscena; 
    View = glm::translate(View,glm::vec3(0.,0.,-d));
		View = glm::rotate(View,glm::radians(-angleX),glm::vec3(1.0,0.0,0.0));
		View = glm::rotate(View,glm::radians(-angleY),glm::vec3(0.0,1.0,0.0));
		View = glm::translate(View,glm::vec3(-vrp.x,-vrp.y,-vrp.z));
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransformOrtho (){
	float right = radiEscena;
	float left = -radiEscena;
	float top = right;
	float bottom = left;
	if(ra>1.0){
		right = radiEscena*ra;
		left = -right; 
	} else if(ra<1.0){
		top = radiEscena/ra; 
		bottom = -top;
	}
	float Znear = d-radiEscena;
	float Zfar = d+radiEscena;
	glm::mat4 Proj=glm::ortho(left, right, bottom, top, Znear, Zfar);
	glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::projectTransform ()
{
    glm::mat4 Proj(1.0f);
		float angleFOV = glm::asin(radiEscena/d);
		if(ra<1){
			angleFOV = glm::atan(glm::tan(angleFOV)/ra);
		}
		znear = d-radiEscena;
		zfar = d+radiEscena;
		fov = 2*angleFOV;
		Proj = glm::perspective (fov, ra, znear, zfar);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::paintGL ()
{

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray (VAO_models[TREE]);
  
  treeTransform();
  //glDrawArrays(GL_TRIANGLES, 0, models[TREE].faces().size()*3);

  glBindVertexArray (VAO_models[LUKE]);
  LukeTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[LUKE].faces().size()*3);

  // Terra
  glBindVertexArray (VAO_Terra);
  terraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
  // Depenen de la vista escollida, activem les respectives funcions
  if(!ortho){
    	viewTransform();
    	projectTransform();
  } else {
  		viewTransformOrtho();
  		projectTransformOrtho();
  }
}

void MyGLWidget::resizeGL (int w, int h)
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
  ra = float(ample) / float(alt);
#else
  ample = w;
  alt = h;
  ra = float(w) / float(h);
#endif
}

// Funció per interactuar amb el dial
void MyGLWidget::rotateTree(int degrees){
	makeCurrent();
	angleArbre = degrees;
	for (int i = 0; i<numtrees; i++){
		rotacioArbresControlada[i] = degrees + rotacioArbres[i];
	}
	update();
}

// Restablir tota l'escena:
void MyGLWidget::resetView(){
	makeCurrent();
	initializeGL();
	emit setOrtho(ortho);
	emit setPers(!ortho);
	update();
}

// Funció per interactuar amb el CheckBox
void MyGLWidget::PersView(bool view){
	makeCurrent();
	ortho = !view;
	emit setOrtho(ortho);
	emit setPers(!ortho);
	viewTransform();
  projectTransform();
	update();
}

// Funció per interactuar amb el CheckBox
void MyGLWidget::orthoView(bool view){
	makeCurrent();
	ortho = view;
	emit setOrtho(ortho);
	emit setPers(!ortho);
	viewTransformOrtho();
  projectTransformOrtho();
	update();
}

// Funció per afegir o treure arbres
void MyGLWidget::setTrees(int numTrees){
	makeCurrent();
	float movimiento = 4.5;
	if (numTrees > numtrees) {
      glm::vec3* newEscalaArbres = new glm::vec3[numTrees];
      float* newRotacioArbres = new float[numTrees];
      glm::vec3* newPosicioArbres = new glm::vec3[numTrees];

      for (int i = 0; i < numtrees; ++i) {
          newEscalaArbres[i] = escalaArbres[i];
          newRotacioArbres[i] = rotacioArbres[i];
          newPosicioArbres[i] = posicioArbres[i];
      }

      for (int i = numtrees; i < numTrees; ++i) {
          newEscalaArbres[i] = glm::vec3(glm::linearRand(0.08f, 0.13f));
          newRotacioArbres[i] = glm::linearRand(0.00f, 360.0f);
          newPosicioArbres[i] = glm::vec3(glm::linearRand(-movimiento, movimiento), 0, glm::linearRand(-movimiento, movimiento));
      }

      delete[] escalaArbres;
      delete[] rotacioArbres;
      delete[] posicioArbres;
      escalaArbres = newEscalaArbres;
      rotacioArbres = newRotacioArbres;
      posicioArbres = newPosicioArbres;
      
    } 
  numtrees = numTrees;
  for(int i = 0; i < numtrees; ++i){
  	rotacioArbresControlada[i] = rotacioArbres[i] + angleArbre;
  }
  update();
}

void MyGLWidget::treeTransform ()
{
  for (int i = 0; i<numtrees; i++){
    glm::mat4 TG(1.0f);
    TG = glm::translate(TG, posicioArbres[i]);
    TG = glm::scale(TG, glm::vec3(escalaArbres[i]));
    TG = glm::translate(TG, -centreBaseModels[TREE]);
		TG = glm::rotate(TG, glm::radians(rotacioArbresControlada[i]), glm::vec3(0.0, 1.0, 0.0));
  	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
  	glDrawArrays(GL_TRIANGLES, 0, models[TREE].faces().size()*3);
	}
}

void MyGLWidget::LukeTransform()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, PosicioLuke);
  TG = glm::scale(TG, glm::vec3(escalaModels[LUKE]));
  TG = glm::translate(TG, -centreBaseModels[LUKE]);
  TG = glm::rotate(TG, rotacioLuke, glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::terraTransform ()
{
  glm::mat4 TG(1.0f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: {
    	// VERSION 2 DEL MOVIMIENTO DEL PERSONAJE
    	PosicioLuke += glm::vec3(sin(rotacioLuke)*movimentLuke, 0, cos(rotacioLuke)*movimentLuke);
    	if(PosicioLuke.z > 5 or PosicioLuke.z < -5 or PosicioLuke.x > 5 or PosicioLuke.x < -5){
    	  PosicioLuke -= glm::vec3(sin(rotacioLuke)*movimentLuke, 0, cos(rotacioLuke)*movimentLuke);
    	}
    	/* VERSION 1 DEL MOVIMIENTO DEL PERSONAJE (FUNCIONA UN POCO MEJOR QUE LA VERSION 2, PERO ES MUCHO MAS COSTOSA)
    	bool delante, detras, derecha, izquierda;
    	delante = detras = derecha = izquierda = false;
			if ((rotacioLuke >= -0.1f and rotacioLuke < M_PI/2.0) or (rotacioLuke > 1.5*M_PI+0.5f)) { // Mirando hacia adelante
				delante = true;
			} else if (rotacioLuke > (M_PI/2.0)+0.1f and rotacioLuke < 1.5f*M_PI) { // Mirando hacia atrás
					detras = true;
			}

			if (rotacioLuke > M_PI+0.6f) { // Mirando hacia la derecha
					derecha = true;
			} else if (rotacioLuke > 0.1f and rotacioLuke < M_PI) { // Mirando hacia la izquierda
				izquierda = true;
			}
			if (delante and PosicioLuke.z<5) { 
					if((izquierda and PosicioLuke.x<5) or (derecha and PosicioLuke.x>-5)){
						PosicioLuke.z += movimentLuke;
					} else if(!izquierda and !derecha){
						PosicioLuke.z += movimentLuke;
					}
			} else if(detras and PosicioLuke.z>-5){
   			if((izquierda and PosicioLuke.x<5) or (derecha and PosicioLuke.x>-5)){
						PosicioLuke.z += -movimentLuke;
					} else if(!izquierda and !derecha){
						PosicioLuke.z += -movimentLuke;
					} 
			} 
			if(izquierda and PosicioLuke.x<5){
				if((delante and PosicioLuke.z<5) or (detras and PosicioLuke.z>-5)){
					PosicioLuke.x += movimentLuke;
				} else if(!delante and !detras){
					PosicioLuke.x += movimentLuke;
				}
			
			} else if(derecha and PosicioLuke.x>-5) {
				if((delante and PosicioLuke.z<5) or (detras and PosicioLuke.z>-5)){
					PosicioLuke.x += -movimentLuke;
				} else if(!delante and !detras){
					PosicioLuke.x += -movimentLuke;
				}
			} */
      break;
    }
    case Qt::Key_Left: { 	
			rotacioLuke += M_PI/4;
			if (rotacioLuke >= (2 * M_PI)-0.5f) {
        rotacioLuke = 0; 
    	}
      break;
    }
    case Qt::Key_Right: {
    	if (rotacioLuke <= 0.3f) {
        rotacioLuke = 2 * M_PI;
    	}
			rotacioLuke -= M_PI/4;
      break;
    }
    case Qt::Key_C: { 
			orthoView(!ortho);
			if(!ortho){
		  	viewTransform();
		  	projectTransform();
			} else {
				viewTransformOrtho();
				projectTransformOrtho();
			}
      break;
    }
    case Qt::Key_R: { // reset
			resetView();
    break;
    }
    case Qt::Key_Plus: {
        angleArbre += 10;
        setDegrees(angleArbre);
        rotateTree(angleArbre);
        break;
    }
    case Qt::Key_Minus: { 
        angleArbre -= 10;
        emit setDegrees(angleArbre);
        rotateTree(angleArbre);
        break;
    }  
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(!ortho){
		makeCurrent();	
		if ( (e->buttons() == Qt::LeftButton) && !(e->modifiers() & Qt::ShiftModifier) ) {
			// Gir al voltant de l'eix Y
			angleY += (e->x() - xClick)*factorAngleY;
			xClick = e->x();
			
			// Gir al voltant de l'eix X
			angleX += (e->y() - yClick)*factorAngleX;
			yClick = e->y();
		}
		update();
	}
}


void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posTerra[4] = {
        glm::vec3(-5.0, 0.0, -5.0),
        glm::vec3(-5.0, 0.0,  5.0),
        glm::vec3( 5.0, 0.0, -5.0),
        glm::vec3( 5.0, 0.0,  5.0)
  };

  glm::vec3 c(0.65, 0.2, 0.05);
  glm::vec3 colTerra[4] = { c, c, c, c };

  // VAO
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posTerra), posTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }

  escala = alcadaDesitjada/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersModels ()
{
  // Càrrega dels models
  models[TREE].load("models/tree.obj");
  models[LUKE].load("models/luke_jedi.obj");

  // Creació de VAOs i VBOs per pintar els models
  glGenVertexArrays(NUM_MODELS, &VAO_models[0]);

  float alcadaDesitjada[NUM_MODELS] = {1,1};//,1,1,1.5,1};

  for (int i = 0; i < NUM_MODELS; i++)
  {
	  // Calculem la capsa contenidora del model
	  calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);

	  glBindVertexArray(VAO_models[i]);

	  GLuint VBO[2];
	  glGenBuffers(2, VBO);

	  // geometria
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_vertices(), GL_STATIC_DRAW);
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  // color
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matdiff(), GL_STATIC_DRAW);
	  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(colorLoc);
  }

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile(":shaders/basicShader.frag");
  vs.compileSourceFile(":shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
}


