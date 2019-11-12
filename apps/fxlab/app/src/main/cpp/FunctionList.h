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
 *
 */

#ifndef ANDROID_FXLAB_FUNCTIONLIST_H
#define ANDROID_FXLAB_FUNCTIONLIST_H

#include <vector>
#include <functional>
#include <array>

class FunctionList {
    std::vector<std::function<void(float *, float *)>> functionList;
public:
    FunctionList() = default;
    FunctionList(const FunctionList &) = delete;
    FunctionList &operator=(const FunctionList &) = delete;

    void addEffect(std::function<void(float *, float *)> f) {
        functionList.emplace_back(std::move(f));
    }

    void operator()(float * begin, float * end) {
        for (auto &f : functionList) {
            f(begin, end);
        }
    }


    void removeEffectAt(unsigned int index) {
        if (index < functionList.size()) {
            functionList.erase(std::next(functionList.begin(), index));
        }
    }

    void rotateEffectAt(unsigned int from, unsigned int to) {
        auto &v = functionList;
        if (from >= v.size() || to >= v.size()) return;
        if (from <= to) {
            std::rotate(v.begin() + from, v.begin() + from + 1, v.begin() + to + 1);
        } else {
            from = v.size() - 1 - from;
            to = v.size() - 1 - to;
            std::rotate(v.rbegin() + from, v.rbegin() + from + 1, v.rbegin() + to + 1);
        }
    }

    void modifyEffectAt(size_t index, std::function<void(float *, float *)> fun) {
        functionList[index] = {std::move(fun)};
    }

};

#endif //ANDROID_FXLAB_FUNCTIONLIST_H

