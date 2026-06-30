#pragma once

#include <binder/Parcel.h>

namespace android {

// Forward declaration – we will not define this class
class ICameraServiceExt;

class CameraServiceExtFactory {
public:
    // Returns a pointer to a function table (as required by OxygenOS)
    static void* getInstance();
    static int onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags);

    // R5 Depth-2 result hook (night-preview fix). The factory stays type-light: it only LOADS +
    // exposes the OEM ext object and the raw resolved CameraServiceExtImpl member-fn pointer; the
    // caller (Camera3OutputUtils::insertResultLocked, which has CaptureResult*/CaptureOutputStates&
    // in scope) casts it to the typed signature and invokes it. isLoaded() is the OOS-faithful
    // "ext-enabled" gate (mirrors the onTransact-loaded check) — there is deliberately NO auth gate
    // here: the ext self-gates on the com.oplus.packageName stamp + its onTransact auth state.
    static bool  isLoaded();                     // ext dlopen + factory resolve succeeded
    static void* extObject();                    // the CameraServiceExtImpl* (member-fn `this`)
    static void* beforeMetadataSendToAppFn();    // void(*)(this, CaptureResult*, uint32_t, CaptureOutputStates&)

    virtual ~CameraServiceExtFactory();

private:
    static void ensureLoaded();
    static void* getExtObject();    // resolves + caches the CameraServiceExtImpl* (member-fn `this`)
    static void* sFunctionTable;   // pointer to function pointer
    static void* sExtObject;        // the real extension object (as void*)
    static int (*sOnTransactFunc)(void*, uint32_t, const Parcel&, Parcel*, uint32_t);
    static void* sBeforeMetadataSendToAppFn;  // resolved CameraServiceExtImpl::beforeMetadataSendToApp
};

} // namespace android
