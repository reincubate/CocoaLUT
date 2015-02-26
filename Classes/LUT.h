//
//  LUT.h
//  DropLUT
//
//  Created by Wil Gieseler on 12/15/13.
//  Copyright (c) 2013 Wil Gieseler. All rights reserved.
//

#import <Foundation/Foundation.h>
#if defined(COCOAPODS_POD_AVAILABLE_GPUImage)
#import <GPUImage/GPUImage.h>
#import "GPUImageCocoaLUTFilter.h"
#endif

@class LUTFormatter;
@class LUTColor;


typedef NS_ENUM(NSInteger, LUT1DSwizzleChannelsMethod) {
    LUT1DSwizzleChannelsMethodAverageRGB,
    LUT1DSwizzleChannelsMethodRec709WeightedRGB,
    LUT1DSwizzleChannelsMethodEdgesRGB,
    LUT1DSwizzleChannelsMethodRedCopiedToRGB,
    LUT1DSwizzleChannelsMethodGreenCopiedToRGB,
    LUT1DSwizzleChannelsMethodBlueCopiedToRGB
};

typedef NS_ENUM(NSInteger, LUT1DReverseStrictnessType) {
    LUT1DReverseStrictnessTypeStrict,
    LUT1DReverseStrictnessTypeAllowFlatSections,
    LUT1DReverseStrictnessTypeAllowChangeInDirection
};

typedef NS_ENUM(NSInteger, LUTDataType) {
    LUTDataTypeRGBAf,
    LUTDataTypeRGBd
};

typedef NS_ENUM(NSInteger, LUTImageRenderPath) {
    LUTImageRenderPathCoreImage,
    LUTImageRenderPathCoreImageSoftware,
    LUTImageRenderPathDirect
};

/**
 *  A three-dimensional color lookup table.
 */
@interface LUT : NSObject <NSCopying, NSCoding>

/**
 *  A lattice that represents the code values of the look up table.
 */

@property (strong) NSString *title;
@property (strong) NSString *descriptionText;

@property (assign) NSUInteger size;
@property (assign) double inputLowerBound;
@property (assign) double inputUpperBound;


/**
 *  A catch-all swap space for arbitrary data that needs to be carried along with the LUT.
 *  Anything goes in here.
 */
@property (strong) NSMutableDictionary *userInfo;

/**
 *  Metadata from the LUT file (empty if LUT was created programatically).
 */
@property (strong) NSMutableDictionary *metadata;

/**
 *  LUT File specific settings such as the integer output depth (for integer luts) and the variant type of the file (ex: .3dl has a Nuke and Lustre type). This shouldn't be modified after setting.
 */
@property (strong) NSDictionary *passthroughFileOptions;

/**
 *  Returns a new `LUT` by reading the contents of a file represented by a file URL. It will automatically detect the type of LUT file format.
 *
 *  @param url A file URL.
 *  @param error The error object (if an error occured).
 *
 *  @return A new `LUT` with the contents of url.
 */
+ (instancetype)LUTFromURL:(NSURL *)url
                     error:(NSError * __autoreleasing *)error;

/**
 *	Loads a `LUT` from NSData, using the formatter that handles a specific extension
 *
 *	@param data      The data to load
 *	@param formatterID The formatter ID for selecting the correct formatter
 *  @param error The error object (if an error occured).
 *
 *  @return A new `LUT` with the contents of the data.
 */
+ (instancetype)LUTFromData:(NSData *)data
                formatterID:(NSString *)formatterID
                      error:(NSError * __autoreleasing *)error;

+ (instancetype)LUTFromBitmapData:(NSData *)data
                      LUTDataType:(LUTDataType)lutDataType
                  inputLowerBound:(double)inputLowerBound
                  inputUpperBound:(double)inputUpperBound;

+ (instancetype)LUTFromDataRepresentation:(NSData *)data;

- (NSData *)dataRepresentation;

- (BOOL)writeToURL:(NSURL *)url
        atomically:(BOOL)atomically
       formatterID:(NSString *)formatterID
           options:(NSDictionary *)options
        conformLUT:(BOOL)conformLUT;

- (NSData *)dataFromLUTWithFormatterID:(NSString *)formatterID
                               options:(NSDictionary *)options;

- (instancetype)initWithSize:(NSUInteger)size
             inputLowerBound:(double)inputLowerBound
             inputUpperBound:(double)inputUpperBound;

+ (instancetype)LUTOfSize:(NSUInteger)size
          inputLowerBound:(double)inputLowerBound
          inputUpperBound:(double)inputUpperBound;


/**
 *  Returns a new `LUT` that maps input colors to output colors with no transformation.
 *
 *  @param size The length of one edge of the cube for the new LUT.
 *
 *  @return A new `LUT` with a lattice size of size.
 */
+ (instancetype)LUTIdentityOfSize:(NSUInteger)size
                  inputLowerBound:(double)inputLowerBound
                  inputUpperBound:(double)inputUpperBound;

- (void)copyMetaPropertiesFromLUT:(LUT *)lut;

- (void) LUTLoopWithBlock:( void ( ^ )(size_t r, size_t g, size_t b) )block;

/**
 *  Returns a new `LUT` with a specified edge size. The LUT generated by tetrahedrally interpolating the receiver's lattice.
 *
 *  @param newSize The length of one edge of the cube for the new LUT.
 *
 *  @return A new LUT with a lattice of size newSize.
 */
- (instancetype)LUTByResizingToSize:(NSUInteger)newSize;


/**
 *  Returns a new `LUT` that is the original LUT of original size combined with otherLUT.
 *
 *  @param otherLUT The LUT to apply to the current lut.
 *
 *  @return A new LUT with the same lattice size as self.lattice.size.
 */
- (LUT *)LUTByCombiningWithLUT:(LUT *)otherLUT;



- (instancetype)LUTByClampingLowerBound:(double)lowerBound
                             upperBound:(double)upperBound;

- (instancetype)LUTByClampingLowerBoundOnly:(double)lowerBound;

- (instancetype)LUTByClampingUpperBoundOnly:(double)upperBound;

- (instancetype)LUTByOffsettingWithColor:(LUTColor *)offsetColor;

- (instancetype)LUTByRemappingValuesWithInputLow:(double)inputLow
                                       inputHigh:(double)inputHigh
                                       outputLow:(double)outputLow
                                      outputHigh:(double)outputHigh
                                         bounded:(BOOL)bounded;

- (instancetype)LUTByRemappingFromInputLowColor:(LUTColor *)inputLowColor
                                      inputHigh:(LUTColor *)inputHighColor
                                      outputLow:(LUTColor *)outputLowColor
                                     outputHigh:(LUTColor *)outputHighColor
                                        bounded:(BOOL)bounded;

- (instancetype)LUTByMultiplyingByColor:(LUTColor *)color;

- (instancetype)LUTByChangingStrength:(double)strength;

- (instancetype)LUTByChangingInputLowerBound:(double)inputLowerBound inputUpperBound:(double)inputUpperBound;

- (instancetype)LUTByInvertingColor;

- (bool)equalsIdentityLUT;

- (bool)equalsLUT:(LUT *)comparisonLUT;

- (bool)equalsLUTEssence:(LUT *)comparisonLUT
             compareType:(bool)compareType
             compareSize:(bool)compareSize
      compareInputBounds:(bool)compareInputBounds;

- (LUTColor *)symetricalMeanAbsolutePercentageError:(LUT *)comparisonLUT;
- (LUTColor *)maximumAbsoluteError:(LUT *)comparisonLUT;
- (LUTColor *)averageAbsoluteError:(LUT *)comparisonLUT;

- (LUTColor *)identityColorAtR:(double)redPoint g:(double)greenPoint b:(double)bluePoint;

- (LUTColor *)colorAtColor:(LUTColor *)color;

- (LUTColor *)colorAtR:(NSUInteger)r g:(NSUInteger)g b:(NSUInteger)b;

- (LUTColor *)colorAtInterpolatedR:(double)redPoint g:(double)greenPoint b:(double)bluePoint;

- (LUTColor *)indexForColor:(LUTColor *)color;

- (LUTColor *)maximumOutputColor;
- (LUTColor *)minimumOutputColor;

- (double)maximumOutputValue;
- (double)minimumOutputValue;

- (void)setColor:(LUTColor *)color r:(NSUInteger)r g:(NSUInteger)g b:(NSUInteger)b;

/**
 *  Returns a Core Image filter that will apply the receiver's transformation in a particular color space.
 *
 *  @param colorSpace The desired color space to use on the filter. Pass `nil` to get a `CIFilter` of type `CIColorCube`.
 *
 *  @return A CIFilter of type `CIColorCubeWithColorSpace`.
 */
- (CIFilter *)coreImageFilterWithColorSpace:(CGColorSpaceRef)colorSpace;

/**
 *  Returns a Core Image filter that will apply the receiver's transformation in the color space of the main screen.
 *
 *  Due to limitations of Core Image, if the receiver has a lattice size larger than 64, the returned CIFilter will represent a scaled version of the LUT as scaled by LUTByResizingToSize:
 *
 *  @return A CIFilter of type `CIColorCubeWithColorSpace` or `CIColorCube`.
 */
- (CIFilter *)coreImageFilterWithCurrentColorSpace;

/**
 *	Essentially returns a bitmap image of the LUT with the specified LUTDataType.
 *
 *	@return A NSData object with bitmap data of the LUT (red major if 3D)
 **/
- (NSData *)bitmapDataWithType:(LUTDataType)lutDataType;


/**
 *  Returns a `CIImage` with the receiver's color transformation applied.
 *
 *  @param image      The input `CIImage` you wish transform.
 *
 *  @return A `CIImage` with a `CIFilter` applied that represents the receiver's color transformation.
 */
- (CIImage *)processCIImage:(CIImage *)image;

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
/**
 *  Returns a `UIImage` with the receiver's color transformation applied.
 *
 *  @param image      The input `UIImage` you wish transform.
 *  @param colorSpace The desired color space to use on the filter. Pass `nil` to apply the transformation without color management.
 *
 *  @return A UIImage with the receiver's color transformation applied.
 */
- (UIImage *)processUIImage:(UIImage *)image withColorSpace:(CGColorSpaceRef)colorSpace;
#elif TARGET_OS_MAC
/**
 *  Returns an `NSImage` with the receiver's color transformation applied.
 *
 *  @param image      The input `NSImage` you wish transform.
 *  @param renderPath The rendering path to use.
 *
 *  @return A `NSImage` with the receiver's color transformation applied.
 */
- (NSImage *)processNSImage:(NSImage *)image
 preserveEmbeddedColorSpace:(BOOL)preserveEmbeddedColorSpace
                 renderPath:(LUTImageRenderPath)renderPath;
#endif


@end

@interface NSBundle (NSBundleCocoaLUTExtension)
-(LUT *)LUTForResource:(NSString *)name extension:(NSString *)extension;
@end
