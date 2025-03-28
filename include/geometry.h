#ifndef GEOMETRY_H
#define GEOMETRY_H

static float square_vdata[] = {
	 0.5f,  0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
};
static unsigned int square_idata[] = {
	0, 1, 2, 2, 3, 0
};

static float ncube_vdata[] = {
    // Create back face
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
    // left face
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // bottom face
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    // front face
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
    // right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    // top face
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
};
static int ncube_idata[] = {
     0,  3,  2,  2,  1,  0,  // Create back face
     4,  7,  5,  5,  7,  6,  // Create left face
     8, 11,  9,  9, 11, 10,  // Create bottom face
    12, 15, 13, 13, 15, 14,  // Create front face
    16, 19, 17, 17, 19, 18,  // Create right face
    20, 23, 21, 21, 23, 22   // Create top face
};

static float cube_vdata[] = {
	/* Position */       /* Colour */
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
};
static unsigned int cube_idata[] = {
	0, 1, 2, 2, 3, 0, // Front face 
	6, 5, 4, 4, 7, 6, // Back face
	1, 5, 6, 6, 2, 1, // Right face
	4, 0, 3, 3, 7, 4, // Left face
	3, 2, 6, 6, 7, 3, // Top face
	5, 1, 0, 0, 4, 5, // Bottom face
};

#endif
