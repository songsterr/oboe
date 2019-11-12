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

        auto *outputData = static_cast<float *>(audioData);

        std::fill(outputData, outputData + numFrames, 0);

        if (inputStream.getState() == oboe::StreamState::Started) {

            oboe::ResultWithValue<int32_t> result = inputStream.read(inputBuffer.get(), numFrames, 0);

            if (mSpinUpCallbacks > 0) {
                // Drain the input.
                int32_t totalFramesRead = 0;
                while (true) {
                    oboe::ResultWithValue result = inputStream.read(inputBuffer.get(),
                            numFrames,
                            0 /* timeout */);
                    if (result.value() == 0 || result.error() != oboe::Result::OK) break;
                }
                // Only counts if we actually got some data.
                if (totalFramesRead > 0) {
                    mSpinUpCallbacks--;
                }

            } else {
                if (result.error() != oboe::Result::OK) return oboe::DataCallbackResult::Stop;
                auto end = inputBuffer.get() + result.value();
                f(inputBuffer.get(), end);
                std::copy(inputBuffer.get(), end, outputData);
            }
        }
        return oboe::DataCallbackResult::Continue;
    }

private:
    int mSpinUpCallbacks = 10; // We will let the streams sync for the first few valid frames
    const size_t kBufferSize;
    oboe::AudioStream &inputStream;
    std::function<void(float *, float *)> f;
    std::unique_ptr<float[]> inputBuffer = std::make_unique<float[]>(kBufferSize);


};


// This callback handles mono in, stereo out synchronized audio passthrough.
// It takes a function which operates on two pointers (beginning and end)
// of underlying data.
/*
class DuplexCallback : public oboe::AudioStreamCallback {
public:

    DuplexCallback(oboe::AudioStream &inStream,
                   std::function<void(float *, float *)> fun,
                   size_t buffer_size, std::function<void(void)> restartFunction) :
            kBufferSize(buffer_size), inputStream(inStream), f(fun), restart(restartFunction) {}


    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *, void *audioData, int32_t numFrames) override {
        float *outputData = static_cast<float *>(audioData);
        // Silence first to simplify glitch detection
        std::fill(outputData, outputData + numFrames * kChannelCount, 0);
        oboe::ResultWithValue<int32_t> result = inputStream.read(inputBuffer.get(), numFrames, 0);
        int32_t framesRead = result.value();
        if (!result) {
            inputStream.requestStop();
            return oboe::DataCallbackResult::Stop;
        }
        if (mSpinUpCallbacks > 0 && framesRead > 0) {
            mSpinUpCallbacks--;
            return oboe::DataCallbackResult::Continue;
        }
        f(inputBuffer.get(), inputBuffer.get() + framesRead);
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
            restart();
        }
    }


private:
    int mSpinUpCallbacks = 10; // We will let the streams sync for the first few valid frames
    static constexpr size_t kChannelCount = 2;
    const size_t kBufferSize;
    oboe::AudioStream &inputStream;
    std::function<void(float *, float *)> f;
    std::function<void(void)> restart;
    std::unique_ptr<float[]> inputBuffer = std::make_unique<float[]>(kBufferSize);
};
*/
#endif //ANDROID_FXLAB_DUPLEXCALLBACK_H
