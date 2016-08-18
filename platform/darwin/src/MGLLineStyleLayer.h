// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineCap) {
    MGLLineStyleLayerLineCapButt,
    MGLLineStyleLayerLineCapRound,
    MGLLineStyleLayerLineCapSquare,
};

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineJoin) {
    MGLLineStyleLayerLineJoinBevel,
    MGLLineStyleLayerLineJoinRound,
    MGLLineStyleLayerLineJoinMiter,
};

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineTranslateAnchor) {
    MGLLineStyleLayerLineTranslateAnchorMap,
    MGLLineStyleLayerLineTranslateAnchorViewport,
};

@interface MGLLineStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Layout Attributes

/**
 The display of line endings.

 The default value of this property is `MGLLineStyleLayerLineCapButt`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineCap;

/**
 The display of lines when joining.

 The default value of this property is `MGLLineStyleLayerLineJoinMiter`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineJoin;

/**
 Used to automatically convert miter joins to bevel joins for sharp angles.

 The default value of this property is `2`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineMiterLimit;

/**
 Used to automatically convert round joins to miter joins for shallow angles.

 The default value of this property is `1.05`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the line will be drawn.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineOpacity;

/**
 The color with which the line will be drawn.

 The default value of this property is an `NSColor` or `UIColor`object whose RGB value is 0, 0, 0 and whose alpha value is 1. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineColor;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 The default value of this property is 0 from the left and 0 from the top. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)

 The default value of this property is `MGLLineStyleLayerLineTranslateAnchorMap`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineTranslateAnchor;

/**
 Stroke thickness.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineWidth;

/**
 Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineGapWidth;

/**
 The line's offset perpendicular to its direction. Values may be positive or negative, where positive indicates "rightwards" (if you were moving in the direction of the line) and negative indicates "leftwards."

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineOffset;

/**
 Blur applied to the line, in pixels.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineBlur;

/**
 Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to pixels, multiply the length by the current line width.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineDasharray;

/**
 Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> linePattern;

@end

NS_ASSUME_NONNULL_END
