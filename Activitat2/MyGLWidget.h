#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/random.hpp>

#include "model.h"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core  {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
    virtual void initializeGL ( );
    virtual void paintGL ( );
    virtual void resizeGL(int, int);

    virtual void keyPressEvent (QKeyEvent *e);
    virtual void mouseMoveEvent (QMouseEvent *e);
    virtual void mouseReleaseEvent (QMouseEvent *e);
    virtual void mousePressEvent (QMouseEvent *e);

    virtual void treeTransform();
    virtual void LukeTransform();

    virtual void terraTransform();
  
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void viewTransform ();
    virtual void projectTransform ();

   glm::vec3 *posicioArbres = nullptr;
   glm::vec3 *escalaArbres = nullptr;
   float *rotacioArbres = nullptr;
   int numtrees;
  
   // glm::vec3 dirLuke, posLuke;
   // float rotLuke;

     void calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &CentreBase);
    // creaBuffersModels - Carreguem els fitxers obj i fem la inicialització dels diferents VAOS i VBOs
    void creaBuffersModels ();
    // creaBuffersTerra - Inicialitzem el VAO i els VBO d'un model fet a mà que representa un terra
    void creaBuffersTerra ();
    // carregaShaders - Carreguem els shaders, els compilem i els linkem. També busquem els uniform locations que fem servir.
    void carregaShaders ();
    // Calcula i emmagatzema la posició, escala i rotació dels numTrees arbres sobre el terreny, i demana a OpenGL que redibuixi l'escena
    
public slots:
    void setTrees(int numTrees);
signals:
		void nArbres(int);

  private:
  
  	void ini_variables();
    int printOglError(const char file[], int line, const char func[]);
   
    // variables per interacció de càmera amb angles d'Euler
    float angleX;
    float angleY;
    
    // bools per controlar si càmera en planta o no
    bool ortho;


    GLuint vertexLoc, colorLoc;

    // uniform locations
    GLuint transLoc, viewLoc, projLoc;

    // enum models - els models estan en un array de VAOs (VAO_models), aquest enum és per fer més llegible el codi.
    typedef enum { TREE = 0, LUKE = 1, NUM_MODELS = 2} ModelType;

    // VAO names
    GLuint VAO_models[NUM_MODELS];
    GLuint VAO_Terra;


     QOpenGLShaderProgram *program;

    // Viewport
    GLint ample, alt;

    // Mouse interaction
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float factorAngleX, factorAngleY;

    // Internal vars
    float radiEscena, rotacioLuke, movimentLuke;
    float fov, ra=1, znear, zfar, d;
    glm::vec3 centreEscena, obs, vrp, up;
	
    // Models, capses contenidores i escales
    Model models[NUM_MODELS];
    glm::vec3 centreBaseModels[NUM_MODELS];
    float escalaModels[NUM_MODELS];
};
