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
    virtual void viewTransformOrtho ();
    virtual void projectTransform ();
    virtual void projectTransformOrtho ();

   glm::vec3 *posicioArbres = nullptr;
   glm::vec3 *escalaArbres = nullptr;
   float *rotacioArbres = nullptr;
   float rotacioArbresControlada[30];
   int numtrees;
   

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
    void orthoView(bool view);
    void PersView(bool view);
    void resetView();
    void rotateTree(int degrees);
signals:
		void numTrees(int);
		void setOrtho(bool);
		void setPers(bool);
		void setDegrees(int);

  private:
  
  	void ini_variables();
    int printOglError(const char file[], int line, const char func[]);
   
    // variables per interacció de càmera amb angles d'Euler
    float angleX;
    float angleY;
    
    // rotació arbres amb + i -
    int angleArbre;
    
    // bool per controlar  tipus de camara seleccionada
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
    // Control de la posicio de Luke
		glm::vec3 PosicioLuke;
    float radiEscena, rotacioLuke, movimentLuke;
    float fov, ra=1, znear, zfar, d;
    glm::vec3 centreEscena, obs, vrp, up;
	
    // Models, capses contenidores i escales
    Model models[NUM_MODELS];
    glm::vec3 centreBaseModels[NUM_MODELS];
    float escalaModels[NUM_MODELS];
};
