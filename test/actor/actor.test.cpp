#include <mbgl/actor/actor.hpp>
#include <mbgl/util/default_thread_pool.hpp>

#include <mbgl/test/util.hpp>

#include <chrono>
#include <functional>
#include <future>
#include <memory>

using namespace mbgl;
using namespace std::chrono_literals;

TEST(Actor, Construction) {
    // Construction is currently synchronous. It may become asynchronous in the future.

    struct Test {
        Test(ActorRef<Test>, bool& constructed) {
            constructed = true;
        };
    };

    ThreadPool pool { 1 };
    bool constructed = false;
    Actor<Test> test(pool, std::ref(constructed));

    EXPECT_TRUE(constructed);
}

TEST(Actor, DestructionBlocksOnReceive) {
    // Destruction blocks until the actor is not receiving.

    struct Test {
        std::promise<void> promise;
        std::future<void> future;
        std::atomic<bool> waited;

        Test(ActorRef<Test>, std::promise<void> promise_, std::future<void> future_)
            : promise(std::move(promise_)),
              future(std::move(future_)),
              waited(false) {
        }

        ~Test() {
            EXPECT_TRUE(waited.load());
        }

        void wait() {
            promise.set_value();
            future.wait();
            std::this_thread::sleep_for(1ms);
            waited = true;
        }
    };

    ThreadPool pool { 1 };

    std::promise<void> enteredPromise;
    std::future<void> enteredFuture = enteredPromise.get_future();

    std::promise<void> exitingPromise;
    std::future<void> exitingFuture = exitingPromise.get_future();

    Actor<Test> test(pool, std::move(enteredPromise), std::move(exitingFuture));

    test.invoke(&Test::wait);
    enteredFuture.wait();
    exitingPromise.set_value();
}

TEST(Actor, DestructionBlocksOnSend) {
    // Destruction blocks until the actor is not being sent a message.

    struct TestScheduler : public Scheduler {
        std::promise<void> promise;
        std::future<void> future;
        std::atomic<bool> waited;

        TestScheduler(std::promise<void> promise_, std::future<void> future_)
            : promise(std::move(promise_)),
              future(std::move(future_)),
              waited(false) {
        }

        ~TestScheduler() {
            EXPECT_TRUE(waited.load());
        }

        void schedule(std::weak_ptr<Mailbox>) final {
            promise.set_value();
            future.wait();
            std::this_thread::sleep_for(1ms);
            waited = true;
        }
    };

    struct Test {
        Test(ActorRef<Test>) {}
        void message() {}
    };

    std::promise<void> enteredPromise;
    std::future<void> enteredFuture = enteredPromise.get_future();

    std::promise<void> exitingPromise;
    std::future<void> exitingFuture = exitingPromise.get_future();

    auto scheduler = std::make_unique<TestScheduler>(std::move(enteredPromise), std::move(exitingFuture));
    auto actor = std::make_unique<Actor<Test>>(*scheduler);

    std::thread thread {
        [] (ActorRef<Test> ref) {
            ref.invoke(&Test::message);
        },
        actor->self()
    };

    enteredFuture.wait();
    exitingPromise.set_value();

    actor.reset();
    scheduler.reset();

    thread.join();
}

TEST(Actor, DestructionAllowedInReceiveOnSameThread) {
    // Destruction doesn't block if occurring on the same
    // thread as receive(). This prevents deadlocks and
    // allows for self-closing actors

    struct Test {

        Test(ActorRef<Test>){};

        void callMeBack(std::function<void ()> callback) {
            callback();
        }
    };

    ThreadPool pool { 1 };

    std::promise<void> callbackFiredPromise;

    auto test = std::make_unique<Actor<Test>>(pool);

    // Callback (triggered while mutex is locked in Mailbox::receive())
    test->invoke(&Test::callMeBack, [&]() {
        // Destroy the Actor/Mailbox in the same thread
        test.reset();
        callbackFiredPromise.set_value();
    });

    auto status = callbackFiredPromise.get_future().wait_for(std::chrono::seconds(1));
    ASSERT_EQ(std::future_status::ready, status);
}

TEST(Actor, OrderedMailbox) {
    // Messages are processed in order.

    struct Test {
        int last = 0;
        std::promise<void> promise;

        Test(ActorRef<Test>, std::promise<void> promise_)
            : promise(std::move(promise_))  {
        }

        void receive(int i) {
            EXPECT_EQ(i, last + 1);
            last = i;
        }

        void end() {
            promise.set_value();
        }
    };

    ThreadPool pool { 1 };

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(pool, std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.invoke(&Test::receive, i);
    }

    test.invoke(&Test::end);
    endedFuture.wait();
}

TEST(Actor, NonConcurrentMailbox) {
    // An individual actor is never itself concurrent.

    struct Test {
        int last = 0;
        std::promise<void> promise;

        Test(ActorRef<Test>, std::promise<void> promise_)
            : promise(std::move(promise_))  {
        }

        void receive(int i) {
            EXPECT_EQ(i, last + 1);
            last = i;
            std::this_thread::sleep_for(1ms);
        }

        void end() {
            promise.set_value();
        }
    };

    ThreadPool pool { 10 };

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(pool, std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.invoke(&Test::receive, i);
    }

    test.invoke(&Test::end);
    endedFuture.wait();
}
