/********************************************************/
/*                     cube.cpp                                                 */
/********************************************************/
/*                Affiche a l'ecran un cube en 3D                      */
/********************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/freeglut.h>   /* Pour les autres systemes */
#endif
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <jpeglib.h>
#include <jerror.h>

/*class Point*/
class Point{
    public :
	//coordonnées x, y et z du point
	double x;
	double y;
	double z;
};


/*Variables globales*/
char presse; //pour récupérer les boutons préssés
int anglex,angley,x,y,xold,yold; //pour la camera
float bat=0.0f; //Variable pour l'animation
bool angle = true; //Variable pour l'animation
float angleB=0.0f; //Variable pour l'animation
int zoom=0;

//Déclaration tableaux images
unsigned char image1[256*256*3], image2[256*256*3], image3[256*256*3], image4[256*256*3], image5[256*256*3];
//Déclaration tableaux textures
unsigned char rock[256][256][3], green[256][256][3], eye[256][256][3], peau[256][256][3], gris[256][256][3];
GLuint texObject[4];

//Tableau pour stocker les sommets de la pyramide et leur couleur
Point pPyra[5]= {
    { -1,-1, 0},
    { -1, 1, 0},
    { 1, 1, 0},
    { 1, -1, 0},
    {0, 0, 1}};

//Tableau pour stocker les indices des sommets par face, hors base, pour la pyramide
int fPyra[4][3]={
  {0,1,4},
  {1,2,4},
  {3,0,4},
  {2,3,4}};

int bPyra[4]={0,3,2,1};

/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche,int x,int y);
void reshape(int x,int y);
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void animation();
void fleche(int touche,int x,int y);
void pyramide();
void loadJpegImage(char *fichier, unsigned char image[]);

int main(int argc,char **argv)
{
    /*Chargement textures*/
    loadJpegImage("rock.jpg", image1);
    loadJpegImage("green.jpg", image2);
    loadJpegImage("eye.jpg", image3);
    loadJpegImage("peau.jpeg", image4);
    loadJpegImage("gris.jpeg", image5);
    for(int i=0;i<256;i++)
    {
    //Initialisation textures
        for(int j=0;j<256;j++)
        {

                rock[i][j][0]=image1[i*256*3+j*3];
                rock[i][j][1]=image1[i*256*3+j*3+1];
                rock[i][j][2]=image1[i*256*3+j*3+2];

                green[i][j][0]=image2[i*256*3+j*3];
                green[i][j][1]=image2[i*256*3+j*3+1];
                green[i][j][2]=image2[i*256*3+j*3+2];

                eye[i][j][0]=image3[i*256*3+j*3];
                eye[i][j][1]=image3[i*256*3+j*3+1];
                eye[i][j][2]=image3[i*256*3+j*3+2];

                peau[i][j][0]=image4[i*256*3+j*3];
                peau[i][j][1]=image4[i*256*3+j*3+1];
                peau[i][j][2]=image4[i*256*3+j*3+2];

                gris[i][j][0]=image5[i*256*3+j*3];
                gris[i][j][1]=image5[i*256*3+j*3+1];
                gris[i][j][2]=image5[i*256*3+j*3+2];
        }
    }

  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(500,500);
  glutCreateWindow("dragon");

  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);
  glEnable(GL_DEPTH_TEST);

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(fleche);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);
  glutIdleFunc(animation);

  // génère un nouvel objet de texture
    glGenTextures(5, texObject);

    //Liaison texture ID
    glBindTexture(GL_TEXTURE_2D, texObject[0]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image1);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texObject[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image2);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texObject[2]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image3);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texObject[3]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image4);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texObject[4]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image5);
    glEnable(GL_TEXTURE_2D);


  /* Entree dans la boucle principale glut */
  glutMainLoop();


  return 0;
}


void affichage()
{

    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-20+zoom,20-zoom,-20+zoom,20-zoom,-50,50);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    glLoadIdentity();
    glRotatef(angley,1.0,1.0,0.0);
    glRotatef(anglex,0.0,1.0,0.0);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texObject[1]);
    //les pattes
    glPushMatrix();

        glRotated(-90,1,0,0);

        glPushMatrix();
            glTranslated(2,3,0);
            glutSolidCylinder(0.75,4,20,8);
            glTranslatef(0,0.18,0);
            glScalef(1.1,1.4,1.2);
            glutSolidSphere(0.75,20,20);
        glPopMatrix();

        glPushMatrix();
            glTranslated(-2,3,0);
            glutSolidCylinder(0.75,4,20,8);
            glTranslatef(0,0.18,0);
            glScalef(1.1,1.4,1.2);
            glutSolidSphere(0.75,20,20);
        glPopMatrix();

        glPushMatrix();
            glTranslated(-2,-3,0);
            glutSolidCylinder(0.75,4,20,8);
            glTranslatef(0,0.18,0);
            glScalef(1.1,1.4,1.2);
            glutSolidSphere(0.75,20,20);
        glPopMatrix();

        glPushMatrix();
            glTranslated(2,-3,0);
            glutSolidCylinder(0.75,4,20,8);
            glTranslatef(0,0.18,0);
            glScalef(1.1,1.4,1.2);
            glutSolidSphere(0.75,20,20);
        glPopMatrix();

    glPopMatrix();

    //Corps
    glPushMatrix();
        glTranslatef(0,3.5,0);
        glScalef(0.9,0.6,1.4);
        glutSolidSphere(4,30,30);
    glPopMatrix();

    //cou
    glPushMatrix();
        glTranslatef(0,7,-6);
        glRotatef(45,1,0,0);
        glutSolidCylinder(1,5,50,50);
    glPopMatrix();

    //tete
    glPushMatrix();
        glTranslatef(0,7.5,-6.5);
        glScalef(1,1,1);
        glutSolidCube(3);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,7.5,-9);
        glScalef(1,0.7,1.2);
        glutSolidCube(2);
    glPopMatrix();

    //bouche
    glPushMatrix();
        glTranslatef(0,7,-7.5);
        glRotatef(angleB,1,0,0);
        glScalef(1,0.2,2.5);
        glutSolidCube(2);
    glPopMatrix();

    //queue
    glPushMatrix();
        glRotatef(45,1,0,0);
        glTranslatef(0,5.2,-1);
        glutSolidCylinder(0.75,5,20,4);
    glPopMatrix();

    //aile droite
    glPushMatrix();

        glTranslatef(3,6,-1);
        glRotated(60-bat,0,0,1);

        glPushMatrix();
            glScalef(6,1,1);
            glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
            glRotated(90,0,1,0);
            glTranslatef(-2,0,3);
            glScalef(5,1,1);
            glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
            glRotated(90,0,1,0);
            glTranslatef(-1.5,0,0);
            glScalef(3,1,1);
            glutSolidCube(1);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texObject[3]);
        //peau proche
        glPushMatrix();
            glTranslatef(-1.5,0,1.5);
            glScalef(2.5,0.5,2);
            glColor3f(1.0,1.0,1.0);
            glutSolidCube(1);
        glPopMatrix();

        //peau
        glPushMatrix();
            glTranslatef(1.5,0,2);
            glScalef(2.5,0.5,4.5);
            glColor3f(1.0,1.0,1.0);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texObject[1]);

    //aile gauche


    glPushMatrix();

        glTranslatef(-3,6,-1);
        glRotated(120+bat,0,0,1);

        glPushMatrix();
            glScalef(6,1,1);
            glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
            glRotated(90,0,1,0);
            glTranslatef(-2,0,3);
            glScalef(5,1,1);
            glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
            glRotated(90,0,1,0);
            glTranslatef(-1.5,0,0);
            glScalef(3,1,1);
            glutSolidCube(1);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texObject[3]);
        //peau proche
        glPushMatrix();
            glTranslatef(-1.5,0,1.5);
            glScalef(2.5,0.5,2);
            glutSolidCube(1);
        glPopMatrix();

        //peau
        glPushMatrix();
            glTranslatef(1.5,0,2);
            glScalef(2.5,0.5,4.5);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texObject[2]);
    //yeux
    glPushMatrix();
        glTranslatef(0,8.5,-8.3);
        glPushMatrix();
            glTranslatef(0.7,0,0);
            glutSolidCube(0.7);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.7,0,0);
            glutSolidCube(0.7);
        glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


    //boue de queue
    glPushMatrix();
        glTranslatef(0,0.8,6.4);
        glRotatef(45,1,0,0);
        glScalef(1.5,1.5,3);
        pyramide();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texObject[4]);
    //détails
    glPushMatrix();
        glRotatef(180,0,1,0);
        glTranslatef(0,0,4);
        glPushMatrix();
            glTranslatef(2.3,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.8,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-2.3,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-1.8,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glRotatef(180,0,1,0);
        glTranslatef(0,0,-2);
        glPushMatrix();
            glTranslatef(2.3,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.8,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-2.3,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-1.8,0,0);
            glutSolidCone(0.2,1,10,3);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glRotatef(-90,1,0,0);
        glTranslatef(0,0,5.8);
        glScalef(0.3,1,1);
        glPushMatrix();
            glutSolidCone(0.7,1.5,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,1.2,-0.3);
            glutSolidCone(0.7,1.5,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,2.4,-0.6);
            glutSolidCone(0.7,1.5,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,-1.2,-0.3);
            glutSolidCone(0.7,1.5,10,3);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,-2.4,-0.6);
            glutSolidCone(0.7,1.5,10,3);
        glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //coordhomogènes : position
    GLfloat position_source0[] = {7, 2.0, 3.0, 0};
    //direction source à distance infinie
    GLfloat direction_source0[] = {1.0, 2.0, 3.0, 0.0};
    //composante diffuse
    GLfloat dif_0[] = {1.0, 1.0, 1.0, 0.5};
    //composante ambiante
    GLfloat amb_0[] = {1.0, 0.0, 0.0, 1};
    //composante spéculaire blanche
    GLfloat spec_0[] = {1.0, 1.0, 1.0, 1.0};

    //spécification des propriétés
    glLightfv(GL_LIGHT0, GL_POSITION,position_source0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb_0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif_0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec_0);
    //activation de la source GL_LIGHT0
    glEnable(GL_LIGHT0);

    GLfloat light1_ambient[] = {1, 1, 1, 1.0};
    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat light1_position[] = { 5.0, 0.0, 6.0, 1.0};
    GLfloat spot_direction[] = { 2.0, 2.0, 0.0};
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
    glEnable(GL_LIGHT1);

    glFlush();

    //On echange les buffers
    glutSwapBuffers();
}


void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd':
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'a':
      glPolygonMode(GL_FRONT,GL_FILL);
      glPolygonMode(GL_FRONT,GL_LINE);
      glutPostRedisplay();
    break;
    case 'Z': // Zoom in
        zoom+=1;
    break;

    case 'z': // Zoom out
        zoom-=1;
    break;

    case 'B': // Ouvre la bouche
        if (angleB>-45)
            angleB-=5;
        else
            std::cout<<"Il va se décrocher la machoire"<<std::endl;

    break;

    case 'b' : //ferme la bouche
        if (angleB<0)
            angleB+=5;
        else
            std::cout<<"La machoire est déjà fermée"<<std::endl;
    break;

    case 'k' :
        glutFullScreen();
    break;

    case 'l' :
        glutReshapeWindow(500,500);
    break;

    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);

    }
}

void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }


void animation()
{

    if (angle)
    {
        bat+=0.1;
        if (bat>25)
            angle=false;
    }
    else
    {
        bat-=0.1;
        if (bat<1)
            angle=true;
    }
    glutPostRedisplay();

}

void fleche(int touche,int x,int y)
{
  switch (touche)
    {
    case GLUT_KEY_UP : //tourne par le bas
        angley++;
    break;

    case GLUT_KEY_DOWN : //tourne par le haut
        angley--;
    break;

    case GLUT_KEY_LEFT : //tourne par la droite
        anglex++;
    break;

    case GLUT_KEY_RIGHT : //tourne par la gauche
        anglex--;
    }
}


void loadJpegImage(char *fichier,unsigned char image[])
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if ((cinfo.image_width!=256)||(cinfo.image_height!=256)) {
    fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
    exit(1);
  }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*256*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}

void pyramide()
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texObject[0]);
  for (int i=0;i<4;i++)
    {
      glBegin(GL_POLYGON);
          glTexCoord2f(0.0, 0.0); glVertex3f(pPyra[fPyra[i][0]].x,pPyra[fPyra[i][0]].y,pPyra[fPyra[i][0]].z);
          glTexCoord2f(0.0,i+1); glVertex3f(pPyra[fPyra[i][1]].x,pPyra[fPyra[i][1]].y,pPyra[fPyra[i][1]].z);
          glTexCoord2f(1.0, 1.0); glVertex3f(pPyra[fPyra[i][2]].x,pPyra[fPyra[i][2]].y,pPyra[fPyra[i][2]].z);
      glEnd();
    }

    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 1.0);   glVertex3f(pPyra[bPyra[0]].x,pPyra[bPyra[0]].y,pPyra[bPyra[0]].z);
        glTexCoord2f(0.0, 0.0);   glVertex3f(pPyra[bPyra[1]].x,pPyra[bPyra[1]].y,pPyra[bPyra[1]].z);
        glTexCoord2f(1.0, 0.0);   glVertex3f(pPyra[bPyra[2]].x,pPyra[bPyra[2]].y,pPyra[bPyra[2]].z);
        glTexCoord2f(1.0, 1.0);   glVertex3f(pPyra[bPyra[3]].x,pPyra[bPyra[3]].y,pPyra[bPyra[3]].z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
