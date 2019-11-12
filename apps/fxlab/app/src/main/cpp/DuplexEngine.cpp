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
//#include "effects/Effects.h"
#include "effects/EchoEffect.h"

DuplexEngine::DuplexEngine() {
    setupStreams();
}

void DuplexEngine::setupStreams() {

    openInputStream();

    mCallback = std::make_unique<DuplexCallback>(
            inputStream,
            [](float *begin, float *end){},
            inputStream->getBufferCapacityInFrames());

    openOutputStream();
    startStreams();

}


oboe::AudioStreamBuilder DuplexEngine::defaultBuilder() {
    return *oboe::AudioStreamBuilder()
            .setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setSampleRateConversionQuality(SampleRateConversionQuality::Medium)
            ->setChannelConversionAllowed(true)
            ->setFormatConversionAllowed(true);
}

oboe::Result DuplexEngine::startStreams() {
    auto result = inputStream->requestStart();
    if (result == oboe::Result::OK) result = outputStream->requestStart();
    if (result != oboe::Result::OK) stopStreams();
    return result;
}

oboe::Result DuplexEngine::stopStreams() {
    oboe::Result outputResult = inputStream->requestStop();
    oboe::Result inputResult = outputStream->requestStop();
    if (outputResult != oboe::Result::OK) return outputResult;
    return inputResult;
}

void DuplexEngine::openInputStream() {

    defaultBuilder().setDirection(Direction::Input)
    ->setChannelCount(1)
    ->setFormat(AudioFormat::Float)
    ->setSampleRate(48000)
    ->openManagedStream(inputStream);


}

void DuplexEngine::openOutputStream() {

    defaultBuilder().setDirection(Direction::Output)
    ->setCallback(mCallback.get())
    ->setChannelCount(1)
    ->setFormat(AudioFormat::Float)
    ->setSampleRate(48000)
    ->openManagedStream(outputStream);

}
