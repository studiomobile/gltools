#import "Shader.h"

NSString *GL_ERROR_DOMAIN = @"GL_DOMAIN";

NSError* mkGLError(int code, NSString *message)
{
    return [NSError errorWithDomain:GL_ERROR_DOMAIN code:code userInfo:[NSDictionary dictionaryWithObject:message forKey:NSLocalizedDescriptionKey]];
}

@implementation Shader

@synthesize type;
@synthesize binding;

- (id)initWithType:(GLuint)shaderType source:(NSString*)shaderSource
{
    self = [super init];
    if (self) {
        type = shaderType;
        source = [shaderSource retain];
    }
    return self;
}

+ (Shader*)vertexShaderWithSource:(NSString*)source
{
    return [[[self alloc] initWithType:GL_VERTEX_SHADER source:source] autorelease];
}

+ (Shader*)fragmentShaderWithSource:(NSString*)source
{
    return [[[self alloc] initWithType:GL_FRAGMENT_SHADER source:source] autorelease];
}

- (GLuint)bind:(NSError**)error
{
    if (!binding) {
        const GLchar *s = (GLchar *)[source UTF8String];
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &s, NULL);
        glCompileShader(shader);
        
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            if (error) {
                NSString *message = @"Failed to compile shader";
                GLint logLength;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0) {
                    GLchar *log = (GLchar *)malloc(logLength);
                    glGetShaderInfoLog(shader, logLength, &logLength, log);
                    message = [NSString stringWithFormat:@"%@: %s", message, log];
                    free(log);
                }
                *error = mkGLError(status, message);
            }
            glDeleteShader(shader);
            return 0;
        }
        
        binding = shader;
    }
    if (error) *error = NULL;
    return binding;
}

- (void)unbind
{
    if (binding) {
        glDeleteShader(binding);
        binding = 0;
    }
}

- (void)dealloc
{
    [source release];
    [self unbind];
    [super dealloc];
}

@end
