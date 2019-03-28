// Copyright (c) 2015-2019 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "luna-service2++/handle.hpp"
#include <gtest/gtest.h>

#include "test_util.hpp"

using namespace std;
using namespace LS;

class Category
    : public Handle
{
public:
    Category(const char *service_name, GMainLoop *main_loop)
        : Handle(registerService(service_name))
    {
        attachToLoop(main_loop);
        LS_CATEGORY_BEGIN(Category, "/")
            LS_CATEGORY_METHOD(method)
        LS_CATEGORY_END
    }

private:
    bool method(LSMessage &message)
    {
        Message request(&message);
        cout << "Got request " << request << endl;
        request.respond("{}");
        return true;
    }
};

TEST(Permissions, OutboundA)
{
    MainLoopT main_loop;

    Category B{"com.B1", main_loop.get()};
    Category C{"com.C", main_loop.get()};

    auto A = registerService("com.A");
    A.attachToLoop(main_loop.get());

    auto c = A.callOneReply("luna://com.B1/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    c = A.callOneReply("luna://com.C/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    main_loop.stop();
}

TEST(Permissions, OutboundB)
{
    MainLoopT main_loop;

    Category A{"com.A", main_loop.get()};
    Category C{"com.C", main_loop.get()};

    auto B = registerService("com.B2");
    B.attachToLoop(main_loop.get());

    auto c = B.callOneReply("luna://com.A/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    c = B.callOneReply("luna://com.C/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    main_loop.stop();
}

TEST(Permissions, OutboundC)
{
    MainLoopT main_loop;

    Category A{"com.A", main_loop.get()};
    Category B{"com.B3", main_loop.get()};

    auto C = registerService("com.C");
    C.attachToLoop(main_loop.get());

    auto c = C.callOneReply("luna://com.A/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    c = C.callOneReply("luna://com.B3/method", "{}");
    EXPECT_FALSE(c.get().isHubError());

    main_loop.stop();
}
