/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of the cross-platform view controller
*/

#import "AAPLViewController.h"
#import "../Renderer/Shaders/AAPLConfig.h"

#include "AAPLRenderer_TraditionalDeferred.h"
#include "AAPLRenderer_SinglePassDeferred.h"

@implementation AAPLViewController
{
    MTK::View *_view;

    Renderer *_renderer;

    MTL::Device * _device;

#if SUPPORT_BUFFER_EXAMINATION
    BufferExaminationManager *_bufferExaminationManager;

    __weak IBOutlet MTKView *_albedoGBufferView;
    __weak IBOutlet MTKView *_normalsGBufferView;
    __weak IBOutlet MTKView *_depthGBufferView;
    __weak IBOutlet MTKView *_shadowGBufferView;
    __weak IBOutlet MTKView *_finalFrameView;
    __weak IBOutlet MTKView *_specularGBufferView;
    __weak IBOutlet MTKView *_shadowMapView;
    __weak IBOutlet MTKView *_lightMaskView;
    __weak IBOutlet MTKView *_lightCoverageView;

#if TARGET_MACOS
    MTKView *_fillView;
    NSView *_placeHolderView;
#endif
#endif
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _device = MTL::CreateSystemDefaultDevice();

    NSAssert(_device, @"Metal is not supported on this device");

    // Set the view to use the default device
    MTKView *mtkView = (MTKView *)self.view;

    mtkView.delegate = self;

    _view = new MTK::View(mtkView, *_device);

    bool useSinglePassDeferred = false;
#if TARGET_MACOS
    if(_device->supportsFamily(MTL::GPUFamilyApple1))
    {
        useSinglePassDeferred = true;
    }
#elif !TARGET_OS_SIMULATOR
    // For iOS or tvOS targets, the sample chooses the single-pass deferred renderer.  Simulator
    // devices do not support features required to run  the single pass deferred renderer, so the
    // app must use the traditional deferred renderer on the simulator.
    useSinglePassDeferred = true;
#endif

    if(useSinglePassDeferred)
    {
        _renderer = new Renderer_SinglePassDeferred( *_view );
    }
    else
    {
        _renderer = new Renderer_TraditionalDeferred( *_view );
    }

    NSAssert(_renderer, @"Renderer failed initialization");

    _renderer->drawableSizeWillChange(*_view, _view->drawableSize());

#if SUPPORT_BUFFER_EXAMINATION

    _bufferExaminationManager = new BufferExaminationManager(*_renderer,
                                                             MTK::View( _albedoGBufferView,   *_device ),
                                                             MTK::View( _normalsGBufferView,  *_device ),
                                                             MTK::View( _depthGBufferView,    *_device ),
                                                             MTK::View( _shadowGBufferView,   *_device ),
                                                             MTK::View( _finalFrameView,      *_device ),
                                                             MTK::View( _specularGBufferView, *_device ),
                                                             MTK::View( _shadowMapView,       *_device ),
                                                             MTK::View( _lightMaskView,       *_device ),
                                                             MTK::View( _lightCoverageView,   *_device ));

    _bufferExaminationManager->updateDrawableSize( _view->drawableSize() );

    _renderer->bufferExaminationManager( _bufferExaminationManager );

#endif

}

- (void)dealloc
{
    delete _renderer;
    _renderer = nullptr;
    delete _device;
    _device = nullptr;

#if SUPPORT_BUFFER_EXAMINATION

    delete _bufferExaminationManager;
    _bufferExaminationManager = nullptr;

#endif
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    MTL::Size newSize = MTL::SizeMake(size.width, size.height, 0);
    _renderer->drawableSizeWillChange(*_view, newSize);

#if SUPPORT_BUFFER_EXAMINATION

    _bufferExaminationManager->updateDrawableSize( newSize );

#endif
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    _renderer->drawInView(*_view);
}

#if TARGET_IOS

#if SUPPORT_BUFFER_EXAMINATION 
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    // Toggle buffer examination mode whenever a touch occurs

    if(_bufferExaminationManager->mode())
    {
        _bufferExaminationManager->mode( ExaminationModeDisabled );
    }
    else
    {
        _bufferExaminationManager->mode( ExaminationModeAll );
    }

    _renderer->validateBufferExaminationMode();
}

#endif // END SUPPORT_BUFFER_EXAMINATION 

- (BOOL)prefersHomeIndicatorAutoHidden
{
    return YES;
}

#elif TARGET_MACOS

- (void)viewDidAppear
{
    // Make the view controller the window's first responder so that it can handle the Key events
    [_view->objCObj().window makeFirstResponder:self];
}

#if SUPPORT_BUFFER_EXAMINATION
-(void)fillWindowWithView:(MTKView*)view
{
    if(view == _fillView)
    {
        return;
    }

    // If there's alread a view filling the window, shrink it back
    if(_fillView)
    {
        assert(_placeHolderView);
        _fillView.autoresizingMask = _placeHolderView.autoresizingMask;
        _fillView.frame = _placeHolderView.frame;
        _fillView = nil;
        [_placeHolderView removeFromSuperview];
        _placeHolderView = nil;
    }

    // If we're providing a new view to fill, enlarge it
    if(view)
    {
        assert(!_placeHolderView);
        assert(!_fillView);
        _placeHolderView = [NSView new];
        _placeHolderView.frame = view.frame;
        _placeHolderView.autoresizingMask = view.autoresizingMask;
        [_view->objCObj() addSubview:_placeHolderView];
        view.frame = _view->objCObj().frame;
        view.autoresizingMask = (NSViewMinXMargin    |
                                 NSViewWidthSizable  |
                                 NSViewMaxXMargin    |
                                 NSViewMinYMargin    |
                                 NSViewHeightSizable |
                                 NSViewMaxYMargin);
        _fillView = view;
    }
}
#endif // SUPPORT_BUFFER_EXAMINATION


- (void)keyDown:(NSEvent *)event
{
#if SUPPORT_BUFFER_EXAMINATION

    ExaminationMode currentMode = _bufferExaminationManager->mode();

    MTKView *focusView = nil;

#endif // SUPPORT_BUFFER_EXAMINATION

    NSString* characters = [event characters];

    for (uint32_t k = 0; k < characters.length; k++)
    {
        unichar key = [characters characterAtIndex:k];

        // When space pressed, toggle buffer examination mode
        switch(key)
        {
            // Pause/Un-pause with spacebar
            case ' ':
            {
                _view->paused( !_view->isPaused() );
                break;
            }
#if SUPPORT_BUFFER_EXAMINATION
            // Enter/exit buffer examination mode with e or return key
            case '\r':
            case '1':
                _bufferExaminationManager->mode( ExaminationModeAll );
                break;
            case '2':
                _bufferExaminationManager->mode( ExaminationModeAlbedo );
                focusView = _albedoGBufferView;
                break;
            case '3':
                _bufferExaminationManager->mode( ExaminationModeNormals );
                focusView = _normalsGBufferView;
                break;
            case '4':
                _bufferExaminationManager->mode( ExaminationModeDepth );
                focusView = _depthGBufferView;
                break;
            case '5':
                _bufferExaminationManager->mode( ExaminationModeShadowGBuffer );
                focusView = _shadowGBufferView;
                break;
            case '6':
                _bufferExaminationManager->mode( ExaminationModeSpecular );
                focusView = _specularGBufferView;
                break;
            case '7':
                _bufferExaminationManager->mode( ExaminationModeShadowMap );
                focusView = _shadowMapView;
                break;
            case '8':
                _bufferExaminationManager->mode( ExaminationModeMaskedLightVolumes );
                focusView = _lightMaskView;
                break;
            case '9':
                _bufferExaminationManager->mode( ExaminationModeFullLightVolumes );
                focusView = _lightCoverageView;
                break;
            case '0':
                _bufferExaminationManager->mode( ExaminationModeDisabled );
                break;
#endif // SUPPORT_BUFFER_EXAMINATION
        }
    }

#if SUPPORT_BUFFER_EXAMINATION

    if(currentMode != _bufferExaminationManager->mode())
    {
        _renderer->validateBufferExaminationMode();
        [self fillWindowWithView:focusView];
    }

#endif // SUPPORT_BUFFER_EXAMINATION
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

#endif // END TARGET_MACOS

@end
