#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define N 16

#define SILENT_MODE
#define ENABLE_PAUSE

#define MASK_N ( (UI32) ( 1 << N ) - 1 )
#define UI64 unsigned __int64
#define UI32 unsigned int


UI64 nsol = 0; // # of solutions


void setqrec( int row, UI32 occ_v, UI32 occ_ld, UI32 occ_rd ) {
	UI32 occ, qc;
	
	if ( row == N ) {
		// last row, solution found if we get here
		nsol++;
		#if !defined SILENT_MODE
		fprintf( stderr, "solutions found: %I64d\r", nsol );
		#endif
	} else {
		occ = MASK_N & ~( occ_v | occ_ld | occ_rd );
		while ( occ ) {
			occ ^= qc = (-occ) & occ;
			setqrec( row + 1, occ_v | qc, ( occ_ld | qc ) << 1, ( occ_rd | qc ) >> 1 );
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
	setqrec( 0, 0, 0, 0 );
	/* for ( int col = 0; col < ( N / 2 ); col++ )
		setqrec( 1, ( 1 << col ), 1 << ( col + 1 ), 1 << ( col - 1 ) ); */
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
