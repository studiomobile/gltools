#import "MD2Model.h"
#import <OpenGLES/ES1/gl.h>
#import "md2.h"

@implementation MD2Model

@synthesize totalFrames;

- (id)initWithModelFromFile:(NSString*)filePath {
    if (![super init]) return nil;

    md2_model_t model;
    memset(&model, 0, sizeof(model));

    BOOL loaded = MD2ReadModel([filePath cStringUsingEncoding:NSUTF8StringEncoding], &model);
    if (!loaded) {
        [self release];
        return nil;
    }

	totalFrames = model.header.num_frames;
	trianges = model.header.num_tris;
	vertBytesPerFrame = model.header.num_vertices * 3 * sizeof(GLfloat);
	size_t texBytes = model.header.num_vertices * 2 * sizeof(GLfloat);
	size_t indBytes = model.header.num_tris * 3 * sizeof(GLushort);
	size_t totalVertBytes = vertBytesPerFrame * model.header.num_frames;

	vertOffset  = 0;
	normOffset  = vertOffset + totalVertBytes;
	texOffset   = normOffset + totalVertBytes;
	indexOffset = texOffset  + texBytes;

	data = [[NSMutableData alloc] initWithLength:totalVertBytes + totalVertBytes + texBytes + indBytes];
	if (!data) {
		MD2FreeModel(&model);
		[self release];
		return nil;
	}

	uint8_t *dataPtr = data.mutableBytes;

	for (size_t f = 0; f < model.header.num_frames; f++) {
		GLfloat *vertPtr = (GLfloat*)(dataPtr + f * vertBytesPerFrame + vertOffset);
		GLfloat *normPtr = (GLfloat*)(dataPtr + f * vertBytesPerFrame + normOffset);

		md2_frame_t *frame = &model.frames[f];
		for (size_t i = 0, offset = 0; i < model.header.num_vertices; i++, offset+=3) {
			md2_vertex_t *vertex = &frame->verts[i];
			const GLfloat *norm = normals_table[vertex->normalIndex];

			normPtr[offset + 0] = norm[0];
			normPtr[offset + 1] = norm[1];
			normPtr[offset + 2] = norm[2];

			vertPtr[offset + 0] = (frame->scale[0] * (GLfloat)vertex->v[0]) + frame->translate[0];
			vertPtr[offset + 1] = (frame->scale[1] * (GLfloat)vertex->v[1]) + frame->translate[1];
			vertPtr[offset + 2] = (frame->scale[2] * (GLfloat)vertex->v[2]) + frame->translate[2];
		}
	}

	GLushort *uvIdx = malloc(sizeof(GLushort) * model.header.num_vertices);

	GLushort *indexPtr = (GLushort*)(dataPtr + indexOffset);
	for (size_t i = 0, offset = 0; i < model.header.num_tris; i++, offset+=3) {
        md2_triangle_t *tri = &model.triangles[i];

		indexPtr[offset + 0] = tri->vertex[0];
		indexPtr[offset + 1] = tri->vertex[1];
		indexPtr[offset + 2] = tri->vertex[2];

		uvIdx[tri->vertex[0]] = tri->st[0];
		uvIdx[tri->vertex[1]] = tri->st[1];
		uvIdx[tri->vertex[2]] = tri->st[2];
	}

	GLfloat *texPtr = (GLfloat*)(dataPtr + texOffset);
	for (size_t i = 0, offset = 0; i < model.header.num_vertices; i++, offset+=2) {
		md2_texCoord_t textCoordsCompacted = model.texcoords[uvIdx[i]];
		texPtr[offset + 0] = (GLfloat)textCoordsCompacted.s / model.header.skinwidth;
		texPtr[offset + 1] = (GLfloat)textCoordsCompacted.t / model.header.skinheight;
	}

	free(uvIdx);
    MD2FreeModel(&model);

    return self;
}


- (void)renderFrame:(NSInteger)frame {
	[self renderFrame:frame swapSide:NO];
}


- (void)renderFrame:(NSInteger)frame swapSide:(BOOL)swap {
	if (frame < 0 || frame >= totalFrames) return;

	uint8_t *dataPtr = data.mutableBytes;
	GLfloat *vertPtr = (GLfloat*)(dataPtr + frame * vertBytesPerFrame + vertOffset);
	GLfloat *normPtr = (GLfloat*)(dataPtr + frame * vertBytesPerFrame + normOffset);
	GLfloat *texPtr  = (GLfloat*)(dataPtr + texOffset);
	GLushort *idxPtr = (GLushort*)(dataPtr + indexOffset);

	GLint face = 0;
	glGetIntegerv(GL_FRONT_FACE, &face);

    // MD2 triangles are clockwise
	GLint modelFace = swap ? GL_CCW : GL_CW;
	if (face != modelFace) {
		glFrontFace(modelFace);
	}

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertPtr);
    glNormalPointer(GL_FLOAT, 0, normPtr);
    glTexCoordPointer(2, GL_FLOAT, 0, texPtr);

    glDrawElements(GL_TRIANGLES, 3 * trianges, GL_UNSIGNED_SHORT, idxPtr);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (face != modelFace) {
		glFrontFace(face);
	}
}


- (void)dealloc {
	[data release];
    [super dealloc];
}


@end
