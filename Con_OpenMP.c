/**
* @author Fernández Nicolás (nicofernandez@alumnos.unc.edu.ar)
* @date Mayo, 2017
* @version b0.1
* 
* \file Con_OpenMP.c
*/

#include <stdio.h>
#include <stdint.h> //uint16_t
#include <math.h> //sqrt()
#include <time.h> //clock() - CLOCKS_PER_SEC
#include <omp.h>

#include "./Recursos/Mem.h"
#include "./Recursos/Print.h"

const char * F_LOC = "pulsos.iq";
const char * F_LOC_SAVE = "res_omp.iq";

/**
 * @brief Busca de forma rapida la cantidad de gates en el archivo y el
 * mayor numero de pulsos
 *
 * @param mayor_columnas mayor cantidad de pulsos
 *
 * @return cantidad de gates en el archivo
 */
unsigned int Cantidad_de_datos( unsigned int * mayor_columnas )
{
	
	FILE * archivo = fopen( F_LOC , "rb" );
	if( archivo == NULL )
		return 0;
	
	*mayor_columnas = 0;
	
	unsigned int cantidad = 0;
	while( 1 ) /// @todo @bug riesgo de bucle infinito
	{
		
		uint16_t validSamples = 0;
		if( fread( &validSamples , 1 , 2 , archivo ) != 2 )
			break;
		
		cantidad++;
		unsigned int pulsos_n = validSamples / 500;
		unsigned int columnas = validSamples / pulsos_n;
		if( *mayor_columnas < columnas )
			*mayor_columnas = columnas;
		
		fseek( archivo , sizeof(float) * (validSamples * 4) , SEEK_CUR );
		
	}
	
	fclose( archivo );
	
	return cantidad;
	
}

/**
 * @brief @bug En desuso
 */
void Hacer_ceros_a_partir_de_pulsos
( matrix ** datos , unsigned int pulsos , unsigned int gate_nro )
{
	
	unsigned int pos;
	unsigned int matriz_nro;
	for( matriz_nro = 0 ; matriz_nro < 4 ; matriz_nro++ )
		for( pos = 0 ; pos < datos[0]->columns ; pos++ )
		{
			
			float ** matriz;
			matriz = (float **)datos[matriz_nro]->m;
			matriz[gate_nro][pulsos + pos] = 0;
			
		}
	
}

/**
 * @brief Levanta los datos del archivo a cuatro matrices V_I, V_Q, H_I 
 * y H_Q
 *
 * @param datos contiene las 4 matrices en orden anterior
 *
 * @return cantidad de gates levantadas, para comprobar
 */
unsigned int Carga_de_datos( matrix ** datos )
{
	
	FILE * archivo = fopen( F_LOC , "rb" );
	if( archivo == NULL )
		return 0;
	
	unsigned int gate_nro = 0;
	while( 1 ) /// @todo @bug riesgo de bucle infinito
	{
		
		uint16_t validSamples = 0;
		if( fread( &validSamples , 1 , 2 , archivo ) != 2 )
			return gate_nro;
		///@todo @bug los return no cierran el archivo
		
		long int pos_arch = ftell( archivo );
		
		gate_nro++;
		unsigned int pulso_n = validSamples / 500;
		unsigned int pulsos = validSamples / pulso_n;
		
		/*if( pulsos < datos[0]->columns )
			Hacer_ceros_a_partir_de_pulsos( datos , pulsos , gate_nro - 1 );*/
			///@todo debido a que la matriz no es cuadrada. Hacer que
			///Mem.h de una base confiable para matrces de columnas
			///dinámicas ADEMAS FALLA, HACER QUE FUNCIONE
		
		unsigned int pulso , pos , matriz_nro;
		for( pulso = 0 ; pulso < pulsos ; pulso++ )
		{
			
			float sumatoria[4] = { 0 , 0 , 0 , 0 };
			for( pos = 0 ; pos < pulso_n ; pos++ )
				for( matriz_nro = 0 ; matriz_nro < 4 ; matriz_nro++ )
				{
					
					float valor = 0;
					size_t s = sizeof( float );
					if( fread( &valor , 1 , s , archivo ) != s )
						return 0;
					sumatoria[matriz_nro] += valor;
					
				}
			for( matriz_nro = 0 ; matriz_nro < 4 ; matriz_nro++ )
			{
				
				float ** pm = (float **)datos[matriz_nro]->m;
				float promedio = sumatoria[matriz_nro] / (float)pulso_n;
				pm[gate_nro - 1][pulso] = promedio;
				
			}
			
		}
		
		fseek( archivo , pos_arch , SEEK_SET );
		fseek( archivo ,
			   sizeof(float) * (long int)validSamples * 4 ,
			   SEEK_CUR );
		
	}//while
	
}

/**
 * @brief Pasa las matrices V_I/H_I y V_Q/H_Q a una con los valores
 * absolutos correspondientes
 * 
 * @param r matriz con parte real V_I/H_I
 * @param r matriz con parte compleja V_Q/H_Q
 *
 * @return matriz con los valores absolutos correspondientes
 */
matrix * Matriz_de_valores_absolutos( matrix * r , matrix * i )
{
	
	matrix * abs = Mem_Create_matrix( r->rows ,
									  r->columns ,
									  r->type_size );
	
	float ** reales = (float **)r->m;
	float ** imaginarios = (float **)i->m;
	float ** valores_absolutos = (float **)abs->m;
	
	unsigned int columna;
	unsigned int fila;
	for( fila = 0 ; fila < abs->rows ; fila++ )
		for( columna = 0 ; columna < abs->columns ; columna++ )
		{
			
			float valor_abs = sqrt( pow(reales[fila][columna] , 2) +
									pow(imaginarios[fila][columna] , 2)
									 );
			valores_absolutos[fila][columna] = valor_abs;
			
		}
	
	return abs;
	
}

/**
 * @brief Calcula la autocorrelacion de cada fila de una matriz de datos
 * 
 * @param M_abs matriz que contiene los valores absolutos a operar
 *
 * @return vector contenedor de los resultados por cada gate
 */
float * Vector_de_autocorrelaciones( matrix * M_abs )
{
	
	float * autoc = Mem_assign_vector_zeros( M_abs->rows , sizeof(float) );
	float ** abs = (float **)M_abs->m;
	
	unsigned int pulsos = M_abs->columns;
	unsigned int gates = M_abs->rows;
	
	unsigned int gate_nro;
	unsigned int pulso_nro;
	
	#ifdef _OPENMP
	double tiempo_inicial_omp = omp_get_wtime();
	#endif
	
#pragma omp parallel for schedule(static) default(none) \
private(gate_nro,pulso_nro) shared(gates,pulsos,abs,autoc)
	
	for( gate_nro = 0 ; gate_nro < gates ; gate_nro++ )
	{
		
		for( pulso_nro = 0 ; pulso_nro < (pulsos - 1) ; pulso_nro++ )
		{
			
			autoc[gate_nro] += abs[gate_nro][pulso_nro] *
							  (-1) *
							  abs[gate_nro][pulso_nro + 1];
			
		}
		
		autoc[gate_nro] /= pulsos;
		
	}
	
	#ifdef _OPENMP
	double tiempo_final_omp = omp_get_wtime();
	double tiempo_omp = ( tiempo_final_omp - tiempo_inicial_omp );
	printf( "\n omp: t = %f''" , tiempo_omp );
	#endif
	
	return autoc;
	
}

/**
 * @brief guarda las correlaciones calculadas para V y H en un archivo
 * de nombre "res_omp.iq"
 * 
 * FORMATO:
 * unsigned int gates
 * float V[0]
 * float H[0]
 * ...
 * float V[gates]
 * float H[gates]
 * 
 * @param V matriz V
 * @param H matriz H
 * @param gates tamaño de las matrices V y H
 */
void Guardar( float * V , float * H , unsigned int gates )
{
	
	FILE * archivo = fopen( F_LOC_SAVE , "wb" );
	if( archivo == NULL )
		return;
	
	fwrite( &gates , 1 , sizeof( unsigned int ) , archivo );
	
	unsigned int gate;
	for( gate = 0 ; gate < gates ; gate++ )
	{
		
		fwrite( &V[gate] , 1 , sizeof( float ) , archivo );
		fwrite( &H[gate] , 1 , sizeof( float ) , archivo );
		
	}
	
	fclose( archivo );
	
}

int main()
{
	
	#ifdef _OPENMP
	int hilos = omp_get_num_procs();
	printf( " Numero de procesadores disponibles = %i \n" , hilos );
	omp_set_num_threads(hilos);
	#endif
	
	unsigned int columnas = 0;
	unsigned int nro_gates = Cantidad_de_datos( &columnas );
	
	matrix * V_I = Mem_Create_matrix( nro_gates ,
									  columnas ,
									  sizeof(float) );
	matrix * V_Q = Mem_Create_matrix( nro_gates ,
									  columnas ,
									  sizeof(float) );
	matrix * H_I = Mem_Create_matrix( nro_gates ,
									  columnas ,
									  sizeof(float) );
	matrix * H_Q = Mem_Create_matrix( nro_gates ,
									  columnas ,
									  sizeof(float) );
	matrix ** datos = Mem_assign( sizeof(matrix *) * 4 );
	datos[0] = V_I;
	datos[1] = V_Q;
	datos[2] = H_I;
	datos[3] = H_Q;
	
	unsigned int num_datos = Carga_de_datos( datos );
	if( num_datos != nro_gates )
	{
		printf( "\n --- ERROR EN LA CARGA DE DATOS --- ");
		return 1;
	}
	
	matrix * V_abs = Matriz_de_valores_absolutos( V_I , V_Q );
	matrix * H_abs = Matriz_de_valores_absolutos( H_I , H_Q );
	
	Mem_Delete_matrix( &V_I );
	Mem_Delete_matrix( &V_Q );
	Mem_Delete_matrix( &H_I );
	Mem_Delete_matrix( &H_Q );
	free( datos );
	datos = NULL;
	
	float * V_autoc = Vector_de_autocorrelaciones( V_abs );
	float * H_autoc = Vector_de_autocorrelaciones( V_abs );
	
	Mem_Delete_matrix( &V_abs );
	Mem_Delete_matrix( &H_abs );
	
	Guardar( V_autoc , H_autoc , nro_gates );
	
	///Libero la memoria
	///@todo no sería necesario si va a finalizar el progama
	Mem_desassign( (void **)&V_autoc );
	Mem_desassign( (void **)&H_autoc );
	
	printf( "\n" );
	
	return 0;
	
}
