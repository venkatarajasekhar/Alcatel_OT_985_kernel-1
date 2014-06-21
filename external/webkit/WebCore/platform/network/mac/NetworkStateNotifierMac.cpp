

#include "config.h"
#include "NetworkStateNotifier.h"

#include <SystemConfiguration/SystemConfiguration.h>

#ifdef BUILDING_ON_TIGER 
// This function is available on Tiger, but not declared in the CFRunLoop.h header on Tiger. 
extern "C" CFRunLoopRef CFRunLoopGetMain(); 
#endif 

namespace WebCore {

static const double StateChangeTimerInterval = 2.0;

void NetworkStateNotifier::updateState()
{
    // Assume that we're offline until proven otherwise.
    m_isOnLine = false;
    
    RetainPtr<CFStringRef> str(AdoptCF, SCDynamicStoreKeyCreateNetworkInterface(0, kSCDynamicStoreDomainState));
    
    RetainPtr<CFPropertyListRef> propertyList(AdoptCF, SCDynamicStoreCopyValue(m_store.get(), str.get()));
    
    if (!propertyList)
        return;
    
    if (CFGetTypeID(propertyList.get()) != CFDictionaryGetTypeID())
        return;
    
    CFArrayRef netInterfaces = (CFArrayRef)CFDictionaryGetValue((CFDictionaryRef)propertyList.get(), kSCDynamicStorePropNetInterfaces);
    if (CFGetTypeID(netInterfaces) != CFArrayGetTypeID())
        return;
    
    for (CFIndex i = 0; i < CFArrayGetCount(netInterfaces); i++) {
        CFStringRef interface = (CFStringRef)CFArrayGetValueAtIndex(netInterfaces, i);
        if (CFGetTypeID(interface) != CFStringGetTypeID())
            continue;
        
        // Ignore the loopback interface.
        if (CFStringFind(interface, CFSTR("lo"), kCFCompareAnchored).location != kCFNotFound)
            continue;

        RetainPtr<CFStringRef> key(AdoptCF, SCDynamicStoreKeyCreateNetworkInterfaceEntity(0, kSCDynamicStoreDomainState, interface, kSCEntNetIPv4));

        RetainPtr<CFArrayRef> keyList(AdoptCF, SCDynamicStoreCopyKeyList(m_store.get(), key.get()));
    
        if (keyList && CFArrayGetCount(keyList.get())) {
            m_isOnLine = true;
            break;
        }
    }
}

void NetworkStateNotifier::dynamicStoreCallback(SCDynamicStoreRef, CFArrayRef, void* info) 
{
    NetworkStateNotifier* notifier = static_cast<NetworkStateNotifier*>(info);
    
    // Calling updateState() could be expensive so we schedule a timer that will do it 
    // when things have cooled down.
    notifier->m_networkStateChangeTimer.startOneShot(StateChangeTimerInterval);
}

void NetworkStateNotifier::networkStateChangeTimerFired(Timer<NetworkStateNotifier>*)
{
    bool oldOnLine = m_isOnLine;
    
    updateState();
    
    if (m_isOnLine == oldOnLine)
        return;

    if (m_networkStateChangedFunction)
        m_networkStateChangedFunction();
}

NetworkStateNotifier::NetworkStateNotifier()
    : m_isOnLine(false)
    , m_networkStateChangedFunction(0)
    , m_networkStateChangeTimer(this, &NetworkStateNotifier::networkStateChangeTimerFired)
{
    SCDynamicStoreContext context = { 0, this, 0, 0, 0 };
    
    m_store.adoptCF(SCDynamicStoreCreate(0, CFSTR("com.apple.WebCore"), dynamicStoreCallback, &context));
    if (!m_store)
        return;

    RetainPtr<CFRunLoopSourceRef> configSource = SCDynamicStoreCreateRunLoopSource(0, m_store.get(), 0);
    if (!configSource)
        return;

    CFRunLoopAddSource(CFRunLoopGetMain(), configSource.get(), kCFRunLoopCommonModes);
    
    RetainPtr<CFMutableArrayRef> keys(AdoptCF, CFArrayCreateMutable(0, 0, &kCFTypeArrayCallBacks));
    RetainPtr<CFMutableArrayRef> patterns(AdoptCF, CFArrayCreateMutable(0, 0, &kCFTypeArrayCallBacks));

    RetainPtr<CFStringRef> key;
    RetainPtr<CFStringRef> pattern;

    key.adoptCF(SCDynamicStoreKeyCreateNetworkGlobalEntity(0, kSCDynamicStoreDomainState, kSCEntNetIPv4));
    CFArrayAppendValue(keys.get(), key.get());

    pattern.adoptCF(SCDynamicStoreKeyCreateNetworkInterfaceEntity(0, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv4));
    CFArrayAppendValue(patterns.get(), pattern.get());

    key.adoptCF(SCDynamicStoreKeyCreateNetworkGlobalEntity(0, kSCDynamicStoreDomainState, kSCEntNetDNS));
    CFArrayAppendValue(keys.get(), key.get());

    SCDynamicStoreSetNotificationKeys(m_store.get(), keys.get(), patterns.get());
    
    updateState();
}
    
}