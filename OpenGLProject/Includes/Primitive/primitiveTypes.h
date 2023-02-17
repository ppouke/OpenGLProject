#pragma once



enum PrimitiveType {
	Plane, Box, Sphere
};


void GetPrimitive(PrimitiveType primitiveType, float* vertices[], unsigned int indices[]) {

	switch (primitiveType)
	{
	case Plane:

		*vertices = {
			// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		*indices = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		break;
	case Box:
		break;
	case Sphere:
		break;
	default:
		break;
	}

}


