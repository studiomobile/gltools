#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface Texture : NSObject {
    NSMutableData *data;
    NSUInteger width;
    NSUInteger height;
}
@property (nonatomic, readonly) NSData *data;
@property (nonatomic, readonly) NSUInteger width;
@property (nonatomic, readonly) NSUInteger height;

- (void)loadFromImage:(UIImage*)image;
- (void)loadFromView:(UIView*)view withSize:(CGSize)size;

- (BOOL)writeGL;

@end
