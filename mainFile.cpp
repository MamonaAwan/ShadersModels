#include <GL\glew.h>
#include <GL\freeglut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
#include <vector>
using namespace std;
#pragma comment(lib, "glew32.lib")

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::perspective;

glm::mat4 ModelViewMatrix, ProjectionMatrix ,MVP;
glm::mat4 NormalMatrix;

//int num = 0;
//GLfloat vertex[10240 * 3][4];
//GLfloat normal[10240 * 3][4];

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

char buf[1024];
static GLuint shaderID;
int Method=0;


void setLK(void)
{
	GLuint loc;
	vec4 Position = glm::vec4(0.5f, 0.5f, -10.0f, 1.0f);
	vec3 La=vec3(0.7f,0.7f,0.7f);
	vec3 Ld=vec3(0.6f,0.9f,0.9f);
	vec3 Ls=vec3(0.6f,1.0f,1.0f);
	float Shininess= 100.0f;
	vec3 Ka=vec3(0.1f,0.4f,0.3f);
	vec3 Kd=vec3(0.1f,0.9f,0.6f);
	vec3 Ks=vec3(0.9f,1.0f,0.0f);

	loc=glGetUniformLocation(shaderID,"Position");
	glUniform4fv(loc,1,&Position[0]);
	loc=glGetUniformLocation(shaderID,"La");
	glUniform3fv(loc,1,&La[0]);
	loc=glGetUniformLocation(shaderID,"Ld");
	glUniform3fv(loc,1,&Ld[0]);
	loc=glGetUniformLocation(shaderID,"Ls");
	glUniform3fv(loc,1,&Ls[0]);
	loc=glGetUniformLocation(shaderID,"Shininess");
	glUniform1f(loc,Shininess);
	loc=glGetUniformLocation(shaderID,"Ka");
	glUniform3fv(loc,1,&Ka[0]);
	loc=glGetUniformLocation(shaderID,"Kd");
	glUniform3fv(loc,1,&Kd[0]);
	loc=glGetUniformLocation(shaderID,"Ks");
	glUniform3fv(loc,1,&Ks[0]);
}
void setMatrices(void)
{
	glm::mat4 view = glm::lookAt( glm::vec3(0.0,0.0,15.0),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0) );
	glm::mat4 model = glm::mat4();
	model[0][0] = model[1][1] = model[2][2] = 0.001;
	glm::mat4 ModelViewMatrix = view * model;
	glm::mat4 NormalMatrix = transpose(inverse(ModelViewMatrix));
	glm::mat4 ProjectionMatrix= glm::perspective(45.0f,0.1f,0.1f,1000.0f);
	glm::mat4 MVP= ProjectionMatrix * ModelViewMatrix;
}
static char* readfile(const char* filename)
{
	FILE* fp= fopen(filename,"r");
	fseek(fp, 0,SEEK_END);
	long file_length =ftell(fp);
	fseek(fp,0,SEEK_SET);
	char * contents = new char[file_length+1];
	for(int i=0;i<file_length+1;i++)
	{
		contents[i]=0;
	}
	fread(contents,1,file_length,fp);
	contents[file_length+1]='\0';
	fclose(fp);
	return contents;
}
GLuint createVertexShader(const char * sourceCode)
{
	GLint status;
	GLuint vertexShaderID=glCreateShader(GL_VERTEX_SHADER);
	if( 0 == vertexShaderID )
	{
	  fprintf(stderr, "Error creating vertex shader.\n");
	  exit(1);
	}
	glShaderSource(vertexShaderID,1,(const GLchar **)&sourceCode,NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
	printf("vs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(vertexShaderID, sizeof(buf), NULL, buf);
	printf("vs log = [%s]\n", buf);
	return vertexShaderID;
}
GLuint createFragmentShader(const char * sourceCode)
{
	GLint status;
	GLuint fragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID,1,(const GLchar **)&sourceCode,NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);
	printf("fs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(fragmentShaderID, sizeof(buf), NULL, buf);
	printf("fs log = [%s]\n", buf);
	return fragmentShaderID;
}
GLuint createShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
	GLuint fullShaderID=glCreateProgram();
	glAttachShader(fullShaderID,vertexShaderID);
	glAttachShader(fullShaderID,fragmentShaderID);
	glLinkProgram(fullShaderID);
	return fullShaderID;
}
void wholeShader(void)
{
	cout<<"\n\nOpenGL: Sphere with Various Shading Methods:\n\n"<<endl;
	
	char * vertexShaderCode;
	char * fragmentShaderCode;
	cout<<"Enter\n1 for Lamberts Model\n2 for Phong Model\n3 for Blinn-Phong Model\n4 for Cook-Torrance Model\n5 for Ward Model: "<<endl;
	cin>>Method;
	while(Method==0);
	while(Method!=1 && Method!=2 && Method!=3 && Method!=4 && Method!=5)
	{
		cout<<"ERROR! Invalid Method"<<endl;
		cout<<"Enter either 1, 2, 3, 4 or 5 for Model Selection: "<<endl;
		cin>>Method;
	}
	if (Method==1)
	{
		cout<<"Lamberts Model Selected"<<endl;
		vertexShaderCode= readfile("lamVS.txt");
		fragmentShaderCode= readfile("lamFS.txt");
	}
	else if(Method==2)
	{
		cout<<"Phong Model Selected"<<endl;
		vertexShaderCode= readfile("phongVS.txt");
		fragmentShaderCode= readfile("phongFS.txt");
	}
	else if(Method==3)
	{
		cout<<"Blin-Phong Model Selected"<<endl;
		vertexShaderCode= readfile("blinVS.txt");
		fragmentShaderCode= readfile("blinFS.txt");
	}
	else if(Method==4)
	{
		cout<<"Cook-Torrance Model Selected"<<endl;
		vertexShaderCode= readfile("cookVS.txt");
		fragmentShaderCode= readfile("cookFS.txt");
	}
	else
	{
		cout<<"Ward Model Selected"<<endl;
		vertexShaderCode= readfile("wardVS.txt");
		fragmentShaderCode= readfile("wardFS.txt");
	}
	
	GLuint vertexShaderID=createVertexShader(vertexShaderCode);
	GLuint fragmentShaderID=createFragmentShader(fragmentShaderCode);
	shaderID= createShaderProgram(vertexShaderID,fragmentShaderID);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram(shaderID);
	GLuint vboHandles[2];
	glGenBuffers(1, vboHandles);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size())*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size()*sizeof(glm::vec3), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), normals.size()*sizeof(glm::vec3), &normals[0]);

	GLuint vaoHandle, loc=0;
	glGenVertexArrays( 1, &vaoHandle );
	glBindVertexArray(vaoHandle);

	loc = glGetAttribLocation(shaderID, "VertexPosition");
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid*)(0));
	loc = glGetAttribLocation(shaderID, "VertexNormal");
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid*)(normals.size()*sizeof(glm::vec3)));
	glEnable(GL_DEPTH_TEST);
}
void display(void) {
	GLuint temp;
	glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setLK();
	setMatrices();
	
	temp = glGetUniformLocation(shaderID, "ModelViewMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &ModelViewMatrix[0][0] );
	temp = glGetUniformLocation(shaderID, "NormalMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &NormalMatrix[0][0]);
	temp = glGetUniformLocation(shaderID, "ProjectionMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	temp = glGetUniformLocation(shaderID, "MVP");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &MVP[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glutSwapBuffers();
}
bool loadOBJ(const char * path, std::vector <glm::vec3 > & out_vertices,std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	FILE * file = fopen(path, "r");
	if( file == NULL )
	{
		printf("Impossible to open the file !\n");
		return false;
	}
	while( 1 )
	{
 
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);
		}
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for(unsigned int i=0; i<vertexIndices.size(); i++ )
	{
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		out_vertices.push_back(vertex);
	}
	for(unsigned int i=0; i<uvIndices.size(); i++ )
	{
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		out_uvs.push_back(uv);
	}
	for(unsigned int i=0; i<normalIndices.size(); i++ )
	{
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		out_normals.push_back(normal);
	}
	
}

int main (int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB| GLUT_DEPTH);
	glutInitWindowSize(750,750);
	glutInitWindowPosition(750,50);
	glutCreateWindow("GL Project");

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	GLenum err = glewInit();

	bool res = loadOBJ("sphere.obj", vertices, uvs, normals);
	wholeShader();

	glutDisplayFunc(display);

	glutMainLoop();
}
