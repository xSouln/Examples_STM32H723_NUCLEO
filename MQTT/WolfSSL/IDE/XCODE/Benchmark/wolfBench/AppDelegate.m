/* AppDelegate.m
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#import "AppDelegate.h"

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <wolfcrypt/test/test.h>

@interface AppDelegate ()

@end

@implementation AppDelegate

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

extern int bench_tls(void);

static func_args args = { 0 } ;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
#if 1
    printf("\nwolfCrypt Test:\n");
    wolfcrypt_test(&args);
    printf("wolfCrypt Test: Return code %d\n", args.return_code);
#else
    (void)args;
#endif
#if 1
    printf("\nwolfCrypt Benchmark:\n");
    benchmark_test(&args);
    printf("wolfCrypt Benchmark: Return code %d\n", args.return_code);
#else
    (void)args;
#endif
#if 0
    bench_tls();
#endif

    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
