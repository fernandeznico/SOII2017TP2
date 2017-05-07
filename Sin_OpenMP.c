#include <stdio.h>
#include <stdint.h>

#include "./Recursos/Mem.h"

const char * F_LOC = "pulsos.iq";

unsigned int Cantidad_de_datos( unsigned int * mayor_columnas )
{
	
	FILE * archivo = fopen( F_LOC , "r" );
	if( archivo == NULL )
		return 0;
	
	*mayor_columnas = 0;
	
	unsigned int cantidad = 0;
	while( 1 ) /// @todo riesgo de bucle infinito
	{
		
		uint16_t validSamples = 0;
		if( fread( &validSamples , 1 , 2 , archivo ) != 2 )
			break;
		
		cantidad++;
		
		unsigned int columnas = (int)( validSamples / 500 ) / 500;
		if( *mayor_columnas < columnas )
			*mayor_columnas = columnas;
		
		fseek( archivo , 4 * (validSamples * 4) , SEEK_CUR );
		
	}
	
	fclose( archivo );
	return cantidad;
	
}

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

unsigned int Carga_de_datos( matrix ** datos )
{
	
	FILE * archivo = fopen( F_LOC , "r" );
	if( archivo == NULL )
		return 0;
	
	unsigned int gate_nro = 0;
	while( 1 ) /// @todo riesgo de bucle infinito
	{
		
		uint16_t validSamples = 0;
		if( fread( &validSamples , 1 , 2 , archivo ) != 2 )
			return gate_nro;
		
		gate_nro++;
		unsigned int pulso_n = validSamples / 500;
		unsigned int pulsos = validSamples / pulso_n;
		
		if( pulsos < datos[0]->columns );
			Hacer_ceros_a_partir_de_pulsos( datos , pulsos , gate_nro );
			///@todo debido a que la matriz no es cuadrada. Hacer que
			///Mem.h de una base confiable para matrces de columnas
			///dinámicas
		
		///@todo SEGUIR ACA
		
	}
	
}

int main()
{
	
	unsigned int columnas = 0;
	unsigned int nro_gates = Cantidad_de_datos( &columnas );
	printf( "\n nro_gates = %u" , nro_gates );
	
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
	
	//Carga_de_datos( datos );
	
	Mem_Delete_matrix( &V_I );
	Mem_Delete_matrix( &V_Q );
	Mem_Delete_matrix( &H_I );
	Mem_Delete_matrix( &H_Q );
	free( datos );
	datos = NULL;
	
	printf( "\n" );
	
	return 0;
	
}
