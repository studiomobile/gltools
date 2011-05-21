#import "Program.h"

@implementation Program

@synthesize binding;

- (id)initWithVertexShader:(Shader*)vsh fragmentShader:(Shader*)fsh
{
    self = [super init];
    if (self) {
        vertex = [vsh retain];
        fragment = [fsh retain];
    }
    return self;
}


+ (Program*)programWithVertexShader:(Shader*)vsh fragmetShader:(Shader*)fsh
{
    return [[[self alloc] initWithVertexShader:vsh fragmentShader:fsh] autorelease];
}


- (GLuint)bindWithAttributes:(NSDictionary*)attrs error:(NSError**)error
{
    if (!binding) {
        GLuint v = [vertex bind:error];
        if (!v) return 0;
        GLuint f = [fragment bind:error];
        if (!f) return 0;
        
        GLuint prog = glCreateProgram();
        glAttachShader(prog, v);
        glAttachShader(prog, f);
        
        for (NSString* attr in attrs) {
            NSNumber *index = [attrs objectForKey:attr];
            glBindAttribLocation(prog, [index unsignedIntValue], [attr cStringUsingEncoding:NSASCIIStringEncoding]);
        }
        
        glLinkProgram(prog);
        
        GLint status;
        glGetProgramiv(prog, GL_LINK_STATUS, &status);
        if (!status) {
            if (error) {
                NSString *message = @"Failed to link program";
                GLint logLength;
                glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0)
                {
                    GLchar *log = (GLchar *)malloc(logLength);
                    glGetProgramInfoLog(prog, logLength, &logLength, log);
                    message = [NSString stringWithFormat:@"%@: %s", message, log];
                    free(log);
                }
                *error = mkGLError(13, message);
            }
            return 0;
        }
        
        [vertex unbind];
        [fragment unbind];
        
        binding = prog;
    }
    if (error) *error = NULL;
    return binding;
}


- (void)unbind
{
    if (binding) {
        glDeleteProgram(binding);
        binding = 0;
    }
}


- (BOOL)validate:(NSError**)error
{
    if (!binding) {
        if (error) *error = mkGLError(10, @"Please bind program first");
        return FALSE;
    }

    glValidateProgram(binding);

    GLint logLength, status;
    glGetProgramiv(binding, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        if (error) {
            NSString *message = @"Program validation failed";
            glGetProgramiv(binding, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                GLchar *log = (GLchar *)malloc(logLength);
                glGetProgramInfoLog(binding, logLength, &logLength, log);
                message = [NSString stringWithFormat:@"%@: %s", message, log];
                free(log);
            }
            *error = mkGLError(5, message);
        }
        return FALSE;
    }
    
    return TRUE;
}


- (int)uniformLocationForName:(NSString*)name
{
    if (!binding) return -1;
    return glGetUniformLocation(binding, [name UTF8String]);
}


- (void)dealloc
{
    [vertex release];
    [fragment release];
    [self unbind];
    [super dealloc];
}

@end
