#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

using namespace std;

const int FIRE_WIDTH = 320;
const int FIRE_HEIGHT = 240;
const int PALETTE_SIZE = 36;

int MAX_FIRE_RISE = 50;

int wind = 0x03;

char palette_idx[FIRE_HEIGHT][FIRE_WIDTH];

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

void print_palette()
{
	cout << "P3\n" << PALETTE_SIZE << " " << 1 << "\n255\n";
	for( int i = 0; i != PALETTE_SIZE; ++i )
	{
		cout << (int( palette[i][0] ) & 0xFF ) << " " << (int( palette[i][1] ) & 0xFF ) << " " << (int( palette[i][2] ) & 0xFF ) << '\n';  
	}
}

void print_fire( ostream& out )
{
	out << "P3\n" << FIRE_WIDTH << " " << FIRE_HEIGHT << "\n255\n";
	for( int i = FIRE_HEIGHT - 1; i >= 0; --i )
	{
		for( int j = 0; j != FIRE_WIDTH; ++j )
		{
			char idx = palette_idx[i][j];
			out << (int( palette[idx][0] ) & 0xFF ) << " " << (int( palette[idx][1] ) & 0xFF ) << " " << (int( palette[idx][2] ) & 0xFF ) << '\n';  
		}
	}
	
}

void generate_gradient()
{
	for( int i = 0; i != PALETTE_SIZE; ++i )
	{
		palette[i][0] = palette[i][1] = palette[i][2] = 0; 	
	}

	char step = 0xFF / 12;
	char acc = 0;
	for( int i = 0; i != 11; ++i )
	{
		palette[i][0] = acc; 	
		acc += step;
	}
	acc = step;
	palette[11][0] = 0xff;
	palette[11][1] = step;
	acc += step;
	for( int i = 12; i != 23; ++i )
	{
		palette[i][0] = 0xff; 		
		palette[i][1] = acc; 	
		acc += step;
	}

	acc = step;
	palette[23][0] = 0xff;
	palette[23][1] = 0xff;
	palette[23][2] = step;
	acc += step;
	for( int i = 24; i != 35; ++i )
	{
		palette[i][0] = palette[i][1] = 0xff; 		
		palette[i][2] = acc; 	
		acc += step;
	}
	palette[35][0] = palette[35][1] = palette[35][2] = 0xff;
}

void print_to_file( const char* filename )
{
	ofstream f( filename, ofstream::out ); // the frame	

	print_fire( f );

	f.close();
}

int main( int argc, const char* argv[] )
{
	//generate_gradient();
	init_fire();	
	int frame = 0;
	char filename[50];

	sprintf( filename, "%i.ppm", frame );
	print_to_file( filename );

	int its = 0;
	if( argc > 1 && sscanf( argv[1], "%i", &its ) && its > 0 )
	{
		int current_height = 1;
		const int RISE_ITS = 36;
		const int OUT_ITS = 72;
		// rise up the fire
		for( int i = 0; i != RISE_ITS; ++i, ++frame )
		{
			rise_up( current_height );
			sprintf( filename, "%i.ppm", frame );
			print_to_file( filename );
		}

		for( int i = 0; i != its; ++i, ++frame )
		{
			update_fire( );
			sprintf( filename, "%i.ppm", frame );
			print_to_file( filename );
		}

		// put out
		for( int i = 0; i != OUT_ITS; ++i, ++frame )
		{
			put_out( current_height );
			sprintf( filename, "%i.ppm", frame );
			print_to_file( filename );
		}

	}
	else
	{
		for( int x = 0; x != FIRE_WIDTH; ++x )
		{
			palette_idx[0][x] = PALETTE_SIZE - 1;
		}

		update_fire();
	}

	print_fire( cout );
	 
	return 0;
}
