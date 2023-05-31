#import <AppKit/AppKit.h>

// Because of a series of design mistakes made a long time ago, it is not
// possible to handle window events or render window content on anything
// other than the main thread when using AppKit.
// While it is possible to handle events in a non-blocking way, this requires
// extensive tinkering and creates other issues, so it is not worth the risk
// given Apple has a record of changing the way Apps must be initialized.
// This means on top of having to interface with AppKit on the main thread only,
// we also have to let it steal the thread so it can spin its internal loops,
// leaving us with having to use a second one for actual work.
// Another side-effect of that is the rendering and event handling are probably
// never truly parallel on macOS apps, while they can be on other platforms
// (something to keep in mind when writing your program).

// run the example main() on a secondary thread (symbol renamed)
extern int real_main(int argc, char** argv);

@interface GloboxThread: NSThread
@end

@implementation GloboxThread
- (void) main
{
	real_main(0, NULL);
}
@end

// sacrifice the main thread to AppKit
int main(int argc, char** argv)
{
	// create a pool to track Apple resources
	NSAutoreleasePool* pool = [NSAutoreleasePool new];


	// create the app's bar menu
	id menubar = [[NSMenu new] autorelease];
	id appMenuItem = [[NSMenuItem new] autorelease];
	id appMenu = [[NSMenu new] autorelease];

	// add items to the menu (non-localized in this example)
	NSMenuItem* menuItem;

	// add the "Hide" item
	menuItem = [appMenu addItemWithTitle:@"Hide" action:@selector(hide:) keyEquivalent:@"h"];
	[menuItem setTarget:NSApp];

	// add the "Hide Others" item
	menuItem = [appMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand | NSEventModifierFlagOption];
	[menuItem setTarget:NSApp];

	// add the "Show All" item
	menuItem = [appMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
	[menuItem setTarget:NSApp];

	// add a separator item
	[appMenu addItem:[NSMenuItem separatorItem]];

	// add the "Quit" item
	menuItem = [appMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
	[menuItem setTarget:NSApp];

	// set the app's bar menu
	[appMenuItem setSubmenu:appMenu];
	[menubar addItem:appMenuItem];


	// create the app
	[NSApplication sharedApplication];

	// set the app's menu
	[NSApp setMainMenu:menubar];

	// set the app's activation policy
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];


	// start the app
	dispatch_async(dispatch_get_main_queue(), ^{
		[NSApp activateIgnoringOtherApps:YES];
	});

	[NSApp finishLaunching];


	// start globox
	// we *have* to use a NSThread for this, posix threads are not guaranteed to
	// play well with AppKit (here we use GloboxThread which inherits NSThread)
	GloboxThread* thread = [GloboxThread new];
	[thread start];


	// handle window events
	[NSApp run];


	// release Apple resources
	[pool release];

	return 0;
}
