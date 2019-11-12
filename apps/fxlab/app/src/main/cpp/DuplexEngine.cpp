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

#include "DuplexEngine.h"
#include "effects/Effects.h"

using namespace oboe;

DuplexEngine::DuplexEngine() {
    beginStreams();
}

void DuplexEngine::beginStreams() {

    openInputStream();
    SAMPLE_RATE = inStream->getSampleRate();

    // TODO: Add slide on function list
    // TODO: Try using Android Studio 4.0 for better C++ linting
    // TODO: is there a way of refactoring out the lambda?
    // TODO: try adding a more complicated effect

    functionList.addEffect(EchoEffect<float *>(0.5, 250));

    mCallback = std::make_unique<DuplexCallback>(*inStream,
            [this](float *begin, float *end){functionList(begin, end);},
        inStream->getBufferCapacityInFrames());

    openOutputStream();

    startStreams();

    /*// This ordering is extremely important
    openInStream();
    SAMPLE_RATE = inStream->getSampleRate();
    createCallback();
    functionList.addEffect(EchoEffect<float*>(0.5, 100));
    openOutStream();
    oboe::Result result = startStreams();
    if (result != oboe::Result::OK) stopStreams();*/
}


oboe::AudioStreamBuilder DuplexEngine::defaultBuilder() {
    return *oboe::AudioStreamBuilder()
            .setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive);
}

oboe::Result DuplexEngine::startStreams() {
    auto result = inStream->requestStart();
    if (result == oboe::Result::OK) result = outStream->requestStart();
    if (result != oboe::Result::OK) stopStreams();
    return result;
    // TODO: Add slide about minimizing latency

}

oboe::Result DuplexEngine::stopStreams() {
    oboe::Result outputResult = inStream->requestStop();
    oboe::Result inputResult = outStream->requestStop();
    if (outputResult != oboe::Result::OK) return outputResult;
    return inputResult;
}

void DuplexEngine::openInputStream() {

    defaultBuilder().setDirection(Direction::Input)
    ->setFormat(AudioFormat::Float)
    ->setChannelCount(1)
    ->openManagedStream(inStream);

}

void DuplexEngine::openOutputStream() {

    defaultBuilder().setDirection(Direction::Output)
    ->setCallback(mCallback.get())
    ->setChannelCount(1)
    ->setFormat(AudioFormat::Float)
    ->openManagedStream(outStream);
}

/*
void DuplexEngine::createCallback() {






    mCallback = std::make_unique<DuplexCallback>(
            *inStream, [&functionStack = this->functionList](float *beg, float *end) {
                (functionStack)(beg, end);
            },
            inStream->getBufferCapacityInFrames(),
            std::bind(&DuplexEngine::beginStreams, this));
}
*/

/*
void DuplexEngine::openInStream() {
    defaultBuilder().setDirection(oboe::Direction::Input)
            ->setFormat(oboe::AudioFormat::Float) // For now
            ->setChannelCount(1) // Mono in for effects processing
            ->openManagedStream(inStream);
}

void DuplexEngine::openOutStream() {
    defaultBuilder().setCallback(mCallback.get())
            ->setSampleRate(inStream->getSampleRate())
            ->setFormat(inStream->getFormat())
            ->setChannelCount(2) // Stereo out
            ->openManagedStream(outStream);
}*/
