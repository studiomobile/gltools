#import <Foundation/Foundation.h>

@interface MD2Model : NSObject {
@private
	NSMutableData *data;
	NSInteger totalFrames;
	NSInteger trianges;
	NSInteger vertBytesPerFrame;
	NSInteger vertOffset;
	NSInteger normOffset;
	NSInteger indexOffset;
	NSInteger texOffset;
}
@property (nonatomic, readonly) NSInteger totalFrames;

- (id)initWithModelFromFile:(NSString*)filePath;

- (void)renderFrame:(NSInteger)frame;
- (void)renderFrame:(NSInteger)frame swapSide:(BOOL)swap;

@end
