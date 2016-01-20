#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define N 16

// #define DUMP_SOLUTIONS
#define SILENT_MODE
// #define ENABLE_PAUSE

#define UI64 unsigned __int64
#define UI32 unsigned int

#define MASK_N ( (UI32) ( 1 << N ) - 1 )
#define TB     ( 1 << ( N - 1 ) )

#define CHESSQ " Q" // "\xDB\xDB"
#define CHESS0 " ." // "\xB0\xB0"
#define CHESS1 " ." // "\xB1\xB1"


UI64 nsol  = 0; // # of total solutions
UI64 nsol2 = 0; // # of unique solutions (2)
UI64 nsol4 = 0; // # of unique solutions (4)
UI64 nsol8 = 0; // # of unique solutions (8)
UI32 board[N]; // board configuration
int qx0, qx1;


void output_solution( void )
{
	fprintf( stderr, "solutions found: %I64d\r", nsol );
	
	#if defined DUMP_SOLUTIONS
		fprintf( stdout, "\n\n" );
		fprintf( stdout, "solution #%I64d", nsol );
		for ( int row = 0; row < N; row++ ) {
			fprintf( stdout, "\n" );
			fprintf( stdout, "%06i", board[ row ] );
			for ( int col = 0; col < N; col++ ) {
				fprintf( stdout, "%s", ( ( board[ row ] >> col ) & 1 ) ?
					CHESSQ : ( ( row + col ) % 2  ) ? CHESS0 : CHESS1 );
			}
		}
		fprintf( stdout, "\n\n" );
	#endif
}

bool check_solution( void ) {
	UI32* board1 = &(board[ N - 1 ]);
	UI32* boardb = &(board[ N ]);
	UI32* boardc;
	UI32 qc, q1;
	int row, qx;
	
	// queen-in-the-corner check
	if ( qx0 == 0 ) {
		nsol += 8;
		nsol8++;
		return true;
	}
	
	// 90-degree-rotation check
	if ( board[ qx0 ] == TB ) {
		for ( qc = TB >> 1; qc > 0; qc >>= 1 ) {
			for ( boardc = board + 1; ( boardc <= board1 ) && ( *boardc != qc ); boardc++ );
			if ( *boardc < qc ) return false;
			if ( *boardc > qc ) break;
		}
		if ( boardc == boardb ) {
			nsol += 2;
			nsol2++;
			return true;
		}
	}
	
	// 180-degree-rotation check
	if ( qx0 == ( N - qx1 - 1 ) ) {
		
	}
}

void setqrec( int row, UI32 occ_v, UI32 occ_ld, UI32 occ_rd, UI32* mask ) {
	UI32 free = (*mask) & ~( occ_v | occ_ld | occ_rd );
	UI32 qc;
	
	if ( row == ( N - 2 ) ) {
		// last row, solution found if we get here
		if ( free ) {
			nsol++;
			#if !defined SILENT_MODE
				board[ N - 2 ] = free;
				output_solution();
			#endif
		}
	} else {
		while ( free ) {
			free ^= qc = (-free) & free;
			board[ row ] = qc;
			setqrec( row + 1, occ_v | qc, ( occ_ld | qc ) << 1, ( occ_rd | qc ) >> 1, mask + 1 );
		}
	}
}

void nqueens( void ) {
	UI32* qmask = ( UI32* ) calloc( N, sizeof( UI32 ) );
	UI32 masks = MASK_N & ~( 1 | TB );
	UI32 occ_v, occ_ld, occ_rd;
	UI32 board0;
	int row;
	
	qmask[ 0 ] = qmask[ N - 1 ] = 0; // always the same for these two
	for ( qx0 = 0, board0 = 1; qx0 < ( N / 2 ); qx0++, board0 <<= 1 ) {
		for ( qx1 = qx0 + 1, occ_v = board0 << 1;
			( qx1 < ( N - qx0 ) ) && ( qx1 != ( N - 1 ) );
			qx1++, occ_v <<= 1 ) {
			// build mask
			for ( row = 1; row < ( N - 1 ); row++ )
				qmask[ row ] = ( ( row < qx0 ) || ( row >= ( N - qx0 ) ) ) ? masks : MASK_N;
			// add last row effects to mask
			if ( qx0 == 0 )	for ( row = 1; row <= qx1; row++ )
				qmask[ row ] &= ( MASK_N ^ TB ); 
			for ( row = N - 2, occ_ld = occ_rd = occ_v; row >= 0; row-- ) {
				occ_ld <<= 1; occ_rd >>= 1;
				qmask[ row ] &= ~( occ_v | occ_ld | occ_rd );
			}
			
			// for ( row = 0; row < N; row++ )
			//	fprintf( stderr, "\nmask[%i/%i][%i] = %i", qx0, qx1, row, qmask[ row ] );
			
			// start calculations
			board[ 0 ] = board0;
			board[ N - 1 ] = occ_v;
			setqrec( 1, board0, board0 << 1, board0 >> 1, qmask + 1 );
		}
	}
}

int main( int argc, char** argv ) {
	clock_t cycles;
	
	
	// output program info to screen
	fprintf( stderr, "\n" );
	fprintf( stderr, ">> %s <<\n", argv[ 0 ] );
	fprintf( stderr, "an approach to the NxN queens problem\n" );
	fprintf( stderr, "by Matthias Stirner\n" );
	fprintf( stderr, "size of board: %ix%i\n", N, N );
	fprintf( stderr, "no of queens: %i\n", N );
	
	// start processing
	cycles = clock();
	nqueens();
	cycles = clock() - cycles;
	
	fprintf( stderr, "solutions found: %I64d\n", nsol );
	fprintf( stderr, "finished in: %imin %isec!\n",
		(int) ( cycles / ( 60 * CLOCKS_PER_SEC ) ),
		(int) ( ( cycles / CLOCKS_PER_SEC ) % 60 ) );
	
	#if defined ENABLE_PAUSE
		// pause before exit
		fprintf( stderr, "\n\n" );
		fprintf( stderr, "< press ENTER >\n" );
		fgetc( stdin );
	#endif
}
