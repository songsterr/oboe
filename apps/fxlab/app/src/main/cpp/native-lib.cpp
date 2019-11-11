/*
 * Copyright  2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>

#include <string>
#include <functional>
#include <utility>
#include <vector>

#include "DuplexEngine.h"
#include "effects/Effects.h"
#include "FunctionList.h"


// JNI Utility functions and globals
static DuplexEngine *enginePtr = nullptr;

// Actual JNI interface
extern "C" {

JNIEXPORT void JNICALL
Java_com_mobileer_androidfxlab_NativeInterface_createAudioEngine(
        JNIEnv,
        jobject /* this */) {
    enginePtr = new DuplexEngine();
}
JNIEXPORT void JNICALL
Java_com_mobileer_androidfxlab_NativeInterface_destroyAudioEngine(
        JNIEnv,
        jobject /* this */) {
    if (!enginePtr) return;
    delete enginePtr;
    enginePtr = nullptr;
}

JNIEXPORT void JNICALL
Java_com_mobileer_androidfxlab_NativeInterface_enablePassthroughNative(
        JNIEnv *, jobject, jboolean jenable) {
    if (!enginePtr) return;
    enginePtr->functionList.mute(!static_cast<bool>(jenable));
}

JNIEXPORT void JNICALL
Java_com_mobileer_androidfxlab_NativeInterface_passMIDIvalue(
        JNIEnv *, jobject, jfloat value) {
    if (!enginePtr) return;
    enginePtr->functionList.modifyEffectAt(0, EchoEffect<float *>(0.5, 100 + (value * 400)));
}

} //extern C

