/**
 * @author Fernández Nicolás (nicofernandez@alumnos.unc.edu.ar)
 * @date Mayo, 2017
 *
 * @brief Manejo de archivos: Lectura, búsqueda de caracteres, etc
 * 
 * \file Print.h
 */

#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

void Print_vector_int( int * vector , long unsigned int rows )
{
	
	long unsigned int row;
	for( row = 0 ; row < rows ; row++ )
		printf( "\n vector[%lu] = %i" , row , vector[row] );
	
}

void Print_matrix_int
( int ** matrix , long unsigned rows , long unsigned columns )
{
	
	long unsigned int row;
	long unsigned int column;
	for( row = 0 ; row < rows ; row++ )
		for( column = 0 ; column < columns ; column++ )
			printf( "\n matrix[%lu][%lu] = %i" ,
					 row ,
					 column ,
					 matrix[row][column] );
	
}

void Print_matrix_float
( float ** matrx , long unsigned rows , long unsigned columns )
{
	
	long unsigned int row;
	long unsigned int column;
	for( row = 0 ; row < rows ; row++ )
		for( column = 0 ; column < columns ; column++ )
			printf( "\n matrix[%lu][%lu] = %f" ,
					 row ,
					 column ,
					 matrx[row][column] );
	
}

void Print_vector_float
( float * vector , long unsigned int size , char * name )
{
	
	long unsigned int pos;
	for( pos = 0 ; pos < size ; pos++ )
		printf( "\n %s[%lu] = %f" ,
				 name ,
				 pos ,
				 vector[pos] );
	
}

#endif
