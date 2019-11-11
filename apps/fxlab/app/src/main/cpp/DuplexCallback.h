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



// This callback handles mono in, stereo out synchronized audio passthrough.
// It takes a function which operates on two pointers (beginning and end)
// of underlying data.

/**
 * TODO: Untemplate
 * @tparam numeric_type
 */
template<class numeric_type>
class DuplexCallback : public oboe::AudioStreamCallback {
public:

    DuplexCallback(oboe::AudioStream &inStream,
                   std::function<void(numeric_type *, numeric_type *)> fun,
                   size_t buffer_size, std::function<void(void)> restartFunction) :
            kBufferSize(buffer_size), inputStream(inStream), processingFunction(fun), restartFunction(restartFunction) {}

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *, void *audioData, int32_t numFrames) override {

        // Write this out during the talk

        auto *outputData = static_cast<numeric_type *>(audioData);
        // Silence first to simplify glitch detection
        std::fill(outputData, outputData + numFrames * kChannelCount, 0);
        oboe::ResultWithValue<int32_t> result = inputStream.read(inputBuffer.get(), numFrames, 0);
        int32_t framesRead = result.value();
        if (result != oboe::Result::OK) {
            inputStream.requestStop();
            return oboe::DataCallbackResult::Stop;
        }

        // Switch to slide here
        // Trying to sync reading and writing
        // doesn't affect latency, just cold start (a few 10s of ms at most)
        if (mSpinUpCallbacks > 0 && framesRead > 0) {
            mSpinUpCallbacks--;
            return oboe::DataCallbackResult::Continue;
        }

        // begin pointer, end pointer
        // don't need to allocate any extra storage because the inputBuffer is only modified by us
        processingFunction(inputBuffer.get(), inputBuffer.get() + framesRead);

        // copy inputBuffer to output
        for (int i = 0; i < framesRead; i++) {
            for (size_t j = 0; j < kChannelCount; j++) {
                *outputData++ = inputBuffer[i];
            }
        }
        return oboe::DataCallbackResult::Continue;
    }

    void onErrorAfterClose(oboe::AudioStream *, oboe::Result result) override {
        inputStream.close();
        if (result == oboe::Result::ErrorDisconnected) {
            restartFunction();
        }
    }


private:
    int mSpinUpCallbacks = 10; // We will let the streams sync for the first few valid frames
    static constexpr size_t kChannelCount = 2;
    const size_t kBufferSize;
    oboe::AudioStream &inputStream;
    std::function<void(numeric_type *, numeric_type *)> processingFunction;
    std::function<void(void)> restartFunction;
    std::unique_ptr<numeric_type[]> inputBuffer = std::make_unique<numeric_type[]>(kBufferSize);
};

#endif //ANDROID_FXLAB_DUPLEXCALLBACK_H
