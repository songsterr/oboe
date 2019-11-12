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
#ifndef ANDROID_FXLAB_DUPLEXCALLBACK_H
#define ANDROID_FXLAB_DUPLEXCALLBACK_H

#include <oboe/Oboe.h>


class DuplexCallback : public oboe::AudioStreamCallback {
public:

    DuplexCallback(oboe::AudioStream &inStream,
                   std::function<void(float *, float *)> fun,
                   size_t buffer_size) :
            kBufferSize(buffer_size), inputStream(inStream), f(fun) {}

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *, void *audioData, int32_t numFrames) override {

        return oboe::DataCallbackResult::Continue;
    }

private:
    int mSpinUpCallbacks = 10; // We will let the streams sync for the first few valid frames
    const size_t kBufferSize;
    oboe::AudioStream &inputStream;
    std::function<void(float *, float *)> f;
    std::unique_ptr<float[]> inputBuffer = std::make_unique<float[]>(kBufferSize);


};
#endif //ANDROID_FXLAB_DUPLEXCALLBACK_H
