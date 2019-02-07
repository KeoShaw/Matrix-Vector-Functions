#include "3dfunctions.h"

//Computes vector cross product for size 3 vector
void crossProductVec(const Vector *vector1, const Vector *vector2, Vector *vector3){
	(*vector3)[0] = (*vector1)[1] * (*vector2)[2] - (*vector1)[2] * (*vector2)[1];
	(*vector3)[1] = (*vector1)[2] * (*vector2)[0] - (*vector1)[0] * (*vector2)[2];
	(*vector3)[2] = (*vector1)[0] * (*vector2)[1] - (*vector1)[1] * (*vector2)[0];
}

//Calculates the cross product of a 3x3 matrix and 1x3 vector and returns the resultant vector in the product parameter. Multiplies mat x vec in that order (matters).
void crossProductMatVec(const Matrix *matrix, const Vector *vector, Vector *product){

    (*product)[0] = (*vector)[0] * (*matrix)[0][0] + (*vector)[1] * (*matrix)[0][1] + (*vector)[2] * (*matrix)[0][2];
    (*product)[1] = (*vector)[0] * (*matrix)[1][0] + (*vector)[1] * (*matrix)[1][1] + (*vector)[2] * (*matrix)[1][2];
    (*product)[2] = (*vector)[0] * (*matrix)[2][0] + (*vector)[1] * (*matrix)[2][1] + (*vector)[2] * (*matrix)[2][2];
}

//Creates a rotation matrix which rotates (in radians) in one axis. Axis parameter must be 'x', 'y' or 'z' (case sensitive). angle is the angle of rotation in radians
void createRotationMatrix(char axis, float angle, Matrix *matrix){
    switch(axis){
        case 'x':
            (*matrix)[0][0] = 1; (*matrix)[0][1] = 0; (*matrix)[0][2] = 0;
            (*matrix)[1][0] = 0; (*matrix)[1][1] = cos(angle); (*matrix)[1][2] = -sin(angle);
            (*matrix)[2][0] = 0; (*matrix)[2][1] = sin(angle); (*matrix)[2][2] = cos(angle);
            break;

        case 'y':
            (*matrix)[0][0] = cos(angle); (*matrix)[0][1] = 0; (*matrix)[0][2] = sin(angle);
            (*matrix)[1][0] = 0; (*matrix)[1][1] = 1; (*matrix)[1][2] = 0;
            (*matrix)[2][0] = -sin(angle); (*matrix)[2][1] = 0; (*matrix)[2][2] = cos(angle);
            break;

        case 'z':
            (*matrix)[0][0] = cos(angle); (*matrix)[0][1] = -sin(angle); (*matrix)[0][2] = 0;
            (*matrix)[1][0] = sin(angle); (*matrix)[1][1] = cos(angle); (*matrix)[1][2] = 0;
            (*matrix)[2][0] = 0; (*matrix)[2][1] = 0; (*matrix)[2][2] = 1;
            break;
        default:
            break;
    }
}

//Return the vector of a vector and scalar multiplication.
ThreeDPoint vecScalMult(const ThreeDPoint *vector, const int scalar){
	ThreeDPoint newVec;
	newVec.x = vector->x * scalar;
	newVec.y = vector->y * scalar;
	newVec.z = vector->z * scalar;
	return(newVec);
}

//Returns the dot product of two vectors.
float dotProdVec(const ThreeDPoint *vector1, const ThreeDPoint *vector2){
	return(vector1->x * vector2->x + vector1->y * vector2->y + vector1->z * vector2->z);
}

//Reads an Off struct from a file the user provides, calculates centre point of the Off object, and radius of a perfect sphere around it.
Off readOFFFile(const char *fileName){

    char buffer[50];
    FILE *inFile;
    int i;
    Off off;

    if((inFile = fopen(fileName, "r")) == NULL){
        printf("file not found");
        exit(-1);
    }

    //Checks for proper file header
    fgets(buffer, 50, inFile);
    if(strcmp(buffer, "OFF\n") != 0){
        printf("Wrong type of file");
        exit(-1);
    }

    //determines number of vertices and faces from file header
    fscanf(inFile, "%d %d %*d", &off.nvert, &off.nfaces);

    //Uses number of vertices and faces to allocate memory to the structure's pointers
    off.vertex = (ThreeDPoint *) malloc(sizeof(ThreeDPoint) * off.nvert);
    off.face= (Face *) malloc(sizeof(Face) * off.nfaces);

    //Parses each line and gives memory for 3 values to each pointer, stores values found in those three slots
    for(i=0;i<off.nvert;i++){
        fscanf(inFile, "%f %f %f", &off.vertex[i].x, &off.vertex[i].y, &off.vertex[i].z);
    }

    for(i=0;i<off.nfaces;i++){
        fscanf(inFile, "%*d %d %d %d", &off.face[i].x, &off.face[i].y, &off.face[i].z);
    }

    fclose(inFile);
    return(off);
}

//Gets file name from command prompt
void getFileName(char *fileName){
    printf("Enter OFF file path\\name: ");
    fgets(fileName, 50, stdin);
    fileName[strlen(fileName)-1] = '\0';
    return;
}

//Find the centre point of a parameter Off struct
void findCentre(const Off *off, GLfloat centrePoint[3]){
    int i;
    centrePoint[0] = 0; centrePoint[1] = 0; centrePoint[2] = 0;

    for(i=0;i<off->nvert;i++){
        centrePoint[0] += off->vertex[i].x;
        centrePoint[1] += off->vertex[i].y;
        centrePoint[2] += off->vertex[i].z;
    }
    centrePoint[0] = centrePoint[0] / off->nvert;
    centrePoint[1] = centrePoint[1] / off->nvert;
    centrePoint[2] = centrePoint[2] / off->nvert;
}

//Find the radius of a perfect sphere around the given Off struct.
float findRadius(const Off *off, const GLfloat centrePoint[3]){
    float radius=0, tempRad;
    ThreeDPoint temp;
    int i;

    for(i=0;i<off->nvert;i++){
        temp.x = off->vertex[i].x - centrePoint[0];
        temp.y = off->vertex[i].y - centrePoint[1];
        temp.z = off->vertex[i].z - centrePoint[2];

        tempRad = sqrtf(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
        if(tempRad > radius){
            radius = tempRad;
        }

    }

    return radius;

}

//Draws an Off struct in OpenGl
void drawOff(const Off *off){

    int i;

    for(i=0;i<off->nfaces;i++){
        glBegin(GL_TRIANGLES);
            int f[3];
            f[0] = off->face[i].x;
            f[1] = off->face[i].y;
            f[2] = off->face[i].z;

            GLfloat v1[3];
            v1[0] = (GLfloat) off->vertex[f[0]].x;
            v1[1] = (GLfloat) off->vertex[f[0]].y;
            v1[2] = (GLfloat) off->vertex[f[0]].z;

            GLfloat v2[3];
            v2[0] = (GLfloat) off->vertex[f[1]].x;
            v2[1] = (GLfloat) off->vertex[f[1]].y;
            v2[2] = (GLfloat) off->vertex[f[1]].z;

            GLfloat v3[3];
            v3[0] = (GLfloat) off->vertex[f[2]].x;
            v3[1] = (GLfloat) off->vertex[f[2]].y;
            v3[2] = (GLfloat) off->vertex[f[2]].z;

            glVertex3fv(v1);
            glVertex3fv(v2);
            glVertex3fv(v3);

        glEnd();

    }
}

void normaliseOff(Off *off, GLfloat centrePoint[3]){
    int i;

    for(i = 0;i < off->nvert; i++){
        off->vertex[i].x -= centrePoint[0];
        off->vertex[i].y -= centrePoint[1];
        off->vertex[i].z -= centrePoint[2];
    }
}


