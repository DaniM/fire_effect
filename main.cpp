#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

using namespace std;

/*
* open gl stuff: a quad with texcoords
*/

GLfloat vertices[] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 

	1.0, -1.0, 0.0f,
	1.0f, 0.0f, 

	-1.0, -1.0, 0.0f, 
	0.0f, 0.0f,
	
	-1.0f, 1.0f, 0.0f,
	0.0f, 1.0f

};
/*
GLfloat vertices[] = {
    // positions          // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
};
*/
GLuint faces[] = {
	0, 1, 3, 
	1, 2, 3
};


const char* vs = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec2 texcoord;\n"
"out vec2 _texcoord;\n"
"void main()\n"
"{\n"
	"gl_Position = vec4( pos, 1.0 );\n"
	"_texcoord = texcoord;\n"
"}";

const char* fs = 
"#version 330 core\n"
"out vec4 outcol;\n"
"in vec2 _texcoord;\n"
"uniform sampler2D _texture0;\n"
"void main()\n"
"{\n"
	"//outcol.rgb = vec3( 0.0 );\n"
	"outcol = texture( _texture0, _texcoord );\n"
"}";

//

bool alive = true;

const int FIRE_WIDTH = 320;
const int FIRE_HEIGHT = 240;
const int PALETTE_SIZE = 36;

int wind = 0x03;

char palette_idx[FIRE_HEIGHT][FIRE_WIDTH];

unsigned char fb[FIRE_HEIGHT * FIRE_WIDTH * 3];

unsigned char palette[PALETTE_SIZE][3] = 
{ 
{0 ,0 ,0},
{21 ,0 ,0},
{42 ,0 ,0},
{63 ,0, 0},
{84, 0, 0},
{105, 0, 0},
{126, 0, 0},
{147, 0, 0},
{168, 0, 0},
{189, 0, 0},
{210, 0, 0},
{255, 21, 0},
{255, 42, 0},
{255, 63, 0},
{255, 84, 0},
{255, 105, 0},
{255, 126, 0},
{255, 147, 0},
{255, 168, 0},
{255, 189, 0},
{255, 210, 0},
{255, 231, 0},
{255, 252, 0},
{255, 255, 21},
{255, 255, 42},
{255, 255, 63},
{255, 255, 84},
{255, 255, 105},
{255, 255, 126},
{255, 255, 147},
{255, 255, 168},
{255, 255, 189},
{255, 255, 210},
{255, 255, 231},
{255, 255, 252},
{255, 255, 255},
};

void fire_color( int i, int j )
{
	int r = ( rand() % wind );
	//int r = rand() % 3;

	int dst_i = i - 1 ;
	int dst_j = j - r + 1;
	int idx = palette_idx[dst_i][dst_j]; 
	if( idx == 0 )
	{
		palette_idx[i][j] = 0;	
	}
	else
	{
		palette_idx[i][j] = idx - ( r & 0x01 );	
	}

}

void init_fire()
{
	for( int y = 0; y != FIRE_HEIGHT; ++y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			palette_idx[y][x] = 0;
		}
	}

	// one white line
	/* remove the first seed for animation
	for( int x = 0; x != FIRE_WIDTH; ++x )
	{
		palette_idx[0][x] = PALETTE_SIZE - 1;
	}
	*/
}

void update_fire( )
{
	for( int y = 1; y != FIRE_HEIGHT; ++y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			fire_color( y, x );
		}
	}
}

void rise_up( int height )
{
	for( int y = 0; y != height; ++y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			if( palette_idx[y][x] < PALETTE_SIZE - 1 )
			{
				palette_idx[y][x]++;
			}

		}
	}

	for( int y = 1; y != FIRE_HEIGHT; ++y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			fire_color( y, x );
		}
	}
}

void put_out( int height )
{
	for( int y = 0; y != height; ++y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			if( palette_idx[y][x] > 0 )
			{
				palette_idx[y][x]--;
			}

		}
	}

	for( int y = FIRE_HEIGHT; y > 0; --y )
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			fire_color( y, x );
		}
	}
}

void dump_palette(  )
{
	for( int i = 0; i != FIRE_HEIGHT; ++i )
	{
		for( int j = 0; j != FIRE_WIDTH; ++j )
		{
			size_t offset =  3 * (FIRE_WIDTH * i + j);
			int idx = palette_idx[i][j];
			fb[offset] = palette[idx][0];
			fb[offset + 1] = palette[idx][1];
			fb[offset + 2] = palette[idx][2];
		}
	}
}

void update()
{
	if( alive )
	{
		rise_up(1);	
	}
	else
	{

		put_out(1);	

	}

	dump_palette();
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mode )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{
		glfwSetWindowShouldClose( window, GL_TRUE );
	}

	if( key == GLFW_KEY_SPACE && action == GLFW_RELEASE )
	{
		alive = !alive;
	}
}



int main( int argc, const char* argv[] )
{
// init open gl window
	glfwInit();
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	GLFWwindow* window = glfwCreateWindow( 800, 600, "Fire", 0, 0 );
	if( !window )
	{
		cout << "Failed to create GLFW window\n";	
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent( window );
	
	glewExperimental = GL_TRUE;
	if( glewInit() != GLEW_OK )
	{
		cout << "Failed to initialize GLEW\n";
		glfwTerminate();
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize( window, &width, &height );
	glViewport( 0, 0, width, height );

	glfwSetKeyCallback( window, key_callback );

//

// bind data

	GLuint vao;
	GLuint vbo;
	glGenVertexArrays( 1, &vao );	
	glGenBuffers( 1, &vbo );

	GLuint ebo;
	glGenBuffers( 1, &ebo );


	glBindVertexArray( vao );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );	
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), 0 ); // pos
		glEnableVertexAttribArray( 0 );
		
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), // texcoord
		(void*) ( 3 * sizeof( GLfloat ) ) );
		glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	GLint success;
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vs, NULL);
		glCompileShader(vertexShader);

	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		cout << "Failed to compile vertex shader\n";

		int size;
		glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &size );
		char* log = static_cast<char*>( malloc( size ) );
		glGetShaderInfoLog( vertexShader, size, 0, log );
		cout << log << "\n";
		free( log );

		glfwTerminate();
		return -1;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fs, NULL);
		glCompileShader(fragmentShader);

	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		cout << "Failed to compile fragment shader\n";

		int size;
		glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &size );
		char* log = static_cast<char*>( malloc( size ) );
		glGetShaderInfoLog( fragmentShader, size, 0, log );
		cout << log << "\n";
		free( log );

		glfwTerminate();
		return -1;
	}



	GLuint shader = glCreateProgram();
		glAttachShader( shader, vertexShader );
		glAttachShader( shader, fragmentShader );
		glLinkProgram( shader );

	glGetProgramiv( shader, GL_LINK_STATUS, &success );
	if( !success )
	{
		cout << "Failed to link the shader\n";
		glfwTerminate();
		return -1;
	}

	glDeleteShader( fragmentShader );
	glDeleteShader( vertexShader );


	glUseProgram( shader ); 
	glUniform1i( glGetUniformLocation( shader, "_texture0" ), 0 );


//init_fire();
//dump_palette();

	GLuint texture;
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, FIRE_WIDTH, FIRE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, fb );
		//glGenerateMipmap( GL_TEXTURE_2D );



//

	init_fire();
	const int frame_rate = 3;
	int frame_count = frame_rate;
	while( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

		glClear( GL_COLOR_BUFFER_BIT );
		
		if( --frame_count <= 0 )
		{
			frame_count = frame_rate;
			update();
			glTextureSubImage2D( texture, 0, 0, 0, FIRE_WIDTH, FIRE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, fb );
		}

		
// prepare the shader

		glActiveTexture( GL_TEXTURE0 );

		glBindTexture( GL_TEXTURE_2D, texture );

		glUseProgram( shader );

//

// draw the quad

		glBindVertexArray( vao );

		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

//

		glfwSwapBuffers( window );
	}

	glfwTerminate();

	return 0;
}
