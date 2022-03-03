#include <GL/glut.h>
#include <gl/gl.h>
#include <stdio.h>

#include "../include/texloader.h"

#define BMP_Header_Length 54
int power_of_two(int n)
{
	if (n <= 0) return 0;
	return (n & (n-1)) == 0;
}
GLuint loadTexture(const char* file_name)
{
	FILE *pFile;
	GLubyte* pixels = 0;
	GLint width, height, total_bytes;
	GLuint last_texture_ID = 0, texture_ID = 0;
	if (! (pFile=fopen(file_name, "rb"))) return 0;
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0) line_bytes ++; // 24 位位图。
		total_bytes = line_bytes * height;
	}
	if(! (pixels=(GLubyte*)malloc(total_bytes)))
	{ fclose(pFile); return 0; }
	if (! fread(pixels, total_bytes, 1, pFile))
	{ free(pixels); fclose(pFile); return 0; }
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width) || !power_of_two(height) || width > max || height > max)
		{
			GLubyte* new_pixels = 0;
			GLint new_line_bytes, new_total_bytes;
			const GLint new_width = 256, new_height = 256;
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0) new_line_bytes ++;
			new_total_bytes = new_line_bytes * new_height;
			if (! (new_pixels=(GLubyte*)malloc(new_total_bytes)))
			{ free(pixels); fclose(pFile); return 0; }
			gluScaleImage(GL_RGB, width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0) { free(pixels); fclose(pFile); return 0; }
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);
	free(pixels); fclose(pFile);
	return texture_ID;
}