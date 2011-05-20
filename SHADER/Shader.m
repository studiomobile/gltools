#import "Shader.h"

NSString *GL_ERROR_DOMAIN = @"GL_DOMAIN";

NSError* mkGLError(int code, NSString *message)
{
    return [NSError errorWithDomain:GL_ERROR_DOMAIN code:code userInfo:[NSDictionary dictionaryWithObject:message forKey:NSLocalizedDescriptionKey]];
}

@implementation Shader

@synthesize type;
@synthesize binding;

- (id)initWithType:(GLuint)shaderType path:(NSString*)shaderPath
{
    self = [super init];
    if (self) {
        type = shaderType;
        path = shaderPath;
    }
    return self;
}

+ (Shader*)vertexShaderWithFile:(NSString*)path
{
    return [[[self alloc] initWithType:GL_VERTEX_SHADER path:path] autorelease];
}

+ (Shader*)fragmentShaderWithFile:(NSString*)path
{
    return [[[self alloc] initWithType:GL_FRAGMENT_SHADER path:path] autorelease];
}

- (GLuint)bind:(NSError**)error
{
    if (!binding) {
        const GLchar *source = (GLchar *)[[NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:error] UTF8String];
        if (!source) return 0;
        
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
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
    [path release];
    [self unbind];
    [super dealloc];
}

@end
