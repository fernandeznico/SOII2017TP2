/**
 * @author Fernández Nicolás (nicofernandez@alumnos.unc.edu.ar)
 * @date Mayo, 2017
 *
 * @brief Manejo de memoria, uso de las funciones malloc y free
 * para crear y liberar matrices
 *
 * \file Mem.h
 */

#ifndef MEM_H
#define MEM_H

#include <stdio.h>
#include <stdlib.h> //exit

typedef struct {
	
	void ** m;
	long unsigned int rows;
	long unsigned int columns;
	size_t type_size;
	
} matrix;

/**
 * @brief Asigna memoria mediante malloc y comprueba, en caso de fallar
 * advierte y cierrra el programa
 *
 * @param s cantidad a asignar
 *
 * @return el retorno de malloc( s )
 */
void * Mem_assign( size_t s )
{
	
	void * ptr = malloc( s );
	
	if( ptr != NULL )
		return ptr;
	
	printf( "\n --- MEMORY ASSIGNMENT FAULT ---  \n" );
	exit( 1 );
	
}

/**
 * @brief Libera la memoria mediante free y apunta a null
 *
 * @param p puntero a liberar
 *
 * @warning pasar por referencia o *p = NULL no tendrá efecto
 */
 void Mem_desassign( void ** p )
{
	
	free( *p );
	*p = NULL;
	
}

void * Mem_assign_vector( unsigned int rows , size_t type_size )
{
	
	if( rows == 0 )
		return NULL;
	
	void * vector = Mem_assign( rows * type_size );
	
	return vector;
	
}

void ** Mem_assign_matrix
( unsigned int rows , unsigned int columns , size_t type_size )
{
	
	void ** p_p = (void **)Mem_assign( rows * sizeof(void *) );
	unsigned int row;
	for( row = 0 ; row < rows ; row++ )
		p_p[row] = Mem_assign( columns * type_size );
	
	return p_p;
	
}

void Mem_desassign_matrix( void *** matrx , unsigned int rows )
{
	
	unsigned int row;
	for( row = 0 ; row < rows ; row++ )
		Mem_desassign( (void *)&( (*matrx)[row] ) );
	
	Mem_desassign( *matrx );
	
}

matrix * Mem_Create_matrix
( unsigned int rows , unsigned int columns , size_t type_size )
{
	
	matrix * m = Mem_assign( sizeof(matrix) );
	
	m->m = Mem_assign_matrix( rows , columns , type_size );
	
	m->rows = rows;
	m->columns = columns;
	m->type_size = type_size;
	
	return m;
	
}

void Mem_Delete_matrix( matrix ** m )
{
	
	Mem_desassign_matrix( (void ***)&( (*m)->m ) , (*m)->rows );
	Mem_desassign( (void *)m );
	///@todo desasigna (void) o (matrix) ???
	///El test no llega a tener problemas de memoria de momento
	
}

#define TEST_MEM_H 1

#if TEST_MEM_H
#include "Print.h"

void Mem_Test_load_vector( int * vector , unsigned int rows )
{
	
	unsigned int row;
	for( row = 0 ; row < rows ; row++ )
		vector[row] = rows - row;
	
}

void Mem_Test_load_matrix
( int ** matrx , unsigned int rows , unsigned int columns )
{
	
	unsigned int row;
	unsigned int column;
	for( row = 0 ; row < rows ; row++ )
		for( column = 0 ; column < columns ; column++ )
			matrx[row][column] = (rows * columns) -
								 (columns * row) -
								  column;
	
}

void Mem_Test( )
{
	
	int ** matrx = NULL;
	
	unsigned int repetitions = 51984651;
	unsigned int repetition;
	for( repetition = 0 ; repetition < repetitions ; repetition++ )
	{
		
		unsigned int rows = (rand() % 5000) + 1;
		unsigned int columns = (rand() % 5000) + 1;
		printf( "\n ( %u , %u )" , rows , columns );
		
		matrx = (int **)Mem_assign_matrix( rows ,
										   columns ,
										   sizeof(int) );
		
		Mem_Test_load_matrix( matrx , rows , columns );
		
		//Print_matrix_int( matrix , rows , columns );
		
		Mem_desassign_matrix( (void ***)&matrx , rows );
		
	}
	
	printf( "\n" );
	
}

void Mem_Test_Struct_Matrix( )
{
	
	matrix * m = NULL;
	
	unsigned int repetitions = 51984651;
	unsigned int repetition;
	for( repetition = 0 ; repetition < repetitions ; repetition++ )
	{
		
		unsigned int rows = (rand() % 5000) + 1;
		unsigned int columns = (rand() % 5000) + 1;
		printf( "\n ( %u , %u )" , rows , columns );
		
		m = Mem_Create_matrix( rows , columns , sizeof(int) );
		
		int ** p_m = (int **)m->m;
		Mem_Test_load_matrix( p_m , m->rows , m->columns );
		
		//Print_matrix_int( matrix , rows , columns );
		
		Mem_Delete_matrix( &m );
		
	}
	
	printf( "\n" );
	
}
#endif

#endif
