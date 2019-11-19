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

using namespace oboe;

class DuplexCallback : public oboe::AudioStreamCallback {
public:

    DuplexCallback(oboe::ManagedStream &inputStream,
                   std::function<void(float *, float *)> processingFunction) :
            mInputStream(inputStream),
            mProcessingFunction(processingFunction),
            mProcessingBuffer(
                    std::make_unique<float[]>(inputStream->getBufferCapacityInFrames())){}

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *, void *audioData, int32_t numFrames) override {

        // TODO

    }

private:
    int mSpinUpCallbacks = 10; // We will let the streams sync for the first few valid frames
    oboe::ManagedStream &mInputStream;
    std::function<void(float *, float *)> mProcessingFunction;
    std::unique_ptr<float[]> mProcessingBuffer;

};
#endif //ANDROID_FXLAB_DUPLEXCALLBACK_H
