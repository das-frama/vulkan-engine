#include "platform.h"

#if defined(PLATFORM_APPLE)

#include "core/logger.h"

#include "container/darray.h"

#include <stdio.h>
#include <stdlib.h>
#include <mach/mach_time.h>
#include <crt_externs.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@class ApplicationDelegate;
@class WindowDelegate;
@class ContentView;

typedef struct internal_state {
	ApplicationDelegate* app_delegate;
	WindowDelegate* wnd_delegate;
	NSWindow* window;
	ContentView* view;
	CAMetalLayer* layer;
	b8 quit_flagged;
} internal_state;

@interface WindowDelegate : NSObject <NSWindowDelegate> {
    platform_state* state;
}

- (instancetype)initWithState:(platform_state*)init_state;

@end // WindowDelegate

@implementation WindowDelegate

- (instancetype)initWithState:(platform_state*)init_state {
    self = [super init];
    
    if (self != nil) {
        state = init_state;
        // state_ptr->quit_flagged = false;
    }
    
    return self;
}

- (BOOL)windowShouldClose:(id)sender {
    // state_ptr->quit_flagged = true;

    // event_context data = {};
    // event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

    return YES;
}

- (void)windowDidResize:(NSNotification *)notification {
    // event_context context;
    // const NSRect contentRect = [state_ptr->view frame];
    // const NSRect framebufferRect = [state_ptr->view convertRectToBacking:contentRect];
    // context.data.u16[0] = (u16)framebufferRect.size.width;
    // context.data.u16[1] = (u16)framebufferRect.size.height;
    // event_fire(EVENT_CODE_RESIZED, 0, context);
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    // event_context context;
    // context.data.u16[0] = 0;
    // context.data.u16[1] = 0;
    // event_fire(EVENT_CODE_RESIZED, 0, context);

    // [state_ptr->window miniaturize:nil];
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    // event_context context;
    // const NSRect contentRect = [state_ptr->view frame];
    // const NSRect framebufferRect = [state_ptr->view convertRectToBacking:contentRect];
    // context.data.u16[0] = (u16)framebufferRect.size.width;
    // context.data.u16[1] = (u16)framebufferRect.size.height;
    // event_fire(EVENT_CODE_RESIZED, 0, context);

    // [state_ptr->window deminiaturize:nil];
}

@end // WindowDelegate

@interface ContentView : NSView <NSTextInputClient> {
    NSWindow* window;
    NSTrackingArea* trackingArea;
    NSMutableAttributedString* markedText;
}

- (instancetype)initWithWindow:(NSWindow*)initWindow;

@end // ContentView

@implementation ContentView

- (instancetype)initWithWindow:(NSWindow*)initWindow {
    self = [super init];
    if (self != nil) {
        window = initWindow;
    }

    return self;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)wantsUpdateLayer {
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    // input_process_button(BUTTON_LEFT, true);
}

- (void)mouseDragged:(NSEvent *)event {
    // Equivalent to moving the mouse for now
    [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event {
    // input_process_button(BUTTON_LEFT, false);
}

- (void)mouseMoved:(NSEvent *)event {
    // const NSPoint pos = [event locationInWindow];
    
    // input_process_mouse_move((i16)pos.x, (i16)pos.y);
}

- (void)rightMouseDown:(NSEvent *)event {
    // input_process_button(BUTTON_RIGHT, true);
}

- (void)rightMouseDragged:(NSEvent *)event  {
    // Equivalent to moving the mouse for now
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event {
    // input_process_button(BUTTON_RIGHT, false);
}

- (void)otherMouseDown:(NSEvent *)event {
    // Interpreted as middle click
    // input_process_button(BUTTON_MIDDLE, true);
}

- (void)otherMouseDragged:(NSEvent *)event {
    // Equivalent to moving the mouse for now
    // [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event {
    // Interpreted as middle click
    // input_process_button(BUTTON_MIDDLE, false);
}

- (void)keyDown:(NSEvent *)event {
    // keys key = translate_keycode((u32)[event keyCode]);

    // input_process_key(key, true);

    // [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent *)event {
    // keys key = translate_keycode((u32)[event keyCode]);

    // input_process_key(key, false);
}

- (void)scrollWheel:(NSEvent *)event {
    // input_process_mouse_wheel((i8)[event scrollingDeltaY]);
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {}

- (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {}

- (void)unmarkText {}

// Defines a constant for empty ranges in NSTextInputClient
static const NSRange kEmptyRange = { NSNotFound, 0 };

- (NSRange)selectedRange {return kEmptyRange;}

- (NSRange)markedRange {return kEmptyRange;}

- (BOOL)hasMarkedText {return false;}

- (nullable NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return nil;}

- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {return [NSArray array];}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return NSMakeRect(0, 0, 0, 0);}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {return 0;}

@end // ContentView

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {}

@end // ApplicationDelegate

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // Posting an empty event at start
    @autoreleasepool {

    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];

    } // autoreleasepool

    [NSApp stop:nil];
}

@end // ApplicationDelegate

b8 platform_start(platform_state* state, const char* app_name, i32 x, i32 y, i32 width, i32 height) {
	state->internal_state = malloc(sizeof(internal_state));
    internal_state* i_state = (internal_state*)state->internal_state;

    @autoreleasepool {
        [NSApplication sharedApplication];

        i_state->app_delegate = [[ApplicationDelegate alloc] init];
        if (!i_state->app_delegate) {
            ERROR("Failed to create application delegate");
            return false;
        }

        [NSApp setDelegate:i_state->app_delegate];

        // Window delegate creation.
        i_state->wnd_delegate = [[WindowDelegate alloc] initWithState:state];
        if (!i_state->wnd_delegate) {
            ERROR("Failed to create window delegate");
            return false;
        }

        // Window creation.
        i_state->window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(x, y, width, height)
            styleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO];
        if (!i_state->window) {
            ERROR("Failed to create window");
            return false;
        }

        // Layer creation.
        i_state->layer = [CAMetalLayer layer];
        if (!i_state->layer) {
            ERROR("Failed to create layer for view");
            return false;
        }

        // View creation.
        i_state->view = [[ContentView alloc] initWithWindow:i_state->window];
        [i_state->view setLayer:i_state->layer];
        [i_state->view setWantsLayer:YES];

        // Setting window properties.
        [i_state->window setLevel:NSNormalWindowLevel];
        [i_state->window setContentView:i_state->view];
        [i_state->window makeFirstResponder:i_state->view];
        [i_state->window setTitle:@(app_name)];
        [i_state->window setDelegate:i_state->wnd_delegate];
        [i_state->window setAcceptsMouseMovedEvents:YES];
        [i_state->window setRestorable:NO];

        if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
            [NSApp run];
        }

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        // Putting window in front of launch.
        [NSApp activateIgnoringOtherApps:YES];
        [i_state->window makeKeyAndOrderFront:nil];

        return true;
    } // autoreleasepool
}

void platform_shut(platform_state* state) {
    internal_state* i_state = (internal_state*)state->internal_state;

    @autoreleasepool {
        [i_state->window orderOut:nil];
        [i_state->window setDelegate:nil];
        [i_state->wnd_delegate release];

        [i_state->view release];
        i_state->view = nil;

        [i_state->window close];
        i_state->window = nil;

        [NSApp setDelegate:nil];
        [i_state->app_delegate release];
        i_state->app_delegate = nil;
    } // autoreleasepool
}

b8 platform_pump_messages(platform_state* state) {
    @autoreleasepool {
        NSEvent* event;
        for (;;) {
            event = [NSApp
                nextEventMatchingMask:NSEventMaskAny
                untilDate:[NSDate distantPast]
                inMode:NSDefaultRunLoopMode
                dequeue:YES];
            if (!event) {
                break;
            }

            [NSApp sendEvent:event];
        }
    } // autoreleasepool

    return true;
}

void* platform_alloc(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void* block, b8 aligned) {
    free(block);
}

void* platform_zero_mem(void* block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_mem(void* dest, const void* src, u64 size) {
    return memcpy(dest, src, size);
}

void* platform_set_mem(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void platform_console_write(const char* msg, u8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], msg);
}

void platform_console_write_err(const char* msg, u8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], msg);
}

f64 platform_get_abs_time() {
    return mach_absolute_time();
}

void platform_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

void platform_get_required_extension_names(const char*** names_darray) {
    darray_push(names_darray, &"VK_KHR_macos_surface");
}


#endif